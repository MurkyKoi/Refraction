#pragma once

#include <array>
#include <filesystem>

#include <imgui/imgui.h>

#include <Core/Common.h>

constexpr auto REFRACTION_THEME_EXTENSION = ".rft";

namespace Refraction::Editor {
	class EditorTheme {
	public:
		enum ColourIndex {
			ColourIndex_Primary1,
			ColourIndex_Primary2,
			ColourIndex_Primary3,
			ColourIndex_Secondary1,
			ColourIndex_Secondary2,
			ColourIndex_Accent1,
			ColourIndex_Accent2,
			ColourIndex_Text1,
			ColourIndex_Text2,
			ColourIndex_Background1,
			ColourIndex_Background2,
			ColourIndex_Background3,
			ColourIndex_Background4,
			ColourIndex_Error,
			ColourIndex_Warning,
			ColourIndex_Success,
			ColourIndex_X,
			ColourIndex_Y,
			ColourIndex_Z,
			ColourIndex_COUNT
		};

		enum class DefaultName {
			Dark,
			Light
		};

		// Loads a default theme
		static void LoadDefault(DefaultName name = DefaultName::Dark);
		// Applies the full theme to the interface
		static void ApplyTheme();
		// Retrieves the current ImGuiStyle's colours
		static void FetchCurrentColours();
		// Shows a window to edit the theme
		static void DrawThemeEditor(bool* pOpen);

		// Loads from a specified file. Returns success.
		static bool LoadFromFile(const std::filesystem::path& path);
		// Saves to either the currently loaded theme file or another specified file. Returns success.
		static bool SaveToFile(std::filesystem::path path = "");

		static void PushColour(const ImGuiCol_ widget, const ColourIndex index, const float alpha = 1.0f) {
			ImVec4 colour = Palette[index];
			colour.w = alpha;
			ImGui::PushStyleColor(widget, colour);
		}
		static void PopColour(const int count = 1) {
			ImGui::PopStyleColor(count);
		}

		static ImVec4 FromRGBA(const uint8_t R, const uint8_t G, const uint8_t B, const uint8_t A = 255) {
			constexpr float rgbIntToFrac = 1.0f / 255;
			return {static_cast<float>(R) * rgbIntToFrac, static_cast<float>(G) * rgbIntToFrac, static_cast<float>(B) * rgbIntToFrac, static_cast<float>(A) * rgbIntToFrac};
		}
	private:
		static std::filesystem::path CurrentThemePath;
		static std::array<ImVec4, ColourIndex_COUNT> Palette;
		static ImFont* DisplayFont;
		static std::filesystem::path DisplayFontSource;
	};
}

