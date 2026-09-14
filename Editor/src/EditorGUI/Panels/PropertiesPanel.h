#pragma once

#include <EditorGUI/Panels/AEditorPanel.h>

namespace Refraction::Editor::GUI {
	class PropertiesPanel : public AEditorPanel {
	public:
		using AEditorPanel::AEditorPanel;
		~PropertiesPanel() override = default;

		void Init() override {}
		void OnDraw() override;
		void OnEvent(Common::Shared<Events::Event> event) override {}

	private:

	};
}
