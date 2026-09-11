#pragma once

#include <vector>
#include <string>

#include <json.hpp>

#include <Core/Common.h>
#include <Core/UUID.h>
#include <Math/Transform.h>

#include "Classes/ClassFactory.h"
#include "Classes/ISerialisable.h"

namespace Refraction::Components {
	class AComponent;
}

namespace Refraction::Objects {
	class AObject : Engine::ISerialisable {
	public:
		typedef std::vector<Common::Shared<Components::AComponent>> ComponentList;
		typedef std::vector<Common::Shared<AObject>> ObjectList;

		Math::Transform mTransform;
		std::string mInstanceName = "Object";
		AObject* mParent = nullptr;

		// Returns an object under the given parent with the target UUID (or with a component with the target UUID)
		static AObject* GetInstanceWithUUID(const UUID& target, AObject* parent);

		AObject() = default;
		AObject(const AObject& object);
		~AObject() override;

		// Returns a child component of a given type (if it exists)
		template<typename T>
		Common::Shared<T> GetComponent() {
			for (auto& comp : mComponents) {
				if (auto casted = dynamic_pointer_cast<T>(comp)) return casted;
			}
			return nullptr;
		}

		// Returns all children components
		ComponentList* GetComponents() { return &mComponents; }

		// Adds a new child component
		template<typename T>
		Common::Shared<T> AddComponent() {
			Common::Shared<T> newComp = Common::NewShared<T>();
			newComp->mParent = this;
			mComponents.push_back(newComp);
			return newComp;
		}


		// Returns a child object of a given type (if it exists)
		template<typename T>
		Common::Shared<T> GetFirstChild() {
			for (auto& obj : mChildren) {
				if (auto casted = dynamic_pointer_cast<T>(obj)) return casted;
			}
			return nullptr;
		}

		// Returns a child object with a given name (if it exists)
		Common::Shared<AObject> GetFirstChild(const std::string& name);

		// Returns all children objects
		ObjectList* GetChildren() { return &mChildren; }

		// Adds a given child object
		void AddChild(const Common::Shared<AObject>& child);

		// Removes itself from its parent
		void Remove() const;
		// Removes a given child object/component by UUID
		void RemoveChild(const UUID& target);

		// Creates a copy of this object and its descendants
		[[nodiscard]] Common::Shared<AObject> Clone() const;
		
		// Returns the UUID of the object
		[[nodiscard]] UUID GetUUID() const { return mUUID; }

		// Returns the world transform of this object (multiplied with ancestors)
		[[nodiscard]] Math::Matrix4 GetWorldMatrix() const;

		// Returns a serialised copy of the object and its Components and children
		virtual nlohmann::json Serialise();
		// Loads data from the provided serialised object
		virtual void Deserialise(std::string serialised);

		std::string GetSerialisedType() override { return "AObject"; }
	protected:
		std::string mClassName;
		ComponentList mComponents;
		ObjectList mChildren;

	private:
		UUID mUUID;
	};

	RFCT_OBJECT_REGISTERFACTORY(AObject)
}
