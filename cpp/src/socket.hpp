#pragma once

#include "error.hpp"
#include <cstddef>
#include <cstdint>
#include <expected>
#include <functional>
#include <optional>
#include <span>
#include <vector>

namespace webserver {

class TcpStream {
  private:
	const std::vector<std::byte> data;
	const int client_socket;

  public:
	TcpStream(int socket, std::vector<std::byte> &&data) noexcept;
	auto get_bytes() noexcept -> const std::span<const std::byte>;
	auto write(std::span<const std::byte> response) noexcept
		-> std::optional<Error>;
};

// OS-Agnostic wrapper around TcpSockets
class TcpListener {
  private:
	std::function<std::expected<TcpStream, Error>()> getter;
	TcpListener() = default;

  public:
	static auto init(uint16_t port) noexcept
		-> std::expected<TcpListener, Error>;
	auto get_incoming() noexcept -> std::expected<TcpStream, Error>;
};
} // namespace webserver
