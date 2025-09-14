
//
//    Copyright 2023 Metehan Gezer
//
//     Licensed under the Apache License, Version 2.0 (the "License");
//     you may not use this file except in compliance with the License.
//     You may obtain a copy of the License at
//
//         http://www.apache.org/licenses/LICENSE-2.0
//

#ifndef GIPECS_GUUID_H
#define GIPECS_GUUID_H

#include <array>
#include <cstdint>
#include <string_view>

class gUUID {
public:
	using bytes = std::array<std::uint8_t, 16>;

	gUUID() noexcept;
	explicit gUUID(const bytes& b) noexcept;

	static gUUID generateV4();

	static gUUID fromString(std::string_view s, bool* ok = nullptr) noexcept;

	std::string toString() const;

	bytes toBytes() const noexcept;

	bool isNil() const noexcept { return hi_ == 0 && lo_ == 0; }

	friend bool operator==(const gUUID& a, const gUUID& b) noexcept {
		return a.hi_ == b.hi_ && a.lo_ == b.lo_;
	}

	friend bool operator!=(const gUUID& a, const gUUID& b) noexcept {
		return !(a == b);
	}

	friend bool operator<(const gUUID& a, const gUUID& b) noexcept {
		return a.hi_ < b.hi_ || (a.hi_ == b.hi_ && a.lo_ < b.lo_);
	}


	std::uint64_t hi() const noexcept { return hi_; }

	std::uint64_t lo() const noexcept { return lo_; }

private:
	std::uint64_t hi_;
	std::uint64_t lo_;

	void fromBytes(const bytes& b) noexcept;

};

// hashing support
namespace std {
template<> struct hash<gUUID> {
  size_t operator()(const gUUID& u) const noexcept {
    // 64-bit mix (xorshift64*)
    std::uint64_t x = u.hi() ^ (u.lo() * 0x9E3779B97F4A7C15ull);
    x ^= x >> 30; x *= 0xBF58476D1CE4E5B9ull;
    x ^= x >> 27; x *= 0x94D049BB133111EBull;
    x ^= x >> 31;
    return x;
  }
};
}  // namespace std

#endif