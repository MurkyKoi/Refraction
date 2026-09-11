#include <Core/Utilities.h>
#include <Math/Quaternion.h>

#include "Transform.h"

namespace RUtil = Refraction::Utilities;

namespace Refraction::Math {
	void SpatialPosition::Translate(const Vector3& delta) {
		CellPosition += delta;
		while (CellPosition.x < 0) {
			GridIndex.x -= 1;
			CellPosition.x += SpatialCellSize;
		}
		while (CellPosition.x > SpatialCellSize) {
			GridIndex.x += 1;
			CellPosition.x -= SpatialCellSize;
		}
		while (CellPosition.y < 0) {
			GridIndex.y -= 1;
			CellPosition.y += SpatialCellSize;
		}
		while (CellPosition.y > SpatialCellSize) {
			GridIndex.y += 1;
			CellPosition.y -= SpatialCellSize;
		}
		while (CellPosition.z < 0) {
			GridIndex.z -= 1;
			CellPosition.z += SpatialCellSize;
		}
		while (CellPosition.z > SpatialCellSize) {
			GridIndex.z += 1;
			CellPosition.z -= SpatialCellSize;
		}
	}


	Transform::Transform() {
		mSpatialPosition.GridIndex = Vector3(0);
		mSpatialPosition.CellPosition = Vector3(0.0f);
		mOrientation = Quaternion();
		mScale = Vector3(1.0f);
	}

	Transform::Transform(const Vector3& pos) : Transform() {
		Translate(pos);
	}

	Transform Transform::FromLookAt(const Vector3& eye, const Vector3& target, const Vector3& targetUp) {
		Transform result(eye);
		result.LookAt(target, targetUp);
		return result;
	}

	Transform Transform::FromMatrix(Matrix4& mat) {
		Transform result(mat.GetTranslation());
		result.mOrientation = Quaternion(mat.ToQuaternion());
		result.mScale = mat.GetScale();
		return result;
	}

	void Transform::Rotate(const float angle, const Vector3& axis) {
		mOrientation *= Quaternion::FromAxisAngle(angle, axis);
	}

	void Transform::Rotate(const Vector3& delta) {
		mOrientation *= Quaternion::FromEulerAngles(delta);
	}

	void Transform::Rotate(const Quaternion& delta) {
		mOrientation *= delta;
	}

	void Transform::Scale(const Vector3& delta) {
		mScale += delta;
	}

	void Transform::LookAt(const Vector3& target, const Vector3& targetUp) {
		Rotate(Quaternion::LookAt(GetWorldPosition(), target, targetUp).ToEulerAngles());
	}

	Matrix4 Transform::ToMatrix() const {
		auto scale = Matrix4::FromScale(mScale);
		const auto rotation = Matrix4::FromRotation(mOrientation);
		const auto translation = Matrix4::FromTranslation(GetWorldPosition());
		return scale * rotation * translation;
	}
}
