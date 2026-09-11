#pragma once

#include <Math/Common.h>
#include <Math/Vector.h>

namespace Refraction::Math {
	class Rect {
	public:
		int x, y, w, h;

		// Create a Rect at x,y with the given size
		Rect(const int x, const int y, const int w, const int h) : x(x), y(y), w(std::max(w, 0)), h(std::max(h, 0)) {}
		// Create a Rect at x,y with the given size
		Rect(const Vector2& pos, const int w, const int h) : Rect(static_cast<int>(pos.x), static_cast<int>(pos.y), w, h) {}
		// Create a Rect at x,y with the given size
		Rect(const Vector2& pos, const Vector2& size) : Rect(static_cast<int>(pos.x), static_cast<int>(pos.y), static_cast<int>(size.x), static_cast<int>(size.y)) {}
		// Create a square Rect at x,y with length of n
		Rect(const int x, const int y, const int n) : Rect(x, y, n, n) {}
		// Create a square Rect at x,y with length of n
		Rect(const Vector2& pos, const int n) : Rect(static_cast<int>(pos.x), static_cast<int>(pos.y), n, n) {}
		// Create a Rect at 0,0 with the given size
		Rect(const int w, const int h) : Rect(0, 0, w, h) {}
		// Create a Rect at 0,0 with the given size
		explicit Rect(const Vector2& size) : Rect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)) {}
		// Create a square Rect at 0,0 with length of n
		explicit Rect(const int n) : Rect(0, 0, n, n) {}
		// Create a zero-length Rect at 0,0
		Rect() : x(0), y(0), w(0), h(0) {}

		bool operator==(const Rect& other) const {
			return (x == other.x) && (y == other.y) && (w == other.w) && (h == other.h);
		}

		[[nodiscard]] float AspectRatio() const { return w / static_cast<float>(h); }

		void ToVector2(Vector2& pos, Vector2& size) const {
			pos.x = static_cast<float>(x);
			pos.y = static_cast<float>(y);
			size.x = static_cast<float>(w);
			size.y = static_cast<float>(h);
		}

		[[nodiscard]] std::string ToString(const PrintFormatArgs fmtArgs = PrintFormatArgs()) const {
			if (fmtArgs.Pretty) {
				return std::string("x: " + std::to_string(x) + "\ny: " + std::to_string(y) + "\nw: " + std::to_string(w) + "\nh: " + std::to_string(h));
			}
			return std::string("{" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(w) + ", " + std::to_string(h) + "}");
		}
	};
}
