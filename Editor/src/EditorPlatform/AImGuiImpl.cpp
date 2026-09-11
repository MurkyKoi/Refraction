#include <format>
#include <utility>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>

#include <Settings.h>
#include <EditorState.h>
//#include <EditorNet.h>
#include <EditorTheme.h>
#include <ImGuiExtension.h>
#include <EditorPlatform/ADialogs.h>
#include <EditorGUI/Modals.h>

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

	void Platform::AImGuiImpl::UpdateInputState() {
		if (ImGui::GetIO().WantCaptureMouse && !(mWindow->mInputState == WindowInputState::VIEWPORT && ImGui::GetIO().MouseDown[1])) {
			mWindow->mInputState = WindowInputState::GUI;
		} else if (mWindow->mInputState == WindowInputState::GUI) {
			mWindow->mInputState = WindowInputState::VIEWPORT;
		}
	}

	void Platform::AImGuiImpl::HideMouse() {
		ImGui::GetIO().MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
	}

	void Platform::AImGuiImpl::DrawMenu() {
		auto& project = EditorState::Temp.ProjectInstance;

		std::function<void()> onQuit = [&] {
			EditorState::Temp.SelectedObject = nullptr;
			project->Close();
			mShouldQuit = true;
			CloseWindow();
		};

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New")) {
					if (project->New(Dialogs::SelectFolder("Select Project Folder"))) {
						EditorState::AddToRecentProjects(project->GetFilePath());
					}
				}
				if (ImGui::MenuItem("Open")) {
					auto path = Dialogs::SelectFile(RFCT_PROJECT_EXTENSION, "Select Project File");
					if (!path.empty()) {
						EditorState::AddToRecentProjects(path);
						project->Open(path);
					}
				}
				if (ImGui::BeginMenu("Open Recent")) {
					for (auto& path : EditorState::Persistent.RecentProjects) {
						if (!std::filesystem::exists(path)) continue;
						if (ImGui::MenuItem(path.filename().string().c_str())) {
							project->Open(path);
						}
					}
					ImGui::EndMenu();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Save", "Ctrl+S", nullptr, project->IsLoaded())) {
					project->Save();
				}
				if (ImGui::MenuItem("Close", nullptr, nullptr, project->IsLoaded())) {
					mCloseProjectModal = true;
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Quit", "Alt+F4")) {
					if (project->IsLoaded()) {
						mQuitModal = true;
					} else {
						onQuit();
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit")) {
				ImGui::MenuItem("lol");
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View")) {
				auto& graphicsSettings = Settings::CurrentSettings->Graphics;
				ImGui::MenuItem("Wireframe", nullptr, &graphicsSettings.WireframeEnabled);
				ImGui::MenuItem("CFAA", nullptr, &graphicsSettings.CFAAEnabled);
				if (ImGui::BeginMenu("CFAA Scale")) {
					if (ImGui::MenuItem("x2", nullptr, graphicsSettings.CFAAScale == 2)) graphicsSettings.CFAAScale = 2;
					if (ImGui::MenuItem("x3", nullptr, graphicsSettings.CFAAScale == 3)) graphicsSettings.CFAAScale = 3;
					if (ImGui::MenuItem("x4", nullptr, graphicsSettings.CFAAScale == 4)) graphicsSettings.CFAAScale = 4;
					if (ImGui::MenuItem("x6", nullptr, graphicsSettings.CFAAScale == 6)) graphicsSettings.CFAAScale = 6;
					if (ImGui::MenuItem("x8", nullptr, graphicsSettings.CFAAScale == 8)) graphicsSettings.CFAAScale = 8;
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Shader Data")) {
					if (ImGui::MenuItem("Final Image", nullptr, (graphicsSettings.ViewportDataView == 0))) graphicsSettings.ViewportDataView = 0;
					if (ImGui::MenuItem("Depth", nullptr, (graphicsSettings.ViewportDataView == 1))) graphicsSettings.ViewportDataView = 1;
					if (ImGui::MenuItem("Diffuse", nullptr, (graphicsSettings.ViewportDataView == 2))) graphicsSettings.ViewportDataView = 2;
					if (ImGui::MenuItem("Specular", nullptr, (graphicsSettings.ViewportDataView == 3))) graphicsSettings.ViewportDataView = 3;
					if (ImGui::MenuItem("Normals", nullptr, (graphicsSettings.ViewportDataView == 4))) graphicsSettings.ViewportDataView = 4;
					if (ImGui::MenuItem("CFAA Contrast", nullptr, (graphicsSettings.ViewportDataView == 5))) graphicsSettings.ViewportDataView = 5;
					ImGui::EndMenu();
				}
				ImGui::Separator();
				ImGui::MenuItem("Viewport", nullptr, &EditorState::Temp.PanelViewportVisible);
				ImGui::MenuItem("Properties", nullptr, &EditorState::Temp.PanelPropertiesVisible);
				ImGui::MenuItem("Explorer", nullptr, &EditorState::Temp.PanelExplorerVisible);
				ImGui::MenuItem("Statistics", nullptr, &EditorState::Temp.PanelStatisticsVisible);
				ImGui::MenuItem("Collaboration", nullptr, &EditorState::Temp.PanelLiveCollabVisible);
				ImGui::MenuItem("Log", nullptr, &EditorState::Temp.PanelLogVisible);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Tools")) {
				ImGui::MenuItem("Theme Editor", nullptr, &mShowThemeEditor);
				ImGui::Separator();
				ImGui::MenuItem("ImGui Demo Window", nullptr, &mShowDemoWindow);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		if (mShowDemoWindow) ImGui::ShowDemoWindow(&mShowDemoWindow);
		if (mShowThemeEditor) {
			EditorTheme::DrawThemeEditor(&mShowThemeEditor);
		};

		if (mQuitModal) {
			GUI::ModalData data{
				.Title = "Quit Editor",
				.Body = "Do you want to save the project before exiting?"
			};

			GUI::Option quitSave;
			quitSave.Name = "Save & Quit";
			quitSave.OnSelect = [&] {
				project->Save();
			};

			GUI::Option quitNoSave;
			quitNoSave.Name = "Quit";

			GUI::ThreeOptionModal(mQuitModal, data, quitSave, quitNoSave, {.Name = "Cancel"}, onQuit);
		}

		if (mCloseProjectModal) {
			GUI::ModalData data{
				.Title = "Close Project",
				.Body = "Do you want to save the project before closing it?"
			};

			GUI::Option saveClose;
			saveClose.Name = "Save";
			saveClose.OnSelect = [&] {
				project->Save();
			};

			GUI::Option noSaveClose;
			noSaveClose.Name = "Don't Save";

			std::function<void()> onClose = [&] {
				EditorState::Temp.SelectedObject = nullptr;
				project->Close();
			};

			GUI::ThreeOptionModal(mCloseProjectModal, data, saveClose, noSaveClose, { .Name = "Cancel" }, onClose);
		}
	}

	void Platform::AImGuiImpl::DrawRibbon() {

	}
	void Platform::AImGuiImpl::DrawStatsBar() {

	}
}
