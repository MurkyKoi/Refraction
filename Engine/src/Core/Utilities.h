#pragma once

#include <string>

#include <glm/glm.hpp>

#include <Math/Vector.h>
#include <Math/Matrix.h>

namespace Refraction::Utilities {
	extern std::string EngineWorkingDirectory;

	extern int RandomI(int max, int min = 0);
	extern float RandomF(float max = 1, float min = 0);

	// Converts delta time (in ms) to a rate per second (e.g. FPS)
	extern std::string DeltaToRate(double deltaTime, int precision = -1);

	extern Math::Vector3 BoolToVector3(bool pX, bool nX, bool pY, bool nY, bool pZ, bool nZ);
	extern inline glm::vec3 NativeToGLMVec3(const Math::Vector3& v) { return {v.x, v.y, v.z}; };
	extern inline Math::Vector3 GLMToNativeVec3(glm::vec3 v) { return {v.x, v.y, v.z}; };
	extern glm::mat3 NativeToGLMMat3(Math::Matrix3 m);
	extern Math::Matrix3 GLMToNativeMat3(glm::mat3 m);
	extern glm::mat4 NativeToGLMMat4(Math::Matrix4 m);
	extern Math::Matrix4 GLMToNativeMat4(glm::mat4 m);
};
