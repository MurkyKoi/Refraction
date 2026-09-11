#include <json.hpp>

#include <Settings.h>
#include <Classes/ClassSerialiser.h>

#include "Camera.h"

namespace Refraction::Objects {
	Common::Shared<Camera> Camera::ActiveCamera = nullptr;
	static auto defaultFrustum = Math::Frustum(Math::Vector2(128), 70.0f, 0.001f, 10000.0f);

	Camera::Camera() : mFrustum(defaultFrustum) {
		mInstanceName = "Camera";
	}

	void Camera::ProcessInput(Math::Vector3 dirInput, const Math::Vector3 &angInput) {

		// Normalize inputs
		if (dirInput.Magnitude() > 0) dirInput.Normalise();

		const auto& cameraSensitivity = Settings::CurrentSettings->Controls.CameraSensitivity;
		const auto& cameraSpeed = Settings::CurrentSettings->Controls.CameraSpeed;

		// Move camera
		auto translateDelta = Math::Vector3();
		if (abs(dirInput.z) > 0.0f) translateDelta += mTransform.GetForwardVector() * (dirInput.z * cameraSpeed);
		if (abs(dirInput.y) > 0.0f) translateDelta += mTransform.GetUpVector() * (dirInput.y * cameraSpeed);
		if (abs(dirInput.x) > 0.0f) translateDelta -= mTransform.GetRightVector() * (dirInput.x * cameraSpeed);
		mTransform.Translate(translateDelta);

		// Rotate camera
		mPitch -= angInput.x * cameraSensitivity;
		mYaw -= angInput.y * cameraSensitivity;

		if (mPitch > 89.0f) mPitch = 89.0f;
		if (mPitch < -89.0f) mPitch = -89.0f;

		const auto qAbsYaw = Math::Quaternion::FromAxisAngle(mYaw, Math::Vector3::Up());
		const auto qAbsPitch = Math::Quaternion::FromAxisAngle(mPitch, Math::Vector3::Right());
		mTransform.mOrientation = qAbsYaw * qAbsPitch;
		mTransform.mOrientation.Normalize();

		mCameraTarget = mTransform.GetWorldPosition() + mTransform.GetForwardVector();
	}

	nlohmann::json Camera::Serialise() {
		return Utilities::ClassSerialiser::AppendJSON(AObject::Serialise(), [&](nlohmann::json& json) {
			json["Frustum"] = Utilities::ClassSerialiser::Serialise(mFrustum);
		});
	}
	void Camera::Deserialise(const std::string serialised) {
		AObject::Deserialise(serialised);
		Utilities::ClassSerialiser::TryParseJSON(serialised, [&](nlohmann::json& json) {
			mFrustum = Utilities::ClassSerialiser::DeserialiseFrustum(json.at("Frustum"));
		});
	}
}