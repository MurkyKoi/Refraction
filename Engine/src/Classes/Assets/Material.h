#pragma once

#include "Asset.h"
#include "Image.h"
#include "Shader.h"

namespace Refraction::Assets {
	class Material : public Asset {
	public:
		static constexpr std::string SerialisedTypeName = "MaterialAsset";

		Common::Ref<Shader> mShader;
		Common::Ref<Image> mDiffuse;
		Common::Ref<Image> mSpecular;
		Common::Ref<Image> mNormal;

		Material();

		void Activate() const;

		std::string GetSerialisedType() override { return "MaterialAsset"; }
	};

	RFCT_ASSET_REGISTERFACTORY(Material)
}