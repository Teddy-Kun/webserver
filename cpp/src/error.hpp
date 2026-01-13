#pragma once

#include <exception>
#include <memory>
#include <optional>
#include <string>

class Error {
  private:
	struct Data {
		std::string text;
		std::optional<std::exception> exception;
	};

	std::unique_ptr<Data> ptr;

  public:
	~Error();
	Error(std::string text,
		  std::optional<std::exception> exception = std::nullopt);
	// Error(Error &) noexcept;
	// Error &operator=(Error &) noexcept;
	Error(Error &&) noexcept;
	Error &operator=(Error &&) noexcept;

	auto warn() const noexcept -> void;
	auto log() const noexcept -> void;
	auto fatal() const noexcept -> void;
};
