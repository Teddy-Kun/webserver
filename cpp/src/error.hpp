#pragma once

#include <memory>
#include <string>

class Error {
  private:
	struct Data {
		std::string text;
	};

	std::unique_ptr<Data> ptr;

  public:
	~Error();
	Error(std::string text);
	// Error(Error &) noexcept;
	// Error &operator=(Error &) noexcept;
	Error(Error &&) noexcept;
	Error &operator=(Error &&) noexcept;

	auto warn() const noexcept -> void;
	auto log() const noexcept -> void;
	auto fatal() const noexcept -> void;
};
