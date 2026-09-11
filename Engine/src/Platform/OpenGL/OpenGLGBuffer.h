#pragma once

#include <Platform/AGBuffer.h>

namespace Refraction::Engine::Platform {
	class OpenGLGBuffer : public AGBuffer {
	public:
		OpenGLGBuffer() = default;
		~OpenGLGBuffer() override;
		
		bool Init(int viewWidth, int viewHeight) override;
		bool Regenerate(int viewWidth, int viewHeight) override;
		void SetShaderTextureIDs() const override;

		void StartFrame() override;
		void BindFramebufferWrite() override;
		void BindFramebufferRead() override;
		void BindFramebufferFull() override;
		void BindCFAAPrepass() override;
		void BindGeometryPass() override;
		void BindLightingPass() override;
		void BindFinalPass() override;
		void BindCFAATexturesForSampling() override;
		void BindTextures() override;

	protected:
		void Cleanup() override;
	};
}
