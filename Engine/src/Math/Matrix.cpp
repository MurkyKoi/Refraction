#include "Matrix.h"

namespace Refraction::Math {
	Vector3 Matrix3::ToEulerAngles() {
		Vector3 newVec;
		const float T1 = atan2f(m[2][1], m[2][2]);
		const float C2 = sqrtf(m[0][0] * m[0][0] + m[1][0] * m[1][0]);
		const float T2 = atan2f(-m[2][0], C2);
		const float S1 = sinf(T1);
		const float C1 = cosf(T1);
		const float T3 = atan2f(S1 * m[0][2] - C1 * m[0][1], C1 * m[1][1] - S1 * m[1][2]);
		newVec.x = -T1;
		newVec.y = -T2;
		newVec.z = -T3;
		return newVec;
	}

	Quaternion Matrix3::ToQuaternion() {
		const float x = m[0][0] - m[1][1] - m[2][2];
		const float y = m[1][1] - m[0][0] - m[2][2];
		const float z = m[2][2] - m[0][0] - m[1][1];
		const float w = m[0][0] + m[1][1] + m[2][2];

		int biggestIndex = 0;
		float biggest = x;
		if (y > biggest) {
			biggest = y;
			biggestIndex = 1;
		}
		if (z > biggest) {
			biggest = z;
			biggestIndex = 2;
		}
		if (w > biggest) {
			biggest = w;
			biggestIndex = 3;
		}

		float biggestVal = sqrtf(biggest + 1.0f) * 0.5f;
		const float mult = 0.25f / biggestVal;

		switch (biggestIndex) {
		case 0:
			return {biggestVal, (m[0][1] + m[1][0]) * mult, (m[2][0] + m[0][2]) * mult, (m[1][2] - m[2][1]) * mult};
		case 1:
			return {(m[0][1] + m[1][0]) * mult, biggestVal, (m[1][2] + m[2][1]) * mult, (m[2][0] - m[0][2]) * mult};
		case 2:
			return {(m[2][0] + m[0][2]) * mult, (m[1][2] + m[2][1]) * mult, biggestVal, (m[0][1] - m[1][0]) * mult};
		case 3:
			return {(m[1][2] - m[2][1]) * mult, (m[2][0] - m[0][2]) * mult, (m[0][1] - m[1][0]) * mult, biggestVal};
		default:
			return {};
		}
	}

	Matrix4 Matrix4::LookAt(const Vector3& from, const Vector3& at, const Vector3& up) {
		const Vector3 forward = (at - from).Normalised();
		const Vector3 right = forward.Cross(up).Normalised();
		const Vector3 matrixUp = right.Cross(forward);

		Matrix4 newMat;
		newMat[0][0] = right.x;
		newMat[1][0] = right.y;
		newMat[2][0] = right.z;
		newMat[0][1] = matrixUp.x;
		newMat[1][1] = matrixUp.y;
		newMat[2][1] = matrixUp.z;
		newMat[0][2] = -forward.x;
		newMat[1][2] = -forward.y;
		newMat[2][2] = -forward.z;
		newMat[3][0] = -right.Dot(from);
		newMat[3][1] = -matrixUp.Dot(from);
		newMat[3][2] = forward.Dot(from);

		return newMat;
	}

	Matrix4 Matrix4::Perspective(float fovY, float aspectRatio, float zNear, float zFar) {
		const float tanHalfFovy = tanf(fovY / 2.0f);

		Matrix4 newMat;
		newMat[0][0] = 1.0f / (aspectRatio * tanHalfFovy);
		newMat[1][1] = 1.0f / (tanHalfFovy);
		newMat[2][2] = -(zFar + zNear) / (zFar - zNear);
		newMat[2][3] = -1.0f;
		newMat[3][2] = -(2.0f * zFar * zNear) / (zFar - zNear);
		return newMat;
	}

	Matrix4 Matrix4::Perspective(const Frustum& frustum) {
		return Perspective(frustum.fovY, frustum.AspectRatio(), frustum.zNear, frustum.zFar);
	}

	Matrix4 Matrix4::FromTranslation(const Vector3& translation) {
		Matrix4 newMat;
		newMat[3][0] = translation.x;
		newMat[3][1] = translation.y;
		newMat[3][2] = translation.z;
		return newMat;
	}

	Matrix4 Matrix4::FromRotation(const float& angle, Vector3 axis) {
		axis.Normalise();

		const float angSin = sinf(angle);
		const float angCos = cosf(angle);
		const float angCos1Min = 1.0f - angCos;

		const float xx = axis.x * axis.x;
		const float xy = axis.x * axis.y;
		const float xz = axis.x * axis.z;
		const float yy = axis.y * axis.y;
		const float yz = axis.y * axis.z;
		const float zz = axis.z * axis.z;

		return {
			Vector4(angCos + xx * angCos1Min, xy * angCos1Min + axis.z * angSin, xz * angCos1Min - axis.y * angSin, 0),
			Vector4(xy * angCos1Min - axis.z * angSin, angCos * yy * angCos1Min, yz * angCos1Min + axis.x * angSin, 0),
			Vector4(xz * angCos1Min + axis.y * angSin, yz * angCos1Min - axis.x * angSin, angCos + zz * angCos1Min, 0),
			Vector4(0, 0, 0, 1)
		};
	}

	Matrix4 Matrix4::FromRotationX(const float& angle) {
		Matrix4 newMat(1);
		newMat[1][1] = cosf(angle);
		newMat[1][2] = sinf(angle);
		newMat[2][1] = -sinf(angle);
		newMat[2][2] = cosf(angle);
		return newMat;
	}
	Matrix4 Matrix4::FromRotationY(const float& angle) {
		Matrix4 newMat(1);
		newMat[0][0] = cosf(angle);
		newMat[0][1] = sinf(angle);
		newMat[1][0] = -sinf(angle);
		newMat[1][1] = cosf(angle);
		return newMat;
	}
	Matrix4 Matrix4::FromRotationZ(const float& angle) {
		Matrix4 newMat(1);
		newMat[1][1] = cosf(angle);
		newMat[1][2] = sinf(angle);
		newMat[2][1] = -sinf(angle);
		newMat[2][2] = cosf(angle);
		return newMat;
	}

	Matrix4 Matrix4::FromRotation(const Quaternion& quat) {
		if (IsNaN(quat.x) || IsNaN(quat.y) || IsNaN(quat.z) || IsNaN(quat.w)) return {};

		Matrix4 result; // Identity Matrix

		float xx = quat.x * quat.x; float yy = quat.y * quat.y; float zz = quat.z * quat.z;
		float xy = quat.x * quat.y; float xz = quat.x * quat.z; float yz = quat.y * quat.z;
		float wx = quat.w * quat.x; float wy = quat.w * quat.y; float wz = quat.w * quat.z;

		result[0][0] = 1.0f - 2.0f * (yy + zz);
		result[0][1] = 2.0f * (xy - wz);
		result[0][2] = 2.0f * (xz + wy);

		result[1][0] = 2.0f * (xy + wz);
		result[1][1] = 1.0f - 2.0f * (xx + zz);
		result[1][2] = 2.0f * (yz - wx);

		result[2][0] = 2.0f * (xz - wy);
		result[2][1] = 2.0f * (yz + wx);
		result[2][2] = 1.0f - 2.0f * (xx + yy);

		return result;
	}

	Matrix4 Matrix4::FromRotationZYX(const Quaternion& quat) {
		const Vector3 rot = quat.ToEulerAngles();
		Matrix4 rx = FromRotationX(rot.x);
		const Matrix4 ry = FromRotationY(rot.y);
		const Matrix4 rz = FromRotationZ(rot.z);

		return rx * ry * rz;
	}

	Matrix4 Matrix4::FromScale(const Vector3& scale) {
		Matrix4 newMat;
		newMat[0][0] = scale.x;
		newMat[1][1] = scale.y;
		newMat[2][2] = scale.z;
		return newMat;
	}

	Matrix4 Matrix4::Translate(const Vector3 &v) {
		Matrix4 result(*this);

		result[3][0] += m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z;
		result[3][1] += m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z;
		result[3][2] += m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z;

		return result;
	}

	Matrix4 Matrix4::Rotate(const Quaternion& quat) {
		Matrix4 lhs = *this;

	    float xx = quat.x * quat.x; float yy = quat.y * quat.y; float zz = quat.z * quat.z;
	    float xy = quat.x * quat.y; float xz = quat.x * quat.z; float yz = quat.y * quat.z;
	    float wx = quat.w * quat.x; float wy = quat.w * quat.y; float wz = quat.w * quat.z;

	    float r00 = 1.0f - 2.0f * (yy + zz);
	    float r01 = 2.0f * (xy - wz);
	    float r02 = 2.0f * (xz + wy);

	    float r10 = 2.0f * (xy + wz);
	    float r11 = 1.0f - 2.0f * (xx + zz);
	    float r12 = 2.0f * (yz - wx);

	    float r20 = 2.0f * (xz - wy);
	    float r21 = 2.0f * (yz + wx);
	    float r22 = 1.0f - 2.0f * (xx + yy);

	    Matrix4 result;

	    result[0][0] = lhs[0][0] * r00 + lhs[0][1] * r10 + lhs[0][2] * r20;
	    result[0][1] = lhs[0][0] * r01 + lhs[0][1] * r11 + lhs[0][2] * r21;
	    result[0][2] = lhs[0][0] * r02 + lhs[0][1] * r12 + lhs[0][2] * r22;
	    result[0][3] = lhs[0][3];

	    result[1][0] = lhs[1][0] * r00 + lhs[1][1] * r10 + lhs[1][2] * r20;
	    result[1][1] = lhs[1][0] * r01 + lhs[1][1] * r11 + lhs[1][2] * r21;
	    result[1][2] = lhs[1][0] * r02 + lhs[1][1] * r12 + lhs[1][2] * r22;
	    result[1][3] = lhs[1][3];

	    result[2][0] = lhs[2][0] * r00 + lhs[2][1] * r10 + lhs[2][2] * r20;
	    result[2][1] = lhs[2][0] * r01 + lhs[2][1] * r11 + lhs[2][2] * r21;
	    result[2][2] = lhs[2][0] * r02 + lhs[2][1] * r12 + lhs[2][2] * r22;
	    result[2][3] = lhs[2][3];

	    result[3][0] = lhs[3][0] * r00 + lhs[3][1] * r10 + lhs[3][2] * r20;
	    result[3][1] = lhs[3][0] * r01 + lhs[3][1] * r11 + lhs[3][2] * r21;
	    result[3][2] = lhs[3][0] * r02 + lhs[3][1] * r12 + lhs[3][2] * r22;
	    result[3][3] = lhs[3][3];

	    return result;
	}

	Matrix4 Matrix4::Scale(const Vector3& scale) {
		Matrix4 result(*this);
		result[0] *= scale.x;
		result[1] *= scale.y;
		result[2] *= scale.z;
		return result;
	}

	Vector3 Matrix4::ToEulerAngles() {
		Vector3 newVec;
		const float T1 = atan2f(m[2][1], m[2][2]);
		const float C2 = sqrtf(m[0][0] * m[0][0] + m[1][0] * m[1][0]);
		const float T2 = atan2f(-m[2][0], C2);
		const float S1 = sinf(T1);
		const float C1 = cosf(T1);
		const float T3 = atan2f(S1 * m[0][2] - C1 * m[0][1], C1 * m[1][1] - S1 * m[1][2]);
		newVec.x = -T1;
		newVec.y = -T2;
		newVec.z = -T3;
		return newVec;
	}

	Quaternion Matrix4::ToQuaternion() {
		const float x = m[0][0] - m[1][1] - m[2][2];
		const float y = m[1][1] - m[0][0] - m[2][2];
		const float z = m[2][2] - m[0][0] - m[1][1];
		const float w = m[0][0] + m[1][1] + m[2][2];

		int biggestIndex = 0;
		float biggest = x;
		if (y > biggest) {
			biggest = y;
			biggestIndex = 1;
		}
		if (z > biggest) {
			biggest = z;
			biggestIndex = 2;
		}
		if (w > biggest) {
			biggest = w;
			biggestIndex = 3;
		}

		float biggestVal = sqrtf(biggest + 1.0f) * 0.5f;
		const float mult = 0.25f / biggestVal;

		switch (biggestIndex) {
			case 0:
				return {biggestVal, (m[0][1] + m[1][0]) * mult, (m[2][0] + m[0][2]) * mult, (m[1][2] - m[2][1]) * mult};
			case 1:
				return {(m[0][1] + m[1][0]) * mult, biggestVal, (m[1][2] + m[2][1]) * mult, (m[2][0] - m[0][2]) * mult};
			case 2:
				return {(m[2][0] + m[0][2]) * mult, (m[1][2] + m[2][1]) * mult, biggestVal, (m[0][1] - m[1][0]) * mult};
		case 3:
			return {(m[1][2] - m[2][1]) * mult, (m[2][0] - m[0][2]) * mult, (m[0][1] - m[1][0]) * mult, biggestVal};
		default:
			return {};
		}
	}

	Vector3 Matrix4::GetTranslation() { return {m[0][3], m[1][3], m[2][3]}; }

	Vector3 Matrix4::GetScale() { return {m[0][0], m[1][1], m[2][2]}; }

	Matrix4 Matrix4::Inverse() {
		float c0 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
		float c2 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
		float c3 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

		float c4 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
		float c6 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
		float c7 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

		float c8 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
		float c10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
		float c11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

		float c12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
		float c14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
		float c15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

		float c16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
		float c18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
		float c19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

		float c20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
		float c22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
		float c23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

		Vector4 f0(c0, c0, c2, c3);
		Vector4 f1(c4, c4, c6, c7);
		Vector4 f2(c8, c8, c10, c11);
		Vector4 f3(c12, c12, c14, c15);
		Vector4 f4(c16, c16, c18, c19);
		Vector4 f5(c20, c20, c22, c23);

		Vector4 v0(m[1][0], m[0][0], m[0][0], m[0][0]);
		Vector4 v1(m[1][1], m[0][1], m[0][1], m[0][1]);
		Vector4 v2(m[1][2], m[0][2], m[0][2], m[0][2]);
		Vector4 v3(m[1][3], m[0][3], m[0][3], m[0][3]);

		Vector4 inv0(v1 * f0 - v2 * f1 + v3 * f2);
		Vector4 inv1(v0 * f0 - v2 * f3 + v3 * f4);
		Vector4 inv2(v0 * f1 - v1 * f3 + v3 * f5);
		Vector4 inv3(v0 * f1 - v1 * f4 + v2 * f5);

		Vector4 signA(+1, -1, +1, -1);
		Vector4 signB(-1, +1, -1, +1);
		Matrix4 invMat(inv0 * signA, inv1 * signB, inv2 * signA, inv3 * signB);

		Vector4 r0(invMat[0][0], invMat[1][0], invMat[2][0], invMat[3][0]);

		Vector4 d0((*this)[0]);
		d0 *= r0;
		float d1 = (d0.x + d0.y) + (d0.z + d0.w);

		float OneOverDeterminant = 1.0f / d1;

		return invMat * Matrix4(OneOverDeterminant);
	}
}
