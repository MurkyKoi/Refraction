#pragma once

#include <EditorGUI/Panels/AEditorPanel.h>

namespace Refraction::Editor::GUI {
	class ExplorerPanel : public AEditorPanel {
	public:
		using AEditorPanel::AEditorPanel;
		~ExplorerPanel() override = default;

		void Init() override {}
		void OnDraw() override;
		void OnEvent(Common::Shared<Events::Event> event) override {}

	private:
		// TODO: filter objects in Explorer
		ImGuiTextFilter mFilter;
	};
}
