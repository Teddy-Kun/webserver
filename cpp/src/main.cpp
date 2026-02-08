#include "cmd.hpp"
#include "error.hpp"
#include "html_files.hpp"
#include "socket.hpp"
#include "utils/format.hpp"
#include <expected>
#include <print>
#include <span>
#include <string_view>

auto main(int argc, char *argv[]) -> int {
	auto args = webserver::parse_args(argc, argv);
	if (!args) [[unlikely]] {
		args.error().fatal();
	}
	std::println("args {}", *args);

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

		auto file = webserver::get_file(args->dir);

		if (!file) {
			file.error().log();
			continue;
		}

		const auto res = stream.write(std::span(file->data(), file->size()));
		if (res) [[unlikely]] {
			res->log();
		}
	}

	return 0;
}
