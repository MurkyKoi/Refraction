#include <format>

#include <imgui/imgui.h>

#include <Classes/ClassHeaders.h>
#include <EditorState.h>

#include "PropertiesPanel.h"

namespace Refraction::Editor::GUI {
	static bool ResetRotationCache = false;
	static UUIDValue LastSelectedUUID = 0;
	static void DrawTransformControls(Math::Transform& transform, std::string uid) {
		if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
			constexpr ImGuiSliderFlags flags = ImGuiSliderFlags_ColorMarkers;
 
			auto& cellPos = transform.mSpatialPosition.CellPosition;
			float guiPos[3] = { cellPos.x, cellPos.y, cellPos.z };
			ImGui::Text("Position");
			ImGui::DragFloat3(std::format("##Position_{}", uid).c_str(), guiPos, 0.01f, 0, 0, "%.3f", flags);
			cellPos.x = guiPos[0];
			cellPos.y = guiPos[1];
			cellPos.z = guiPos[2];
			ImGui::Text("Rotation");
			static auto rotation = Math::Vector3();

			float guiRot[3] = { rotation.x, rotation.y, rotation.z };
			if (ImGui::DragFloat3(std::format("##Rotation_{}", uid).c_str(), guiRot, 1.00f, 0, 0, "%.3f", flags)) {
				rotation.x = guiRot[0];
				rotation.y = guiRot[1];
				rotation.z = guiRot[2];
				transform.mOrientation = Math::Quaternion::FromEulerAngles(rotation);
			} else if (!ImGui::IsItemActive() || ResetRotationCache) {
				rotation = transform.mOrientation.ToEulerAngles();
				ResetRotationCache = false;
			}
			auto& scale = transform.mScale;
			float guiScale[3] = { scale.x, scale.y, scale.z };
			ImGui::Text("Scale");
			ImGui::DragFloat3(std::format("##Scale_{}", uid).c_str(), guiScale, 0.01f, 0, 0, "%.3f", flags);
			scale.x = guiScale[0];
			scale.y = guiScale[1];
			scale.z = guiScale[2];

			ImGui::Text(std::format("As Matrix4: {}", transform.ToMatrix().ToString({ .Pretty = true })));

			ImGui::TreePop();
		}

	}

	static void DrawComponentProperties(Common::Shared<Components::AComponent> component) {
		std::string compFullTypeName = typeid(*component).name();
		auto compTypeName = compFullTypeName.substr(compFullTypeName.find_last_of(':') + 1);
		ImGui::Text("Component Type: " + compTypeName);
		ImGui::Text("UUID: " + component->GetUUID().AsString());

		if (auto casted = Common::AsA<Components::Mesh>(component)) {
			auto model = casted->mModel.lock();
			if (model) {
				Common::Ref<Assets::ModelMetadata> metaWeak;
				Engine::AssetManager::Try([&](const Common::Shared<Engine::AssetManager>& manager) {
					metaWeak = manager->FetchMetadata<Assets::ModelMetadata>(model->GetUUID());
				});
				if (auto meta = metaWeak.lock()) {
					ImGui::Text("Mesh Source: " + meta->AssetPath.string());
				} else {
					ImGui::Text("Mesh Source:");
				}
			} else {
				ImGui::Text("Mesh Source:");
			}
			ImGui::Spacing();
			DrawTransformControls(casted->mTransform, "Mesh");
		} else if (auto casted = Common::AsA<Components::APhysics>(component)) {
			ImGuiSliderFlags flags = ImGuiSliderFlags_ColorMarkers;
			if (ImGui::TreeNode("Linear Velocity")) {
				auto& linearVel = casted->mLinearVelocity;
				float guiLinearVel[3] = { linearVel.x, linearVel.y, linearVel.z };
				ImGui::DragFloat3(std::format("##RigidPhysicsLinear").c_str(), guiLinearVel, 0.01f, 0, 0, "%.3f", flags);
				linearVel.x = guiLinearVel[0];
				linearVel.y = guiLinearVel[1];
				linearVel.z = guiLinearVel[2];
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Angular Velocity")) {
				auto& angularVel = casted->mAngularVelocity;
				float guiAngularVel[3] = { angularVel.x, angularVel.y, angularVel.z };
				ImGui::DragFloat3(std::format("##RigidPhysicsAngular").c_str(), guiAngularVel, 0.01f, 0, 0, "%.3f", flags);
				angularVel.x = guiAngularVel[0];
				angularVel.y = guiAngularVel[1];
				angularVel.z = guiAngularVel[2];
				ImGui::TreePop();
			}
		} else if (auto casted = Common::AsA<Components::Billboard>(component)) {
			ImGui::Checkbox("Render on top", &casted->mRenderOnTop);
			ImGui::Spacing();
			DrawTransformControls(casted->mTransform, "Billboard");
		}
		ImGui::Separator();
	}

	void PropertiesPanel::OnDraw() {
		if (!EditorState::Temp.PanelPropertiesVisible) return;
		ImGui::SetNextWindowSizeConstraints({ 150, 50 }, { FLT_MAX, FLT_MAX });
		ImGui::Begin("Properties", &EditorState::Temp.PanelPropertiesVisible);
		if (EditorState::Temp.SimulatingGame) ImGui::BeginDisabled();

		auto& obj = EditorState::Temp.SelectedObject;
		if (!obj) {
			ImGui::Text("No object selected");
			ResetRotationCache = true;
		} else {
			ImGui::Text("Instance Name: " + obj->mInstanceName);
			std::string objFullTypeName = typeid(obj).name();
			auto objTypeName = objFullTypeName.substr(objFullTypeName.find_last_of(':') + 1);
			ImGui::Text("Object Type: " + objTypeName);
			const auto uuid = obj->GetUUID();
			if (uuid.AsInt() != LastSelectedUUID) {
				LastSelectedUUID = uuid.AsInt();
				ResetRotationCache = true;
			}
			ImGui::Text("UUID: " + uuid.AsString());
			ImGui::Separator();

			ImGui::Text(std::format("World Transform: {}", obj->GetWorldMatrix().ToString({ .Pretty = true })));
			DrawTransformControls(obj->mTransform, "Object");

			ImGui::Separator();

			if (ImGui::CollapsingHeader(std::format("Components ({})", obj->GetComponents()->size()).c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
				for (auto& comp : *obj->GetComponents()) {
					if (ImGui::TreeNode(comp->GetDisplayName())) {
						DrawComponentProperties(comp);
						ImGui::TreePop();
						ImGui::SameLine();
					}
					if (!comp->mRequired && ImGui::Button(std::format("Delete##{}", comp->GetUUID().AsString()).c_str())) {
						obj->RemoveChild(comp->GetUUID());
						continue;
					}
				}
			}
			ImGui::SameLine(0, 1.0f);
			if (ImGui::Button("+ Add")) {
				if (ImGui::BeginPopup("ComponentAddPopup")) {
					if (!obj->GetComponent<Components::Mesh>() && ImGui::Button("MeshComponent")) {
						obj->AddComponent<Components::Mesh>();
					}
					if (!obj->GetComponent<Components::APhysics>() && ImGui::Button("PhysicsComponent")) {
						obj->AddComponent<Components::APhysics>();
					}
					if (!obj->GetComponent<Components::Billboard>() && ImGui::Button("BillboardComponent")) {
						obj->AddComponent<Components::Billboard>();
					}

					ImGui::EndPopup();
				}
			}
		}
		if (EditorState::Temp.SimulatingGame) ImGui::EndDisabled();
		ImGui::End();
	}
}
