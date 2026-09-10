#pragma once

#include <vector>

#include <Core/Common.h>
#include <Core/EventSystem.h>

namespace Refraction::Events {
	class ProgramCloseEvent : public Events::Event {
	public:
		ProgramCloseEvent() {
			mName = "ProgramClose";
		}
	};
}

namespace Refraction::Engine {

	class ALayer {
	public:
		virtual ~ALayer() = default;

		virtual void OnAttach() {}
		virtual void OnPass() {}
		virtual void OnDetach() {}
		virtual void OnEvent(Common::Shared<Events::Event> event) {}
	};

	class LayerStack : public Events::AEventDispatcher {
	public:
		LayerStack() = default;
		~LayerStack() override;

		void PushLayer(const Common::Shared<ALayer>& layer);
		void PopLayer(const Common::Shared<ALayer>& layer);

		void OnPass() const;
		void OnDetach() const;
		void Dispatch(Common::Shared<Events::Event> event) override;
	private:
		std::vector<Common::Shared<ALayer>> mLayers;
	};
}