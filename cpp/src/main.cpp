#include "socket.hpp"
#include <expected>
#include <print>
#include <span>
#include <string_view>

auto main() -> int {
	auto res = webserver::TcpListener::init(7878);
	if (!res) {
		res.error().fatal();
	}

	auto listener = *res;

	while (true) {
		auto msg = listener.get_incoming();
		if (!msg) {
			msg.error().warn();
			continue;
		}

		auto stream = *msg;
		auto bytes = stream.get_bytes();
		std::string_view sv{reinterpret_cast<const char *>(bytes.data()),
							bytes.size()};
		std::println("Message:\n{}\n", sv);

		constexpr std::string_view response = "Hello world";
		const auto res = stream.write(std::as_bytes(std::span(response)));
		if (res) {
			res->log();
		}
	}

	return 0;
}
