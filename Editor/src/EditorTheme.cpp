#include <fstream>

#include <json.hpp>

#include <Core/FileHandling.h>
#include <Classes/ClassSerialiser.h>
#include <ImGuiExtension.h>
#include <EditorPlatform/ADialogs.h>

#include "EditorTheme.h"

namespace Refraction::Editor {
	constexpr const char* ColourIndexStr[] = {
		"Primary1",
		"Primary2",
		"Primary3",
		"Secondary1",
		"Secondary2",
		"Accent1",
		"Accent2",
		"Text1",
		"Text2",
		"Background1",
		"Background2",
		"Background3",
		"Background4",
		"Error",
		"Warning",
		"Success",
		"X",
		"Y",
		"Z",
	};

	std::filesystem::path EditorTheme::CurrentThemePath = "";
	std::array<ImVec4, EditorTheme::ColourIndex_COUNT> EditorTheme::Palette = {};
	ImFont* EditorTheme::DisplayFont = nullptr;
	std::filesystem::path EditorTheme::DisplayFontSource = "";

	void EditorTheme::LoadDefault(DefaultName name) {
		switch (name) {
		case DefaultName::Dark:
		default:
			Palette.at(ColourIndex_Primary1) = FromRGBA(77, 77, 79);
			Palette.at(ColourIndex_Primary2) = FromRGBA(70, 70, 77);
			Palette.at(ColourIndex_Primary3) = FromRGBA(30, 30, 30);
			Palette.at(ColourIndex_Secondary1) = FromRGBA(20, 20, 20);
			Palette.at(ColourIndex_Secondary2) = FromRGBA(55, 55, 61);
			Palette.at(ColourIndex_Accent1) = FromRGBA(66, 150, 250);
			Palette.at(ColourIndex_Accent2) = FromRGBA(96, 115, 181);
			Palette.at(ColourIndex_Text1) = FromRGBA(255, 255, 255);
			Palette.at(ColourIndex_Text2) = FromRGBA(128, 128, 128);
			Palette.at(ColourIndex_Background1) = FromRGBA(37, 37, 38);
			Palette.at(ColourIndex_Background2) = FromRGBA(30, 30, 30);
			Palette.at(ColourIndex_Background3) = FromRGBA(51, 51, 51);
			Palette.at(ColourIndex_Background4) = FromRGBA(0, 0, 0);
			Palette.at(ColourIndex_Error) = FromRGBA(219, 72, 115);
			Palette.at(ColourIndex_Warning) = FromRGBA(213, 152, 87);
			Palette.at(ColourIndex_Success) = FromRGBA(174, 243, 87);
			Palette.at(ColourIndex_X) = FromRGBA(219, 72, 115);
			Palette.at(ColourIndex_Y) = FromRGBA(174, 243, 87);
			Palette.at(ColourIndex_Z) = FromRGBA(118, 162, 250);

			DisplayFontSource = FileHandling::GetResourcesPath() / "fonts" / "OpenSans-Regular.ttf";
			DisplayFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(DisplayFontSource.string().c_str());
			break;
		case DefaultName::Light:
			// nah
			break;
		}
	}

	void EditorTheme::ApplyTheme() {
		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_WindowBg] = Palette[ColourIndex_Background1];
		style.Colors[ImGuiCol_PopupBg] = Palette[ColourIndex_Background2];
		style.Colors[ImGuiCol_Border] = Palette[ColourIndex_Secondary2];
		style.Colors[ImGuiCol_Header] = Palette[ColourIndex_Primary3];
		style.Colors[ImGuiCol_HeaderHovered] = Palette[ColourIndex_Primary2];
		style.Colors[ImGuiCol_HeaderActive] = Palette[ColourIndex_Secondary2];
		style.Colors[ImGuiCol_Button] = Palette[ColourIndex_Primary3];
		style.Colors[ImGuiCol_ButtonHovered] = Palette[ColourIndex_Primary1];
		style.Colors[ImGuiCol_ButtonActive] = Palette[ColourIndex_Primary2];
		style.Colors[ImGuiCol_CheckMark] = Palette[ColourIndex_Text1];
		style.Colors[ImGuiCol_SliderGrab] = Palette[ColourIndex_Secondary2];
		style.Colors[ImGuiCol_SliderGrabActive] = Palette[ColourIndex_Accent1];
		style.Colors[ImGuiCol_CheckboxSelectedBg] = Palette[ColourIndex_Background3];
		style.Colors[ImGuiCol_FrameBg] = Palette[ColourIndex_Primary3];
		style.Colors[ImGuiCol_FrameBgHovered] = Palette[ColourIndex_Primary1];
		style.Colors[ImGuiCol_FrameBgActive] = Palette[ColourIndex_Primary2];
		style.Colors[ImGuiCol_Tab] = Palette[ColourIndex_Background2];
		style.Colors[ImGuiCol_TabHovered] = Palette[ColourIndex_Secondary2];
		style.Colors[ImGuiCol_TabActive] = Palette[ColourIndex_Secondary2];
		style.Colors[ImGuiCol_TabSelectedOverline] = Palette[ColourIndex_Accent1];
		style.Colors[ImGuiCol_TabDimmedSelectedOverline] = Palette[ColourIndex_Primary1];
		style.Colors[ImGuiCol_TabUnfocused] = Palette[ColourIndex_Secondary2];
		style.Colors[ImGuiCol_TabUnfocusedActive] = Palette[ColourIndex_Secondary2];
		style.Colors[ImGuiCol_TableRowBg] = Palette[ColourIndex_Background2];
		style.Colors[ImGuiCol_TableRowBgAlt] = Palette[ColourIndex_Background1];
		style.Colors[ImGuiCol_TitleBg] = Palette[ColourIndex_Background2];
		style.Colors[ImGuiCol_TitleBgActive] = Palette[ColourIndex_Background2];
		style.Colors[ImGuiCol_TitleBgCollapsed] = Palette[ColourIndex_Background2];
		style.Colors[ImGuiCol_ScrollbarGrab] = Palette[ColourIndex_Secondary2];
		style.Colors[ImGuiCol_ResizeGrip] = Palette[ColourIndex_Secondary2];
		style.Colors[ImGuiCol_ResizeGripHovered] = Palette[ColourIndex_Secondary2];
		style.Colors[ImGuiCol_ResizeGripActive] = Palette[ColourIndex_Secondary2];
		style.Colors[ImGuiCol_Separator] = Palette[ColourIndex_Primary2];
		style.Colors[ImGuiCol_SeparatorHovered] = Palette[ColourIndex_Secondary2];
		style.Colors[ImGuiCol_SeparatorActive] = Palette[ColourIndex_Secondary2];
		style.Colors[ImGuiCol_Text] = Palette[ColourIndex_Text1];
		style.Colors[ImGuiCol_TextDisabled] = Palette[ColourIndex_Text2];
		style.Colors[ImGuiCol_MenuBarBg] = Palette[ColourIndex_Secondary1];
	}

	void EditorTheme::FetchCurrentColours() {
		const ImGuiStyle& style = ImGui::GetStyle();
		Palette.at(ColourIndex_Primary1) = style.Colors[ImGuiCol_FrameBgHovered];
		Palette.at(ColourIndex_Primary2) = style.Colors[ImGuiCol_FrameBgActive];
		Palette.at(ColourIndex_Primary3) = style.Colors[ImGuiCol_FrameBg];
		Palette.at(ColourIndex_Secondary1) = style.Colors[ImGuiCol_MenuBarBg];
		Palette.at(ColourIndex_Secondary2) = style.Colors[ImGuiCol_Border];
		Palette.at(ColourIndex_Accent1) = style.Colors[ImGuiCol_SliderGrabActive];
		Palette.at(ColourIndex_Accent2) = FromRGBA(96, 115, 181);
		Palette.at(ColourIndex_Text1) = style.Colors[ImGuiCol_Text];
		Palette.at(ColourIndex_Text2) = style.Colors[ImGuiCol_TextDisabled];
		Palette.at(ColourIndex_Background1) = style.Colors[ImGuiCol_WindowBg];
		Palette.at(ColourIndex_Background2) = style.Colors[ImGuiCol_PopupBg];
		Palette.at(ColourIndex_Background3) = style.Colors[ImGuiCol_CheckboxSelectedBg];
		//Palette.at(ColourIndex_Background4) = FromRGBA(0, 0, 0);
		//Palette.at(ColourIndex_Error) = FromRGBA(219, 72, 115);
		//Palette.at(ColourIndex_Warning) = FromRGBA(213, 152, 87);
		//Palette.at(ColourIndex_Success) = FromRGBA(174, 243, 87);
		//Palette.at(ColourIndex_X) = FromRGBA(219, 72, 115);
		//Palette.at(ColourIndex_Y) = FromRGBA(174, 243, 87);
		//Palette.at(ColourIndex_Z) = FromRGBA(118, 162, 250);
	}

	void EditorTheme::DrawThemeEditor(bool* pOpen) {
		ImGui::Begin("Theme Editor", pOpen);
		auto& style = ImGui::GetStyle();

		if (ImGui::BeginTabBar("Theme Editor Tabs")) {
			if (ImGui::BeginTabItem("General")) {
				ImGui::Text("Current theme file path: " + CurrentThemePath.string());
				if (ImGui::Button("Save Changes")) {
					if (!std::filesystem::exists(CurrentThemePath)) {
						SaveToFile("./EditorTheme.rft");
					} else {
						SaveToFile();
					}
				}
				if (ImGui::Button("Load Default")) {
					LoadDefault();
				}
				if (ImGui::Button("Load From File")) {
					if (const auto path = Dialogs::SelectFile(REFRACTION_THEME_EXTENSION, "Choose theme file"); std::filesystem::exists(path)) LoadFromFile(path);
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Colours")) {
				for (unsigned int i = 0; i < ColourIndex_COUNT; i++) {
					auto& col = Palette[i];
					int i4Col[4] = { static_cast<int>(ceilf(col.x * 255)), static_cast<int>(ceilf(col.y * 255)), static_cast<int>(ceilf(col.z * 255)), static_cast<int>(ceilf(col.w * 255)) };
					ImGui::SliderInt4(ColourIndexStr[i], i4Col, 0, 255, "%d", ImGuiSliderFlags_ColorMarkers);
					col = FromRGBA(static_cast<uint8_t>(i4Col[0]), static_cast<uint8_t>(i4Col[1]), static_cast<uint8_t>(i4Col[2]), static_cast<uint8_t>(i4Col[3]));
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Fonts")) {
				ImGui::Text("Current font source path: " + DisplayFontSource.string());
				if (ImGui::DragFloat("Font Size", &style.FontSizeBase, 0.20f, 5.0f, 100.0f, "%.0f")) {
					style._NextFrameFontSizeBase = style.FontSizeBase;
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Widgets")) {
				ImGui::Text("TBD");
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}

		ImGui::End();

		// Show changes in real time
		ApplyTheme();
	}

	bool EditorTheme::LoadFromFile(const std::filesystem::path& path) {
		Log::Editor.Info("Loading theme from file " + path.string());

		const auto contents = FileHandling::ReadFile(path);
		Utilities::ClassSerialiser::TryParseJSON(contents, [&](nlohmann::json json) {
			for (unsigned int i = 0; i < ColourIndex_COUNT; i++) {
				if (json.contains(std::to_string(i))) {
					const auto col = Utilities::ClassSerialiser::DeserialiseVector4(json.at(std::to_string(i)));
					Palette[i] = ImVec4(col.x, col.y, col.z, col.w);
				}
			}
			if (json.contains("DisplayFontSource")) {
				DisplayFontSource = json.at("DisplayFontSource").get<std::string>();
				if (std::filesystem::exists(DisplayFontSource)) {
					DisplayFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(DisplayFontSource.string().c_str());
				}
			}
			if (!DisplayFont) {
				Log::Editor.Warn("No valid display font specified, using default");
				DisplayFontSource = FileHandling::GetResourcesPath() / "fonts" / "OpenSans-Regular.ttf";
				DisplayFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(DisplayFontSource.string().c_str());
			}
			if (json.contains("DisplayFontSize")) {
				ImGui::GetStyle().FontSizeBase = json.at("DisplayFontSize").get<float>();
			}
		});

		CurrentThemePath = path;
		Log::Editor.Info("Successfully loaded theme");
		return true;
	}

	bool EditorTheme::SaveToFile(std::filesystem::path path) {
		if (path.empty()) {
			if (CurrentThemePath.empty()) {
				Log::Editor.Error("Attempt to save theme without loading from a file first");
				return false;
			}
			path = CurrentThemePath;
		}
		Log::Editor.Info("Saving current theme to file " + path.string());

		const auto serialised = Utilities::ClassSerialiser::AppendJSON({}, [&](nlohmann::json& json) {
			for (unsigned int i = 0; i < ColourIndex_COUNT; i++) {
				const auto& col = Palette[i];
				json[std::to_string(i)] = Utilities::ClassSerialiser::Serialise(Math::Vector4(col.x, col.y, col.z, col.w));
			}
			json["DisplayFontSource"] = DisplayFontSource;
			json["DisplayFontSize"] = ImGui::GetStyle().FontSizeBase;
		});

		std::ofstream dataFile(path);
		if (!dataFile.is_open()) {
			Log::Editor.Error("Could not open path for writing");
			return false;
		}
		dataFile << serialised.dump(RFCT_JSON_INDENT);

		Log::Editor.Info("Successfully saved theme");
		return true;
	}

}
