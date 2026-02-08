#include "request.hpp"
#include "../utils/format.hpp"
#include <expected>
#include <format>
#include <string>

namespace webserver {
auto constexpr to_string(HttpMethod method) -> std::string {
	switch (method) {
	case HttpMethod::Get:
		return "GET";
	case HttpMethod::Head:
		return "HEAD";
	case HttpMethod::Post:
		return "POST";
	case HttpMethod::Put:
		return "PUT";
	case HttpMethod::Delete:
		return "DELETE";
	case HttpMethod::Connect:
		return "CONNECT";
	case HttpMethod::Options:
		return "OPTIONS";
	case HttpMethod::Trace:
		return "TRACE";
	case HttpMethod::Patch:
		return "PATCH";
	}
	return "ILLEGAL HTTP REQUEST TYPE";
}

auto constexpr http_req_type_from_string(std::string_view str)
	-> std::optional<HttpMethod> {
	// early exit without comparing strings
	if (str.length() < 3 || str.length() > 7) [[unlikely]] {
		return std::nullopt;
	}

	if (str == "GET")
		return HttpMethod::Get;
	if (str == "HEAD")
		return HttpMethod::Head;
	if (str == "POST")
		return HttpMethod::Post;
	if (str == "PUT")
		return HttpMethod::Put;
	if (str == "DELETE")
		return HttpMethod::Delete;
	if (str == "CONNECT")
		return HttpMethod::Connect;
	if (str == "OPTIONS")
		return HttpMethod::Options;
	if (str == "TRACE")
		return HttpMethod::Trace;
	if (str == "PATCH")
		return HttpMethod::Patch;

	return std::nullopt;
}

HttpRequest::HttpRequest(HttpMethod method, std::string_view uri,
						 std::string_view version, std::string_view rest)
	: method(method), uri(std::move(uri)), version(std::move(version)),
	  rest(std::move(rest)) {}

auto HttpRequest::from_string(std::string_view str) noexcept
	-> std::expected<HttpRequest, Error> {
	auto find_next = [&](size_t start) -> size_t {
		return str.find(' ', start);
	};

	size_t method_end = find_next(0);
	if (method_end == std::string_view::npos) [[unlikely]] {
		return std::unexpected<Error>(std::in_place, "illegal HTTP header");
	}

	size_t uri_end = find_next(method_end + 1);
	if (uri_end == std::string_view::npos) [[unlikely]] {
		return std::unexpected<Error>(std::in_place, "illegal HTTP header");
	}

	size_t version_end = find_next(uri_end + 1);
	if (version_end == std::string_view::npos) [[unlikely]] {
		return std::unexpected<Error>(std::in_place, "illegal HTTP header");
	}

	auto method_str = str.substr(0, method_end);
	std::optional<HttpMethod> method = http_req_type_from_string(method_str);
	if (!method) [[unlikely]] {
		return std::unexpected<Error>(
			std::in_place, std::format("illegal http method {}", method_str));
	}

	return HttpRequest(*method,
					   str.substr(method_end + 1, uri_end - method_end - 1),
					   str.substr(uri_end + 1, version_end - uri_end - 1),
					   str.substr(version_end + 1));
}

auto HttpRequest::to_string() noexcept -> std::string {
	return std::format("METHOD: {}; URI: {}; VERSION: {}", this->method,
					   this->uri, this->version);
}

} // namespace webserver
