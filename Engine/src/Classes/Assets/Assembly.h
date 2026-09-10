#pragma once

#include "Asset.h"

#include <Classes/Objects/AObject.h>

namespace Refraction::Assets {
	class Assembly : public Asset {
	public:
		// Returns a new copy of the assembly
		Common::Shared<Objects::AObject> Get() const;

		std::string GetSerialisedType() override { return "Assembly"; }
	private:
		// Serialises an object tree
		static std::string Serialise(const Common::Shared<Objects::AObject> &root);
		// Deserialises into a tree of objects
		static Common::Shared<Objects::AObject> Deserialise(const std::string& tree);
	};

	RFCT_ASSET_REGISTERFACTORY(Assembly)
}