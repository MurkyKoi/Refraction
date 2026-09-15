#pragma once

#include <GLFW/glfw3.h>

#include <Platform/AWindow.h>

enum WindowInputState {
	VIEWPORT,
	GUI,
	NONE
};

namespace Refraction::Engine::Platform {
	class GLFWWindow : public AWindow {
	public:
		bool mFramebufferResized = false;

		GLFWWindow();

		void Init() override;
		void InitInput() override;
		void OnUpdate(Common::Shared<Objects::Camera> camera) override;
		void Cleanup() override;

		[[nodiscard]] void* GetNativeWindow() const override { return mHandle; }
		[[nodiscard]] bool ShouldClose() const override;
		[[nodiscard]] bool IsFullscreen() const override;
		void SetRect(Math::Rect newRect) override;
		void Maximise() override;
		void Minimise() override;
		void Restore() override;
		void Close() override;
	private:
		GLFWwindow* mHandle = nullptr;
	};
}
