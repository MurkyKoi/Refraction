#pragma once

#include <Core/Singleton.h>
#include <Classes/ISerialisable.h>

#define RFCT_ASSET_REGISTERFACTORY(ClassName, ParentClassName) \
	namespace { \
		bool ClassName##_registered = Engine::ClassFactory::RegisterAsset<ClassName, ParentClassName>( \
			ClassName::GetSerialisedType(), [] { \
			return Common::NewShared<ClassName>(); \
		}); \
	}

#define RFCT_OBJECT_REGISTERFACTORY(ClassName, ParentClassName) \
	namespace { \
		bool ClassName##_registered = Engine::ClassFactory::RegisterObject<ClassName, ParentClassName>( \
			ClassName::GetSerialisedType(), [] { \
			return Common::NewShared<ClassName>(); \
		}); \
	}

#define RFCT_COMPONENT_REGISTERFACTORY(ClassName, ParentClassName) \
	namespace { \
		bool ClassName##_registered = Engine::ClassFactory::RegisterComponent<ClassName, ParentClassName>( \
			ClassName::GetSerialisedType(), [] { \
			return Common::NewShared<ClassName>(); \
		}); \
	}

namespace Refraction::Assets { class Asset; }
namespace Refraction::Objects { class AObject; }
namespace Refraction::Components { class AComponent; }

namespace Refraction::Engine {
    class ClassFactory : Singleton<ClassFactory> {
    public:
    	typedef std::function<Common::Shared<Assets::Asset>()> AssetCtor;
    	typedef std::function<Common::Shared<Objects::AObject>()> ObjectCtor;
    	typedef std::function<Common::Shared<Components::AComponent>()> ComponentCtor;

    	template<typename ClassName, typename ParentClassName>
    	static bool RegisterAsset(const std::string& metatype, const AssetCtor &ctor) {
    		static_assert(std::is_base_of_v<ISerialisable<ClassName, ParentClassName>, ClassName>, "Class does not inherit from ISerialisable.");
    		GetAssetFactoryRegistry()[metatype] = ctor;
    		return true;
    	}

    	static Common::Shared<Assets::Asset> CreateAsset(const std::string& metatype) {
    		auto& registry = GetAssetFactoryRegistry();
		    if (const auto it = registry.find(metatype); it != registry.end()) return it->second();
    		return nullptr;
    	}

    	template<typename ClassName, typename ParentClassName>
    	static bool RegisterObject(const std::string& metatype, const ObjectCtor &ctor) {
    		static_assert(std::is_base_of_v<ISerialisable<ClassName, ParentClassName>, ClassName>, "Class does not inherit from ISerialisable.");
    		GetObjectFactoryRegistry()[metatype] = ctor;
    		return true;
    	}

    	static Common::Shared<Objects::AObject> CreateObject(const std::string& metatype) {
    		auto& registry = GetObjectFactoryRegistry();
    		if (const auto it = registry.find(metatype); it != registry.end()) return it->second();
    		return nullptr;
    	}

    	template<typename ClassName, typename ParentClassName>
    	static bool RegisterComponent(const std::string& metatype, const ComponentCtor &ctor) {
    		static_assert(std::is_base_of_v<ISerialisable<ClassName, ParentClassName>, ClassName>, "Class does not inherit from ISerialisable.");
    		GetComponentFactoryRegistry()[metatype] = ctor;
    		return true;
    	}

    	static Common::Shared<Components::AComponent> CreateComponent(const std::string& metatype) {
    		auto& registry = GetComponentFactoryRegistry();
    		if (const auto it = registry.find(metatype); it != registry.end()) return it->second();
    		return nullptr;
    	}
    private:
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
