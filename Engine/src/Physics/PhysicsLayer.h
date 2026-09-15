#pragma once

#include <Core/Common.h>
#include <Core/EventSystem.h>
#include <Core/LayerSystem.h>
#include <Interface/Project.h>

namespace Refraction::Events {
}

namespace Refraction::Engine {
	class PhysicsLayer : public ALayer {
	public:
		PhysicsLayer(const Common::Shared<Events::AEventDispatcher>& eventDispatcher, const Common::Shared<Project>& projectInstance);

		void OnAttach() override;
		void OnDetach() override;
		void OnPass() override;
		void OnEvent(Common::Shared<Events::Event> event) override;
	private:
		Common::Shared<Events::AEventDispatcher> mEventDispatcher;
		Common::Shared<Project> mProjectInstance;
	};
}
