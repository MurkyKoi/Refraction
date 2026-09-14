#include <json.hpp>

#include <Core/Common.h>
#include <Core/FileHandling.h>
#include <Classes/ClassSerialiser.h>
#include <Interface/AssetManager.h>

#include "Mesh.h"

namespace Refraction::Components {
	int Mesh::FrameMeshCount = 0;
	int Mesh::FrameVertexCount = 0;

	Mesh::Mesh() {
		mClassName = "MeshComponent";
		mTransform = Math::Transform();

		Engine::AssetManager::Try([&](const Common::Shared<Engine::AssetManager>& manager) {
			mShader = manager->GetAsset<Assets::Shader>("gbufferShader");
		});
	}

	void Mesh::Render() {
		if (mModel.expired()) return;
		auto model = mModel.lock();
		const auto fragments = model->mFragments;
		model.reset();

		if (mShader.expired()) {
			Engine::AssetManager::Try([&](const Common::Shared<Engine::AssetManager>& manager) {
				mShader = manager->GetAsset<Assets::Shader>("gbufferShader");
			});
			if (mShader.expired()) throw Common::RuntimeError("Failed to render mesh, could not find shader");
		}

		const auto shader = mShader.lock();
		shader->Activate();
		shader->SetUniformMat4("modelTransform", mTransform.ToMatrix() * mParent->GetWorldMatrix());

		switch (mCullMode) {
			default: case MeshFaceCullMode::NONE:
				glDisable(GL_CULL_FACE);
				break;
			case MeshFaceCullMode::FRONT:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				break;
			case MeshFaceCullMode::BACK:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				break;
			case MeshFaceCullMode::FRONT_AND_BACK:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT_AND_BACK);
				break;
		}
		for (auto& mesh : fragments) {
			mesh->Draw();
			FrameVertexCount += static_cast<int>(mesh->mVertices.size());
			FrameMeshCount++;
		}
	}

	nlohmann::json Mesh::Serialise() {
		return Utilities::ClassSerialiser::AppendJSON(AComponent::Serialise(), [&](nlohmann::json& json) {
			json["Transform"] = Utilities::ClassSerialiser::Serialise(mTransform);
			if (const auto model = mModel.lock()) {
				json["ModelUUID"] = model->GetUUID();
			}
		});
	}

	void Mesh::Deserialise(const std::string serialised) {
		AComponent::Deserialise(serialised);
		Utilities::ClassSerialiser::TryParseJSON(serialised, [&](nlohmann::json& json) {
			mTransform = Utilities::ClassSerialiser::DeserialiseTransform(json.at("Transform"));
			Engine::AssetManager::Try([&](const Common::Shared<Engine::AssetManager>& manager) {
				if(json.contains("ModelUUID")) mModel = manager->GetAsset<Assets::Model>(json.at("ModelUUID").get<UUIDValue>());
			});
		});
	}
}
