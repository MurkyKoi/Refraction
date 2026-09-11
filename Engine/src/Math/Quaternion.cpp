#include "Quaternion.h"

#include <Math/Vector.h>
#include <Math/Matrix.h>

namespace Refraction::Math {
	Quaternion Quaternion::FromAxisAngle(const float a, const Vector3& vec) {
		Quaternion newQuat;

		const auto halfAngle = ToRadians(a / 2);
		const float sinHalfAngle = sinf(halfAngle);
		const float cosHalfAngle = cosf(halfAngle);

		newQuat.x = vec.x * sinHalfAngle;
		newQuat.y = vec.y * sinHalfAngle;
		newQuat.z = vec.z * sinHalfAngle;
		newQuat.w = cosHalfAngle;
		return newQuat;
	}

	Quaternion Quaternion::FromEulerAngles(const Vector3& vec) {
		const float cX = cosf(ToRadians(vec.x) * 0.5f), sX = sinf(ToRadians(vec.x) * 0.5f);
		const float cY = cosf(ToRadians(vec.y) * 0.5f), sY = sinf(ToRadians(vec.y) * 0.5f);
		const float cZ = cosf(ToRadians(vec.z) * 0.5f), sZ = sinf(ToRadians(vec.z) * 0.5f);

		return {
			sX * cY * cZ + cX * sY * sZ,
			cX * sY * cZ - sX * cY * sZ,
			cX * cY * sZ - sX * sY * cZ,
			cX * cY * cZ + sX * sY * sZ
		};
	}

	Quaternion Quaternion::FromMatrix3(Matrix3 mat) { return mat.ToQuaternion(); }
	Quaternion Quaternion::FromMatrix4(Matrix4 mat) { return mat.ToQuaternion(); }

	Quaternion Quaternion::RotationBetweenEulerAngles(Vector3 start, Vector3 end) {
		start.Normalise();
		end.Normalise();

		const auto cosTheta = start.Dot(end);
		Vector3 rotAxis;

		// Case for vectors facing opposite directions
		if (cosTheta < -1 + 0.001f) {
			rotAxis = Vector3::Front().Cross(start);

			if ((rotAxis * rotAxis).Magnitude() < 0.01) {
				rotAxis = Vector3::Right().Cross(start);
			}

			rotAxis.Normalise();
			return FromAxisAngle(180.0f, rotAxis);
		}

		rotAxis = start.Cross(end);

		const auto s = sqrtf((1 + cosTheta) * 2);
		const auto invs = 1 / s;

		return {
			rotAxis.x * invs,
			rotAxis.y * invs,
			rotAxis.z * invs,
			s * 0.5f
		};
	}

	Quaternion Quaternion::LookIn(const Vector3& direction, const Vector3& up) {
		Matrix3 result;

		result[2] = -direction.Normalised();
		const auto right = up.Cross(result[2]);
		result[0] = right * (1 / sqrtf(std::max(0.00001f, right.Dot(right))));
		result[1] = result[2].Cross(result[0]);

		// Use Matrix4 constructor
		return FromMatrix3(result);
	}

	Quaternion Quaternion::LookAt(const Vector3& from, const Vector3& at, const Vector3& up) {
		return Matrix4::LookAt(from, at, up).ToQuaternion();
	}

	bool Quaternion::IsZero() const {
		return !x && !y && !z && !w;
	}

	float Quaternion::Dot(const Quaternion& other) const {
		return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w);
	}

	Vector3 Quaternion::ToEulerAngles() const {
		Vector3 result;

		// 1. Extract Pitch (X) - Using your exact YXZ structural signature
		// The sign on the pitch component determines if looking down is positive or negative
		float sinPitch = 2.0f * (w * x - y * z);

		// Domain protection trap for asinf
		if (sinPitch > 1.0f)  sinPitch = 1.0f;
		if (sinPitch < -1.0f) sinPitch = -1.0f;

		// 2. GIMBAL LOCK POLE SAFETY: If pitching straight up/down, handle it explicitly
		if (fabsf(sinPitch) >= 0.99999f) {
			result.x = (sinPitch > 0.0f) ? 90.0f : -90.0f;

			// At 90 degrees, Yaw and Roll blend. Assign everything to Yaw and lock Roll to 0.
			// If your controls still flip here, change the minus to a plus: (y + x)
			result.y = ToDegrees(2.0f * atan2f(y - x, w));
			result.z = 0.0f;
		}
		else {
			// 3. STANDARD UNPACKING: Match your specific YXZ packing matrix signs exactly
			result.x = ToDegrees(asinf(sinPitch)); // Pitch (X)

			// Yaw (Y) Extraction
			float sinYaw = 2.0f * (w * y + z * x);
			float cosYaw = 1.0f - 2.0f * (x * x + y * y);
			result.y = ToDegrees(atan2f(sinYaw, cosYaw));

			// Roll (Z) Extraction
			float sinRoll = 2.0f * (w * z + x * y);
			float cosRoll = 1.0f - 2.0f * (x * x + z * z);
			result.z = ToDegrees(atan2f(sinRoll, cosRoll));
		}

		// Safety checks against float errors
		if (IsNaN(result.x)) result.x = 0.0f;
		if (IsNaN(result.y)) result.y = 0.0f;
		if (IsNaN(result.z)) result.z = 0.0f;

		return result;
	}

	Vector3 Quaternion::operator*(const Vector3 &v) const {
		const Vector3 quatVec(x, y, z);
		const auto t = quatVec.Cross(v) * 2.0f;

		return v + (t * w) - quatVec.Cross(t);
	}

	void Quaternion::Normalize() {
		const float nSq = x * x + y * y + z * z + w * w;
		if (nSq < 0.00001f) {
			x = 0;
			y = 0;
			z = 0;
			w = 1;
			return;
		}
		const float inv = 1.0f / sqrtf(nSq);
		w *= inv;
		x *= inv;
		y *= inv;
		z *= inv;
	}

	Quaternion Quaternion::SLerp(Quaternion other, const float time) const {
		float cosA = x * other.x + y * other.y + z * other.z + w * other.w;
		if (cosA < 0.0f) {
			cosA = -cosA;
			other = -other;
		}

		// use Lerp if close to end
		if (cosA > 0.999f) {
			return {Lerp(x, other.x, time), Lerp(y, other.y, time), Lerp(z, other.z, time), Lerp(w, other.w, time)};
		}

		const float alpha = acosf(cosA);
		Quaternion result(*this);

		result *= sinf(1.0f - time);
		other *= sinf(time * alpha);

		return (result + other) / sinf(alpha);
	}

	Quaternion Quaternion::NLerp(Quaternion other, const float time) const {
		if (const float l2 = Dot(other); l2 < 0.0f) {
			other = -other;
		}
		return {Lerp(x, other.x, time), Lerp(y, other.y, time), Lerp(z, other.z, time), Lerp(w, other.w, time)};
	}

	Vector3 operator*(const Vector3& v, const Quaternion& q) {
		return q.Inverse() * v;
	}

}
