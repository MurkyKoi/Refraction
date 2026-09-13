#include <glad/glad.h>

#include <Settings.h>
#include <Classes/Assets/Shader.h>
#include <Interface/Project.h>

#include "OpenGLGBuffer.h"

namespace Refraction::Engine::Platform {
	OpenGLGBuffer::~OpenGLGBuffer() {
		OpenGLGBuffer::Cleanup();
	}

	bool OpenGLGBuffer::Init(const int viewWidth, const int viewHeight) {
		// Init framebuffer
		glGenFramebuffers(1, &mFBID);
		glGenFramebuffers(1, &mCFAAFBID);

		return Regenerate(viewWidth, viewHeight);
	}

	bool OpenGLGBuffer::Regenerate(const int viewWidth, const int viewHeight) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBID);

		const auto& graphicsSettings = Settings::CurrentSettings->Graphics;
		const auto scale = (graphicsSettings.CFAAEnabled) ? graphicsSettings.CFAAScale : 1;
		const int gpassW = viewWidth * scale;
		const int gpassH = viewHeight * scale;
		const int cfaaPrepassW = viewWidth;
		const int cfaaPrepassH = viewHeight;

		const TextureStructure nativeDepthStruct = { .Width = gpassW, .Height = gpassH, .Format = TextureFormat::NONE, .Filtering = TextureFiltering::POINT, .MipmapMode = TextureMipmapMode::DISABLED };
		auto nativeRGBStruct = nativeDepthStruct; nativeRGBStruct.Format = TextureFormat::RGB8;
		auto nativeRGBA16FStruct = nativeDepthStruct; nativeRGBA16FStruct.Format = TextureFormat::RGBA16F;
		auto finalStruct = nativeRGBStruct; finalStruct.Filtering = TextureFiltering::BILINEAR;

		if (mDepth.expired()) mDepth = ATexture::MakeTexture(nativeDepthStruct);
		else mDepth.lock()->Regenerate(nativeDepthStruct);

		if (mDiffuse.expired()) mDiffuse = ATexture::MakeTexture(nativeRGBStruct);
		else mDiffuse.lock()->Regenerate(nativeRGBStruct);

		if (mNormal.expired()) mNormal = ATexture::MakeTexture(nativeRGBA16FStruct);
		else mNormal.lock()->Regenerate(nativeRGBA16FStruct);

		if (mPosition.expired()) mPosition = ATexture::MakeTexture(nativeRGBA16FStruct);
		else mPosition.lock()->Regenerate(nativeRGBA16FStruct);

		if (mSMR.expired()) mSMR = ATexture::MakeTexture(nativeRGBStruct);
		else mSMR.lock()->Regenerate(nativeRGBStruct);

		if (mCFAAData.expired()) mCFAAData = ATexture::MakeTexture(nativeRGBStruct);
		else mCFAAData.lock()->Regenerate(nativeRGBStruct);

		if (mFinal.expired()) mFinal = ATexture::MakeTexture(finalStruct);
		else mFinal.lock()->Regenerate(finalStruct);

		glBindFramebuffer(GL_FRAMEBUFFER, mFBID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mDiffuse.lock()->GetBufferID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mNormal.lock()->GetBufferID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mPosition.lock()->GetBufferID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, mSMR.lock()->GetBufferID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, mCFAAData.lock()->GetBufferID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, mFinal.lock()->GetBufferID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepth.lock()->GetBufferID(), 0);

		if (const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); status != GL_FRAMEBUFFER_COMPLETE) {
			if (!status) {
				const auto result = glGetError();
				throw std::runtime_error("FRAMEBUFFER CONSTRUCT ERROR | " + std::to_string(result));
			}
			Log::Render.Error("FRAMEBUFFER CONSTRUCT FAILED | " + std::to_string(status));
			return false;
		}

		if (graphicsSettings.CFAAEnabled) {
			const TextureStructure cfaaDepthStruct = { .Width = cfaaPrepassW, .Height = cfaaPrepassH, .Format = TextureFormat::NONE, .Filtering = TextureFiltering::POINT, .MipmapMode = TextureMipmapMode::DISABLED };
			auto cfaaRGBStruct = cfaaDepthStruct; cfaaRGBStruct.Format = TextureFormat::RGB8;
			auto cfaaRGBA16FStruct = cfaaDepthStruct; cfaaRGBA16FStruct.Format = TextureFormat::RGBA16F;

			if (mCFAADepth.expired()) mCFAADepth = ATexture::MakeTexture(cfaaDepthStruct);
			else mCFAADepth.lock()->Regenerate(cfaaDepthStruct);

			if (mCFAADiffuse.expired()) mCFAADiffuse = ATexture::MakeTexture(cfaaRGBStruct);
			else mCFAADiffuse.lock()->Regenerate(cfaaRGBStruct);

			if (mCFAANormal.expired()) mCFAANormal = ATexture::MakeTexture(cfaaRGBA16FStruct);
			else mCFAANormal.lock()->Regenerate(cfaaRGBA16FStruct);

			if (mCFAAPosition.expired()) mCFAAPosition = ATexture::MakeTexture(cfaaRGBA16FStruct);
			else mCFAAPosition.lock()->Regenerate(cfaaRGBA16FStruct);

			if (mCFAASMR.expired()) mCFAASMR = ATexture::MakeTexture(cfaaRGBStruct);
			else mCFAASMR.lock()->Regenerate(cfaaRGBStruct);

			glBindFramebuffer(GL_FRAMEBUFFER, mCFAAFBID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mCFAADiffuse.lock()->GetBufferID(), 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mCFAANormal.lock()->GetBufferID(), 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mCFAAPosition.lock()->GetBufferID(), 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, mCFAASMR.lock()->GetBufferID(), 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mCFAADepth.lock()->GetBufferID(), 0);

			if (const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); status != GL_FRAMEBUFFER_COMPLETE) {
				if (!status) {
					const auto result = glGetError();
					throw std::runtime_error("CFAA FRAMEBUFFER CONSTRUCT ERROR | " + std::to_string(result));
				}
				Log::Render.Error("CFAA FRAMEBUFFER CONSTRUCT FAILED | " + std::to_string(status));
				return false;
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	}

	void OpenGLGBuffer::SetShaderTextureIDs() const {
		Common::Ref<Assets::Shader> gbufferWeak;
		Common::Ref<Assets::Shader> lightingWeak;
		AssetManager::Try([&](const Common::Shared<AssetManager>& assetManager) {
			gbufferWeak = assetManager->GetAsset<Assets::Shader>("gbufferShader");
			lightingWeak = assetManager->GetAsset<Assets::Shader>("lightingShader");
		});

		if (!gbufferWeak.expired()) {
			const auto shader = gbufferWeak.lock();
			shader->Activate();
			shader->SetUniformInt("tDiffuse", 0);
			shader->SetUniformInt("tSpecular", 1);
			shader->SetUniformInt("tCFAADiffuse", 2);
			shader->SetUniformInt("tCFAANormal", 3);
			shader->SetUniformInt("tCFAAPosition", 4);
			shader->SetUniformInt("tCFAASMR", 5);
			shader->SetUniformInt("tCFAADepth", 6);
		}

		if (!lightingWeak.expired()) {
			const auto shader = lightingWeak.lock();
			shader->Activate();
			shader->SetUniformInt("gDiffuse", 0);
			shader->SetUniformInt("gNormal", 1);
			shader->SetUniformInt("gPosition", 2);
			shader->SetUniformInt("gSMR", 3);
			shader->SetUniformInt("gDepth", 4);
			shader->SetUniformInt("gCFAAData", 5);
		}
	}

	void OpenGLGBuffer::StartFrame() {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBID);
		glDrawBuffer(GL_COLOR_ATTACHMENT5);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void OpenGLGBuffer::BindCFAAPrepass() {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mCFAAFBID);
		const GLenum prepassBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, prepassBuffers);
	}

	void OpenGLGBuffer::BindGeometryPass() {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBID);
		const GLenum drawBuffs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
		glDrawBuffers(5, drawBuffs);
	}

	void OpenGLGBuffer::BindLightingPass() {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBID);
		glDrawBuffer(GL_COLOR_ATTACHMENT5);
		BindTextures();
	}

	void OpenGLGBuffer::BindFinalPass() {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, mFBID);
		glReadBuffer(GL_COLOR_ATTACHMENT5);
	}

	void OpenGLGBuffer::BindCFAATexturesForSampling() {
		if (!mCFAADiffuse.expired()) mCFAADiffuse.lock()->Activate(2);
		if (!mCFAANormal.expired()) mCFAANormal.lock()->Activate(3);
		if (!mCFAAPosition.expired()) mCFAAPosition.lock()->Activate(4);
		if (!mCFAASMR.expired()) mCFAASMR.lock()->Activate(5);
		if (!mCFAADepth.expired()) mCFAADepth.lock()->Activate(6);
	}

	void OpenGLGBuffer::BindTextures() {
		const auto textures = GetTextureArray();
		for (int i = 0; i < static_cast<int>(textures.size()) - 1; i++) {
			if (textures[i].expired()) continue;
			textures[i].lock()->Activate(i);
		}
	}

	void OpenGLGBuffer::Cleanup() {
		if (mFBID) glDeleteFramebuffers(1, &mFBID);
		if (mCFAAFBID) glDeleteFramebuffers(1, &mCFAAFBID);
		for (const auto textures = GetTextureArray(); auto& tex : textures) {
			if (const auto locked = tex.lock()) locked->Unload();
		}

		for (const auto cfaaTextures = GetCFAATextureArray(); auto& tex : cfaaTextures) {
			if (const auto locked = tex.lock()) locked->Unload();
		}
	}
}
