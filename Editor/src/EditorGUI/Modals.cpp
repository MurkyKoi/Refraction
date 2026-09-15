#include <EditorTheme.h>

#include "Modals.h"

namespace Refraction::Editor::GUI {
	void Confirm(bool& shouldDisplay, const ModalData& data, const std::function<void()>& onConfirm) {
		if (!shouldDisplay) return;

		ImGui::OpenPopup(data.Title.c_str());
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));

		ImGui::SetNextWindowSizeConstraints({ 300.0f, 0.0f }, { 400.0f, FLT_MAX });
		EditorTheme::PushColour(ImGuiCol_PopupBg, EditorTheme::ColourIndex_Background1);
		if (ImGui::BeginPopupModal(data.Title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			EditorTheme::PushColour(ImGuiCol_Text, EditorTheme::ColourIndex_Warning);
			ImGui::TextWrapped(data.Body.c_str());
			EditorTheme::PopColour();

			auto modalWidth = ImGui::GetContentRegionAvail().x;
			auto buttonWidth = modalWidth * 0.5f - 5.0f;
			ImGui::Dummy({ 5.0f, 0.0f });

			EditorTheme::PushColour(ImGuiCol_Button, EditorTheme::ColourIndex_Primary3);
			if (ImGui::Button("Yes", ImVec2(buttonWidth, 0))) {
				onConfirm();
				shouldDisplay = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("No", ImVec2(buttonWidth, 0))) {
				shouldDisplay = false;
				ImGui::CloseCurrentPopup();
			}

			EditorTheme::PopColour();
			ImGui::EndPopup();
		}
		EditorTheme::PopColour();
		ImGui::PopStyleVar();
	}

	void ThreeOptionModal(bool& shouldDisplay, const ModalData& data, const Option& opt1, const Option& opt2, const Option& opt3, const std::function<void()>& onNotOpt3) {
		if (!shouldDisplay) return;
		bool opt3Chosen = false;

		ImGui::OpenPopup(data.Title.c_str());
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));

		constexpr float spacing = 5.0f;
		const float widest = std::max({ ImGui::CalcTextSize(opt1.Name.c_str()).x, ImGui::CalcTextSize(opt2.Name.c_str()).x, ImGui::CalcTextSize(opt3.Name.c_str()).x });
		const auto buttonWidth = widest + ImGui::GetStyle().FramePadding.x * 2.0f;

		ImGui::SetNextWindowSize({ buttonWidth * 3.0f + spacing * 4.0f, 0.0f });
		EditorTheme::PushColour(ImGuiCol_PopupBg, EditorTheme::ColourIndex_Background1);
		if (ImGui::BeginPopupModal(data.Title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			EditorTheme::PushColour(ImGuiCol_Text, EditorTheme::ColourIndex_Warning);
			ImGui::TextWrapped(data.Body.c_str());
			EditorTheme::PopColour();

			EditorTheme::PushColour(ImGuiCol_Button, EditorTheme::ColourIndex_Primary3);
			ImGui::Dummy({ 0.0f, 5.0f });

			float cursorX = spacing;
			ImGui::SetCursorPosX(cursorX);
			if (ImGui::Button(opt1.Name.c_str(), ImVec2(buttonWidth, 0))) {
				opt1.OnSelect();
				shouldDisplay = false;
				ImGui::CloseCurrentPopup();
			}

			cursorX += spacing + buttonWidth;
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorX);
			if (ImGui::Button(opt2.Name.c_str(), ImVec2(buttonWidth, 0))) {
				opt2.OnSelect();
				shouldDisplay = false;
				ImGui::CloseCurrentPopup();
			}

			cursorX += spacing + buttonWidth;
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorX);
			if (ImGui::Button(opt3.Name.c_str(), ImVec2(buttonWidth, 0))) {
				opt3.OnSelect();
				shouldDisplay = false;
				opt3Chosen = true;
				ImGui::CloseCurrentPopup();
			}

			EditorTheme::PopColour();
			ImGui::EndPopup();
		}
		EditorTheme::PopColour();
		ImGui::PopStyleVar();

		// Do whatever if the final option (usually "Cancel") wasn't picked
		if (!shouldDisplay && !opt3Chosen) onNotOpt3();
	}
}
