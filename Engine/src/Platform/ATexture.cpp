#include <Platform/ARenderingAPI.h>
#include <Platform/OpenGL/OpenGLTexture.h>

#include "ATexture.h"

namespace Refraction::Engine::Platform {
	std::map<uint64_t, Common::Shared<ATexture>> ATexture::TexturePool = {};

	Common::Ref<ATexture> ATexture::MakeTexture(const TextureStructure& texStruct) {
		switch (ARenderingAPI::GetAPI()) {
		case RenderingAPI::NONE: default:
			Log::Render.Warn("Attempt to create texture without an active API");
			return {};
		case RenderingAPI::OPENGL: {
			auto newTex = Common::NewShared<OpenGLTexture>(texStruct);
			TexturePool[newTex->mUUID.AsInt()] = newTex;
			return newTex;
		}
		case RenderingAPI::VULKAN:
			Log::Render.Warn("Attempt to create texture with Vulkan. Not implemented yet.");
			return {};
		}
	}

	Common::Ref<ATexture> ATexture::FromPath(const std::filesystem::path& path) {
		switch (ARenderingAPI::GetAPI()) {
		case RenderingAPI::NONE: default:
			Log::Render.Warn("Attempt to create texture without an active API");
			return {};
		case RenderingAPI::OPENGL: {
			auto newTex = OpenGLTexture::GetFromPath(path);
			TexturePool[newTex->mUUID.AsInt()] = newTex;
			return newTex;
		}
		case RenderingAPI::VULKAN:
			Log::Render.Warn("Attempt to create texture with Vulkan. Not implemented yet.");
			return {};
		}
	}

	Common::Ref<ATexture> ATexture::FromID(const unsigned int id) {
		switch (ARenderingAPI::GetAPI()) {
		case RenderingAPI::NONE: default:
			Log::Render.Warn("Attempt to get texture without an active API");
			return {};
		case RenderingAPI::OPENGL:
			return OpenGLTexture::GetFromID(id);	
		case RenderingAPI::VULKAN:
			Log::Render.Warn("Attempt to get texture with Vulkan. Not implemented yet.");
			return {};
		}
	}

	void ATexture::ClearTexturePool() {
		for (auto it = TexturePool.begin(); it != TexturePool.end(); ) {
			auto& [uuid, tex] = *it;
			tex.reset();
			it = TexturePool.erase(it);
		}
	}
}

