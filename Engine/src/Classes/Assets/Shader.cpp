#include <Core/FileHandling.h>
#include <Core/Utilities.h>

#include "Shader.h"

namespace Refraction::Assets {
	std::unordered_map<std::string, uint64_t> Shader::LoadedShaders = {};


	nlohmann::json ShaderMetadata::Serialise() {
		auto result = AssetMetadata::Serialise();
		return result;
	}

	void ShaderMetadata::Deserialise(const std::string data) {
		AssetMetadata::Deserialise(data);
	}

	bool Shader::CheckLogErrors(const GLuint shader, const std::string& type) {
		GLint success;
		GLchar log[1024];
		if (type != "PROGRAM") {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, 1024, nullptr, log);
				Log::Render.Error("SHADER COMPILATION FAILED | " + type + "\n" + log + "\n--- COMPILE ERROR LOG END ---");
			}
			return !success;
		}
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, nullptr, log);
			Log::Render.Error("PROGRAM LINK FAILED | " + type + "\n" + log + "\n--- LINK ERROR LOG END ---");
		}
		return !success;
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

	void Shader::SetUniformBool(const std::string& name, const bool value) const {
		glUniform1i(GetUniformLocation(name), static_cast<int>(value));
	}

	void Shader::SetUniformInt(const std::string& name, const int value) const {
		glUniform1i(GetUniformLocation(name), value);
	}

	void Shader::SetUniformFloat(const std::string& name, const float value) const {
		glUniform1f(GetUniformLocation(name), value);
	}

	void Shader::SetUniformVec2(const std::string& name, const float x, const float y) const {
		glUniform2f(GetUniformLocation(name), x, y);
	}

	void Shader::SetUniformVec2(const std::string& name, const Math::Vector2& value) const {
		SetUniformVec2(name, value.x, value.y);
	}

	void Shader::SetUniformVec3(const std::string& name, const float x, const float y, const float z) const {
		glUniform3f(GetUniformLocation(name), x, y, z);
	}

	void Shader::SetUniformVec3(const std::string& name, const Math::Vector3& value) const {
		SetUniformVec3(name, value.x, value.y, value.z);
	}

	void Shader::SetUniformVec4(const std::string& name, const float x, const float y, const float z, const float w) const {
		glUniform4f(GetUniformLocation(name), x, y, z, w);
	}

	void Shader::SetUniformMat3(const std::string& name, Math::Matrix3 matrix) const {
		glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
	}

	void Shader::SetUniformMat4(const std::string& name, Math::Matrix4 matrix) const {
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
	}

	void Shader::OnLoadAsset(const Common::Shared<AssetMetadata> metadata) {
		const auto meta = Common::AsA<ShaderMetadata>(metadata);
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
		const auto vertShader = FileHandling::GetFirstFileOfExtInFolder(meta->AssetPath, ".vert");
		const auto fragShader = FileHandling::GetFirstFileOfExtInFolder(meta->AssetPath, ".frag");

		if (!(vertShader.exists() && fragShader.exists())) {
			Log::Render.Warn("Skipping shader creation, missing source files");
			return;
		}

		const std::string vertPath = vertShader.path().string();
		const std::string fragPath = fragShader.path().string();

		const std::string vertSource = FileHandling::ReadFile(vertPath);
		const std::string fragSource = FileHandling::ReadFile(fragPath);

		const char* pVertSource = vertSource.data();
		const char* pFragSource = fragSource.data();

		const unsigned int vert = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vert, 1, &pVertSource, nullptr);
		glCompileShader(vert);
		if (CheckLogErrors(vert, "VERTEX")) return;
		Log::Render.Info("Compiled vertex shader");

		const unsigned int frag = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frag, 1, &pFragSource, nullptr);
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

	GLint Shader::GetUniformLocation(std::string name) const {
		const auto loc = glGetUniformLocation(mID, name.c_str());
		if (loc < 0) Log::Render.Warn("Invalid uniform name {}", name);
		return loc;
	}
}
