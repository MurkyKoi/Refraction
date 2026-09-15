#pragma once

#include <Core/Common.h>
#include <Core/EventSystem.h>
#include <Platform/AWindow.h>
#include <EditorPlatform/AImGuiImpl.h>

namespace Refraction::Editor::GUI {
	class WindowTitleBar {
	public:
		WindowTitleBar(
			const Common::Shared<Events::AEventDispatcher>& eventDispatcher,
			const Common::Shared<Engine::Platform::AWindow>& window,
			const Common::Shared<Platform::AImGuiImpl>& imGuiImpl
			);
		~WindowTitleBar() = default;

		void Init();
		void Draw(float yOffset);
		void OnEvent(Common::Shared<Events::Event> event);
		[[nodiscard]] float GetHeight() const { return mBarHeight; }

	private:
		Common::Shared<Events::AEventDispatcher> mEventDispatcher;
		Common::Shared<Engine::Platform::AWindow> mWindow;
		Common::Shared<Platform::AImGuiImpl> mImGuiImpl;

		float mBarHeight;
		bool mShowDemoWindow = false;
		bool mShowThemeEditor = false;
		bool mQuitModal = false;
		bool mCloseProjectModal = false;
	};
}
