#include <string>

#include <GLFW/glfw3.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_IMPL_OPENGL_DEBUG
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <Core/Common.h>

#include "ImGuiImpl.h"

namespace Refraction::Editor::Platform::OpenGL {
	void ImGuiImpl::Init() {
		if (!ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(mWindow->GetNativeWindow()), true)) throw std::runtime_error("Failed to init ImGui for GLFW");
		if (!ImGui_ImplOpenGL3_Init("#version 330")) throw std::runtime_error("Failed to init ImGui for OpenGL");

		Log::Editor.Info("ImGui initialised");
	}

	void ImGuiImpl::BeginDraw() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiImpl::EndDraw() {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	void ImGuiImpl::CloseWindow() {
		glfwSetWindowShouldClose(static_cast<GLFWwindow*>(mWindow->GetNativeWindow()), true);
	}
}
