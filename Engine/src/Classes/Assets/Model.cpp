
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Classes/ClassSerialiser.h>
#include <Interface/AssetManager.h>

#include "Model.h"

namespace Refraction::Assets {
	static void ProcessNode(const std::string& sourcePath, std::vector<Common::Ref<Material>>& materials, std::vector<Common::Shared<Engine::Platform::AMeshFragment>>& fragments, const aiNode* node, const aiScene* scene);
	static Common::Shared<Engine::Platform::AMeshFragment> ProcessMesh(const std::vector<Common::Ref<Material>> &materials, const aiMesh *mesh);
	static std::vector<Common::Ref<Image>> LoadMaterialTextures(const std::string &sourcePath, const aiMaterial *mat, aiTextureType type);

	void ProcessNode(const std::string& sourcePath, std::vector<Common::Ref<Material>>& materials, std::vector<Common::Shared<Engine::Platform::AMeshFragment>>& fragments, const aiNode* node, const aiScene* scene) {
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			fragments.push_back(ProcessMesh(materials, mesh));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			ProcessNode(sourcePath, materials, fragments, node->mChildren[i], scene);
		}
	}

	Common::Shared<Engine::Platform::AMeshFragment> ProcessMesh(const std::vector<Common::Ref<Material>> &materials, const aiMesh *mesh) {
		std::vector<Engine::sVertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Common::Shared<Image>> diffuseMaps;
		std::vector<Common::Shared<Image>> specularMaps;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Engine::sVertex vertex;

			const aiVector3D importPos = mesh->mVertices[i];
			vertex.pos = Math::Vector3(importPos.x, importPos.y, importPos.z);

			const aiVector3D importNormal = mesh->mNormals[i];
			vertex.normal = Math::Vector3(importNormal.x, importNormal.y, importNormal.z);

			if (mesh->HasTextureCoords(0)) {
				const aiVector3D importTexCoord = mesh->mTextureCoords[0][i];
				vertex.texCoord = Math::Vector2(importTexCoord.x, importTexCoord.y);
			} else
				vertex.texCoord = Math::Vector2(0.0f);

			vertices.push_back(vertex);
		}


		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			const aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		return Engine::Platform::AMeshFragment::MakeMeshFragment(vertices, indices, materials[mesh->mMaterialIndex]);
	}

	std::vector<Common::Ref<Image>> LoadMaterialTextures(const std::string &sourcePath, const aiMaterial *mat, const aiTextureType type) {
		std::vector<Common::Ref<Image>> textures;
		Engine::AssetManager::Try([&](const Common::Shared<Engine::AssetManager>& assetManager) {
			for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
				aiString str;
				mat->GetTexture(type, i, &str);

				std::string fullPath = sourcePath + "/" + str.C_Str();

				auto texture = assetManager->GetAsset<Image>(fullPath);
				textures.push_back(texture);
			}
		});
		return textures;
	}

	nlohmann::json ModelMetadata::Serialise() {
		return Utilities::ClassSerialiser::AppendJSON(AssetMetadata::Serialise(), [&](nlohmann::json& json) {
			json["VertexCount"] = VertexCount;
			json["PolyCount"] = PolyCount;
		});
	}

	void ModelMetadata::Deserialise(const std::string data) {
		AssetMetadata::Deserialise(data);
		Utilities::ClassSerialiser::TryParseJSON(data, [&](nlohmann::json& json) {
			if (json.contains("VertexCount")) VertexCount = json.at("VertexCount").get<int>();
			if (json.contains("PolyCount")) PolyCount = json.at("PolyCount").get<int>();
		});
	}

	void Model::OnLoadAsset(const Common::Shared<AssetMetadata> metadata) {
		const auto meta = Common::AsA<ModelMetadata>(metadata);
		if (!meta) {
			Log::SError("Metadata cast failed");
			return;
		}

		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(meta->AssetPath.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			Log::SError("MODEL LOAD FAILED | " + std::string(import.GetErrorString()));
			return;
		}

		const auto importSourcePath = meta->AssetPath.string().substr(0, meta->AssetPath.string().find_last_of('/'));

		// Create materials
		Log::Project.Info("Parsing materials at {}", importSourcePath);
		if (scene->mNumMaterials > 0) {
			for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
				const auto importMat = scene->mMaterials[i];

				Common::Ref<Material> matWeak;
				Engine::AssetManager::Try([&](const Common::Shared<Engine::AssetManager>& assetManager) {
					matWeak = assetManager->MakeVolatile<Material>();
				});
				if (matWeak.expired()) continue;
				const auto mat = matWeak.lock();

				if (auto diffuseMaps = LoadMaterialTextures(importSourcePath, importMat, aiTextureType_DIFFUSE); !diffuseMaps.empty()) mat->mDiffuse = diffuseMaps[0];
				else {
					Log::SWarn("Imported material does not associate with any diffuse textures, using default texture.");
				}
				if (auto specularMaps = LoadMaterialTextures(importSourcePath, importMat, aiTextureType_SPECULAR); !specularMaps.empty()) mat->mSpecular = specularMaps[0];
				else {
					Log::SWarn("Imported material does not associate with any specular textures, using default texture.");
				}
				mMaterials.push_back(matWeak);
			}
		} else { // Create default material
			Common::Ref<Material> matWeak;
			Engine::AssetManager::Try([&](const Common::Shared<Engine::AssetManager>& assetManager) {
				matWeak = assetManager->MakeVolatile<Material>();
			});
			mMaterials.push_back(matWeak);
		}

		// Load meshes
		Log::SInfo("Parsing mesh data...");
		ProcessNode(importSourcePath, mMaterials, mFragments, scene->mRootNode, scene);
	}

	// TODO: Save material files with mesh
	void Model::OnSave() {
		Asset::OnSave();
	}
}

