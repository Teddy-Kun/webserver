#include "error.hpp"
#include <exception>
#include <format>
#include <print>

Error::~Error() noexcept = default;
Error::Error(Error &&) noexcept = default;
Error &Error::operator=(Error &&) noexcept = default;

Error::Error(std::string text, std::exception_ptr exception)
	: ptr(std::make_unique<Data>(Data{std::move(text), std::move(exception)})) {
}

[[nodiscard]] auto Error::to_string() const noexcept -> std::string {
	if (!this->ptr) [[unlikely]]
		return "invalid error (moved-from)";

	// The usage of std::format technically means that, this function can throw
	// an exception. However this should only happen, if the system runs out of
	// memory, in which case terminating with std::terminate() is probably the
	// best thing that can happen to the system

	if (this->ptr->exception) {
		try {
			std::rethrow_exception(this->ptr->exception);
		} catch (const std::exception &e) {
			const auto buffer = e.what();
			return std::format("{}\nException: {}", this->ptr->text, buffer);
		} catch (const char *s) {
			return std::format("{}\nException: {}", this->ptr->text, s);
		} catch (...) {
			return std::format("{}\nUnknown non-standard exception",
							   this->ptr->text);
		}
	}

	return this->ptr->text;
}

auto Error::warn() const noexcept -> void {
	std::println(stderr, "warn: {}", this->to_string());
}

auto Error::log() const noexcept -> void {
	std::println(stderr, "error: {}", this->to_string());
}

auto Error::fatal() const noexcept -> void {
	std::println(stderr, "error: {}", this->to_string());
	std::exit(1);
}
