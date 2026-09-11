#pragma once

#include <Math/Vector.h>
#include <Math/Matrix.h>
#include <Math/Rect.h>
#include <Math/Frustum.h>

#include "AObject.h"

namespace Refraction::Objects {
	class Camera : public AObject {
	public:
		static Common::Shared<Camera> ActiveCamera;
		Math::Frustum mFrustum;

		Camera();

		void ProcessInput(Math::Vector3 dirInput, const Math::Vector3 &angInput);
		[[nodiscard]] Math::Matrix4 GetViewMatrix() const { return Math::Matrix4::LookAt(mTransform.GetWorldPosition(), mCameraTarget, mTransform.GetUpVector()); };

		nlohmann::json Serialise() override;
		void Deserialise(std::string serialised) override;

		std::string GetSerialisedType() override { return "Camera"; }
	private:
		Math::Vector3 mCameraTarget = Math::Vector3::Front();
		float mPitch = 0, mYaw = 0;
	};

	RFCT_OBJECT_REGISTERFACTORY(Camera)
}
