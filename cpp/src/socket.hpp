#pragma once

#include "error.hpp"
#include <cstddef>
#include <cstdint>
#include <expected>
#include <functional>
#include <optional>
#include <span>
#include <string_view>
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

class HttpRequest {
  public:
	std::string_view method;
	std::string_view uri;
	std::string_view version;
	std::string_view rest;

	auto static from_string(std::string_view str) noexcept
		-> std::expected<HttpRequest, Error>;

	auto to_string() noexcept -> std::string;

  private:
	HttpRequest(std::string_view method, std::string_view uri,
				std::string_view version, std::string_view rest);
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
