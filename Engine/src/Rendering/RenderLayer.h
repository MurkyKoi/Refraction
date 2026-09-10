#pragma once

#include <Core/Common.h>
#include <Core/EventSystem.h>
#include <Core/LayerSystem.h>
#include <Math/Rect.h>
#include <Classes/Assets/Image.h>
#include <Rendering/Renderer.h>
#include <Interface/Project.h>

namespace Refraction::Events {
	class FrameRenderedEvent : public Events::Event {
	public:
		Common::Ref<Assets::Image> mFrame;

		explicit FrameRenderedEvent(const Common::Ref<Assets::Image> &newFrame) : mFrame(newFrame) {
			mName = "FrameRendered";
		}
	};
	class ViewportResizedEvent : public Events::Event {
	public:
		Math::Rect mViewportRect;

		explicit ViewportResizedEvent(const Math::Rect newRect) : mViewportRect(newRect) {
			mName = "ViewportResized";
		}
	};
}

namespace Refraction::Engine {
	class RenderLayer : public ALayer {
	public:
		RenderLayer(Common::Shared<Events::AEventDispatcher> eventDispatcher, Common::Shared<Project> projectInstance);

		void OnAttach() override;
		void OnDetach() override;
		void OnPass() override;
		void OnEvent(Common::Shared<Events::Event> event) override;
	private:
		Common::Shared<Events::AEventDispatcher> mEventDispatcher;
		Common::Shared<Project> mProjectInstance;

		Renderer mRenderer;
	};
}


