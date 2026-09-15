#include <deque>
#include <format>

#include <glad/glad.h>

#include <Core/Utilities.h>
#include "Settings.h"
#include <Math/Vector.h>

#include "GLFWWindow.h"


namespace RPlatform = Refraction::Engine::Platform;
namespace RCommon = Refraction::Common;

namespace Refraction::Engine::Platform {
	GLFWWindow::GLFWWindow() = default;

	static void GLFWErrorCallback(int code, const char* description) {
		Log::Render.Error(std::format("GLFW | CODE: {} | MESSAGE: {}", code, description));
	}

	void GLFWWindow::Init() {
		glfwInit();
		glfwSetErrorCallback(GLFWErrorCallback);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_CONTEXT_DEBUG, true);
		glfwWindowHint(GLFW_DECORATED, false);

		mHandle = glfwCreateWindow(Settings::CurrentSettings->Window.Width, Settings::CurrentSettings->Window.Height, Settings::CurrentSettings->Window.Title, nullptr, nullptr);
		if (!mHandle) {
			glfwTerminate();
			throw std::runtime_error("Failed to create GLFW window");
		}
		glfwMakeContextCurrent(mHandle);
		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
			throw std::runtime_error("Failed to initialize GLAD");
		}
		glViewport(0, 0, Settings::CurrentSettings->Window.Width, Settings::CurrentSettings->Window.Height);

		glfwSetWindowUserPointer(mHandle, this);
	}

	static void OnMouseScroll(GLFWwindow*, double, const double yoffset) {
		auto& cameraSpeed = Settings::CurrentSettings->Controls.CameraSpeed;
		cameraSpeed = std::clamp(yoffset > 0 ? cameraSpeed * 1.1f : cameraSpeed * 0.9f, 0.01f, 5.0f);
	}

	void GLFWWindow::InitInput() {
		glfwSetScrollCallback(mHandle, OnMouseScroll);
		mInput.inputEnabled = true;
	}

	void GLFWWindow::OnUpdate(const Common::Shared<Objects::Camera> camera) {
		if (mInput.inputEnabled) {

			// Get inputs
			mInput.keyW = (glfwGetKey(mHandle, GLFW_KEY_W) == GLFW_PRESS);
			mInput.keyA = (glfwGetKey(mHandle, GLFW_KEY_A) == GLFW_PRESS);
			mInput.keyS = (glfwGetKey(mHandle, GLFW_KEY_S) == GLFW_PRESS);
			mInput.keyD = (glfwGetKey(mHandle, GLFW_KEY_D) == GLFW_PRESS);
			mInput.keyQ = (glfwGetKey(mHandle, GLFW_KEY_Q) == GLFW_PRESS);
			mInput.keyE = (glfwGetKey(mHandle, GLFW_KEY_E) == GLFW_PRESS);
			mInput.keyEsc = (glfwGetKey(mHandle, GLFW_KEY_ESCAPE) == GLFW_PRESS);
			mInput.mouseLeft = (glfwGetMouseButton(mHandle, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
			mInput.mouseRight = (glfwGetMouseButton(mHandle, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);

			double currentMouseX, currentMouseY;
			glfwGetCursorPos(mHandle, &currentMouseX, &currentMouseY);

			// Process inputs
			mInput.inputFocus = (mInputState == WindowInputState::VIEWPORT);

			if (mInput.keyEsc && (mInputState == WindowInputState::VIEWPORT)) {
				mInputState = WindowInputState::NONE;
			}
			if (mInput.mouseLeft && (mInputState == WindowInputState::NONE)) {
				mInputState = WindowInputState::VIEWPORT;
			}

			// Map WASDQE to Vector3 directional input
			const auto dirInput = Utilities::BoolToVector3(mInput.keyD, mInput.keyA, mInput.keyE, mInput.keyQ, mInput.keyW, mInput.keyS);

			// Camera movement
			if (mInput.inputFocus && mInput.mouseRight) {
				glfwSetInputMode(mHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				const auto mouseDelta = Math::Vector2(static_cast<float>(currentMouseX - mInput.lastMouseX), static_cast<float>(mInput.lastMouseY - currentMouseY));
				const auto angInput = Math::Vector3(mouseDelta.y, mouseDelta.x, 0.0f);

				camera->ProcessInput(dirInput, angInput);
			} else {
				glfwSetInputMode(mHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}

			mInput.lastMouseX = currentMouseX;
			mInput.lastMouseY = currentMouseY;
		}


		glfwPollEvents();
		glfwSwapBuffers(mHandle);

		Math::Rect currentRect;
		glfwGetWindowPos(mHandle, &currentRect.x, &currentRect.y);
		glfwGetFramebufferSize(mHandle, &currentRect.w, &currentRect.h);
		if (currentRect != mRect) {
			mRect = currentRect;
			if(!mIgnoreWindowResize) mShouldFramebufferRegen = true;
		}
	}

	void GLFWWindow::Cleanup() {
		glfwTerminate();
	}

	bool GLFWWindow::ShouldClose() const {
		return glfwWindowShouldClose(mHandle);
	}

	bool GLFWWindow::IsFullscreen() const {
		return glfwGetWindowAttrib(mHandle, GLFW_MAXIMIZED) == GLFW_TRUE;
	}

	void GLFWWindow::SetRect(const Math::Rect newRect) {
		glfwSetWindowPos(mHandle, newRect.x, newRect.y);
		glfwSetWindowSize(mHandle, newRect.w, newRect.h);
	}

	void GLFWWindow::Maximise() {
		glfwMaximizeWindow(mHandle);
	}

	void GLFWWindow::Minimise() {
		glfwIconifyWindow(mHandle);
	}

	void GLFWWindow::Restore() {
		glfwRestoreWindow(mHandle);
	}

	void GLFWWindow::Close() {
		glfwSetWindowShouldClose(mHandle, true);
	}
}
