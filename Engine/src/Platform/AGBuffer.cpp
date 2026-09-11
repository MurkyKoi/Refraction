#include <Platform/OpenGL/OpenGLGBuffer.h>
#include <Platform/ARenderingAPI.h>
#include <Interface/AssetManager.h>

#include "AGBuffer.h"

namespace Refraction::Engine::Platform {
	Common::Shared<AGBuffer> AGBuffer::CreateGBuffer() {
		switch (ARenderingAPI::GetAPI()) {
		case RenderingAPI::NONE: default:
			Log::Render.Warn("Attempt to create GBuffer without an active API");
			return nullptr;
		case RenderingAPI::OPENGL:
			return Common::NewShared<OpenGLGBuffer>();
		case RenderingAPI::VULKAN:
			Log::Render.Warn("Attempt to create GBuffer with Vulkan. Not implemented yet.");
			return nullptr;
		}
	}

	Common::Ref<Assets::Image> AGBuffer::GetLastRenderedFrame() {
		Common::Ref<Assets::Image> imgWeak;
		AssetManager::Try([&](const Common::Shared<AssetManager>& manager) {
			if (!mFinalImageUUID) {
				// Generate a new image
				imgWeak = manager->MakeVolatile<Assets::Image>();
				if (const auto img = imgWeak.lock()) {
					mFinalImageUUID = img->GetUUID();
				}
			} else {
				// Try grabbing the image
				imgWeak = manager->GetAsset<Assets::Image>(mFinalImageUUID);
				if (imgWeak.expired()) {
					// Try regenerating the image
					imgWeak = manager->MakeVolatile<Assets::Image>();
					if (const auto img = imgWeak.lock()) {
						mFinalImageUUID = img->GetUUID();
					}
				}
			}
		});
		if (auto img = imgWeak.lock()) {
			img->mTexture = mFinal;
			return img;
		}
		return {};
	}

	AGBuffer::AGBuffer() = default;

	AGBuffer::~AGBuffer() = default;
}
