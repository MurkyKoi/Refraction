#include "RenderLayer.h"

#include <utility>

namespace Refraction::Engine {
	RenderLayer::RenderLayer(Common::Shared<Events::AEventDispatcher> eventDispatcher, Common::Shared<Project> projectInstance)
		: mEventDispatcher(std::move(std::move(eventDispatcher))), mProjectInstance(std::move(projectInstance)) {}

	void RenderLayer::OnAttach() {
		mRenderer.Init();
	}
	void RenderLayer::OnDetach() {
		Platform::ATexture::ClearTexturePool();
	}

	void RenderLayer::OnPass() {
		if (mProjectInstance->IsLoaded()) {
			mRenderer.RenderFrame(mProjectInstance);
			mEventDispatcher->Dispatch(Common::NewShared<Events::FrameRenderedEvent>(mRenderer.GetFinalOutput()));
		}
	}

	void RenderLayer::OnEvent(const Common::Shared<Events::Event> event) {
		// Update renderer for a resized viewport
		if (auto e = Common::AsA<Events::ViewportResizedEvent>(event)) {
			mRenderer.SetViewport(Math::Rect(e->mViewportRect.x, e->mViewportRect.y, e->mViewportRect.w, e->mViewportRect.h));
		} else if (auto e = Common::AsA<Events::ProgramCloseEvent>(event)) {

		}
	}
}
