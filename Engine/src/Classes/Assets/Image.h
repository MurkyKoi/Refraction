#pragma once

#include <string>

#include <Classes/ClassFactory.h>
#include <Platform/ATexture.h>

#include "Asset.h"

constexpr auto RFCT_TEXTURE_TYPE_DIFFUSE = "tDiffuse";
constexpr auto RFCT_TEXTURE_TYPE_SPECULAR = "tSpecular";

namespace Refraction::Assets {
	struct ImageMetadata : public AssetMetadata {
		int Width = 0;
		int Height = 0;
		int Channels = 3;
		std::string Type = RFCT_TEXTURE_TYPE_DIFFUSE;

		ImageMetadata() = default;
		~ImageMetadata() override = default;

		nlohmann::json Serialise() override;
		void Deserialise(std::string data) override;
	};

	class Image : public Engine::ISerialisable<Image, Asset> {
	public:
		static constexpr std::string SerialisedTypeName = "ImageAsset";

		Common::Ref<Engine::Platform::ATexture> mTexture = {};

		Image() = default;
		~Image() override;

		MetadataType GetMetadataType() override { return MetadataType::Image; }
	protected:
		void OnLoadAsset(Common::Shared<AssetMetadata> metadata) override;
	private:
		unsigned int mID = 0;
	};

	RFCT_ASSET_REGISTERFACTORY(Image, Asset)
}
