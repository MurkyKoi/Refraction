#pragma once

#include "Asset.h"
#include "Image.h"
#include "Shader.h"

namespace Refraction::Assets {
	class Material : public Engine::ISerialisable<Material, Asset> {
	public:
		static constexpr std::string SerialisedTypeName = "MaterialAsset";

		Common::Ref<Shader> mShader;
		Common::Ref<Image> mDiffuse;
		Common::Ref<Image> mSpecular;
		Common::Ref<Image> mNormal;

		Material();

		void Activate();
	};

	RFCT_ASSET_REGISTERFACTORY(Material, Asset)
}