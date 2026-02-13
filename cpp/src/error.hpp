#pragma once

#include "src/http/response.hpp"
#include <exception>
#include <memory>
#include <string>

namespace webserver {
class [[nodiscard]] Error {
  private:
	struct Data {
		std::string text;
		std::exception_ptr exception;
		HttpCode code;
	};

	// Having data in an internal struct behind a pointer means that error is
	// only 8 bytes on the stack. The reduced size means that if it is used in
	// std::expected, the size of the result is as small as possible. This
	// increases the chance that our data is passed in registers, instead of on
	// the stack. TLDR, its an optimization for the happy path, that accepts heap
	// memory allocation in case of an error. Also allows for very easy, simple
	// and fast move assignments.
	std::unique_ptr<Data> ptr;

	[[nodiscard]] auto to_string() const noexcept -> std::string;

  public:
	~Error() noexcept;
	// Error should always be constructed like this
	// std::unexpected<Error>(std::in_place, ...);
	Error(std::string text, std::exception_ptr exception = nullptr,
		  HttpCode code = HttpCode::InternalServerError);
	// this should be a move only type
	Error(Error &) = delete;
	Error &operator=(Error &) = delete;
	Error(Error &&) noexcept;
	Error &operator=(Error &&) noexcept;

	auto warn() const noexcept -> void;
	auto log() const noexcept -> void;
	auto fatal() const noexcept -> void;
};
} // namespace webserver
