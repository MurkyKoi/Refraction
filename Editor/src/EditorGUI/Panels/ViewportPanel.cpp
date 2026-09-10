#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <Rendering/RenderLayer.h>
#include <EditorState.h>

#include "ViewportPanel.h"

namespace Refraction::Editor::GUI {
	void ViewportPanel::Init() {
	}

	void ViewportPanel::OnDraw() {
		if (!EditorState::Temp.PanelViewportVisible) return;
		ImGui::SetNextWindowSizeConstraints({ 640, 480 }, { FLT_MAX, FLT_MAX });
		ImGui::Begin("Viewport", &EditorState::Temp.PanelViewportVisible, ImGuiWindowFlags_NoCollapse);

		if (mFirstDraw) {
			mLastViewportRect = Math::Rect(Math::FromImVec2(ImGui::GetWindowPos()), Math::FromImVec2(ImGui::GetContentRegionAvail()));
			mFirstDraw = false;
		}
		// Skip if no frame or no project is open
		if (const auto& project = EditorState::Temp.ProjectInstance; !project || mFrame == nullptr || mFrame->mTexture.expired() || !project->IsLoaded()) {
			ImGui::End();
			return;
		}
		const auto frameTex = mFrame->mTexture.lock();

		ImGui::GetCurrentWindow();

		mViewportRect = Math::Rect(Math::FromImVec2(ImGui::GetWindowPos()), Math::FromImVec2(ImGui::GetContentRegionAvail()));

		// On viewport resize
		if (mViewportRect.w != mLastViewportRect.w || mViewportRect.h != mLastViewportRect.h) {
			mEventDispatcher->Dispatch(Common::NewShared<Events::ViewportResizedEvent>(mViewportRect));
		}

		const float windowWidth = ImGui::GetContentRegionAvail().x;
		const float windowHeight = ImGui::GetContentRegionAvail().y;

		// Common::Ref<Assets::ImageMetadata> frameMeta;
		// Engine::AssetManager::Try([&](const Common::Shared<Engine::AssetManager>& manager) {
		// 	frameMeta = manager->FetchMetadata<Assets::ImageMetadata>(mFrame->GetUUID());
		// });
		// //ImVec2 imageSize = ImVec2((float)frameMeta->Width, (float)frameMeta->Height);
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		const ImVec2 pos = ImGui::GetCursorScreenPos();
		drawList->AddImage(frameTex->GetBufferID(), ImVec2(pos.x, pos.y), ImVec2(pos.x + windowWidth, pos.y + windowHeight), ImVec2(0, 1), ImVec2(1, 0));
		

		// Don't update if RMB is down
		if (!ImGui::GetIO().MouseDown[1]) {
			EditorState::Temp.ViewportHovered = ImGui::IsWindowHovered();
		}

		ImGui::End();

		mLastViewportRect = mViewportRect;
	}

	void ViewportPanel::OnEvent(const Common::Shared<Events::Event> event) {
		if (const auto e = Common::AsA<Events::FrameRenderedEvent>(event)) {
			mFrame = e->mFrame.lock();
		}
	}
}
