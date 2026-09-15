#include <Platform/OpenGL/OpenGLMeshFragment.h>
#include <Platform/ARenderingAPI.h>

#include "AMeshFragment.h"

namespace Refraction::Engine::Platform {
	Common::Shared<AMeshFragment> AMeshFragment::MakeMeshFragment(const std::vector<sVertex>& vertices, const std::vector<unsigned int>& indices, const Common::Ref<Assets::Material>& material) {
		switch (ARenderingAPI::GetAPI()) {
			case RenderingAPI::NONE: default:
				Log::Render.Warn("Attempt to create mesh fragment without an active API");
				return nullptr;
			case RenderingAPI::OPENGL:
				auto fragment = Common::NewShared<OpenGLMeshFragment>(vertices, indices, material);
				fragment->Upload();
				return fragment;
		}
	}

	AMeshFragment::AMeshFragment(const std::vector<sVertex>& vertices, const std::vector<unsigned int>& indices, const Common::Ref<Assets::Material>& material) {
		mVertices = vertices;
		mIndices = indices;
		mMaterial = material;
	}

	AMeshFragment::~AMeshFragment() = default;
}
