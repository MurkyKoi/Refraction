#pragma once

#include <Math/Common.h>
#include <Math/Vector.h>

namespace Refraction::Math {
	class Frustum {
	public:
		int w, h;
		float fovY, zNear, zFar;

		// Create a Frustum with the given width, height, FOV and Z range
		Frustum(const int w, const int h, const float fovY, const float zNear, const float zFar) : w(std::max(w, 0)), h(std::max(h, 0)), fovY(std::max(fovY, 10.0f)), zNear(std::max(zNear, 0.0f)), zFar(std::max(zFar, 0.0f)) {};
		// Create a Frustum with the given width, height, FOV and Z range
		Frustum(const Vector2& size, const float fovY, const float zNear, const float zFar) : Frustum(static_cast<int>(size.x), static_cast<int>(size.y), fovY, zNear, zFar) {};
		// Create a default Frustum
		Frustum() : Frustum(1920, 1080, 70.0f, 0.01f, 10000.0f) {};

		bool operator==(const Frustum& other) const {
			return (w == other.w) && (h == other.h) && (zNear == other.zNear) && (zFar == other.zFar);
		}

		[[nodiscard]] float AspectRatio() const { return w / static_cast<float>(h); }

		[[nodiscard]] std::string ToString(const PrintFormatArgs fmtArgs = PrintFormatArgs()) const {
			if (fmtArgs.Pretty) {
				return std::string("w: " + std::to_string(w) + "\nh: " + std::to_string(h) + "\nfovY: " + std::to_string(fovY) + "\nzNear: " + std::to_string(zNear) + "\nzFar: " + std::to_string(zFar));
			}
			return std::string("{" + std::to_string(w) + ", " + std::to_string(h) + ", " + std::to_string(fovY) + ", " + std::to_string(zNear) + ", " + std::to_string(zFar) + "}");
		}
	};
}
