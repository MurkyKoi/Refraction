#pragma once

#include <Core/Common.h>
#include <Classes/Assets/Asset.h>

namespace Refraction::Engine {
	// Loads new assets into the project
	class AssetImporter {
	public:
		AssetImporter() = default;
		~AssetImporter() = default;

		static UUIDValue Import(const std::filesystem::path &sourcePath);
	};
}
