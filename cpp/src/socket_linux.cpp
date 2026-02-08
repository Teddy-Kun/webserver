#include "error.hpp"
#include "socket.hpp"
#include <cstddef>
#include <cstdint>
#include <expected>
#include <format>
#include <netinet/in.h>
#include <optional>
#include <print>
#include <span>
#include <string_view>
#include <sys/socket.h>
#include <sys/types.h>
#include <vector>

namespace webserver {
TcpStream::TcpStream(int client_socket, std::vector<std::byte> &&data)
	: data(std::move(data)), client_socket(client_socket) {}

auto TcpStream::get_bytes() noexcept -> const std::span<const std::byte> {
	return this->data;
}

auto TcpStream::write(const std::span<const std::byte> response)
	-> std::optional<Error> {
	auto http_response = std::format(
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html; charset=UTF-8\r\n"
		"Content-Length: {}\r\n"
		"Connection: close\r\n"
		"\r\n"
		"{}",
		response.size(),
		std::string_view(reinterpret_cast<const char *>(response.data()),
						 response.size()));

	auto bytes_sent = send(this->client_socket, http_response.data(),
						   http_response.size(), 0);

	if (bytes_sent == -1) {
		return std::make_optional<Error>("Error sending response");
	}

	std::println("Send {} bytes", bytes_sent);

	return std::nullopt;
};

auto TcpListener::init(uint16_t port) noexcept
	-> std::expected<TcpListener, Error> {
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
		return std::unexpected<Error>(
			std::in_place, std::format("Error creating socket: {}", server_fd));

	// 3. Define the address (IP and Port)
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY; // Listen on all available interfaces
	address.sin_port =
		htons(port); // Host-to-Network Short (Endianness conversion)

	const auto bind_res =
		bind(server_fd, (struct sockaddr *)&address, sizeof(address));
	if (bind_res < 0)
		return std::unexpected<Error>(std::in_place,
									  std::format("Bind failed: {}", bind_res));

	const auto listen_res = listen(server_fd, 3);
	if (listen_res < 0)
		return std::unexpected<Error>(
			std::in_place, std::format("Listen failed: {}", listen_res));

	std::println("Server is listening on port 7878");

	auto listen = [server_fd]() -> std::expected<TcpStream, Error> {
		// 6. Accept an incoming connection
		sockaddr_in address;
		socklen_t addrlen = sizeof(address);
		auto client_socket =
			accept(server_fd, (struct sockaddr *)&address, &addrlen);
		if (client_socket < 0)
			return std::unexpected<Error>(
				std::in_place, std::format("Accept failed: {}", client_socket));

		// 7. Receive data
		std::vector<std::byte> full_request;
		std::byte buffer[4096];
		bool complete = false;

		while (!complete) {
			ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer));

			if (bytes_read < 0)
				return std::unexpected<Error>(
					std::in_place,
					std::format("Reading request failed: {}", client_socket));
			if (bytes_read == 0)
				return std::unexpected<Error>(std::in_place,
											  "Socket closed by client");

			full_request.insert(full_request.end(), buffer,
								buffer + bytes_read);

			std::string_view current_data(
				reinterpret_cast<const char *>(full_request.data()),
				full_request.size());
			if (current_data.ends_with("\r\n\r\n")) {
				complete = true;
			}
		}

		std::println("Message received: len({})", full_request.size());

		// TODO: If the request has a Body, parse 'Content-Length'
		// and continue reading until 'full_request' contains that many extra
		// bytes.

		return TcpStream(client_socket, std::move(full_request));
	};

	auto listener = TcpListener();
	listener.getter = listen;

	return listener;
}

auto TcpListener::get_incoming() noexcept -> std::expected<TcpStream, Error> {
	return this->getter();
}
} // namespace webserver
