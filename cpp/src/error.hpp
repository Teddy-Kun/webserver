#ifndef ERROR_H
#define ERROR_H

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

	auto warn() -> void;
	auto log() -> void;
	auto fatal() -> void;
};

#endif
