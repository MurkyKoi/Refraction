#include "LayerSystem.h"


namespace Refraction::Engine {
	LayerStack::~LayerStack() {
		OnDetach();
	}

	void LayerStack::PushLayer(const Common::Shared<ALayer>& layer) {
		mLayers.push_back(layer);
		layer->OnAttach();
	}

	void LayerStack::PopLayer(const Common::Shared<ALayer>& layer) {
		for (auto it = mLayers.begin(); it != mLayers.end(); ) {
			if (auto& lyr = *it; lyr != layer) continue;
			layer->OnDetach();
			it = mLayers.erase(it);
			break;
		}
	}

	void LayerStack::OnPass() const {
		for (auto& layer : mLayers) {
			layer->OnPass();
		}
	}

	void LayerStack::OnDetach() const {
		for (auto& layer : mLayers) {
			layer->OnDetach();
		}
	}

	void LayerStack::Dispatch(const Common::Shared<Events::Event> event) {
		for (const auto& layer : mLayers) {
			layer->OnEvent(event);
			if (event->Consumed()) break;
		}
	}

}
