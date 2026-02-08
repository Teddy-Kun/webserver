#include "cmd.hpp"
#include "error.hpp"
#include "html_files.hpp"
#include "http/request.hpp"
#include "socket.hpp"
#include "src/http/response.hpp"
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
		std::string_view string_req{
			reinterpret_cast<const char *>(bytes.data()), bytes.size()};
		std::println("Message:\n{}\n", string_req);

		auto req = webserver::HttpRequest::from_string(string_req);
		if (!req) [[unlikely]] {
			req.error().log();
			continue;
		}
		auto http_req = *req;

		if (http_req.method != webserver::HttpMethod::Get) {
			auto resp =
				webserver::HttpResponse(webserver::HttpCode::MethodNotAllowed);
			stream.write(resp);
			continue;
		}

		std::println("{}", http_req.to_string());

		auto file = webserver::get_file(args->dir);

		if (!file) {
			file.error().log();
			continue;
		}

		auto resp = webserver::HttpResponse(
			webserver::HttpCode::Ok,
			std::string_view{reinterpret_cast<char *>(file->data()),
							 file->size()});

		const auto res = stream.write(resp);
		if (res) [[unlikely]] {
			res->log();
		}
	}

	return 0;
}
