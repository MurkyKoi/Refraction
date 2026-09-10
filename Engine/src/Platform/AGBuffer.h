#pragma once

#include <Core/Common.h>
#include <Classes/Assets/Image.h>
#include <Platform/ATexture.h>

namespace Refraction::Engine::Platform {
	class AGBuffer {
	public:
		// Creates a new GBuffer using the active rendering API
		static Common::Shared<AGBuffer> CreateGBuffer();

		Common::Ref<Assets::Image> GetLastRenderedFrame();

		// Initialises the GBuffer textures
		virtual bool Init(unsigned int viewWidth, unsigned int viewHeight) = 0;
		// Regenerates the GBuffer for a new viewport size
		virtual bool Regenerate(unsigned int viewWidth, unsigned int viewHeight) = 0;
		virtual void SetShaderTextureIDs() const = 0;

		virtual void StartFrame() = 0;
		virtual void BindFramebufferWrite() = 0;
		virtual void BindFramebufferRead() = 0;
		virtual void BindFramebufferFull() = 0;
		virtual void BindGeometryPass() = 0;
		virtual void BindLightingPass() = 0;
		virtual void BindFinalPass() = 0;
		virtual void BindTextures() = 0;

	protected:
		static constexpr unsigned int TextureCount = 6;

		unsigned int mFBID = 0;
		Common::Ref<ATexture> mDiffuse;
		Common::Ref<ATexture> mNormal;
		Common::Ref<ATexture> mPosition;
		Common::Ref<ATexture> mSMR; // Specular R, Metallic G, Roughness B
		Common::Ref<ATexture> mDepth;
		Common::Ref<ATexture> mCFAAData;
		Common::Ref<ATexture> mFinal;
		UUIDValue mFinalImageUUID = 0;

		AGBuffer();
		virtual ~AGBuffer();

		std::vector<Common::Ref<ATexture>> GetTextureArray() {
			return { mDiffuse, mNormal, mPosition, mSMR, mDepth, mCFAAData, mFinal };
		}

		virtual void Cleanup() = 0;
	};
}
