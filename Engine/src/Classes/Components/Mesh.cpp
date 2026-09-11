#include <json.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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

		Engine::AssetManager::Try([&](Common::Shared<Engine::AssetManager> manager) {
			mShader = manager->GetAsset<Assets::Shader>("gbufferShader");
		});
	}

	void Mesh::Render() {
		if (mModel.expired()) return;
		auto model = mModel.lock();
		auto fragments = model->mFragments;
		model.reset(); // Free ownership

		if (mShader.expired()) {
			Engine::AssetManager::Try([&](Common::Shared<Engine::AssetManager> manager) {
				mShader = manager->GetAsset<Assets::Shader>("gbufferShader");
			});
			if (mShader.expired()) throw Common::RuntimeError("Failed to render mesh, could not find shader");
		}

		auto shader = mShader.lock();
		shader->Activate();
		shader->SetUniformMat4("modelTransform", mTransform.ToMatrix()* mParent->GetWorldMatrix());
		for (auto& mesh : fragments) {
			mesh->Draw();
			FrameVertexCount += (int)mesh->mVertices.size();
			FrameMeshCount++;
		}
	}

	nlohmann::json Mesh::Serialise() {
		return Utilities::ClassSerialiser::AppendJSON(AComponent::Serialise(), [&](nlohmann::json& json) {
			json["Transform"] = Utilities::ClassSerialiser::Serialise(mTransform);
			if (auto model = mModel.lock()) {
				json["ModelUUID"] = model->GetUUID();
			}
		});
	}

	void Mesh::Deserialise(std::string serialised) {
		AComponent::Deserialise(serialised);
		Utilities::ClassSerialiser::TryParseJSON(serialised, [&](nlohmann::json& json) {
			mTransform = Utilities::ClassSerialiser::DeserialiseTransform(json.at("Transform"));
			Engine::AssetManager::Try([&](Common::Shared<Engine::AssetManager> manager) {
				if(json.contains("ModelUUID")) mModel = manager->GetAsset<Assets::Model>(json.at("ModelUUID").get<UUIDValue>());
			});
		});
	}
}
