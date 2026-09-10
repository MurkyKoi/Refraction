#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>

#include <Core/Common.h>
#include <Core/Time.h>
#include <Core/Utilities.h>

#include "UUID.h"

static std::string IntToHex(const int i, const uint16_t width) {
	std::stringstream stream;
	stream << "0x" << std::setfill('0') << std::setw(width) << std::hex << i;
	return stream.str();
}

namespace Refraction {
	std::unordered_set<UUIDValue> UUID::UUIDHistory = { 0 };

	UUID UUID::FromExisting(const UUIDValue id, const bool ignoreExisting) {
		UUID newID;
		newID.mElapsedSeconds = static_cast<uint16_t>(id >> 48);
		newID.mElapsedMilliseconds = static_cast<uint16_t>(id >> 32);
		newID.mRandomFirst = static_cast<uint16_t>(id >> 16);
		newID.mRandomSecond = static_cast<uint16_t>(id);

		if (!ignoreExisting) {
			if (UUIDHistory.contains(newID.AsInt())) {
				Log::SError("UUID " + newID.AsString() + " already exists, returning Null UUID");
				return Null();
			}
			if (WriteToHistory) UUIDHistory.insert(newID.AsInt());
		}
		return newID;
	}

	UUID UUID::Deserialise(const std::string& serialised) {
		return FromExisting(std::stoull(serialised));
	}

	UUID::UUID() {
		int repeatCounter = 0;

		do {
			mElapsedSeconds = static_cast<uint16_t>(Time::GetSystemSec());
			mElapsedMilliseconds = static_cast<uint16_t>(Time::GetSystemMSec()) + repeatCounter;

			mRandomFirst = static_cast<uint16_t>(Utilities::RandomI(0xFFFF));
			mRandomSecond = static_cast<uint16_t>(Utilities::RandomI(0xFFFF));

			if (UUIDHistory.empty()) break;
			// Increment repeat counter
			repeatCounter++;
		} while (UUIDHistory.contains(AsInt()));

		if (WriteToHistory) UUIDHistory.insert(AsInt());
	}

	UUID::UUID(const UUID& other) {
		mElapsedSeconds = other.mElapsedSeconds;
		mElapsedMilliseconds = other.mElapsedMilliseconds;
		mRandomFirst = other.mRandomFirst;
		mRandomSecond = other.mRandomSecond;
	}

	std::string UUID::AsString() const {
		return IntToHex(mElapsedSeconds, 4) + "-" + IntToHex(mElapsedMilliseconds, 4) + "-" + IntToHex(mRandomFirst, 4) + "-" + IntToHex(mRandomSecond, 4);
	}

	UUIDValue UUID::AsInt() const {
		UUIDValue result = 0;
		result += static_cast<UUIDValue>(mElapsedSeconds) << 48;
		result += static_cast<UUIDValue>(mElapsedMilliseconds) << 32;
		result += static_cast<UUIDValue>(mRandomFirst) << 16;
		result += static_cast<UUIDValue>(mRandomSecond);
		return result;
	}

	void UUID::Reset() {
		// Remove from history to let another UUID take this in the one-in-a-never chance it happens to regenerate
		if(WriteToHistory && AsInt() != 0) UUIDHistory.erase(AsInt());
		mElapsedSeconds = 0;
		mElapsedMilliseconds = 0;
		mRandomFirst = 0;
		mRandomSecond = 0;
	}

	std::string UUID::Serialise() const {
		return std::to_string(AsInt());
	}

	UUID::UUID(const uint16_t initValue) {
		mElapsedSeconds = initValue;
		mElapsedMilliseconds = initValue;
		mRandomFirst = initValue;
		mRandomSecond = initValue;
	}

	namespace Utilities {
		static std::vector<UUIDValue> ToInts(const std::vector<UUID>& uuids) {
			std::vector<UUIDValue> result;
			for (auto& uuid : uuids) {
				result.push_back(uuid.AsInt());
			}
			return result;
		}

		static std::vector<UUID> FromInts(const std::vector<UUIDValue>& uuids) {
			std::vector<UUID> result;
			for (auto& uuid : uuids) {
				result.push_back(UUID::FromExisting(uuid));
			}
			return result;
		}

		static std::vector<std::string> ToStrings(const std::vector<UUID>& uuids) {
			std::vector<std::string> result;
			for (auto& uuid : uuids) {
				result.push_back(uuid.AsString());
			}
			return result;
		}
	}
}