#pragma once

#include <string>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>

#include <Core/Common.h>
#include <Math/Vector.h>
#include <Classes/Objects/AObject.h>
#include <Platform/AWindow.h>
#include <Interface/Project.h>

namespace Refraction::Editor::Platform {
	class AImGuiImpl {
	public:
		Common::Shared<Objects::AObject> mSelectedObject = nullptr;

		explicit AImGuiImpl(Common::Shared<Engine::Platform::AWindow> window);
		virtual ~AImGuiImpl() = default;

		virtual void Init() = 0;
		virtual void BeginDraw() = 0;
		virtual void EndDraw() = 0;

		void HideMouse();
		void UpdateInputState();
		void DrawMenu();
		void DrawRibbon();
		void DrawStatsBar();

		[[nodiscard]] bool ShouldQuit() const { return mShouldQuit; }

	protected:
		Common::Shared<Engine::Platform::AWindow> mWindow;

		virtual void CloseWindow() = 0;

	private:
		bool mShouldQuit = false;

		float mMenuHeight = 8;
		float mRibbonHeight = 48;
		float mStatsBarHeight = 8;

		bool mShowDemoWindow = false;
		bool mShowThemeEditor = false;
		bool mQuitModal = false;
		bool mCloseProjectModal = false;
	};
}