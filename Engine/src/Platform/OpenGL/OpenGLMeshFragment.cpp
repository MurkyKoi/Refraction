#include "OpenGLMeshFragment.h"

namespace Refraction::Engine::Platform {
	void OpenGLMeshFragment::Upload() {
		// Create buffers
		glGenVertexArrays(1, &mVAO);
		glGenBuffers(1, &mVBO);
		glGenBuffers(1, &mEBO);

		glBindVertexArray(mVAO);

		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Engine::sVertex), &mVertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);

		// Load vertex data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), static_cast<void *>(nullptr));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), reinterpret_cast<void *>(offsetof(Engine::sVertex, normal)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(sVertex), reinterpret_cast<void *>(offsetof(Engine::sVertex, texCoord)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}

	void OpenGLMeshFragment::Draw() {
		if(const auto mat = mMaterial.lock()) mat->Activate();

		glBindVertexArray(mVAO);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mIndices.size()), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}
}
