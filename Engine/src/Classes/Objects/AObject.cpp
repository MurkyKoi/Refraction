#include <json.hpp>

#include <Classes/Components/AComponent.h>
#include <Classes/ClassSerialiser.h>

#include "AObject.h"


namespace Refraction::Objects {
	AObject* AObject::GetInstanceWithUUID(const UUID& target, AObject* parent) {
		for (const auto& comp : *parent->GetComponents()) {
			if (comp->GetUUID() == target) return parent;
		}
		AObject* obj = nullptr;
		for (auto& child : *parent->GetChildren()) {
			obj = GetInstanceWithUUID(target, child.get());
		}
		return obj;
	}

	AObject::AObject(const AObject& object) {
		mTransform = object.mTransform;
		mInstanceName = object.mInstanceName;
		mParent = object.mParent;
		mUUID = UUID();

		// Deep copy components and children
		for (auto& comp : object.mComponents) {
			Common::Shared<Components::AComponent> newComp = Common::NewShared<Components::AComponent>(*comp);
			newComp->mParent = this;
			mComponents.push_back(newComp);
		}

		for (auto& child : object.mChildren) {
			Common::Shared<AObject> newChild = Common::NewShared<AObject>(*child);
			newChild->mParent = this;
			mChildren.push_back(newChild);
		}
	}

	AObject::~AObject() {
		mUUID.Reset();
	}

	Common::Shared<AObject> AObject::GetFirstChild(const std::string& name) {
		for (auto& obj : mChildren) {
			if (obj->mInstanceName == name) return obj;
		}
		return nullptr;
	}

	void AObject::AddChild(const Common::Shared<AObject>& child) {
		if (child->mParent == this) return; // Child is already parented to this object
		child->mParent = this;
		mChildren.push_back(child);
	}

	void AObject::Remove() const {
		if (!mParent) return;
		mParent->RemoveChild(mUUID);
	}

	void AObject::RemoveChild(const UUID& target) {
		for (size_t i = 0; i < mChildren.size(); i++) {
			const auto& child = mChildren[i];
			if (child->GetUUID() != target) continue;
			child->mParent = nullptr;
			mChildren.erase(std::next(mChildren.begin(), i));
			return;
		}

		// It isn't a child object, try components
		for (size_t i = 0; i < mComponents.size(); i++) {
			const auto& comp = mComponents[i];
			if (comp->GetUUID() != target) continue;
			comp->mParent = nullptr;
			mComponents.erase(std::next(mComponents.begin(), i));
			return;
		}

		// Well it isn't a child component either. Get mad at the caller.
	}

	Common::Shared<AObject> AObject::Clone() const {
		return Common::NewShared<AObject>(*this);
	}

	nlohmann::json AObject::Serialise() {
		return Utilities::ClassSerialiser::AppendJSON({}, [&](nlohmann::json& json) {
			json["SerialisedType"] = GetSerialisedType();
			json["UUID"] = mUUID.Serialise();
			json["ClassName"] = mClassName;
			json["InstanceName"] = mInstanceName;
			json["Transform"] = Utilities::ClassSerialiser::Serialise(mTransform);
			json["Components"] = {};
			for (const auto& comp : mComponents) {
				json["Components"][comp->GetUUID().Serialise()] = comp->Serialise();
			}
			json["Children"] = {};
			for (const auto& child : mChildren) {
				json["Children"][child->GetUUID().Serialise()] = child->Serialise();
			}

			//Log::SInfo("Serialised object " + mInstanceName + " with UUID " + mUUID.AsString());
		});
	}
	void AObject::Deserialise(std::string serialised) {
		Utilities::ClassSerialiser::TryParseJSON(serialised, [&](nlohmann::json& json) {
			mUUID = UUID::Deserialise(json.at("UUID"));
			mClassName = json.at("ClassName").get<std::string>();
			mInstanceName = json.at("InstanceName").get<std::string>();
			mTransform = Utilities::ClassSerialiser::DeserialiseTransform(json.at("Transform"));
			mComponents.clear();
			for (auto& compData : json.at("Components")) {
				auto newComp = Utilities::ClassSerialiser::DeserialiseComponent(compData.dump());
				newComp->mParent = this;
				mComponents.push_back(newComp);
			}
			mChildren.clear();
			for (auto& childData : json.at("Children")) {
				auto newChild = Utilities::ClassSerialiser::DeserialiseObject(childData.dump());
				newChild->mParent = this;
				mChildren.push_back(newChild);
			}

			//Log::SInfo("Deserialised object " + mInstanceName + " with UUID " + mUUID.AsString());
		});
	}

	Math::Matrix4 AObject::GetWorldMatrix() const {
		if (mParent) return mTransform.ToMatrix() * mParent->GetWorldMatrix();
		return mTransform.ToMatrix();
	}
}
