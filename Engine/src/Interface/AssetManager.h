#pragma once

#include <unordered_map>
#include <utility>

#include <Core/Common.h>
#include <Core/Singleton.h>
#include <Core/FileHandling.h>
#include <Classes/Assets/Asset.h>

namespace Refraction::Engine {
	// Singleton that handles the AssetMap and Assets directory
	class AssetManager : public Singleton<AssetManager> {
	public:
		AssetManager(std::filesystem::path projectPath) : mProjectPath(std::move(projectPath)) {}
		~AssetManager() = default;

		// Get asset by UUID
		template <typename AssetType> requires Common::DerivesFrom<Assets::Asset, AssetType>
		Common::Ref<AssetType> GetAsset(const UUIDValue uuid) {
			if (mAssetMap.contains(uuid)) return Common::NewRef<AssetType>(std::dynamic_pointer_cast<AssetType>(mAssetMap.at(uuid)));
			// Asset isn't in memory, try finding and loading it
			if (auto metaPath = FindMetadataFile(uuid); std::filesystem::exists(metaPath)) {
				Log::SInfo("Asset with UUID " + std::to_string(uuid) + " is unloaded, registering now");
				return Common::NewRef<AssetType>(std::dynamic_pointer_cast<AssetType>(RegisterAsset(metaPath).lock()));
			}
			return {};
		}

		// Get asset by path
		template <typename AssetType> requires Common::DerivesFrom<Assets::Asset, AssetType>
		Common::Ref<AssetType> GetAsset(std::filesystem::path assetPath) {
			// If invalid path, try appending to assets directory path
			if (!IsValidAsset(assetPath)) assetPath = mProjectPath / "Assets" / assetPath;
			for (auto &val: mAssetMap | std::views::values) {
				const auto& asset = val;
				auto metaWeak = FetchMetadata(asset->GetUUID());
				if (metaWeak.expired()) continue;
				const auto meta = metaWeak.lock();
				if (meta->AssetPath == assetPath) {
					return Common::NewRef<AssetType>(std::dynamic_pointer_cast<AssetType>(asset));
				}
			}
			// Asset isn't in memory, try loading it
			if (IsValidAsset(assetPath)) {
				auto metaPath = GetMetadataPath(assetPath);
				if (!metaPath) return {};
				Log::SInfo("Asset at path " + assetPath.string() + " is unloaded, registering now");
				return Common::NewRef<AssetType>(std::dynamic_pointer_cast<AssetType>(RegisterAsset(metaPath.value()).lock()));
			}
			return {};
		}
		Common::Ref<Assets::Asset> GetAsset(const std::filesystem::path &assetPath) { return GetAsset<Assets::Asset>(assetPath); }

		// Attempts to fetch an asset's metadata with the given UUID
		template <typename MetadataType> requires Common::DerivesFrom<Assets::AssetMetadata, MetadataType>
		Common::Ref<MetadataType> FetchMetadata(const UUIDValue uuid) {
			// Try searching map
			if (mMetadataMap.contains(uuid)) return Common::NewRef<MetadataType>(std::dynamic_pointer_cast<MetadataType>(mMetadataMap.at(uuid)));
			// Not loaded, try deep search on disk
			auto meta = RecursiveFindMetadataByUUID(mProjectPath / "Assets", uuid);
			if (meta) {
				// Save in memory
				mMetadataMap[uuid] = meta;
				return Common::NewRef<MetadataType>(std::dynamic_pointer_cast<MetadataType>(meta));
			}
			// Not found
			return {};
		}
		Common::Ref<Assets::AssetMetadata> FetchMetadata(const UUIDValue uuid) { return FetchMetadata<Assets::AssetMetadata>(uuid); }

		template <typename AssetType> requires Common::DerivesFrom<Assets::Asset, AssetType>
		Common::Ref<AssetType> MakeVolatile() {
			auto newAsset = Common::NewShared<AssetType>();
			if (Assets::Asset* base = Common::AsA<Assets::Asset>(newAsset)) {
				base->MakeVolatile();
				mAssetMap[base->GetUUID()] = newAsset;
				return newAsset;
			} else {
				Log::SError("MakeVolatile was passed an invalid Asset type");
				return {};
			}
		}

		// Loads all assets into memory using their metadata under the project folder
		void RegisterAllAssets();
		// Loads the specified asset into memory using its metadata
		Common::Ref<Assets::Asset> RegisterAsset(const std::filesystem::path& metadataPath);

		// Unloads all assets and metadata from memory
		void UnloadAll();

		// Determines whether the asset is valid (has metadata file, in current project, etc)
		bool IsValidAsset(const std::filesystem::path &assetPath) const;
		// Returns path of the asset's metadata file
		std::optional<std::filesystem::path> GetMetadataPath(std::filesystem::path assetPath) const;
	private:
		std::unordered_map<UUIDValue, Common::Shared<Assets::Asset>> mAssetMap = {};
		std::unordered_map<UUIDValue, Common::Shared<Assets::AssetMetadata>> mMetadataMap = {};
		std::filesystem::path mProjectPath;

		void RecursiveRegisterAssets(const std::filesystem::path& folder);

		static Common::Shared<Assets::AssetMetadata> RecursiveFindMetadataByUUID(const std::filesystem::path& folder, UUIDValue uuid);
		Common::Shared<Assets::AssetMetadata> LoadMetadata(const std::filesystem::path& metadataPath);

		// Attempts to find an asset's metadata file with the given UUID
		inline std::filesystem::path FindMetadataFile(UUIDValue uuid) {
			auto metaRef = FetchMetadata(uuid);
			if (auto meta = metaRef.lock()) return meta->GetPath();
			return "";
		}
	};
}
