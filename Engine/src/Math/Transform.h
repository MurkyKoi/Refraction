#pragma once

#include <Math/Common.h>
#include <Math/Vector.h>
#include <Math/Quaternion.h>
#include <Math/Matrix.h>

namespace Refraction::Math {
	constexpr int SpatialCellSize = 128;
	struct SpatialPosition {
		Vector3 GridIndex = Vector3::Zero();
		Vector3 CellPosition = Vector3::Zero();

		[[nodiscard]] Vector3 ToWorld() const { return CellPosition + (GridIndex * SpatialCellSize); }
		void Translate(const Vector3& delta);
	};

	class Transform {
	public:
		SpatialPosition mSpatialPosition;
		Quaternion mOrientation;
		Vector3 mScale;

		Transform();

		explicit Transform(const Vector3& pos);

		// Creates a Transform looking at a target
		static Transform FromLookAt(const Vector3& eye, const Vector3& target, const Vector3& targetUp = Vector3::Up());
		// Creates a Transform from a Matrix4
		static Transform FromMatrix(Matrix4& mat);

		void Translate(const Vector3 &delta) { mSpatialPosition.Translate(delta); }
		// Rotate using an angle (degrees) axis
		void Rotate(float angle, const Vector3 &axis);
		// Rotate using Euler angles (degrees)
		void Rotate(const Vector3 &delta);
		// Rotate using a Quaternion
		void Rotate(const Quaternion& delta);
		void Scale(const Vector3 &delta);
		// Rotates the Transform to look at a target
		void LookAt(const Vector3& target, const Vector3& targetUp = Vector3::Up());

		// Generates the Transform's matrix
		[[nodiscard]] Matrix4 ToMatrix() const;
		[[nodiscard]] Vector3 GetWorldPosition() const { return mSpatialPosition.ToWorld(); }
		[[nodiscard]] Vector3 GetForwardVector() const { return mOrientation.Forward(); }
		[[nodiscard]] Vector3 GetRightVector() const { return mOrientation.Right(); }
		[[nodiscard]] Vector3 GetUpVector() const { return mOrientation.Up(); }

		[[nodiscard]] std::string ToString(PrintFormatArgs fmtArgs = PrintFormatArgs()) const {
			const auto gridIndexStr = mSpatialPosition.GridIndex.ToString({ .AsInt = true, .Pretty = fmtArgs.Pretty });
			const auto cellPosStr = mSpatialPosition.CellPosition.ToString({ .AsInt = fmtArgs.AsInt, .Pretty = fmtArgs.Pretty });
			const auto orientationStr = mOrientation.ToString({ .AsInt = fmtArgs.AsInt, .Pretty = fmtArgs.Pretty });
			const auto scaleStr = mScale.ToString({ .AsInt = fmtArgs.AsInt, .Pretty = fmtArgs.Pretty });
			if (fmtArgs.Pretty) {
				return std::string("Position: [GridIndex: " + gridIndexStr + ", CellPosition: " + cellPosStr + "]\nOrientation: " + orientationStr + "\nScale: " + scaleStr);
			}
			return std::string("{[" + gridIndexStr + ", " + cellPosStr + "], " + orientationStr + ", " + scaleStr + "}");
		}
	};
}