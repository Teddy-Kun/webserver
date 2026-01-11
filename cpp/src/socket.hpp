#ifndef SOCKET_H
#define SOCKET_H

#include "error.hpp"
#include <expected>
#include <functional>

// OS-Agnostic wrapper around TcpSockets
class TcpListener {
  private:
	std::function<std::expected<std::vector<std::byte>, Error>()> getter;
	TcpListener() = default;

  public:
	static auto init() -> std::expected<TcpListener, Error>;
	auto get_incoming() -> std::expected<std::vector<std::byte>, Error>;
};

#endif
