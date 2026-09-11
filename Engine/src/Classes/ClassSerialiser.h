#pragma once

#include <json.hpp>

#include <string>
#include <functional>

#include <Core/Common.h>
#include <Classes/Components/AComponent.h>
#include <Classes/Objects/AObject.h>
#include <Classes/Assets/Asset.h>
#include <Math/Vector.h>
#include <Math/Quaternion.h>
#include <Math/Rect.h>
#include <Math/Frustum.h>
#include <Math/Transform.h>

namespace Refraction::Utilities {
	class ClassSerialiser {
	public:
		// Tries to parse a JSON dump, handles JSON errors
		static void TryParseJSON(const std::string &dump, const std::function<void(nlohmann::json &)> &fn);
		// Appends to a parsed JSON dump, handles JSON errors
		static std::string TryAppendJSON(std::string dump, const std::function<void(nlohmann::json&)> &fn);
		// Appends to a JSON object, handles JSON errors
		static nlohmann::json AppendJSON(nlohmann::json jsonObj, const std::function<void(nlohmann::json&)>& fn);

		static nlohmann::json Serialise(const Common::Shared<Assets::Asset> &asset);
		static nlohmann::json Serialise(const Common::Shared<Objects::AObject>& object);
		static nlohmann::json Serialise(const Common::Shared<Components::AComponent>& comp);
		static Common::Shared<Assets::Asset> DeserialiseAsset(const Common::Shared<Assets::AssetMetadata>& metadata);
		static Common::Shared<Objects::AObject> DeserialiseObject(const std::string &serialisedData);
		static Common::Shared<Components::AComponent> DeserialiseComponent(const std::string &serialisedData);

		template<typename ObjectType>
		static Common::Shared<ObjectType> DeserialiseObject(const std::string& serialisedData) {
			const auto obj = DeserialiseObject(serialisedData);
			return dynamic_pointer_cast<ObjectType>(obj);
		}
		template<typename ComponentType>
		static Common::Shared<ComponentType> DeserialiseComponent(const std::string& serialisedData) {
			const auto comp = DeserialiseComponent(serialisedData);
			return dynamic_pointer_cast<ComponentType>(comp);
		}

		static nlohmann::json Serialise(Math::Vector2 vec);
		static nlohmann::json Serialise(Math::Vector3 vec);
		static nlohmann::json Serialise(Math::Vector4 vec);
		static nlohmann::json Serialise(Math::Quaternion quat);
		static nlohmann::json Serialise(Math::Rect rect);
		static nlohmann::json Serialise(Math::Frustum frustum);
		static nlohmann::json Serialise(const Math::Transform& transform);
		static Math::Vector2 DeserialiseVector2(nlohmann::json data);
		static Math::Vector2 DeserialiseVector2(const std::string& serialisedData);
		static Math::Vector3 DeserialiseVector3(nlohmann::json data);
		static Math::Vector3 DeserialiseVector3(const std::string& serialisedData);
		static Math::Vector4 DeserialiseVector4(nlohmann::json data);
		static Math::Vector4 DeserialiseVector4(const std::string& serialisedData);
		static Math::Quaternion DeserialiseQuaternion(nlohmann::json data);
		static Math::Quaternion DeserialiseQuaternion(const std::string& serialisedData);
		static Math::Rect DeserialiseRect(nlohmann::json data);
		static Math::Rect DeserialiseRect(const std::string& serialisedData);
		static Math::Frustum DeserialiseFrustum(nlohmann::json data);
		static Math::Frustum DeserialiseFrustum(const std::string& serialisedData);
		static Math::Transform DeserialiseTransform(nlohmann::json data);
		static Math::Transform DeserialiseTransform(const std::string& serialisedData);
	};
}


