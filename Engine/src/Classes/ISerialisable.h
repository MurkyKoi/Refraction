#pragma once

#include <string>

namespace Refraction::Engine {
	template <typename Concrete, typename Base>
	class ISerialisable : public Base {
	public:
		using Base::Base;

		static std::string GetSerialisedType() {
			static_assert(std::is_same_v<decltype(Concrete::SerialisedTypeName), const std::string>, "Concrete class must define static constexpr std::string SerialisedTypeName");
			return Concrete::SerialisedTypeName;
		}
	};
}
