#pragma once

#include <string>

namespace Refraction::Engine {
	class ISerialisable {
	public:
		virtual ~ISerialisable() = default;

		virtual std::string GetSerialisedType() = 0;
	};
}
