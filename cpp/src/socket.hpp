#pragma once

#include "error.hpp"
#include <cstdint>
#include <expected>
#include <functional>

// OS-Agnostic wrapper around TcpSockets
class TcpListener {
  private:
	std::function<std::expected<std::vector<std::byte>, Error>()> getter;
	TcpListener() = default;

  public:
	static auto init(uint16_t port) noexcept -> std::expected<TcpListener, Error>;
	auto get_incoming() noexcept
		-> std::expected<std::vector<std::byte>, Error>;
};
