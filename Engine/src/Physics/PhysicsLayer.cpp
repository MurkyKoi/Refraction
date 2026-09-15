#include "PhysicsLayer.h"

namespace Refraction::Engine {
	PhysicsLayer::PhysicsLayer(const Common::Shared<Events::AEventDispatcher>& eventDispatcher, const Common::Shared<Project>& projectInstance)
		: mEventDispatcher(eventDispatcher), mProjectInstance(projectInstance) {}

	void PhysicsLayer::OnAttach() {
	}

	void PhysicsLayer::OnDetach() {
	}

	void PhysicsLayer::OnPass() {
		if (mProjectInstance->IsLoaded()) {
			if (const auto scene = mProjectInstance->GetActiveScene().lock()) {
				scene->TickScene(mProjectInstance->GetGlobalObjects());
			}
		}
	}

	void PhysicsLayer::OnEvent(Common::Shared<Events::Event> event) {
	}
}
