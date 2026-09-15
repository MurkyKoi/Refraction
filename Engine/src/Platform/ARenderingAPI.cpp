#include <Platform/OpenGL/OpenGLRenderingAPI.h>

#include "ARenderingAPI.h"

namespace Refraction::Engine::Platform {
	Common::Shared<ARenderingAPI> ARenderingAPI::Get() {
		switch (CurrentAPI) {
			case RenderingAPI::NONE: default:
				Log::Render.Warn("Creating a Renderer without an API");
				return nullptr;
			case RenderingAPI::OPENGL:
				Log::Render.Info("Creating a Renderer using OpenGL");
				return Common::NewShared<OpenGLRenderingAPI>();
			case RenderingAPI::VULKAN:
				Log::Render.Warn("Vulkan is unsupported, defaulting to OpenGL");
				CurrentAPI = RenderingAPI::OPENGL;
				return Get();
		}
	}

	RenderingAPI ARenderingAPI::CurrentAPI = RenderingAPI::OPENGL;
}
