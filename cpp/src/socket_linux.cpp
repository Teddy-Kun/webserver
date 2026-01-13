#include "error.hpp"
#include "socket.hpp"
#include <expected>
#include <format>
#include <netinet/in.h>
#include <print>
#include <sys/socket.h>
#include <vector>

auto TcpListener::init() noexcept -> std::expected<TcpListener, Error> {
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
		return std::unexpected<Error>(
			std::in_place, std::format("Error creating socket: {}", server_fd));

	// 3. Define the address (IP and Port)
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY; // Listen on all available interfaces
	address.sin_port =
		htons(7878); // Host-to-Network Short (Endianness conversion)

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

	auto listen =
		[server_fd]() -> std::expected<std::vector<std::byte>, Error> {
		// 6. Accept an incoming connection
		sockaddr_in address;
		socklen_t addrlen = sizeof(address);
		int client_socket =
			accept(server_fd, (struct sockaddr *)&address, &addrlen);
		if (client_socket < 0)
			return std::unexpected<Error>(
				std::in_place, std::format("Accept failed: {}", client_socket));

		// 7. Receive data
		std::vector<std::byte> buffer(1024);
		ssize_t bytes_read = read(client_socket, buffer.data(), 1024);
		buffer.resize(bytes_read);
		std::println("Message received: len({})", bytes_read);

		return buffer;
	};

	auto listener = TcpListener();
	listener.getter = listen;

	return listener;
}

auto TcpListener::get_incoming() noexcept
	-> std::expected<std::vector<std::byte>, Error> {
	return this->getter();
}
