#pragma once

#include <Classes/Assets/Material.h>

namespace Refraction::Engine {
	struct sVertex {
		Math::Vector3 pos;
		Math::Vector3 normal;
		Math::Vector2 texCoord;
	};

	namespace Platform {
		class AMeshFragment {
		public:
			std::vector<sVertex> mVertices;
			std::vector<unsigned int> mIndices;
			Common::Ref<Assets::Material> mMaterial;

			// Returns a mesh fragment using the current rendering API
			static Common::Shared<AMeshFragment> MakeMeshFragment(const std::vector<sVertex>& vertices, const std::vector<unsigned int>& indices, const Common::Ref<Assets::Material>& material);

			AMeshFragment(const std::vector<sVertex>& vertices, const std::vector<unsigned int>& indices, const Common::Ref<Assets::Material>& material);
			virtual ~AMeshFragment();

			virtual void Upload() = 0;
			virtual void Draw() = 0;
		};
	}
}
