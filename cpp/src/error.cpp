#include "error.hpp"
#include <print>

Error::~Error() = default;
Error::Error(Error &&) noexcept = default;
Error &Error::operator=(Error &&) noexcept = default;

Error::Error(std::string text)
	: ptr(std::make_unique<Data>(Data{std::move(text)})) {}

auto Error::warn() const -> void {
	std::println(stderr, "warn: {}", this->ptr->text);
}

auto Error::log() const -> void {
	std::println(stderr, "error: {}", this->ptr->text);
}

auto Error::fatal() const -> void {
	std::println(stderr, "error: {}", this->ptr->text);
	std::exit(1);
}
