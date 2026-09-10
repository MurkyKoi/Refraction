#pragma once

#include <Math/Vector.h>
#include "AComponent.h"

namespace Refraction::Components {
	class APhysics : public AComponent {
	public:
		Math::Vector3 mLinearVelocity;
		Math::Vector3 mAngularVelocity;

		APhysics();

		void Tick(float delta) override;

		nlohmann::json Serialise() override;
		void Deserialise(std::string serialised) override;

		std::string GetSerialisedType() override { return "APhysicsComponent"; }
	};

	RFCT_COMPONENT_REGISTERFACTORY(APhysics)
}