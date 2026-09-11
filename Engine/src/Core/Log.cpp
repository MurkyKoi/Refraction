#include <format>
#include <chrono>
#include <iostream>
#include <filesystem>

#include "Common.h"

import cpptrace;

#ifdef _DEBUG
#define DEFAULT_LOG_FRAME 2
#else
#define DEFAULT_LOG_FRAME 2
#endif

#ifdef __MINGW64__
#define LAMBDA_CLASSNAME "function"
#endif
#ifdef _MSC_VER
#define LAMBDA_CLASSNAME "<lambda"
#endif

namespace {
	using Refraction::Log;
	std::string ANSI24RGB(Log::Colour colour) {
		return std::format("\033[38;2;{};{};{}m", colour.R, colour.G, colour.B);
	};
	Log::Colour white = { .R = 255,.G = 255,.B = 255 };
	Log::Colour black = { .R = 0,.G = 0,.B = 0 };
	Log::Colour separatorColour = white;
	Log::Colour threadColour = white;
	Log::Colour timestampColour = { .R = 64, .G = 210, .B = 255 };
	Log::Colour classColour = { .R = 110, .G = 255, .B = 124 };
	Log::Colour functionColour = { .R = 96, .G = 200, .B = 96 };

	std::string LastClassPrinted;
}

namespace Refraction {
	static void OnConsoleLog(const Log::Colour colour, const std::string& message, const bool newLine) {
		using std::vformat, std::make_format_args, std::clog;

		if (newLine) clog << '\n';
		clog << ANSI24RGB(colour) << message << ANSI24RGB(white); // Reset to white after printing
	}

	std::string Log::GenerateTimestamp() {
		using namespace std::chrono;

		// get time variables
		const auto now = system_clock::now();
		const auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
		const auto timer = system_clock::to_time_t(now);

		#pragma warning(suppress : 4996)
		const std::tm bt = *std::localtime(&timer);

		std::ostringstream oss;

		oss << std::put_time(&bt, "%H:%M:%S"); // HH:MM:SS
		oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

		return oss.str();
	};

	void Log::SInfo(const std::string& message) {
		GenerateLog("Refraction", message, "INFO", white, false, Colour{ .R = 200, .G = 255, .B = 255 });
	}
	void Log::SWarn(const std::string& message) {
		GenerateLog("Refraction", message, "WARN", Colour{ .R = 255, .G = 160, .B = 70 });
	}
	void Log::SError(const std::string& message) {
		GenerateLog("Refraction", message, "ERR", Colour{ .R = 255, .G = 60, .B = 60 }, true);
	}

	void Log::InitConsoleLog() {
		AddLogCallback(OnConsoleLog);
	}

	void Log::InternalInfo(const std::string& message) const {
		GenerateLog(mName, message, "INFO", white, false, Colour{ .R = 200, .G = 255, .B = 255 });
	}
	void Log::InternalWarn(const std::string& message) const {
		GenerateLog(mName, message, "WARN", Colour{ .R = 255, .G = 160, .B = 70 });
	}
	void Log::InternalError(const std::string& message) const {
		GenerateLog(mName, message, "ERR", Colour{ .R = 255, .G = 60, .B = 60 }, true);
	}

	void Log::GenerateLog(const std::string& logName, const std::string &message, const std::string& logType, const Colour printColour, const bool printStack, Colour typeColour) {

		const std::string timestamp = GenerateTimestamp();
		if (!Common::RuntimeExternalReady) {
			// Skip cpptrace and just print text

			for (auto& callback : Callbacks) {
				callback(timestampColour, "[" + timestamp + "]", true);
				callback(separatorColour, " - ", false);
				callback(typeColour, logType, false);
				callback(separatorColour, " - ", false);
				callback(printColour, message, false);
			}

			return;
		}

		// Get print information
		auto [frames] = cpptrace::stacktrace::current();

		// Use 3rd frame if not lambda (otherwise 4th)
		int frameIndex = DEFAULT_LOG_FRAME;
		std::string callerSymbol;


		// Step frames if sitting inside of a lambda
		std::string className = "[unknown]";
		bool testLambda = true;
		while (testLambda) {
			callerSymbol = cpptrace::prune_symbol(frames[frameIndex].symbol);

			// cpptrace doesn't seem to prune properly in release builds so we gotta do some manual pruning
			if (const auto symbolPos = callerSymbol.find_last_of('('); symbolPos != std::string::npos) {
				callerSymbol = callerSymbol.substr(0, symbolPos);
			}

			if (auto symbolPos = callerSymbol.find_last_of(':'); symbolPos != std::string::npos) {
				auto classSymbolStr = callerSymbol.substr(0, symbolPos - 1);

				// Get class name from the remaining symbol string
				if (symbolPos = classSymbolStr.find_last_of(':'); symbolPos != std::string::npos) {
					className = classSymbolStr.substr(symbolPos + 1, -1);
				} else className = classSymbolStr.substr(0, -1);

				// Only stop traversing when the class name is NOT anonymous and isn't the Log class
				// TODO: make a dynamic blacklist so this isn't hardcoded
				if (!className.starts_with(LAMBDA_CLASSNAME) && !className.starts_with("Log") && !className.starts_with("Singleton") && !className.starts_with("ClassSerialiser")) {
					testLambda = false;
					break;
				}
			}
			// Go to previous frame
			frameIndex++;
		}
		
		// Get function and class name from frame
		std::string functionName = callerSymbol;

		// Get class and function name from the symbols in the frame
		if (auto symbolPos = callerSymbol.find_last_of(':'); symbolPos != std::string::npos) {

			functionName = callerSymbol.substr(symbolPos + 1, -1);
			const auto classSymbolStr = callerSymbol.substr(0, symbolPos - 1);

			// Get class name from the remaining symbol string
			if (symbolPos = classSymbolStr.find_last_of(':'); symbolPos != std::string::npos) {
				className = classSymbolStr.substr(symbolPos + 1, -1);
			} else className = classSymbolStr.substr(0, -1);
		}

		// Print class name only once
		if (LastClassPrinted != className) {
			LastClassPrinted = className;

			for (auto& callback : Callbacks) {
				callback(threadColour, logName + " - ", true);
				callback(classColour, "class " + LastClassPrinted, false);
				callback(separatorColour, " - ", false);
			}
		}

		if (typeColour == black) typeColour = printColour;

		if (printStack) {
			// Send to callbacks
			for (auto& callback : Callbacks) {
				callback(timestampColour, "[" + timestamp + "]", true);
				callback(separatorColour, " - ", false);
				callback(typeColour, logType + " ", false);
				callback(functionColour, functionName, false);
				callback(separatorColour, " - ", false);
				callback(printColour, message, false);
				callback(printColour, "Stack trace, recent first:", true);
				for (size_t i = 2; i < frames.size(); i++) {
					auto& frame = frames[i];
					if (frame.symbol == "main()" || frame.symbol == "main") break; // stop after reaching entrypoint
					callback(printColour, std::format("#{} ", i-2), true);
					callback({ .R = 255, .G = 160, .B = 70 }, frame.symbol, false);
					callback(white, std::format("\tAt line {} in file ", frame.line.value_or(0)), true);
					callback(classColour, frame.filename, false);
					callback(white, cpptrace::get_snippet(frame.filename, frame.line.value_or(0), 1), true);
				}
				callback(printColour, "--- STACKTRACE END ---", true);
			}
		} else {
			// Send to callbacks
			for (auto& callback : Callbacks) {
				callback(timestampColour, "[" + timestamp + "]", true);
				callback(separatorColour, " - ", false);
				callback(typeColour, logType + " ", false);
				callback(functionColour, functionName, false);
				callback(separatorColour, " - ", false);
				callback(printColour, message, false);
			}
		}
	}

	std::vector<Log::LogCallback> Log::Callbacks = {};

	Log Log::Render = Log("Renderer");
	Log Log::Physics = Log("Physics");
	Log Log::Runtime = Log("Runtime");
	Log Log::Project = Log("Project");
	Log Log::Editor = Log("Editor");

	Common::RuntimeError::RuntimeError(const std::string& msg) : std::runtime_error(msg) {
		// Log here to produce a better stacktrace
		Log::SError("A runtime error occured.");
	}
}
