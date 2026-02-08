// source https://github.com/nathan-baggs/ufps/blob/main/src/utils/formatter.h

// Including this file allows the standard formatter to be used on any object
// that either has a member function `std::string to_string()` or has a free
// floating function `std::string to_string(MyType arg)`

#pragma once

#include <concepts>
#include <format>
#include <string>

namespace webserver {

template <class T>
concept HasToStringMember = requires(T a) {
	{ a.to_string() } -> std::convertible_to<std::string>;
};

template <class T>
concept HasToStringFree = requires(T a) {
	{ to_string(a) } -> std::convertible_to<std::string>;
};

namespace util {

struct ToStringCPO {
	template <HasToStringMember T>
	auto operator()(T &&obj) const -> std::string {
		return obj.to_string();
	}

	template <class T>
		requires(!HasToStringMember<T> && HasToStringFree<T>)
	auto operator()(T &&obj) const -> std::string {
		return to_string(obj);
	}
};

inline constexpr auto to_string = ToStringCPO{};

} // namespace util

template <class T> struct Formatter {
	constexpr auto parse(std::format_parse_context &ctx) {
		return std::ranges::begin(ctx);
	}

	auto format(const T &obj, std::format_context &ctx) const {
		return std::format_to(ctx.out(), "{}", util::to_string(obj));
	}
};
} // namespace webserver

template <class T>
concept CanFormat = requires(T a) {
	{ webserver::util::to_string(a) } -> std::convertible_to<std::string>;
};

template <CanFormat T> struct std::formatter<T> : webserver::Formatter<T> {};
