#pragma once

#include <Core/Common.h>
#include <Math/Vector.h>
#include <Math/Rect.h>

namespace Refraction::Engine::Platform {
	enum class RenderingAPI {
		NONE = 0,
		OPENGL,
		VULKAN,
	};

	class ARenderingAPI {
	public:
		virtual ~ARenderingAPI() = default;

		static Common::Shared<ARenderingAPI> Get();
		static RenderingAPI GetAPI() { return CurrentAPI; }

		virtual void Init() = 0;
		virtual void Clear(Math::Vector4 colour) = 0;
		virtual void SetViewportRect(Math::Rect rect) = 0;

	private:
		static RenderingAPI CurrentAPI;
	};
}
