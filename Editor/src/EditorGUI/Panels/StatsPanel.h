#pragma once

#include <deque>

#include <EditorGUI/Panels/AEditorPanel.h>

namespace Refraction::Editor::GUI {
	class StatsPanel : public AEditorPanel {
	public:
		using AEditorPanel::AEditorPanel;
		~StatsPanel() override = default;

		void Init() override;
		void OnDraw() override;
		void OnEvent(Common::Shared<Events::Event> event) override;

	private:
		std::deque<float> mDeltaHistory = {};
		Math::Rect mViewportRect;
	};
}
