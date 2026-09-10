#pragma once

#include <Classes/Assets/Image.h>
#include <EditorGUI/Panels/AEditorPanel.h>

namespace Refraction::Editor::GUI {
	class ViewportPanel : public AEditorPanel {
	public:
		using AEditorPanel::AEditorPanel;
		~ViewportPanel() override = default;

		void Init() override;
		void OnDraw() override;
		void OnEvent(Common::Shared<Events::Event> event) override;
	private:
		Common::Shared<Assets::Image> mFrame;
		Math::Rect mLastViewportRect;
		Math::Rect mViewportRect;
		bool mFirstDraw = true;
	};
}
