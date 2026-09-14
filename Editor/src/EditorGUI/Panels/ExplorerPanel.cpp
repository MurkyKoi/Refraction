#include <format>

#include <imgui/imgui.h>

#include <Classes/ClassHeaders.h>
#include <EditorState.h>

#include "ExplorerPanel.h"

namespace Refraction::Editor::GUI {
	static void MakeTree(const Common::Shared<Objects::AObject>& obj) {
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

		auto& currentlySelected = EditorState::Temp.SelectedObject;
		if (currentlySelected && obj == currentlySelected) flags |= ImGuiTreeNodeFlags_Selected;
		if (obj->GetChildren()->empty()) flags |= ImGuiTreeNodeFlags_Leaf;

		const auto nodeName = std::format("{}##{}ExplorerTreeNode", obj->mInstanceName, obj->GetUUID().AsString());
		const auto isOpen = ImGui::TreeNodeEx(nodeName.c_str(), flags);
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
			currentlySelected = obj;
		}
		if (isOpen) {
			if (obj == currentlySelected && ImGui::BeginPopupContextItem(nodeName.c_str())) {
				if (ImGui::Button("Add Child")) {
					const auto newObj = Common::NewShared<Objects::AObject>();
					obj->AddChild(newObj);
				}
				if (ImGui::Button("Delete")) obj->mParent->RemoveChild(obj->GetUUID());
				ImGui::EndPopup();
			}
			for (const auto& child : *obj->GetChildren()) {
				MakeTree(child);
			}
			ImGui::TreePop();
		}

	}

	void ExplorerPanel::OnDraw() {
		if (!EditorState::Temp.PanelExplorerVisible) return;
		ImGui::SetNextWindowSizeConstraints({ 150, 50 }, { FLT_MAX, FLT_MAX });
		ImGui::Begin("Explorer", &EditorState::Temp.PanelExplorerVisible);
		if (EditorState::Temp.SimulatingGame) ImGui::BeginDisabled();

		if (const auto& project = EditorState::Temp.ProjectInstance; !project->IsLoaded()) {
			ImGui::Text("No project loaded");
		} else {
			ImGui::SeparatorText("Scenes"); ImGui::SameLine();
			if (ImGui::Button("Add Scene")) {
				project->NewScene();
			}
			for (auto& sceneWeak : project->GetScenes()) {
				if (sceneWeak.expired()) continue;
				MakeTree(sceneWeak.lock());
			}
			ImGui::SeparatorText("Global Objects"); ImGui::SameLine();
			if (ImGui::Button("Add")) {
				// TODO: Object spawner menu
				const auto newObj = Common::NewShared<Objects::AObject>();
				project->NewGlobalObject();
			}
			for (auto& globalObjWeak : project->GetGlobalObjects()) {
				if (globalObjWeak.expired()) continue;
				MakeTree(globalObjWeak.lock());
			}
		}

		if (EditorState::Temp.SimulatingGame) ImGui::EndDisabled();
		ImGui::End();
	}
}
