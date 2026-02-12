#include "cmd.hpp"
#include "src/error.hpp"
#include <expected>
#include <filesystem>
#include <format>
#include <string>

namespace webserver {
auto to_string(CmdArgs args) noexcept -> std::string {
	return std::format("{{ dir: {} }}", args.dir.string());
}

auto parse_args(int argc, char *argv[]) noexcept
	-> std::expected<CmdArgs, Error> {
	CmdArgs args = CmdArgs{std::filesystem::current_path()};

	if (argc >= 2) {
		auto path = std::filesystem::path(argv[1]);
		if (!std::filesystem::is_directory(path)) [[unlikely]] {
			return std::unexpected<Error>(
				std::in_place,
				std::format("{} is not a directory", path.string()));
		}

		args.dir = path;
	}

	return std::expected<CmdArgs, Error>(std::move(args));
}
} // namespace webserver
