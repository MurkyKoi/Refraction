#include <format>
#include <utility>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>

#include <Settings.h>
#include <EditorState.h>
//#include <EditorNet.h>
#include <EditorTheme.h>
#include <ImGuiExtension.h>

#include "AImGuiImpl.h"

namespace Refraction::Editor {
	using Engine::Platform::WindowInputState;

	Platform::AImGuiImpl::AImGuiImpl(Common::Shared<Engine::Platform::AWindow> window) : mWindow(std::move(window)) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(6, 6);
		style.FramePadding = ImVec2(2, 2);
		style.ItemSpacing = ImVec2(8, 2);
		style.ItemInnerSpacing = ImVec2(4, 2);
		style.WindowBorderSize = 1;
		style.ChildBorderSize = 1;
		style.PopupBorderSize = 1;
		style.FrameBorderSize = 0;
		style.WindowRounding = 1;
		style.ChildRounding = 0;
		style.FrameRounding = 0;
		style.PopupRounding = 0;
		style.GrabRounding = 0;
		style.ScrollbarSize = 8;
		style.ScrollbarRounding = 0;
		style.ScrollbarPadding = 2;
		style.TabBorderSize = 0;
		style.TabBarBorderSize = 1;
		style.TabRounding = 0;
	}

	void Platform::AImGuiImpl::HideMouse() {
		ImGui::GetIO().MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
	}

	void Platform::AImGuiImpl::UpdateInputState() {
		if (ImGui::GetIO().WantCaptureMouse && !(mWindow->mInputState == WindowInputState::VIEWPORT && ImGui::GetIO().MouseDown[1])) {
			mWindow->mInputState = WindowInputState::GUI;
		} else if (mWindow->mInputState == WindowInputState::GUI) {
			mWindow->mInputState = WindowInputState::VIEWPORT;
		}
	}

	void Platform::AImGuiImpl::DrawRibbon() {

	}
	void Platform::AImGuiImpl::DrawStatsBar() {

	}
}
