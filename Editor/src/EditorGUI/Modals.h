#pragma once

#include <string>
#include <functional>

#include <Core/Common.h>
#include <ImGuiExtension.h>

namespace Refraction::Editor::GUI {
	struct ModalData {
		std::string Title;
		std::string Body;
	};
	struct Option {
		std::string Name;
		std::function<void()> OnSelect = []{};
	};

	// Draws a modal that lets the user confirm or deny an action.
	// Takes a callback that runs when confirmed.
	// Since it flows like ImGui code, you can use an ImGui button or such to control the shouldDisplay parameter and call this every frame regardless.
	extern void Confirm(bool& shouldDisplay, const ModalData& data, const std::function<void()>& onConfirm);

	// Draws a modal that gives the user 3 options. Each option has an associated name and callback.
	// The ifNotOpt3 parameter defines a callback when either option 1 or 2 is selected, but not option 3.
	// Since it flows like ImGui code, you can use an ImGui button or such to control the shouldDisplay parameter and call this every frame regardless.
	extern void ThreeOptionModal(bool& shouldDisplay, const ModalData& data, const Option& opt1, const Option& opt2, const Option& opt3 = { .Name = "Cancel" }, const std::function<void()>& onNotOpt3 = [] {});
}
