#include "WindowTitleBar.h"

#include <Interface/Project.h>
#include <EditorState.h>
#include <EditorTheme.h>
#include <Core/LayerSystem.h>
#include <EditorPlatform/ADialogs.h>
#include <EditorGUI/Modals.h>

namespace Refraction::Editor::GUI {
	WindowTitleBar::WindowTitleBar(
		const Common::Shared<Events::AEventDispatcher>& eventDispatcher,
		const Common::Shared<Engine::Platform::AWindow>& window,
		const Common::Shared<Platform::AImGuiImpl>& imGuiImpl
		) : mEventDispatcher(eventDispatcher), mWindow(window), mImGuiImpl(imGuiImpl), mBarHeight(8.0f) {}

	void WindowTitleBar::Init() {}

	void WindowTitleBar::Draw(const float yOffset) {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 6.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 10.0f));

		mBarHeight = ImGui::GetFrameHeight();
		const float titlebarWidth = ImGui::GetContentRegionAvail().x;

		const ImGuiViewport* vp = ImGui::GetMainViewport();
		const auto titleBarPos = ImVec2(vp->Pos.x, vp->Pos.y + yOffset);
		ImGui::SetNextWindowPos(titleBarPos);
		ImGui::SetNextWindowSize(ImVec2(vp->Size.x, mBarHeight));
		ImGui::SetNextWindowViewport(vp->ID);

		constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |ImGuiWindowFlags_MenuBar;

		ImGui::Begin("##Titlebar", nullptr, flags);
		const bool titlebarHovered = ImGui::IsWindowHovered();

		const ImVec2 windowSize = ImGui::GetWindowSize();
		const ImVec2 barStart = ImGui::GetWindowPos();
		const auto barEnd = ImVec2(barStart.x + windowSize.x, barStart.y + windowSize.y);
		auto barStartScreen = ImVec2(barStart.x - vp->Pos.x, barStart.y - vp->Pos.y);
		auto barEndScreen = ImVec2(barEnd.x - vp->Pos.x, barEnd.y - vp->Pos.y);

		const float frameHeight = ImGui::GetFrameHeight();
		const float iconWidth = frameHeight - 2.0f;
		const float rightButtonsWidth = iconWidth * 3.0f + ImGui::GetStyle().ItemSpacing.x * 2.0f;


		//TODO: rest of titlebar shi

		if (ImGui::BeginMenuBar()) {
			ImGui::Text("ICO");
			ImGui::SameLine(iconWidth);

			auto ItemLabel = [](const std::string& icon, const std::string& label) -> std::string {
				if (icon.empty()) return std::string("         ") + label;
				return std::string("   ") + icon + "  " + label;
			};

			const auto& project = EditorState::Temp.ProjectInstance;
			std::function onQuit = [&] {
				EditorState::Temp.SelectedObject = nullptr;
				project->Close();
				mEventDispatcher->Dispatch(Common::NewShared<Events::ProgramCloseEvent>());
				mWindow->Close();
			};

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(3, 3));
			const float originalBorder = ImGui::GetStyle().PopupBorderSize;
			ImGui::GetStyle().PopupBorderSize = 0.0f;
			EditorTheme::PushColour(ImGuiCol_PopupBg, EditorTheme::ColourIndex_Background3);
			ImGui::SetNextWindowSizeConstraints(ImVec2(150, 0), ImVec2(FLT_MAX, FLT_MAX));
			EditorTheme::PushColour(ImGuiCol_Text, EditorTheme::ColourIndex_Text2);
			if (ImGui::BeginMenu("File")) {
				EditorTheme::PushColour(ImGuiCol_HeaderHovered, EditorTheme::ColourIndex_Accent2);
				EditorTheme::PushColour(ImGuiCol_Text, EditorTheme::ColourIndex_Text1);

				if (ImGui::MenuItem(ItemLabel("", "New").c_str())) {
					if (project->New(Dialogs::SelectFolder("Select Project Folder"))) {
						EditorState::AddToRecentProjects(project->GetFilePath());
					}
				}
				if (ImGui::MenuItem(ItemLabel("", "Open").c_str())) {
					if (const auto path = Dialogs::SelectFile(RFCT_PROJECT_EXTENSION, "Select Project File"); !path.empty()) {
						EditorState::AddToRecentProjects(path);
						project->Open(path);
					}
				}
				if (ImGui::BeginMenu(ItemLabel("", "Open Recent").c_str())) {
					for (auto& path : EditorState::Persistent.RecentProjects) {
						if (!std::filesystem::exists(path)) continue;
						if (ImGui::MenuItem(path.filename().string().c_str())) {
							project->Open(path);
						}
					}
					ImGui::EndMenu();
				}

				ImGui::Separator();
				if (ImGui::MenuItem(ItemLabel("", "Save").c_str(), "Ctrl+S")) { auto _ = project->Save(); }
				ImGui::Separator();
				//if (ImGui::MenuItem(ItemLabel("", "Export").c_str())) { EditorState::Temp.DialogExportVisible = true; }
				//ImGui::Separator();
				if (ImGui::MenuItem(ItemLabel("", "Close").c_str())) {
					mCloseProjectModal = true;
				}
				if (ImGui::MenuItem("Quit", "Alt+F4")) {
					if (project->IsLoaded()) {
						mQuitModal = true;
					} else {
						onQuit();
					}
				}

				EditorTheme::PopColour(2);
				ImGui::EndMenu();
			}
			ImGui::SetNextWindowSizeConstraints(ImVec2(150, 0), ImVec2(FLT_MAX, FLT_MAX));
			if (ImGui::BeginMenu("View")) {
				EditorTheme::PushColour(ImGuiCol_HeaderHovered, EditorTheme::ColourIndex_Accent2);
				EditorTheme::PushColour(ImGuiCol_Text, EditorTheme::ColourIndex_Text1);

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
					if (ImGui::MenuItem("Final Image", nullptr, graphicsSettings.ViewportDataView == 0)) graphicsSettings.ViewportDataView = 0;
					if (ImGui::MenuItem("Depth", nullptr, graphicsSettings.ViewportDataView == 1)) graphicsSettings.ViewportDataView = 1;
					if (ImGui::MenuItem("Diffuse", nullptr, graphicsSettings.ViewportDataView == 2)) graphicsSettings.ViewportDataView = 2;
					if (ImGui::MenuItem("Specular", nullptr, graphicsSettings.ViewportDataView == 3)) graphicsSettings.ViewportDataView = 3;
					if (ImGui::MenuItem("Normals", nullptr, graphicsSettings.ViewportDataView == 4)) graphicsSettings.ViewportDataView = 4;
					if (ImGui::MenuItem("CFAA Contrast", nullptr, graphicsSettings.ViewportDataView == 5)) graphicsSettings.ViewportDataView = 5;
					ImGui::EndMenu();
				}
				ImGui::Separator();
				ImGui::MenuItem("Viewport", nullptr, &EditorState::Temp.PanelViewportVisible);
				ImGui::MenuItem("Properties", nullptr, &EditorState::Temp.PanelPropertiesVisible);
				ImGui::MenuItem("Explorer", nullptr, &EditorState::Temp.PanelExplorerVisible);
				ImGui::MenuItem("Statistics", nullptr, &EditorState::Temp.PanelStatisticsVisible);
				ImGui::MenuItem("Collaboration", nullptr, &EditorState::Temp.PanelLiveCollabVisible);
				ImGui::MenuItem("Log", nullptr, &EditorState::Temp.PanelLogVisible);

				EditorTheme::PopColour(2);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Tools")) {
				EditorTheme::PushColour(ImGuiCol_HeaderHovered, EditorTheme::ColourIndex_Accent2);
				EditorTheme::PushColour(ImGuiCol_Text, EditorTheme::ColourIndex_Text1);

				ImGui::MenuItem("Theme Editor", nullptr, &mShowThemeEditor);
				ImGui::Separator();
				ImGui::MenuItem("ImGui Demo Window", nullptr, &mShowDemoWindow);

				EditorTheme::PopColour(2);
				ImGui::EndMenu();
			}
			ImGui::GetStyle().PopupBorderSize = originalBorder;
			ImGui::PopStyleVar();
			EditorTheme::PopColour(2); // PopupBg text2

			if (project->IsLoaded()) {
				const float currentX = ImGui::GetCursorPosX();
				const char* icon = EditorState::Temp.SimulatingGame ? "X" : "V";
				const ImVec2 textSize = ImGui::CalcTextSize(icon);
				if (const float centerX = (windowSize.x * 0.5f) - (textSize.x * 0.5f); centerX > currentX) ImGui::SetCursorPosX(centerX);
				const auto btnIconCol = EditorState::Temp.SimulatingGame ? EditorTheme::ColourIndex_Warning : EditorTheme::ColourIndex_Text1;
				EditorTheme::PushColour(ImGuiCol_Button, EditorTheme::ColourIndex_Primary1, 0.0f);
				EditorTheme::PushColour(ImGuiCol_Text, btnIconCol);
				if (ImGui::Button(icon)) {
					EditorState::Temp.SimulatingGame = !EditorState::Temp.SimulatingGame;
					if (EditorState::Temp.SimulatingGame) project->SimulateGame();
					else project->StopSimulatingGame();
				}
				EditorTheme::PopColour(2);
			}

			if (project->IsLoaded()) {
				const std::string projectName = project->GetFilePath().filename().string();
				std::string sceneName;
				auto hasScene = false;
				if (const auto sc = project->GetActiveScene(); !sc.expired()) { sceneName = sc.lock()->mInstanceName; hasScene = true; }
				//ImGui::PushFont(Fonts()->notoSansBold);
				const float projectWidth = ImGui::CalcTextSize(projectName.c_str()).x;
				const float bcIconWidth = hasScene ? ImGui::CalcTextSize(">").x : 0;
				const float sceneWidth = hasScene ? ImGui::CalcTextSize(sceneName.c_str()).x : 0;
				const float totalWidth = projectWidth + bcIconWidth + sceneWidth + (hasScene ? 4.0f : 0);
				ImGui::SetCursorPosX(windowSize.x - rightButtonsWidth - totalWidth - 6.0f);
				EditorTheme::PushColour(ImGuiCol_Text, EditorTheme::ColourIndex_Text2);
				ImGui::TextUnformatted(projectName.c_str());
				if (hasScene) {
					ImGui::SameLine(0, 2.0f);
					const ImVec2 pos = ImGui::GetCursorPos();
					ImGui::SetCursorPosY(pos.y + 1.0f);
					ImGui::TextUnformatted(">");
					ImGui::SetCursorPosY(pos.y);
					ImGui::SameLine(0, 2.0f);
					ImGui::TextUnformatted(sceneName.c_str());
				}
				EditorTheme::PopColour();
				//ImGui::PopFont();
			}
			ImGui::SetCursorPosX(windowSize.x - rightButtonsWidth);
			EditorTheme::PushColour(ImGuiCol_Button, EditorTheme::ColourIndex_Primary1, 0.0f);
			const auto buttonSize = ImVec2(frameHeight * 1.1f, frameHeight - 2.0f);
			// Minimize
			//ImGui::PushFont(Fonts()->codicon);
			if (ImGui::Button("-", buttonSize)) mWindow->Minimise();
			ImGui::SameLine(0,0);

			if (const char* icon = mWindow->IsFullscreen() ? "P" : "O"; ImGui::Button(icon, buttonSize)) {
				if (mWindow->IsFullscreen()) mWindow->Restore();
				else mWindow->Maximise();
			}
			ImGui::SameLine(0,0);

			EditorTheme::PushColour(ImGuiCol_ButtonHovered, EditorTheme::ColourIndex_Error);
			if (ImGui::Button("X", buttonSize)) {
				if (project->IsLoaded()) {
					mQuitModal = true;
				} else {
					onQuit();
				}
			}
			EditorTheme::PopColour();
			//ImGui::PopFont();
			EditorTheme::PopColour();

			ImGui::EndMenuBar();


			if (mShowDemoWindow) ImGui::ShowDemoWindow(&mShowDemoWindow);
			if (mShowThemeEditor) {
				EditorTheme::DrawThemeEditor(&mShowThemeEditor);
			}

			if (mQuitModal) {
				ModalData data{
					.Title = "Quit Editor",
					.Body = "Do you want to save the project before exiting?"
				};

				Option quitSave;
				quitSave.Name = "Save & Quit";
				quitSave.OnSelect = [&] {
					auto _ = project->Save();
				};

				Option quitNoSave;
				quitNoSave.Name = "Quit";

				ThreeOptionModal(mQuitModal, data, quitSave, quitNoSave, {.Name = "Cancel"}, onQuit);
			}

			if (mCloseProjectModal) {
				ModalData data{
					.Title = "Close Project",
					.Body = "Do you want to save the project before closing it?"
				};

				Option saveClose;
				saveClose.Name = "Save";
				saveClose.OnSelect = [&] {
					auto _ = project->Save();
				};

				Option noSaveClose;
				noSaveClose.Name = "Don't Save";

				std::function onClose = [&] {
					EditorState::Temp.SelectedObject = nullptr;
					project->Close();
				};

				ThreeOptionModal(mCloseProjectModal, data, saveClose, noSaveClose, { .Name = "Cancel" }, onClose);
			}
		}
		ImGui::End();
		ImGui::PopStyleVar(2);
		bool hoveringDragArea = titlebarHovered && !ImGui::IsAnyItemHovered();
	}

	void WindowTitleBar::OnEvent(Common::Shared<Events::Event> event) {}
}
