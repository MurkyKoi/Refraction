#pragma once

#include <Core/Common.h>
#include <Classes/Objects/AObject.h>
#include <Platform/AWindow.h>

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
		void DrawRibbon();
		void DrawStatsBar();
	protected:
		Common::Shared<Engine::Platform::AWindow> mWindow;

		virtual void CloseWindow() = 0;

	private:
		float mRibbonHeight = 48;
		float mStatsBarHeight = 8;
	};
}