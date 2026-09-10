#pragma once

#include <Core/Singleton.h>

#include "ISerialisable.h"

#define RFCT_ASSET_REGISTERFACTORY(AssetType) \
	namespace { \
		bool AssetType##_registered = Engine::ClassFactory::RegisterAsset<AssetType>([] { \
			return Common::NewShared<AssetType>(); \
		}); \
	}

#define RFCT_OBJECT_REGISTERFACTORY(ObjectType) \
	namespace { \
		bool ObjectType##_registered = Engine::ClassFactory::RegisterObject<ObjectType>([] { \
			return Common::NewShared<ObjectType>(); \
		}); \
	}

#define RFCT_COMPONENT_REGISTERFACTORY(ComponentType) \
	namespace { \
		bool ComponentType##_registered = Engine::ClassFactory::RegisterComponent<ComponentType>([] { \
			return Common::NewShared<ComponentType>(); \
		}); \
	}

namespace Refraction::Assets { class Asset; }
namespace Refraction::Objects { class AObject; }
namespace Refraction::Components { class AComponent; }


namespace Refraction::Engine {
	template<typename T>
	concept IsSerialisable = std::is_base_of_v<ISerialisable, T>;

    class ClassFactory : Singleton<ClassFactory> {
    public:
    	typedef std::function<Common::Shared<Assets::Asset>()> AssetCtor;
    	typedef std::function<Common::Shared<Objects::AObject>()> ObjectCtor;
    	typedef std::function<Common::Shared<Components::AComponent>()> ComponentCtor;

    	template<typename Type>
    	static std::string FindSerialisedTypeName() {
    		const auto key = typeid(Type).name();
		    if (const auto& registry = GetSerialisedNameRegistry(); registry.contains(key)) {
		    	return registry.at(key);
    		}
    		return "";
    	}

    	template<typename AssetType> requires(IsSerialisable<AssetType>)
    	static bool RegisterAsset(const AssetCtor &ctor) {
    		auto temp = AssetType();
    		GetAssetFactoryRegistry()[temp.GetSerialisedType()] = ctor;
    		GetSerialisedNameRegistry()[typeid(AssetType).name()] = temp.GetSerialisedType();
    		return true;
    	}

    	static Common::Shared<Assets::Asset> CreateAsset(const std::string& metatype) {
    		auto& registry = GetAssetFactoryRegistry();
		    if (const auto it = registry.find(metatype); it != registry.end()) return it->second();
    		return nullptr;
    	}

    	template<typename ObjectType> requires(IsSerialisable<ObjectType>)
    	static bool RegisterObject(const ObjectCtor &ctor) {
    		auto temp = ObjectType();
    		GetObjectFactoryRegistry()[temp.GetSerialisedType()] = ctor;
    		GetSerialisedNameRegistry()[typeid(ObjectType).name()] = temp.GetSerialisedType();
    		return true;
    	}

    	static Common::Shared<Objects::AObject> CreateObject(const std::string& metatype) {
    		auto& registry = GetObjectFactoryRegistry();
    		if (const auto it = registry.find(metatype); it != registry.end()) return it->second();
    		return nullptr;
    	}

    	template<typename ComponentType> requires(IsSerialisable<ComponentType>)
    	static bool RegisterComponent(const ComponentCtor &ctor) {
    		auto temp = ComponentType();
    		GetComponentFactoryRegistry()[temp.GetSerialisedType()] = ctor;
    		GetSerialisedNameRegistry()[typeid(ComponentType).name()] = temp.GetSerialisedType();
    		return true;
    	}

    	static Common::Shared<Components::AComponent> CreateComponent(const std::string& metatype) {
    		auto& registry = GetComponentFactoryRegistry();
    		if (const auto it = registry.find(metatype); it != registry.end()) return it->second();
    		return nullptr;
    	}
    private:
    	// Stores `[typeid(type).name()] = typeObj.GetSerialisedType()`
    	static std::unordered_map<std::string, std::string>& GetSerialisedNameRegistry() {
    		static std::unordered_map<std::string, std::string> instance;
    		return instance;
    	}
    	static std::unordered_map<std::string, AssetCtor>& GetAssetFactoryRegistry() {
    		static std::unordered_map<std::string, AssetCtor> instance;
    		return instance;
    	}
    	static std::unordered_map<std::string, ObjectCtor>& GetObjectFactoryRegistry() {
    		static std::unordered_map<std::string, ObjectCtor> instance;
    		return instance;
    	}
    	static std::unordered_map<std::string, ComponentCtor>& GetComponentFactoryRegistry() {
    		static std::unordered_map<std::string, ComponentCtor> instance;
    		return instance;
    	}
    };
}
