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

	auto warn() const -> void;
	auto log() const -> void;
	auto fatal() const -> void;
};
