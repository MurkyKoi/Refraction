#include <random>

#include "Utilities.h"

static std::random_device randDevice;
static std::mt19937 randGen(randDevice());

namespace Refraction::Utilities {
	int RandomI(const int max, const int min) {
		std::uniform_int_distribution<> dist(min, max);
		return dist(randGen);
	}

	float RandomF(const float max, const float min) {
		std::uniform_real_distribution<> dist(min, max);
		return static_cast<float>(dist(randGen));
	}


	// Math utilities
	///

	std::string DeltaToRate(const double deltaTime, const int precision) {
		const double fps = 1.0 / deltaTime;
		if (precision < 0) return std::to_string(fps);
		if (precision == 0) return std::to_string(trunc(fps));

		const std::string fpsString = std::to_string(fps);
		return fpsString.substr(0, fpsString.find('.') + precision + 1);
	}

	Math::Vector3 BoolToVector3(const bool pX, const bool nX, const bool pY, const bool nY, const bool pZ, const bool nZ) {
		return {pX - nX, pY - nY, pZ - nZ};
	}

	glm::mat3 NativeToGLMMat3(Math::Matrix3 m) {
		return {m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2]};
	}

	Math::Matrix3 GLMToNativeMat3(glm::mat3 m) {
		Math::Matrix3 newMat;
		for (unsigned int row = 0; row < 3; row++) {
			for (unsigned int col = 0; col < 3; col++) {
				newMat[row][col] = m[static_cast<int>(row)][static_cast<int>(col)];
			}
		}
		return newMat;
	}

	glm::mat4 NativeToGLMMat4(Math::Matrix4 m) {
		return {m[0][0], m[0][1], m[0][2], m[0][3], m[1][0], m[1][1], m[1][2], m[1][3], m[2][0], m[2][1], m[2][2], m[2][3], m[3][0], m[3][1], m[3][2], m[3][3]};
	}

	Math::Matrix4 GLMToNativeMat4(glm::mat4 m) {
		Math::Matrix4 newMat;
		for (unsigned int row = 0; row < 4; row++) {
			for (unsigned int col = 0; col < 4; col++) {
				newMat[row][col] = m[static_cast<int>(row)][static_cast<int>(col)];
			}
		}
		return newMat;
	}
}
