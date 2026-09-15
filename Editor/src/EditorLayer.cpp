#include <Rendering/RenderLayer.h>
#include <EditorState.h>
#include <EditorTheme.h>
//#include <EditorNet.h>
#include <EditorGUI/Panels/ExplorerPanel.h>
#include <EditorGUI/Panels/PropertiesPanel.h>
#include <EditorGUI/Panels/ViewportPanel.h>
#include <EditorGUI/Panels/LogPanel.h>
#include <EditorGUI/Panels/StatsPanel.h>
//#include <EditorGUI/Panels/LiveCollabPanel.h>

#include "EditorLayer.h"

constexpr auto ImGuiMenuHeight = 16;
constexpr auto ImGuiRibbonHeight = 48;

namespace Refraction::Editor {
	EditorLayer::EditorLayer(
		const Common::Shared<Events::AEventDispatcher>& eventDispatcher,
		const Common::Shared<Engine::Project>& projectInstance,
		const Common::Shared<Engine::Platform::AWindow>& window,
		const Common::Shared<Platform::AImGuiImpl>& imGuiImpl) : mEventDispatcher(eventDispatcher), mProjectInstance(projectInstance), mWindow(window), mImGuiImpl(imGuiImpl) {
		mTitleBar = Common::NewUnique<GUI::WindowTitleBar>(mEventDispatcher, mWindow, mImGuiImpl);
		mEditorPanels.push_back(Common::NewUnique<GUI::ExplorerPanel>(eventDispatcher, mWindow));
		mEditorPanels.push_back(Common::NewUnique<GUI::PropertiesPanel>(eventDispatcher, mWindow));
		mEditorPanels.push_back(Common::NewUnique<GUI::ViewportPanel>(eventDispatcher, mWindow));
		mEditorPanels.push_back(Common::NewUnique<GUI::LogPanel>(eventDispatcher, mWindow));
		mEditorPanels.push_back(Common::NewUnique<GUI::StatsPanel>(eventDispatcher, mWindow));
		//mEditorPanels.push_back(Common::NewURef<GUI::LiveCollabPanel>(eventDispatcher, mWindow));
	}

	void EditorLayer::OnAttach() {
		if (EditorState::Deserialise()) {
			mWindow->SetRect(EditorState::Persistent.WindowRect);
		} else {
			Log::Editor.Warn("Failed to deserialise EditorState");
		}
		EditorState::Temp.ProjectInstance = mProjectInstance;
		mWindow->mIgnoreWindowResize = true;
		mImGuiImpl->Init();

		if (const auto themeFilePath = FileHandling::GetWorkingDirectory() / ("EditorTheme" + std::string(REFRACTION_THEME_EXTENSION)); std::filesystem::exists(themeFilePath)) {
			EditorTheme::LoadFromFile(themeFilePath);
			EditorTheme::ApplyTheme();
		} else {
			EditorTheme::LoadDefault();
			EditorTheme::ApplyTheme();
		}

		for (const auto& panel : mEditorPanels) {
			panel->Init();
		}
	}

	void EditorLayer::OnDetach() {
		EditorState::Temp.SelectedObject = nullptr;
		EditorState::Temp.ProjectInstance = nullptr;
		EditorState::Persistent.WindowRect = mWindow->GetRect();
		if (!EditorState::Serialise()) {
			Log::Editor.Warn("Failed to serialise EditorState");
		}
	}

	void EditorLayer::OnPass() {
		const auto activeScene = mProjectInstance->GetActiveScene().lock();
		if (activeScene) {
			if (const auto sceneChildren = activeScene->GetChildren(); !sceneChildren->empty()) {
				mImGuiImpl->mSelectedObject = sceneChildren->at(0);
			}
		}
		mImGuiImpl->BeginDraw();

		auto titleBarOffset = 0.0f;
		if(mWindow->IsFullscreen()) titleBarOffset = 6.0f;
		mTitleBar->Draw(titleBarOffset);

		mImGuiImpl->DrawRibbon();

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		//Log::Editor.Info(Math::Vector2(viewport->Size.x, viewport->Size.y).ToString());
		const auto totalOffset = ImGuiMenuHeight + ImGuiRibbonHeight + mTitleBar->GetHeight();
		ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + totalOffset));
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - totalOffset));
		ImGui::SetNextWindowViewport(viewport->ID);
		constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		                                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		                                         ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		if (!mProjectInstance->IsLoaded()) {
			// TODO: Launcher
		} else {

		}
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::Begin("##DockspaceArea", nullptr, windowFlags);
		ImGui::PopStyleVar(2);

		ImGuiStyle& style = ImGui::GetStyle();
		const float minSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 300.0f;
		ImGui::DockSpace(ImGui::GetID("EditorDockspace"));
		style.WindowMinSize.x = minSizeX;

		for (const auto& panel : mEditorPanels) {
			panel->OnDraw();
		}
		ImGui::End();

		mImGuiImpl->EndDraw();
		mImGuiImpl->UpdateInputState();

		if (EditorState::Temp.ViewportHovered) {
			mWindow->mInputState = Engine::Platform::WindowInputState::VIEWPORT;
		} else {
			mWindow->mInputState = Engine::Platform::WindowInputState::GUI;
		}
	}

	void EditorLayer::OnEvent(const Common::Shared<Events::Event> event) {
		if (auto asCloseEvent = Common::AsA<Events::ProgramCloseEvent>(event)) {
			// TODO: close confirmation modal
		}

		for (const auto& panel : mEditorPanels) {
			panel->OnEvent(event);
			if (event->Consumed()) break;
		}
	}
}

