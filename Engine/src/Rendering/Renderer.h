#pragma once

#include <chrono>

#include <Core/Common.h>
#include <Math/Rect.h>
#include <Platform/AGBuffer.h>

#include <Classes/Assets/Shader.h>
#include <Interface/Project.h>
#include "Models/BaseScene.h"
#include "Buffers/UniformBufferObject.h"


namespace Refraction::Engine {
	enum class RendererState {
		NONE,
		INIT,
		RUNNING,
		CLEANUP,
		EXIT
	};

	class Renderer {
	public:
		Renderer();

		void Init();
		void RenderFrame(const Common::Shared<Project> &projectInstance);

		void SetViewport(const Math::Rect rect) { mViewportRect = rect; };

		[[nodiscard]] RendererState GetState() const { return mState; }
		[[nodiscard]] Common::Ref<Assets::Image> GetFinalOutput() const { return mFinalOutput; }
	private:
		void UpdateUniformBuffers(const Common::Shared<Project> &projectInstance);
		void Cleanup();

		// Deferred shading functions
		void DSPassGeometry(const Common::Shared<Project> &projectInstance) const;
		void DSPassLighting(const Common::Shared<Project> &projectInstance) const;
		void DSPassFinal() const;

		RendererState mState = RendererState::NONE;

		UniformBufferObject* mUBO = nullptr;
		Common::Ref<Assets::Shader> mGeomPassShader = {};
		Common::Ref<Assets::Shader> mLightingPassShader = {};
		Common::Ref<Assets::Shader> mCFAAPrepassShader = {};
		Common::Ref<Assets::Shader> mSkyShader = {};
		Common::Shared<Platform::AGBuffer> mGBuffer = nullptr;
		Math::Rect mViewportRect;
		Math::Rect mViewportRectLast = mViewportRect;
		Common::Ref<Assets::Image> mFinalOutput;

		BaseScene* mLoadedScene = nullptr;

		bool mShouldRender = true;
		bool mWireframeMode = false;
		bool mCFAALastState = false;
		double mElapsedRenderTime = 0;
		double mDeltaRenderTime = 0;
		double mElapsedTickTime = 0;
		double mDeltaTickTime = 0;
		std::chrono::steady_clock::time_point mStartRenderTime;
		std::chrono::steady_clock::time_point mStartTickTime;
	};

}
