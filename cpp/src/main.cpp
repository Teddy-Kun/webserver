#include "socket.hpp"
#include <expected>
#include <print>
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

		auto bytes = *msg;
		std::string_view sv{reinterpret_cast<const char *>(bytes.data()),
							bytes.size()};
		std::println("Message:\n{}\n", sv);
	}

	return 0;
}
