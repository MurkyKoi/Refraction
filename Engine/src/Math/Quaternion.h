#pragma once

#include <string>

#include <Math/Common.h>
#include <Math/Vector.h>

namespace Refraction::Math {
	class Matrix3;
	class Matrix4;

	class Quaternion {
	public:
		float x;
		float y;
		float z;
		float w;

		// Creates a Quaternion by directly setting the components
		Quaternion(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w) {}
		// Creates an identity Quaternion
		Quaternion() : x(0), y(0), z(0), w(1) {}

		// Creates a Quaternion from an angle (degrees) and an axis
		static Quaternion FromAxisAngle(float a, const Vector3& vec);
		// Creates a Quaternion from a rotational Vector3 (degrees)
		static Quaternion FromEulerAngles(const Vector3& vec);
		// Creates a Quaternion from a Matrix3
		static Quaternion FromMatrix3(Matrix3 mat);
		// Creates a Quaternion from a Matrix4
		static Quaternion FromMatrix4(Matrix4 mat);
		// Creates a Quaternion using the angular difference between two vectors
		static Quaternion RotationBetweenEulerAngles(Vector3 start, Vector3 end);
		// Creates a Quaternion using a given direction
		static Quaternion LookIn(const Vector3& direction, const Vector3& up = Vector3::Up());
		// Creates a Quaternion using a given eye and target
		static Quaternion LookAt(const Vector3& from, const Vector3& at, const Vector3& up = Vector3::Up());

		static bool AreSimilar(const Quaternion& q1, const Quaternion& q2) {
			return fabs(q1.Dot(q2) - 1.0) < 0.001;
		}

		Quaternion(const Quaternion& other) {
			x = other.x;
			y = other.y;
			z = other.z;
			w = other.w;
		}
		Quaternion& operator=(const Quaternion& other) {
			if (this != &other) {
				x = other.x;
				y = other.y;
				z = other.z;
				w = other.w;
			}
			return *this;
		}

		explicit operator Vector4() const {
			return {x, y, z, w};
		}

		Quaternion operator-() const {
			return {-x, -y, -z, -w};
		}
		Quaternion operator+(const Quaternion& other) const {
			return {x + other.x, y + other.y, z + other.z, w + other.w};
		}
		Quaternion operator-(const Quaternion& other) const {
			return {x - other.x, y - other.y, z - other.z, w - other.w};
		}
		Quaternion operator*(const Quaternion& other) const {
			const Vector3 thisVec(x, y, z);
			const Vector3 otherVec(other.x, other.y, other.z);

			const Vector3 resultVec = otherVec * w + thisVec * other.w + thisVec.Cross(otherVec);
			const float resultW = w * other.w - thisVec.Dot(otherVec);
			return {resultVec.x, resultVec.y, resultVec.z, resultW};
		};
		Quaternion operator*(const float& n) const {
			return {x * n, y * n, z * n, w * n};
		};
		Quaternion operator/(const float& n) const {
			return {x / n, y / n, z / n, w / n};
		}

		void operator+=(const Quaternion& other) {
			x += other.x;
			y += other.y;
			z += other.z;
			w += other.w;
		}
		void operator-=(const Quaternion& other) {
			x -= other.x;
			y -= other.y;
			z -= other.z;
			w -= other.w;
		}

		void operator*=(const Quaternion& other) {
			const Vector3 thisVec(x, y, z);
			const Vector3 otherVec(other.x, other.y, other.z);

			const Vector3 resultVec = otherVec * w + thisVec * other.w + thisVec.Cross(otherVec);
			x = resultVec.x;
			y = resultVec.y;
			z = resultVec.z;
			w = w * other.w - thisVec.Dot(otherVec);
		}
		void operator*=(const float& n) {
			x *= n;
			y *= n;
			z *= n;
			w *= n;
		}
		Vector3 operator*(const Vector3& v) const;

		bool operator==(const Quaternion& v) const {
			return AreSimilar(*this, v);
		}

		void Normalize();

		// [[nodiscard]] Vector3 Forward() const {
		// 	return *this * Vector3(0.0f, 0.0f, 1.0f);
		// }
		// [[nodiscard]] Vector3 Up() const {
		// 	return *this * Vector3(0.0f, 1.0f, 0.0f);
		// }
		// [[nodiscard]] Vector3 Right() const {
		// 	return *this * Vector3(1.0f, 0.0f, 0.0f);
		// }
		[[nodiscard]] Vector3 Forward() const {
			return {
				2.0f * (x * z + w * y),
				2.0f * (y * z - w * x),
				1.0f - 2.0f * (x * x + y * y)
			};
		}
		[[nodiscard]] Vector3 Up() const {
			return {
				2.0f * (x * y - w * z),
				1.0f - 2.0f * (x * x + z * z),
				2.0f * (y * z + w * x)
			};
		}
		[[nodiscard]] Vector3 Right() const {
			return {
				1.0f - 2.0f * (y * y + z * z),
				2.0f * (x * y + w * z),
				2.0f * (x * z - w * y)
			};
		}

		[[nodiscard]] Vector3 ToEulerAngles() const;
		[[nodiscard]] bool IsZero() const;
		[[nodiscard]] float Dot(const Quaternion& other) const;

		[[nodiscard]] Quaternion SLerp(Quaternion other, float time) const;
		[[nodiscard]] Quaternion NLerp(Quaternion other, float time) const;

		[[nodiscard]] std::string ToString(const PrintFormatArgs fmtArgs = PrintFormatArgs()) const {
			const std::string xStr = fmtArgs.AsInt ? std::to_string(static_cast<int>(x)) : std::to_string(x);
			const std::string yStr = fmtArgs.AsInt ? std::to_string(static_cast<int>(y)) : std::to_string(y);
			const std::string zStr = fmtArgs.AsInt ? std::to_string(static_cast<int>(z)) : std::to_string(z);
			const std::string wStr = fmtArgs.AsInt ? std::to_string(static_cast<int>(w)) : std::to_string(w);
			if (fmtArgs.Pretty) {
				return std::string("x: " + xStr + "\ny: " + yStr + "\nz: " + zStr + "\nw: " + wStr);
			}
			return std::string("{" + xStr + ", " + yStr + ", " + zStr + ", " + wStr + "}");
		}

		[[nodiscard]] Quaternion Conjugate() const { return {-x, -y, -z, w}; }

		[[nodiscard]] Quaternion Normalized() const {
			auto copy = Quaternion(x, y, z, w);
			copy.Normalize();
			return copy;
		}

		[[nodiscard]] Quaternion Inverse() const {
			if (IsZero()) return *this;
			return Conjugate() / Dot(*this);
		}

		void ToAxisAngle(float& angle, Vector3& axis) const {
			const float cosHalfAngle = w;
			const float halfAngle = acosf(cosHalfAngle);
			const float sinHalfAngle = sinf(halfAngle);
			angle = halfAngle * 2;

			axis.x = x / sinHalfAngle;
			axis.y = y / sinHalfAngle;
			axis.z = z / sinHalfAngle;
		};

		void ResetNANs() {
			if (IsNaN(x)) x = 0;
			if (IsNaN(y)) y = 0;
			if (IsNaN(z)) z = 0;
			if (IsNaN(w)) w = 0;
		}
	};
	Vector3 operator*(const Vector3& v, const Quaternion& q);
}
