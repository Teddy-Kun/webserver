#include "error.hpp"
#include <exception>
#include <optional>
#include <print>

Error::~Error() noexcept = default;
Error::Error(Error &&) noexcept = default;
Error &Error::operator=(Error &&) noexcept = default;

Error::Error(std::string text, std::optional<std::exception> exception)
	: ptr(std::make_unique<Data>(Data{std::move(text), std::move(exception)})) {
}

auto Error::warn() const noexcept -> void {
	// println can't error here, since the formatting is hardcoded and valid
	std::println(stderr, "warn: {}", this->ptr->text);
}

auto Error::log() const noexcept -> void {
	// println can't error here, since the formatting is hardcoded and valid
	std::println(stderr, "error: {}", this->ptr->text);
}

auto Error::fatal() const noexcept -> void {
	// println can't error here, since the formatting is hardcoded and valid
	std::println(stderr, "error: {}", this->ptr->text);
	std::exit(1);
}
