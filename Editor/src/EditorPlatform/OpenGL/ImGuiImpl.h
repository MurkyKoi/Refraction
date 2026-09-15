#pragma once

#include <EditorPlatform/AImGuiImpl.h>

namespace Refraction::Editor::Platform::OpenGL {
	class ImGuiImpl : public AImGuiImpl {
	public:
		using AImGuiImpl::AImGuiImpl;

		void Init() override;
		void BeginDraw() override;
		void EndDraw() override;
	protected:
		void CloseWindow() override;
	};
}