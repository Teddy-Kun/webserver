#include "error.hpp"
#include "socket.hpp"
#include <expected>
#include <netinet/in.h>
#include <print>
#include <sys/socket.h>
#include <vector>

auto TcpListener::init() noexcept -> std::expected<TcpListener, Error> {
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
		return std::unexpected<Error>(std::in_place, "Error creating socket");

	// 3. Define the address (IP and Port)
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY; // Listen on all available interfaces
	address.sin_port =
		htons(7878); // Host-to-Network Short (Endianness conversion)

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		return std::unexpected<Error>(std::in_place, "Bind failed");

	if (listen(server_fd, 3) < 0)
		return std::unexpected<Error>(std::in_place, "Listen failed");

	std::println("Server is listening on port 7878");

	auto listen =
		[server_fd]() -> std::expected<std::vector<std::byte>, Error> {
		// 6. Accept an incoming connection
		sockaddr_in address;
		socklen_t addrlen = sizeof(address);
		int client_socket =
			accept(server_fd, (struct sockaddr *)&address, &addrlen);
		if (client_socket < 0)
			return std::unexpected<Error>(std::in_place, "Accept failed");

		// 7. Receive data
		std::vector<std::byte> buffer(4096);
		ssize_t bytes_read = read(client_socket, buffer.data(), 1024);
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
