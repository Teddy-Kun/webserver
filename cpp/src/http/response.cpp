#include "response.hpp"
#include "../utils/format.hpp"
#include <cstdint>
#include <format>
#include <string>

namespace webserver {
auto constexpr to_string(HttpCode code) noexcept -> std::string {
	uint16_t i = static_cast<uint16_t>(code);
	return std::format("{}", i);
}

HttpResponse::HttpResponse(HttpCode code,
						   std::optional<std::string_view> content) noexcept
	: code(std::move(code)), content(std::move(content)) {}

auto HttpResponse::to_string() const noexcept -> std::string {
	if (!this->content)
		return std::format("HTTP/1.1 {} OK\r\n", this->code);

	auto content = *this->content;
	return std::format("HTTP/1.1 {}\r\n"
					   "Content-Type: text/html; charset=UTF-8\r\n"
					   "Content-Length: {}\r\n"
					   "Connection: close\r\n"
					   "\r\n"
					   "{}",
					   this->code, content.size(), content);
}
} // namespace webserver
