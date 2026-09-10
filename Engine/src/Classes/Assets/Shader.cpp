#include <Core/FileHandling.h>
#include <Core/Utilities.h>

#include "Shader.h"

namespace Refraction::Assets {
	std::unordered_map<std::string, uint64_t> Shader::LoadedShaders = {};


	nlohmann::json ShaderMetadata::Serialise() {
		auto result = AssetMetadata::Serialise();
		return result;
	}

	void ShaderMetadata::Deserialise(std::string data) {
		AssetMetadata::Deserialise(data);
	}

	Shader::~Shader() {
		if (LoadedShaders.empty()) return;
		if (glIsProgram(mID)) {
			glDeleteProgram(mID);
		}
	}

	void Shader::Activate() const {
		glUseProgram(mID);
	}

	void Shader::SetUniformBool(const std::string& name, bool value) const {
		glUniform1i(glGetUniformLocation(mID, name.c_str()), (int)value);
	}

	void Shader::SetUniformInt(const std::string& name, int value) const {
		glUniform1i(glGetUniformLocation(mID, name.c_str()), value);
	}

	void Shader::SetUniformFloat(const std::string& name, float value) const {
		glUniform1f(glGetUniformLocation(mID, name.c_str()), value);
	}

	void Shader::SetUniformVec2(const std::string& name, float x, float y) const {
		glUniform2f(glGetUniformLocation(mID, name.c_str()), x, y);
	}

	void Shader::SetUniformVec2(const std::string& name, const Math::Vector2& value) const {
		SetUniformVec2(name, value.x, value.y);
	}

	void Shader::SetUniformVec3(const std::string& name, float x, float y, float z) const {
		glUniform3f(glGetUniformLocation(mID, name.c_str()), x, y, z);
	}

	void Shader::SetUniformVec3(const std::string& name, const Math::Vector3& value) const {
		SetUniformVec3(name, value.x, value.y, value.z);
	}

	void Shader::SetUniformVec4(const std::string& name, float x, float y, float z, float w) const {
		glUniform4f(glGetUniformLocation(mID, name.c_str()), x, y, z, w);
	}

	void Shader::SetUniformMat3(const std::string& name, Refraction::Math::Matrix3 matrix) const {
		glUniformMatrix3fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
	}

	void Shader::SetUniformMat4(const std::string& name, Refraction::Math::Matrix4 matrix) const {
		glUniformMatrix4fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
	}

	void Shader::OnLoadAsset(Common::Shared<AssetMetadata> metadata) {
		auto meta = Common::AsA<ShaderMetadata>(metadata);
		if (!meta) {
			Log::SError("Metadata cast failed");
			return;
		}

		mName = meta->AssetPath.filename().string();
		Log::Render.Info("Creating shader " + mName);

		// Gather additional metadata
		if (meta->AssetPath.extension() != RFCT_ASSET_METADATA_EXTENSION) {
			meta->ProgramCount = FileHandling::GetFilesInFolder(meta->AssetPath).size();
		}

		// Get all shader files (.vert and .frag) in the folder
		auto vertShader = FileHandling::GetFirstFileOfExtInFolder(meta->AssetPath, ".vert");
		auto fragShader = FileHandling::GetFirstFileOfExtInFolder(meta->AssetPath, ".frag");

		if (!(vertShader.exists() && fragShader.exists())) {
			Log::Render.Warn("Skipping shader creation, missing source files");
			return;
		}

		std::string vertPath = vertShader.path().string();
		std::string fragPath = fragShader.path().string();

		std::string vertSource = FileHandling::ReadFile(vertPath);
		std::string fragSource = FileHandling::ReadFile(fragPath);

		const char* pVertSource = vertSource.data();
		const char* pFragSource = fragSource.data();

		unsigned int vert, frag;
		vert = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vert, 1, &pVertSource, NULL);
		glCompileShader(vert);
		if (CheckLogErrors(vert, "VERTEX")) return;
		Log::Render.Info("Compiled vertex shader");

		frag = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frag, 1, &pFragSource, NULL);
		glCompileShader(frag);
		if (CheckLogErrors(frag, "FRAGMENT")) return;
		Log::Render.Info("Compiled fragment shader");

		mID = glCreateProgram();
		glAttachShader(mID, vert);
		glAttachShader(mID, frag);
		glLinkProgram(mID);
		glDeleteShader(vert);
		glDeleteShader(frag);
		if (CheckLogErrors(mID, "PROGRAM")) return;
		Log::Render.Info("Linked shader program");

		LoadedShaders[mName] = meta->AssetUUID.AsInt();
	}

	bool Shader::CheckLogErrors(GLuint shader, const std::string type) {
		GLint success;
		GLchar log[1024];
		if (type != "PROGRAM") {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, 1024, NULL, log);
				Log::Render.Error("SHADER COMPILATION FAILED | " + type + "\n" + log + "\n--- COMPILE ERROR LOG END ---");
			}
			return (!success);
		} else {
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shader, 1024, NULL, log);
				Log::Render.Error("PROGRAM LINK FAILED | " + type + "\n" + log + "\n--- LINK ERROR LOG END ---");
			}
			return (!success);
		}
	}

}
