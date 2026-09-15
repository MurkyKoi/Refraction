#pragma once

#include <Core/LayerSystem.h>
#include <Core/EventSystem.h>
#include <Platform/AWindow.h>
#include <Interface/Project.h>
#include <EditorPlatform/AImGuiImpl.h>
#include <EditorGUI/Panels/AEditorPanel.h>
#include <EditorGUI/WindowTitleBar.h>

namespace Refraction::Editor {
	class EditorLayer : public Engine::ALayer {
	public:
		EditorLayer(
			const Common::Shared<Events::AEventDispatcher>& eventDispatcher,
			const Common::Shared<Engine::Project>& projectInstance,
			const Common::Shared<Engine::Platform::AWindow>& window,
			const Common::Shared<Platform::AImGuiImpl>& imGuiImpl
			);

		void OnAttach() override;
		void OnDetach() override;
		void OnPass() override;
		void OnEvent(Common::Shared<Events::Event> event) override;

	private:
		Common::Shared<Events::AEventDispatcher> mEventDispatcher;
		Common::Shared<Engine::Project> mProjectInstance;
		Common::Shared<Engine::Platform::AWindow> mWindow;
		Common::Shared<Platform::AImGuiImpl> mImGuiImpl;

		Common::Unique<GUI::WindowTitleBar> mTitleBar;
		std::vector<Common::Unique<GUI::AEditorPanel>> mEditorPanels;
	};
}