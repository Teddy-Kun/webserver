#include "src/error.hpp"
#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <system_error>
#include <vector>

namespace webserver {
auto get_file(std::filesystem::path base_dir, std::string_view uri) noexcept
	-> std::expected<std::vector<std::byte>, Error> {
	auto req = uri.size() ? uri.substr(1) : uri;

	std::filesystem::path path;
	if (req.size() == 0)
		path = base_dir.append("index.html");
	else {
		if (req.contains('.'))
			path = base_dir.append(req);
		else
			path = base_dir.append(std::format("{}.html", req));
	}

	auto file = std::ifstream(path, std::ios::binary);
	if (!file.is_open()) [[unlikely]] {
		path = base_dir.append("404.html");
		file = std::ifstream(path, std::ios::binary);
		if (!file.is_open())
			return std::unexpected<Error>(std::in_place,
										  "Could not open file index.html");
	}

	std::error_code ec;
	const auto size = std::filesystem::file_size(path, ec);
	if (ec) [[unlikely]] {
		const auto msg = std::format("Could not get file size: [{}] {}",
									 ec.value(), ec.message());
		return std::unexpected<Error>(std::in_place, msg);
	}

	std::vector<std::byte> buffer(size);

	if (!file.read(reinterpret_cast<char *>(buffer.data()), size))
		[[unlikely]] {
		return std::unexpected<webserver::Error>(
			std::in_place, "Could not read file index.html");
	}

	return buffer;
}
} // namespace webserver
