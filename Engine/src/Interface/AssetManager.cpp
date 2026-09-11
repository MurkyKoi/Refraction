#include <Classes/ClassSerialiser.h>

#include "AssetManager.h"

namespace Refraction::Engine {
	void AssetManager::RegisterAllAssets() {
		Log::Project.Info("Registering all assets in project...");
		RecursiveRegisterAssets(mProjectPath / "Assets");
	}

	Common::Ref<Assets::Asset> AssetManager::RegisterAsset(const std::filesystem::path& metadataPath) {
		const auto pathStr = metadataPath.string();
		if (!(std::filesystem::exists(metadataPath) && std::filesystem::is_regular_file(metadataPath) && metadataPath.has_extension() && metadataPath.extension() == RFCT_ASSET_METADATA_EXTENSION)) {
			Log::Project.Error("Invalid metadata file at path " + pathStr);
			return {};
		}

		const auto meta = LoadMetadata(metadataPath);
		auto asset = Utilities::ClassSerialiser::DeserialiseAsset(meta);
		if (const auto& uuid = meta->AssetUUID; !mAssetMap.contains(uuid)) {
			mAssetMap[uuid] = asset;
			return asset;
		} else {
			Log::Project.Warn("Asset with UUID " + uuid.AsString() + " already exists, returning existing asset");
			return mAssetMap.at(uuid);
		}
	}

	void AssetManager::UnloadAll() {
		int volatileCount = 0;
		for (const auto &asset: mAssetMap | std::views::values) {
			if (asset->IsVolatile()) volatileCount++;
		}
		Log::Project.Info("Unloading {} assets ({} volatile)", mAssetMap.size(), volatileCount);
		Log::Project.Info("Unloading {} metadata files", mMetadataMap.size());

		for (auto it = mAssetMap.begin(); it != mAssetMap.end(); ) {
			auto& [uuid, asset] = *it;
			asset.reset();
			it = mAssetMap.erase(it);
		}

		for (auto it = mMetadataMap.begin(); it != mMetadataMap.end(); ) {
			auto& [uuid, meta] = *it;
			meta.reset();
			it = mMetadataMap.erase(it);
		}
	}

	bool AssetManager::IsValidAsset(const std::filesystem::path &assetPath) const {
		if (const auto metaPathOpt = GetMetadataPath(assetPath); !metaPathOpt) return false;
		return true;
	}

	std::optional<std::filesystem::path> AssetManager::GetMetadataPath(std::filesystem::path assetPath) const {
		const auto assetName = assetPath.filename().string();
		// Make sure it's a full path before searching it
		if (assetPath.string().find(mProjectPath.string()) == std::string::npos) assetPath = mProjectPath / "Assets" / assetPath;
		for (const auto files = FileHandling::GetFilesOfExtInFolder(assetPath.parent_path(), RFCT_ASSET_METADATA_EXTENSION); auto& file : files) {
			if (file.path().filename().string() == assetName) return std::make_optional(file.path());
		}
		return std::nullopt;
	}

	void AssetManager::RecursiveRegisterAssets(const std::filesystem::path& folder) {
		for (const auto metaFiles = FileHandling::GetFilesOfExtInFolder(folder, RFCT_ASSET_METADATA_EXTENSION); auto& metaFile : metaFiles) {
			RegisterAsset(metaFile);
		}
		for (const auto folders = FileHandling::GetFoldersInFolder(folder); auto& child : folders) {
			RecursiveRegisterAssets(child);
		}
	}

	Common::Shared<Assets::AssetMetadata> AssetManager::RecursiveFindMetadataByUUID(const std::filesystem::path& folder, const UUIDValue uuid) {
		for (const auto metaFiles = FileHandling::GetFilesOfExtInFolder(folder, RFCT_ASSET_METADATA_EXTENSION); auto& metaFile : metaFiles) {
			auto dataStr = FileHandling::ReadFile(metaFile);
			auto meta = Assets::AssetMetadata::CastedDeserialise(dataStr);
			// Return result
			// TODO: Optimise by only doing CastedDeserialise after testing UUID
			if (meta->AssetUUID.AsInt() == uuid) return meta;
		}
		// Continue searching
		for (const auto folders = FileHandling::GetFoldersInFolder(folder); auto& child : folders) {
			RecursiveFindMetadataByUUID(child, uuid);
		}
		return nullptr;
	}

	Common::Shared<Assets::AssetMetadata> AssetManager::LoadMetadata(const std::filesystem::path& metadataPath) {
		if (!std::filesystem::exists(metadataPath)) {
			Log::Project.Error("Could not find metadata file at path " + metadataPath.string());
			return nullptr;
		}
		const auto dataStr = FileHandling::ReadFile(metadataPath);

		// TODO: Optimise by checking if UUID already exists in map before doing CastedDeserialise
		auto meta = Assets::AssetMetadata::CastedDeserialise(dataStr);

		// Save in memory
		mMetadataMap[meta->AssetUUID] = meta;
		return meta;
	}
}
