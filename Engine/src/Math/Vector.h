#pragma once

#include <stdexcept>
#include <string>

#include <Math/Common.h>

namespace Refraction::Math {
	class Vector2 {
	public:
		float x = 0;
		float y = 0;

		Vector2(const float x, const float y) : x(x), y(y) {}
		Vector2(const int x, const int y) : Vector2(static_cast<float>(x), static_cast<float>(y)) {}
		explicit Vector2(const float n) : x(n), y(n) {}
		explicit Vector2(const int n) : Vector2(static_cast<float>(n)) {}
		Vector2() = default;

		Vector2(const Vector2& v) {
			x = v.x;
			y = v.y;
		}

		Vector2& operator=(const Vector2& v2) {
			if (this != &v2) {
				x = v2.x;
				y = v2.y;
			}
			return *this;
		}

		Vector2 operator-() const {
			return Vector2(-x, -y);
		}

		Vector2 operator+(const Vector2& v2) const { return Vector2(x + v2.x, y + v2.y); }
		Vector2 operator-(const Vector2& v2) const { return Vector2(x - v2.x, y - v2.y); }
		Vector2 operator*(const Vector2& v2) const { return Vector2(x * v2.x, y * v2.y); }
		Vector2 operator*(const float n) const { return Vector2(x * n, y * n); }

		void operator+=(const Vector2& v2) { x += v2.x; y += v2.y; }
		void operator-=(const Vector2& v2) { x -= v2.x; y -= v2.y; }
		void operator*=(const Vector2& v2) { x *= v2.x; y *= v2.y; }
		void operator*=(const float n) { x *= n; y *= n; }

		bool operator==(const Vector2& v2) const = default;
		bool operator<(const Vector2& v2) const { return (x < v2.x) && (y < v2.y); }
		bool operator>(const Vector2& v2) const { return (x > v2.x) && (y > v2.y); }
		float& operator[](const unsigned int index) {
			if (index == 0) return x;
			if (index == 1) return y;
			throw std::runtime_error("Index out of range");
		}

		void Normalise() { *this *= 1.0f / sqrtf(Dot(*this)); }
		[[nodiscard]] float Magnitude() const { return fabsf(x) + fabsf(y); }
		[[nodiscard]] float Dot(const Vector2& v2) const { return x * v2.x + y * v2.y; };
		[[nodiscard]] float Distance(const Vector2& v2) const { return sqrtf(powf(x - v2.x, 2) + powf(y - v2.y, 2)); };

		[[nodiscard]] Vector2 Normalised() const {
			auto copy = Vector2(x, y);
			copy.Normalise();
			return copy;
		}

		[[nodiscard]] std::string ToString(const PrintFormatArgs fmtArgs = PrintFormatArgs()) const {
			const std::string xStr = fmtArgs.AsInt ? std::to_string(static_cast<int>(x)) : std::to_string(x);
			const std::string yStr = fmtArgs.AsInt ? std::to_string(static_cast<int>(y)) : std::to_string(y);
			if (fmtArgs.Pretty) {
				return std::string("x: " + xStr + "\ny: " + yStr);
			}
			return std::string("{" + xStr + ", " + yStr + "}");
		}
	};

	class Vector3 {
	public:
		float x = 0;
		float y = 0;
		float z = 0;

		static Vector3 Front() { return {0, 0, -1}; }
		static Vector3 Right() { return {1, 0, 0}; }
		static Vector3 Up() { return {0, 1, 0}; }
		static Vector3 X() { return {1, 0, 0}; }
		static Vector3 Y() { return {0, 1, 0}; }
		static Vector3 Z() { return {0, 0, 1}; }
		static Vector3 Zero() { return Vector3(0); }
		static Vector3 One() { return Vector3(1); }

		Vector3(const float x, const float y, const float z) : x(x), y(y), z(z) {}
		Vector3(const int x, const int y, const int z) : x(static_cast<float>(x)), y(static_cast<float>(y)), z(static_cast<float>(z)) {}
		Vector3(const Vector3& v) = default;
		explicit Vector3(const float n) : x(n), y(n), z(n) {}
		explicit Vector3(const int n) : x(static_cast<float>(n)), y(static_cast<float>(n)), z(static_cast<float>(n)) {}
		Vector3() = default;

		Vector3& operator=(const Vector3& v2) {
			if (this != &v2) { x = v2.x; y = v2.y; z = v2.z; }
			return *this;
		}

		Vector3 operator-() const { return Vector3(-x, -y, -z); }


		Vector3 operator+(const Vector3 v2) const { return Vector3(x + v2.x, y + v2.y, z + v2.z); }
		Vector3 operator-(const Vector3 v2) const { return Vector3(x - v2.x, y - v2.y, z - v2.z); }
		Vector3 operator*(const Vector3 v2) const { return Vector3(x * v2.x, y * v2.y, z * v2.z); }
		Vector3 operator*(const float n) const { return Vector3(x * n, y * n, z * n); }

		void operator+=(const Vector3 v2) { x += v2.x; y += v2.y; z += v2.z; }
		void operator-=(const Vector3 v2) { x -= v2.x; y -= v2.y; z -= v2.z; }
		void operator*=(const Vector3 v2) { x *= v2.x; y *= v2.y; z *= v2.z; }
		void operator*=(const float n) { x *= n; y *= n; z *= n; }

		bool operator==(const Vector3& v2) const = default;
		bool operator<(const Vector3& v2) const { return (x < v2.x) && (y < v2.y) && (z < v2.z); }
		bool operator>(const Vector3& v2) const { return (x > v2.x) && (y > v2.y) && (z > v2.z); }
		float& operator[](const unsigned int index) {
			if (index == 0) return x;
			if (index == 1) return y;
			if (index == 2) return z;
			throw std::runtime_error("Index out of range");
		}

		Vector3 operator*(const Vector2& v2) const { return Vector3(x * v2.x, y * v2.y, z); }
		Vector3 operator+(const Vector2& v2) const { return Vector3(x + v2.x, y + v2.y, z); }
		Vector3 operator-(const Vector2& v2) const { return Vector3(x - v2.x, y - v2.y, z); }

		void Normalise() { (*this) *= 1.0f / sqrtf(Dot((*this))); }
		[[nodiscard]] float Magnitude() const { return fabsf(x) + fabsf(y) + fabsf(z); }
		[[nodiscard]] float Dot(const Vector3& v2) const { return x * v2.x + y * v2.y + z * v2.z; };
		[[nodiscard]] float Distance(const Vector3& v2) const { return sqrtf(powf(x - v2.x, 2) + powf(y - v2.y, 2) + powf(z - v2.z, 2)); };
		[[nodiscard]] Vector3 Cross(const Vector3& v2) const { return Vector3(y * v2.z - z * v2.y, z * v2.x - x * v2.z, x * v2.y - y * v2.x); };

		[[nodiscard]] Vector3 Normalised() const {
			auto copy = Vector3(x, y, z);
			copy.Normalise();
			return copy;
		}

		[[nodiscard]] std::string ToString(const PrintFormatArgs fmtArgs = PrintFormatArgs()) const {
			const std::string xStr = fmtArgs.AsInt ? std::to_string(static_cast<int>(x)) : std::to_string(x);
			const std::string yStr = fmtArgs.AsInt ? std::to_string(static_cast<int>(y)) : std::to_string(y);
			const std::string zStr = fmtArgs.AsInt ? std::to_string(static_cast<int>(z)) : std::to_string(z);
			if (fmtArgs.Pretty) {
				return std::string("x: " + xStr + "\ny: " + yStr + "\nz: " + zStr);
			}
			return std::string("{" + xStr + ", " + yStr + ", " + zStr + "}");
		}
	};

	class Vector4 {
	public:
		float x = 0;
		float y = 0;
		float z = 0;
		float w = 0;

		Vector4(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w) {}
		explicit Vector4(const float n) : x(n), y(n), z(n), w(n) {}
		Vector4() = default;

		Vector4(const Vector4& v) {
			x = v.x;
			y = v.y;
			z = v.z;
			w = v.w;
		}

		Vector4& operator=(const Vector4& v2) {
			if (this != &v2) {
				x = v2.x;
				y = v2.y;
				z = v2.z;
				w = v2.w;
			}
			return *this;
		}

		Vector4 operator-() const {
			return Vector4(-x, -y, -z, -w);
		}


		Vector4 operator+(const Vector4& v2) const { return Vector4(x + v2.x, y + v2.y, z + v2.z, w + v2.w); }
		Vector4 operator-(const Vector4& v2) const { return Vector4(x - v2.x, y - v2.y, z - v2.z, w - v2.w); }
		Vector4 operator*(const Vector4& v2) const { return Vector4(x * v2.x, y * v2.y, z * v2.z, w * v2.w); }
		Vector4 operator*(const float n) const { return Vector4(x * n, y * n, z * n, w * n); }

		void operator+=(const Vector4& v2) { x += v2.x; y += v2.y; z += v2.z; w += v2.w; }
		void operator-=(const Vector4& v2) { x -= v2.x; y -= v2.y; z -= v2.z; w -= v2.w; }
		void operator*=(const Vector4& v2) { x *= v2.x; y *= v2.y; z *= v2.z; w *= v2.w; }
		void operator*=(const float n) { x *= n; y *= n; z *= n; w *= n; }

		bool operator==(const Vector4& v2) const = default;
		bool operator<(const Vector4& v2) const { return (x < v2.x) && (y < v2.y) && (z < v2.z) && (w < v2.w); }
		bool operator>(const Vector4& v2) const { return (x > v2.x) && (y > v2.y) && (z > v2.z) && (w > v2.w); }
		float& operator[](const unsigned int index) {
			if (index == 0) return x;
			if (index == 1) return y;
			if (index == 2) return z;
			if (index == 3) return w;
			throw std::runtime_error("Index out of range");
		}

		// Compatibility with smaller vectors
		Vector4 operator*(const Vector3 &v2) const { return Vector4(x * v2.x, y * v2.y, z * v2.z, w); }
		Vector4 operator+(const Vector3 &v2) const { return Vector4(x + v2.x, y + v2.y, z + v2.z, w); }
		Vector4 operator-(const Vector3 &v2) const { return Vector4(x - v2.x, y - v2.y, z - v2.z, w); }
		Vector4 operator*(const Vector2& v2) const { return Vector4(x * v2.x, y * v2.y, z, w); }
		Vector4 operator+(const Vector2& v2) const { return Vector4(x + v2.x, y + v2.y, z, w); }
		Vector4 operator-(const Vector2& v2) const { return Vector4(x - v2.x, y - v2.y, z, w); }

		void operator+=(const Vector3 &v2) {
			x += v2.x;
			y += v2.y;
			z += v2.z;
		}
		void operator-=(const Vector3 &v2) {
			x -= v2.x;
			y -= v2.y;
			z -= v2.z;
		}

		void Normalise() { *this *= 1.0f / sqrtf(Dot(*this)); }
		[[nodiscard]] float Magnitude() const { return fabsf(x) + fabsf(y) + fabsf(z) + fabsf(w); }
		[[nodiscard]] float Dot(const Vector4& v2) const { return x * v2.x + y * v2.y + z * v2.z + w * v2.w; };
		[[nodiscard]] float Distance(const Vector4& v2) const { return sqrtf(powf(x - v2.x, 2) + powf(y - v2.y, 2) + powf(z - v2.z, 2) + powf(w - v2.w, 2)); };

		[[nodiscard]] Vector4 Normalised() const {
			auto copy = Vector4(x, y, z, w);
			copy.Normalise();
			return copy;
		}

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
	};
}