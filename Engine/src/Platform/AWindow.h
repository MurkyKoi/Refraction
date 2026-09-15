#pragma once

#include <string>

#include <Settings.h>
#include <Core/Common.h>
#include <Math/Rect.h>
#include <Classes/Objects/Camera.h>

namespace Refraction::Engine::Platform {
	struct WindowProperties {
		std::string Name;
		int Width = 0;
		int Height = 0;
	};

	enum class WindowInputState {
		VIEWPORT,
		GUI,
		NONE
	};
	enum class WindowAPI {
		NONE = 0,
		GLFW
	};

	class AWindow {
	public:
		static Common::Shared<AWindow> Get();
		static WindowAPI GetAPI() { return CurrentAPI; }

		WindowInputState mInputState = WindowInputState::NONE;
		struct {
			double lastMouseX = Settings::CurrentSettings->Window.Width / 2.0;
			double lastMouseY = Settings::CurrentSettings->Window.Height / 2.0;
			bool inputEnabled = false;
			bool inputFocus = false;
			bool keyW = false;
			bool keyA = false;
			bool keyS = false;
			bool keyD = false;
			bool keyQ = false;
			bool keyE = false;
			bool keyEsc = false;
			bool mouseLeft = false;
			bool mouseRight = false;
		} mInput;
		bool mShouldFramebufferRegen = false;
		bool mIgnoreWindowResize = false;

		virtual ~AWindow() = default;

		virtual void Init() = 0;
		virtual void InitInput() = 0;
		virtual void OnUpdate(Common::Shared<Objects::Camera> camera) = 0;
		virtual void Cleanup() = 0;
		[[nodiscard]] virtual Math::Rect GetRect() const { return mRect; }
		[[nodiscard]] virtual void* GetNativeWindow() const = 0;
		[[nodiscard]] virtual bool ShouldClose() const = 0;
		[[nodiscard]] virtual bool IsFullscreen() const = 0;
		// Sets the position and size of the window
		virtual void SetRect(Math::Rect newRect) = 0;
		virtual void Maximise() = 0;
		virtual void Minimise() = 0;
		virtual void Restore() = 0;
		virtual void Close() = 0;


	protected:
		WindowInputState mInputStateLast = WindowInputState::NONE;
		Math::Rect mRect = Math::Rect(512);

	private:
		static WindowAPI CurrentAPI;
	};
}
