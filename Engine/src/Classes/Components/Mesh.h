#pragma once

#include <vector>
#include <filesystem>

#include <Math/Transform.h>
#include <Classes/Assets/Model.h>

#include "AComponent.h"

namespace Refraction::Components {
	class Mesh : public AComponent {
	public:
		Math::Transform mTransform;
		Common::Ref<Assets::Model> mModel;
		Common::Ref<Assets::Shader> mShader;

		static int FrameMeshCount;
		static int FrameVertexCount;

		Mesh();

		void Tick(float delta) override {};
		void Render() override;

		nlohmann::json Serialise() override;
		void Deserialise(std::string serialised) override;

		std::string GetSerialisedType() override { return "MeshComponent"; }
	};

	RFCT_COMPONENT_REGISTERFACTORY(Mesh)
}