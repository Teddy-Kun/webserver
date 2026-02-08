#include "src/error.hpp"
#include <expected>
#include <optional>
#include <string_view>

namespace webserver {
enum class HttpMethod {
	Get,
	Head,
	Post,
	Put,
	Delete,
	Connect,
	Options,
	Trace,
	Patch,
};

auto constexpr to_string(HttpMethod method) -> std::string;
auto constexpr http_req_type_from_string(std::string_view str)
	-> std::optional<HttpMethod>;

class HttpRequest {
  public:
	HttpMethod method;
	std::string_view uri;
	std::string_view version;
	std::string_view rest;

	auto static from_string(std::string_view str) noexcept
		-> std::expected<HttpRequest, Error>;

	auto to_string() noexcept -> std::string;

  private:
	HttpRequest(HttpMethod method, std::string_view uri,
				std::string_view version, std::string_view rest);
};
} // namespace webserver
