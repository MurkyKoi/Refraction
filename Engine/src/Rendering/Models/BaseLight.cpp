#include <Interface/AssetManager.h>

#include "BaseLight.h"

namespace RMath = Refraction::Math;
using RMath::Transform;

BaseLight::BaseLight() {
	mTransform = new Transform();
}

void BaseLight::UpdateShaderUniforms(unsigned int index) {
	if (mLightShader.expired()) {
		Refraction::Engine::AssetManager::Try([&](Refraction::Common::Shared<Refraction::Engine::AssetManager> manager) {
			mLightShader = manager->GetAsset<Refraction::Assets::Shader>("gbufferShader");
		});
		if (mLightShader.expired()) throw Refraction::Common::RuntimeError("Failed to update light shader uniforms, could not find shader");
	}
	auto shader = mLightShader.lock();

	const float linear = 0.8f;
	const float quadratic = 0.4f;
	const float radius = 200.0f;
	//shader->SetUniformVec3("lights[" + std::to_string(index) + "].Position", mTransform->GetWorldPosition());
	//shader->SetUniformVec3("lights[" + std::to_string(index) + "].Color", mLightColor);
	//shader->SetUniformFloat("lights[" + std::to_string(index) + "].Linear", linear);
	//shader->SetUniformFloat("lights[" + std::to_string(index) + "].Quadratic", quadratic);
	//shader->SetUniformFloat("lights[" + std::to_string(index) + "].Radius", radius);
}