#pragma once

#include <utility>
#include <vector>
#include <string>
#include <functional>
#include <stdexcept>
#include <memory>
#include <format>

#define INVALID_UNIFORM_LOCATION 0xffffffff
#define GLCheckError() (glGetError() == GL_NO_ERROR)

constexpr int RFCT_JSON_INDENT = 1;

namespace Refraction {
	namespace Common {
		// Shared pointer wrapper
		template<typename T>
		using Shared = std::shared_ptr<T>;
		// Creates a new Shared (shared pointer)
		template<typename T, typename... Args>
		constexpr Shared<T> NewShared(Args&&... args) {
			return std::make_shared<T>(std::forward<Args>(args)...);
		}

		// Weak pointer wrapper
		template<typename T>
		using Ref = std::weak_ptr<T>;
		// Creates a new Ref (weak pointer)
		template<typename T, typename... Args>
		constexpr Ref<T> NewRef(Shared<T> shared) {
			return std::weak_ptr<T>(shared);
		}

		// Unique pointer wrapper
		template<typename T>
		using URef = std::unique_ptr<T>;
		// Creates a new URef (unique pointer)
		template<typename T, typename... Args>
		constexpr URef<T> NewURef(Args&&... args) {
			return std::make_unique<T>(std::forward<Args>(args)...);
		}

		template<typename T, typename O>
		constexpr T* AsA(O&& object) {
			return dynamic_cast<T*>(object);
		}
		template<typename T, typename O>
		constexpr bool IsA(O&& object) {
			return dynamic_cast<T*>(object) != nullptr;
		}
		template<typename T, typename O>
		constexpr T* AsA(Shared<O> object) {
			return dynamic_cast<T*>(object.get());
		}
		template<typename T, typename O>
		constexpr bool IsA(Shared<O> object) {
			return dynamic_cast<T*>(object.get()) != nullptr;
		}

		template <typename Base, typename Target>
		concept DerivesFrom = std::is_base_of_v<Base, Target>;

		class RuntimeError : public std::runtime_error {
		public:
			RuntimeError(const std::string& msg);
		};

		// Use to prevent calls to external libraries during pre-main() initialisation
		extern bool RuntimeExternalReady;
	}

	class Log {
	public:
		struct Colour {
			int R, G, B;
			bool operator==(const Colour& other) const {
				return (R == other.R) && (G == other.G) && (B == other.B);
			}
		};
		// Takes the colour, the body, and whether to put this on a new line
		typedef std::function<void(Colour colour, std::string body, bool newline)> LogCallback;

		static std::string GenerateTimestamp();
		static void SInfo(const std::string& message);
		static void SWarn(const std::string& message);
		static void SError(const std::string& message);
		static void AddLogCallback(const LogCallback& callback) { Callbacks.push_back(callback); }
		static void InitConsoleLog();

		static Log Render;
		static Log Physics;
		static Log Runtime;
		static Log Project;
		static Log Editor;

		Log() : mName("Refraction") {}
		explicit Log(std::string name) : mName(std::move(name)) {}

		void Info(const std::string &message) const { InternalInfo(message); }
		template<typename... Args>
		void Info(const std::string format, Args&&... args) {
			InternalInfo(std::vformat(format, std::make_format_args(args...)));
		}
		void Warn(const std::string &message) const { InternalWarn(message); }
		template<typename... Args>
		void Warn(const std::string format, Args&&... args) {
			InternalWarn(std::vformat(format, std::make_format_args(args...)));
		}
		void Error(const std::string &message) const { InternalError(message); }
		template<typename... Args>
		void Error(const std::string format, Args&&... args) {
			InternalError(std::vformat(format, std::make_format_args(args...)));
		}
	protected:
		static std::vector<LogCallback> Callbacks;
		std::string mName;
	private:
		void InternalInfo(const std::string& message) const;
		void InternalWarn(const std::string& message) const;
		void InternalError(const std::string& message) const;
		static void GenerateLog(const std::string& logName, const std::string &message, const std::string& logType, Colour printColour, bool printStack = false, Colour typeColour = { .R = 0,.G = 0,.B = 0 });
	};

}
