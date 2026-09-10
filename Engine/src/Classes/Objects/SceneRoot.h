#pragma once

#include "AObject.h"

namespace Refraction::Objects {
	class SceneRoot : public AObject {
	public:
		SceneRoot() {
			mInstanceName = "SceneRoot";
		}

		// Ticks all objects in this scene. Must pass global objects to tick them too.
		void TickScene(std::vector<Common::Ref<AObject>> globalObjects);
		// Renders all objects in this scene. Must pass global objects to render them too.
		void RenderScene(std::vector<Common::Ref<AObject>> globalObjects);

		// Removes the object/component with the specified UUID
		void Remove(UUID target);

		nlohmann::json Serialise() override;
		void Deserialise(std::string serialised) override;

		std::string GetSerialisedType() override { return "SceneRoot"; }
	private:
		void PreTick(AObject* object);
		void Tick(AObject* object);
		void PostTick(AObject* object);
		void PreRender(AObject* object);
		void Render(AObject* object);
		void PostRender(AObject* object);
	};

	RFCT_OBJECT_REGISTERFACTORY(SceneRoot)
}
