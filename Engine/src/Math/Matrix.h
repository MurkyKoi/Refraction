#pragma once

#include <string>

#include <Math/Common.h>
#include <Math/Vector.h>
#include <Math/Quaternion.h>
#include <Math/Frustum.h>

namespace Refraction::Math {
	class Matrix3 {
		static constexpr uint8_t Size = 3;
	public:
		Vector3 m[Size] = {};

		Matrix3() {
			for (uint8_t i = 0; i < Size; i++) {
				for (uint8_t j = 0; j < Size; j++) {
					if (i == j) m[i][j] = 1;
				}
			}
		}

		explicit Matrix3(const float init) {
			for (uint8_t i = 0; i < Size; i++) {
				for (uint8_t j = 0; j < Size; j++) {
					if (i == j) m[i][j] = init;
				}
			}
		}

		Matrix3& operator=(const Matrix3& other) {
			if (this != &other) {
				for (uint8_t i = 0; i < Size; i++) {
					m[i] = other.m[i];
				}
			}
			return *this;
		}

		Vector3& operator[](uint8_t i) {
			return m[i];
		}

		Matrix3 operator*(Matrix3 other) {
			Matrix3 result;

			for (uint8_t i = 0; i < Size; i++) {
				for (uint8_t j = 0; j < Size; j++) {
					result[i][j] = 0;

					for (uint8_t k = 0; k < Size; k++) {
						result[i][j] += m[i][k] * other[k][j];
					}
				}
			}
			return result;
		}
		void operator*=(Matrix3 other) {
			for (auto & i : m) {
				for (uint8_t j = 0; j < Size; j++) {
					for (uint8_t k = 0; k < Size; k++) {
						i[j] += i[k] * other[k][j];
					}
				}
			}
		}

		Matrix3 Transpose() {
			Matrix3 result;

			for (uint8_t i = 0; i < Size; i++) {
				for (uint8_t j = 0; j < Size; j++) {
					result[i][j] = m[j][i];
				}
			}
			return result;
		}

		// Convert the matrix to a rotational Vector3 (degrees)
		Vector3 ToEulerAngles();
		Quaternion ToQuaternion();

		std::string ToString(const PrintFormatArgs fmtArgs = PrintFormatArgs()) {
			std::string out;
			for (uint8_t row = 0; row < Size; row++) {
				if (!fmtArgs.Pretty && row > 0) out += ", ";
				out += "{ ";
				for (uint8_t col = 0; col < Size; col++) {
					if (col > 0) out += ", ";
					out += fmtArgs.AsInt ? std::to_string(static_cast<int>(m[row][col])) : std::to_string(m[row][col]);
				}
				out += fmtArgs.Pretty ? " }\n" : " }";
			}
			return out;
		}
	};

	class Matrix4 {
		static constexpr uint8_t Size = 4;
	public:
		Vector4 m[Size] = {};

		Matrix4() {
			for (uint8_t i = 0; i < Size; i++) {
				for (uint8_t j = 0; j < Size; j++) {
					if (i == j) m[i][j] = 1;
				}
			}
		}

		explicit Matrix4(const float init) {
			for (uint8_t i = 0; i < Size; i++) {
				for (uint8_t j = 0; j < Size; j++) {
					if (i == j) m[i][j] = init;
				}
			}
		}
		Matrix4(const Vector4& r0, const Vector4& r1, const Vector4& r2, const Vector4& r3) {
			m[0] = r0;
			m[1] = r1;
			m[2] = r2;
			m[3] = r3;
		}

		static Matrix4 LookAt(const Vector3& from, const Vector3& at, const Vector3& up = Vector3::Up());
		static Matrix4 Perspective(float fovY, float aspectRatio, float zNear, float zFar);
		static Matrix4 Perspective(const Frustum& frustum);
		static Matrix4 FromTranslation(const Vector3& translation);
		static Matrix4 FromRotation(const float& angle, Vector3 axis);
		static Matrix4 FromRotationX(const float& angle);
		static Matrix4 FromRotationY(const float& angle);
		static Matrix4 FromRotationZ(const float& angle);
		static Matrix4 FromRotation(const Quaternion& quat);
		static Matrix4 FromRotationZYX(const Quaternion& quat);
		static Matrix4 FromScale(const Vector3& scale);

		Matrix4& operator=(const Matrix4& other) {
			if (this != &other) {
				for (uint8_t i = 0; i < Size; i++) {
					m[i] = other.m[i];
				}
			}
			return *this;
		}

		Vector4& operator[](const uint8_t i) {
			return m[i];
		}

		Matrix4 operator*(Matrix4 other) {
			Matrix4 result;

			for (uint8_t i = 0; i < Size; i++) {
				for (uint8_t j = 0; j < Size; j++) {
					result[i][j] = 0;

					for (uint8_t k = 0; k < Size; k++) {
						result[i][j] += m[i][k] * other[k][j];
					}
				}
			}
			return result;
		}
		void operator*=(Matrix4 other) {
			for (auto & i : m) {
				for (uint8_t j = 0; j < Size; j++) {
					for (uint8_t k = 0; k < Size; k++) {
						i[j] += i[k] * other[k][j];
					}
				}
			}
		}

		Matrix4 Inverse();
		Matrix4 Transpose() {
			Matrix4 result;

			for (uint8_t i = 0; i < Size; i++) {
				for (uint8_t j = 0; j < Size; j++) {
					result[i][j] = m[j][i];
				}
			}
			return result;
		}

		// Translate matrix using a Vector3
		Matrix4 Translate(const Vector3& v);
		// Rotate using a Quaternion
		Matrix4 Rotate(const Quaternion &quat);
		Matrix4 Scale(const Vector3 &scale);

		Vector3 GetTranslation();
		Vector3 GetScale();


		// Convert the matrix to a rotational Vector3 (degrees)
		Vector3 ToEulerAngles();
		Quaternion ToQuaternion();

		std::string ToString(const PrintFormatArgs fmtArgs = PrintFormatArgs()) {
			std::string out;
			for (uint8_t row = 0; row < Size; row++) {
				if (!fmtArgs.Pretty && row > 0) out += ", ";
				out += "{ ";
				for (uint8_t col = 0; col < Size; col++) {
					if (col > 0) out += ", ";
					out += fmtArgs.AsInt ? std::to_string(static_cast<int>(m[row][col])) : std::to_string(m[row][col]);
				}
				out += fmtArgs.Pretty ? " }\n" : " }";
			}
			return out;
		}
	};
}
