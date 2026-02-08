#include "socket.hpp"
#include "src/error.hpp"
#include <expected>
#include <filesystem>
#include <fstream>
#include <print>
#include <span>
#include <string_view>
#include <system_error>

auto main() -> int {
	auto res = webserver::TcpListener::init(7878);
	if (!res) [[unlikely]] {
		res.error().fatal();
	}

	auto listener = *res;

	while (true) {
		auto msg = listener.get_incoming();
		if (!msg) [[unlikely]] {
			msg.error().warn();
			continue;
		}

		auto stream = *msg;
		auto bytes = stream.get_bytes();
		std::string_view sv{reinterpret_cast<const char *>(bytes.data()),
							bytes.size()};
		std::println("Message:\n{}\n", sv);

		const std::filesystem::path path = "index.html";

		auto file = std::ifstream(path, std::ios::binary);
		if (!file.is_open()) [[unlikely]] {
			webserver::Error("Could not open file index.html").log();
			continue;
		}

		std::error_code ec;
		const auto size = std::filesystem::file_size(path, ec);
		if (ec) [[unlikely]] {
			const auto msg = std::format("Could not get file size: [{}] {}",
										 ec.value(), ec.message());
			webserver::Error(msg).log();
			continue;
		}

		std::vector<std::byte> buffer(size);

		if (!file.read(reinterpret_cast<char *>(buffer.data()), size))
			[[unlikely]] {
			webserver::Error("Could not read file index.html").log();
			continue;
		}

		const auto res = stream.write(buffer);
		if (res) [[unlikely]] {
			res->log();
		}
	}

	return 0;
}
