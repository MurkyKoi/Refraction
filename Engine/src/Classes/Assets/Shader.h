#pragma once

#include <string>

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <Math/Vector.h>
#include <Math/Matrix.h>
#include "Classes/ClassFactory.h"

#include "Asset.h"

namespace Refraction::Assets {
	struct ShaderMetadata : AssetMetadata {
		size_t ProgramCount = 0;

		ShaderMetadata() = default;
		~ShaderMetadata() override = default;

		nlohmann::json Serialise() override;
		void Deserialise(std::string data) override;
	};

	class Shader : public Asset {
	public:
		Shader() = default;
		~Shader() override;

		void Activate() const;

		void SetUniformBool(const std::string& name, bool value) const;
		void SetUniformInt(const std::string& name, int value) const;
		void SetUniformFloat(const std::string& name, float value) const;
		void SetUniformVec2(const std::string& name, const Math::Vector2& value) const;
		void SetUniformVec2(const std::string& name, float x, float y) const;
		void SetUniformVec3(const std::string& name, const Math::Vector3& value) const;
		void SetUniformVec3(const std::string& name, float x, float y, float z) const;
		void SetUniformVec4(const std::string& name, float x, float y, float z, float w) const;
		void SetUniformMat3(const std::string& name, Math::Matrix3 matrix) const;
		void SetUniformMat4(const std::string& name, Math::Matrix4 matrix) const;

		[[nodiscard]] std::string GetName() const { return mName; };

		std::string GetSerialisedType() override { return "ShaderAsset"; }
		MetadataType GetMetadataType() override { return MetadataType::Shader; }
	protected:
		void OnLoadAsset(Common::Shared<AssetMetadata> metadata) override;
	private:
		static std::unordered_map<std::string, uint64_t> LoadedShaders;
		static bool CheckLogErrors(GLuint shader, const std::string& type);

		unsigned int mID = 0;
		std::string mName;

		GLint GetUniformLocation(std::string name) const;
	};

	RFCT_ASSET_REGISTERFACTORY(Shader)
}
