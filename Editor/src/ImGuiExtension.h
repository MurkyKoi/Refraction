#pragma once

#include <string>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>

#include <Math/Vector.h>

// Custom ImGui functions
namespace ImGui {
	static void Text(const std::string& text) { Text(text.c_str()); }
	static bool TreeNode(const std::string& label) { return TreeNode(label.c_str()); }
}

namespace Refraction::Math {
	static Vector2 FromImVec2(const ImVec2 vec) {
		return {vec.x, vec.y};
	}
}