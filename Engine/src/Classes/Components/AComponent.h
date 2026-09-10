#pragma once

#include <string>

#include <json.hpp>

#include <Core/Common.h>
#include <Core/UUID.h>
#include <Classes/Objects/AObject.h>

namespace Refraction::Components {
	class AComponent : Engine::ISerialisable {
	public:
		Objects::AObject* mParent = nullptr;
		bool mRequired = false; // Determines whether this component is required by its parent object

		AComponent();
		~AComponent() override;

		virtual void PreTick() {};
		virtual void Tick(float delta) {};
		virtual void PostTick() {};
		virtual void PreRender() {};
		virtual void Render() {};
		virtual void PostRender() {};

		[[nodiscard]] UUID GetUUID() const { return mUUID; }
		[[nodiscard]] std::string GetDisplayName() const { return mClassName; };

		// Returns a serialised copy of the component
		virtual nlohmann::json Serialise();
		// Loads data from the provided serialised component
		virtual void Deserialise(std::string serialised);

		std::string GetSerialisedType() override { return "AComponent"; }
	protected:
		std::string mClassName = "BaseComponent";
	private:
		UUID mUUID;
	};

	RFCT_COMPONENT_REGISTERFACTORY(AComponent)
}