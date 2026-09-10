#include <json.hpp>

#include <Core/FileHandling.h>
#include <Classes/ClassSerialiser.h>
#include <Interface/AssetManager.h>

#include "Assembly.h"

using nlohmann::json;

namespace Refraction::Assets {
	Common::Shared<Objects::AObject> Assembly::Get() const {
		Common::Ref<AssetMetadata> metaWeak;
		Engine::AssetManager::Try([&](const Common::Shared<Engine::AssetManager>& assetManager) {
			metaWeak = assetManager->FetchMetadata(GetUUID());
		});
		if (const auto meta = metaWeak.lock()) {
			return Deserialise(FileHandling::ReadFile(meta->AssetPath));
		}
		return nullptr;
	}

	std::string Assembly::Serialise(const Common::Shared<Objects::AObject>& root) {
		return root->Serialise();
	}

	Common::Shared<Objects::AObject> Assembly::Deserialise(const std::string& tree) {
		return Utilities::ClassSerialiser::DeserialiseObject(tree);
	}
}

