#include "error.hpp"
#include <exception>
#include <optional>
#include <print>
#include <tuple>

Error::~Error() = default;
Error::Error(Error &&) noexcept = default;
Error &Error::operator=(Error &&) noexcept = default;

Error::Error(std::string text, std::optional<std::exception> exception)
	: ptr(std::make_unique<Data>(Data{std::move(text), std::move(exception)})) {
}

auto Error::warn() const noexcept -> void {
	try {
		std::println(stderr, "warn: {}", this->ptr->text);
	} catch (std::exception e) {
		std::ignore = e;
	}
}

auto Error::log() const noexcept -> void {
	try {
		std::println(stderr, "error: {}", this->ptr->text);
	} catch (std::exception e) {
		std::ignore = e;
	}
}

auto Error::fatal() const noexcept -> void {
	try {
		std::println(stderr, "error: {}", this->ptr->text);
	} catch (std::exception e) {
		std::ignore = e;
	}
	std::exit(1);
}
