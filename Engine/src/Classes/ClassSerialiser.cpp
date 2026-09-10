#include <json.hpp>

#include <Classes/ClassHeaders.h>
#include <Interface/AssetManager.h>

#include "ClassSerialiser.h"

using nlohmann::json;

namespace Refraction::Utilities {
	void ClassSerialiser::TryParseJSON(const std::string& dump, const std::function<void(json&)>& fn) {
		try {
			json data = json::parse(dump);
			fn(data);
		} catch (const json::parse_error& err) {
			throw Common::RuntimeError("JSON Parse Error: " + std::string(err.what()));
		} catch (const json::out_of_range& err) {
			throw Common::RuntimeError("JSON Index Error: " + std::string(err.what()));
		} catch (const json::exception& err) {
			throw Common::RuntimeError("JSON Error: " + std::string(err.what()));
		}
	}

	std::string ClassSerialiser::TryAppendJSON(std::string dump, const std::function<void(json&)> &fn) {
		// Replace empty string with empty json to prevent errors
		if (dump.empty()) dump = "{}";
		std::string result;
		TryParseJSON(dump, [&](json& data) {
			fn(data);
			result = data.dump(RFCT_JSON_INDENT);
		});
		return result;
	}

	json ClassSerialiser::AppendJSON(json jsonObj, const std::function<void(json&)>& fn) {
		try {
			fn(jsonObj);
		} catch (const json::parse_error& err) {
			throw Common::RuntimeError("JSON Parse Error: " + std::string(err.what()));
		} catch (const json::out_of_range& err) {
			throw Common::RuntimeError("JSON Index Error: " + std::string(err.what()));
		} catch (const json::exception& err) {
			throw Common::RuntimeError("JSON Error: " + std::string(err.what()));
		}
		return jsonObj;
	}

	json ClassSerialiser::Serialise(const Common::Shared<Assets::Asset> &asset) {
		Common::Ref<Assets::AssetMetadata> metaWeak;
		Engine::AssetManager::Try([&](const Common::Shared<Engine::AssetManager>& assetManager) {
			metaWeak = assetManager->FetchMetadata(asset->GetUUID());
		});
		if (const auto meta = metaWeak.lock()) {
			return meta->Serialise();
		}
		throw Common::RuntimeError("Failed to fetch metadata to serialise");
	}
	json ClassSerialiser::Serialise(const Common::Shared<Objects::AObject>& object) {
		return object->Serialise();
	}

	json ClassSerialiser::Serialise(const Common::Shared<Components::AComponent>& comp) {
		return comp->Serialise();
	}

	Common::Shared<Assets::Asset> ClassSerialiser::DeserialiseAsset(const Common::Shared<Assets::AssetMetadata>& metadata) {
		Common::Shared<Assets::Asset> deserialised = Engine::ClassFactory::CreateAsset(metadata->AssetType);
		Log::SInfo("Loading asset of type " + metadata->AssetType);
		deserialised->LoadAsset(metadata->AssetUUID);
		return deserialised;
	}

	Common::Shared<Objects::AObject> ClassSerialiser::DeserialiseObject(const std::string& serialisedData) {
		std::string objectClassName;
		TryParseJSON(serialisedData, [&](json& data) {
			objectClassName = data.at("SerialisedType").get<std::string>();
		});

		Common::Shared<Objects::AObject> deserialised = Engine::ClassFactory::CreateObject(objectClassName);
		Log::SInfo("Deserialising object of type " + objectClassName);
		deserialised->Deserialise(serialisedData);
		return deserialised;
	}

	Common::Shared<Components::AComponent> ClassSerialiser::DeserialiseComponent(const std::string& serialisedData) {
		std::string compClassName;
		TryParseJSON(serialisedData, [&](json& data) {
			compClassName = data.at("SerialisedType").get<std::string>();
		});

		Common::Shared<Components::AComponent> deserialised = Engine::ClassFactory::CreateComponent(compClassName);
		Log::SInfo("Deserialising component of type " + compClassName);
		deserialised->Deserialise(serialisedData);
		return deserialised;
	}

	json ClassSerialiser::Serialise(Math::Vector2 vec) {
		json result;
		if (vec.x != vec.x) vec.x = 0;
		if (vec.y != vec.y) vec.y = 0;
		result["X"] = vec.x;
		result["Y"] = vec.y;
		return result;
	}
	json ClassSerialiser::Serialise(Math::Vector3 vec) {
		json result;
		if (vec.x != vec.x) vec.x = 0;
		if (vec.y != vec.y) vec.y = 0;
		if (vec.z != vec.z) vec.z = 0;
		result["X"] = vec.x;
		result["Y"] = vec.y;
		result["Z"] = vec.z;
		return result;
	}
	json ClassSerialiser::Serialise(Math::Vector4 vec) {
		json result;
		if (vec.x != vec.x) vec.x = 0;
		if (vec.y != vec.y) vec.y = 0;
		if (vec.z != vec.z) vec.z = 0;
		if (vec.w != vec.w) vec.w = 0;
		result["X"] = vec.x;
		result["Y"] = vec.y;
		result["Z"] = vec.z;
		result["W"] = vec.w;
		return result;
	}
	json ClassSerialiser::Serialise(Math::Quaternion quat) {
		json result;
		if (quat.x != quat.x) quat.x = 0;
		if (quat.y != quat.y) quat.y = 0;
		if (quat.z != quat.z) quat.z = 0;
		if (quat.w != quat.w) quat.w = 0;
		result["X"] = quat.x;
		result["Y"] = quat.y;
		result["Z"] = quat.z;
		result["W"] = quat.w;
		return result;
	}
	json ClassSerialiser::Serialise(Math::Orientation orient) {
		json result;
		if (orient.mPitch != orient.mPitch) orient.mPitch = 0;
		if (orient.mYaw != orient.mYaw) orient.mYaw = 0;
		if (orient.mRoll != orient.mRoll) orient.mRoll = 0;
		result["Pitch"] = orient.mPitch;
		result["Yaw"] = orient.mYaw;
		result["Roll"] = orient.mRoll;
		return result;
	}
	json ClassSerialiser::Serialise(Math::Rect rect) {
		json result;
		result["X"] = rect.x;
		result["Y"] = rect.y;
		result["W"] = rect.w;
		result["H"] = rect.h;
		return result;
	}
	json ClassSerialiser::Serialise(Math::Frustum frustum) {
		json result;
		result["FovY"] = frustum.fovY;
		result["W"] = frustum.w;
		result["H"] = frustum.h;
		result["ZNear"] = frustum.zNear;
		result["ZFar"] = frustum.zFar;
		return result;
	}
	json ClassSerialiser::Serialise(const Math::Transform& transform) {
		json result;
		result["SpatialPosition"]["GridIndex"] = Serialise(transform.mSpatialPosition.GridIndex);
		result["SpatialPosition"]["CellPosition"] = Serialise(transform.mSpatialPosition.CellPosition);
		result["Orientation"] = Serialise(transform.mOrientation);
		result["Scale"] = Serialise(transform.mScale);
		return result;
	}
	Math::Vector2 ClassSerialiser::DeserialiseVector2(json data) {
		return {data.at("X").get<float>(), data.at("Y").get<float>()};
	}
	Math::Vector2 ClassSerialiser::DeserialiseVector2(const std::string& serialisedData) {
		Math::Vector2 result;
		TryParseJSON(serialisedData, [&](const json& jsonObj) {
			result = DeserialiseVector2(jsonObj);
		});
		return result;
	}
	Math::Vector3 ClassSerialiser::DeserialiseVector3(json data) {
		return {data.at("X").get<float>(), data.at("Y").get<float>(), data.at("Z").get<float>()};
	}
	Math::Vector3 ClassSerialiser::DeserialiseVector3(const std::string& serialisedData) {
		Math::Vector3 result;
		TryParseJSON(serialisedData, [&](const json& jsonObj) {
			result = DeserialiseVector3(jsonObj);
		});
		return result;
	}
	Math::Vector4 ClassSerialiser::DeserialiseVector4(json data) {
		return {data.at("X").get<float>(), data.at("Y").get<float>(), data.at("Z").get<float>(), data.at("W").get<float>()};
	}
	Math::Vector4 ClassSerialiser::DeserialiseVector4(const std::string& serialisedData) {
		Math::Vector4 result;
		TryParseJSON(serialisedData, [&](const json& jsonObj) {
			result = DeserialiseVector4(jsonObj);
		});
		return result;
	}
	Math::Quaternion ClassSerialiser::DeserialiseQuaternion(json data) {
		return {data.at("X").get<float>(), data.at("Y").get<float>(), data.at("Z").get<float>(), data.at("W").get<float>()};
	}
	Math::Quaternion ClassSerialiser::DeserialiseQuaternion(const std::string& serialisedData) {
		Math::Quaternion result;
		TryParseJSON(serialisedData, [&](const json& jsonObj) {
			result = DeserialiseQuaternion(jsonObj);
		});
		return result;
	}
	Math::Orientation ClassSerialiser::DeserialiseOrientation(json data) {
		return Math::Vector3(data.at("Pitch").get<float>(), data.at("Yaw").get<float>(), data.at("Roll").get<float>());
	}
	Math::Orientation ClassSerialiser::DeserialiseOrientation(const std::string& serialisedData) {
		Math::Orientation result;
		TryParseJSON(serialisedData, [&](const json& jsonObj) {
			result = DeserialiseOrientation(jsonObj);
		});
		return result;
	}
	Math::Rect ClassSerialiser::DeserialiseRect(json data) {
		return {data.at("X").get<int>(), data.at("Y").get<int>(), data.at("W").get<int>(), data.at("H").get<int>()};
	}
	Math::Rect ClassSerialiser::DeserialiseRect(const std::string& serialisedData) {
		Math::Rect result;
		TryParseJSON(serialisedData, [&](const json& jsonObj) {
			result = DeserialiseRect(jsonObj);
		});
		return result;
	}
	Math::Frustum ClassSerialiser::DeserialiseFrustum(json data) {
		return {Math::Vector2(data.at("W").get<float>(), data.at("H").get<float>()), data.at("FovY").get<float>(), data.at("ZNear").get<float>(), data.at("ZFar").get<float>()};
	}
	Math::Frustum ClassSerialiser::DeserialiseFrustum(const std::string& serialisedData) {
		Math::Frustum result;
		TryParseJSON(serialisedData, [&](const json& jsonObj) {
			result = DeserialiseFrustum(jsonObj);
		});
		return result;
	}
	Math::Transform ClassSerialiser::DeserialiseTransform(json data) {
		Math::Transform result;
		result.mSpatialPosition.GridIndex = DeserialiseVector3(data.at("SpatialPosition").at("GridIndex"));
		result.mSpatialPosition.CellPosition = DeserialiseVector3(data.at("SpatialPosition").at("CellPosition"));
		result.mOrientation = DeserialiseOrientation(data.at("Orientation"));
		result.mScale = DeserialiseVector3(data.at("Scale"));
		return result;
	}
	Math::Transform ClassSerialiser::DeserialiseTransform(const std::string& serialisedData) {
		Math::Transform result;
		TryParseJSON(serialisedData, [&](const json& jsonObj) {
			result = DeserialiseTransform(jsonObj);
		});
		return result;
	}
}
