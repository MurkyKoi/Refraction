#include <Interface/AssetManager.h>

#include "Material.h"

namespace Refraction::Assets {
	Material::Material() {
		Engine::AssetManager::Try([&](const Common::Shared<Engine::AssetManager>& assetManager) {
			mDiffuse = assetManager->GetAsset<Image>("Basic.png");
			mSpecular = assetManager->GetAsset<Image>("Basic.png");
			mShader = assetManager->GetAsset<Shader>("gbufferShader");
		});
	}

	void Material::Activate() const {
		if (const auto img = mDiffuse.lock()) {
			if (const auto tex = img->mTexture.lock()) {
				tex->Activate(0);
			}
		}
		if (const auto img = mSpecular.lock()) {
			if (const auto tex = img->mTexture.lock()) {
				tex->Activate(1);
			}
		}
		//mNormal->Activate(2);

		if (const auto shader = mShader.lock()) {
			shader->Activate();
			shader->SetUniformInt(RFCT_TEXTURE_TYPE_DIFFUSE, 0);
			shader->SetUniformInt(RFCT_TEXTURE_TYPE_SPECULAR, 1);
		}
	}
}
