#include <Core/Utilities.h>
#include <Classes/ClassSerialiser.h>

#include "AComponent.h"

namespace Refraction::Components {
	AComponent::AComponent() {
		mUUID = UUID();
	}

	AComponent::~AComponent() {
		mUUID.Reset();
	}

	nlohmann::json AComponent::Serialise() {
		return Utilities::ClassSerialiser::AppendJSON({}, [&](nlohmann::json& json) {
			json["SerialisedType"] = GetSerialisedType();
			json["UUID"] = mUUID.Serialise();
			json["ClassName"] = mClassName;
			json["ParentUUID"] = mParent ? mParent->GetUUID().Serialise() : UUID::Null().Serialise();
			json["Required"] = mRequired;
		});
	}

	void AComponent::Deserialise(std::string serialised) {
		Utilities::ClassSerialiser::TryParseJSON(serialised, [&](nlohmann::json& json) {
			mClassName = json.at("ClassName").get<std::string>();
			mUUID = UUID::Deserialise(json.at("UUID"));
			mRequired = json.at("Required").get<bool>();
		});
	}
}
