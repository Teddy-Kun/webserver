#include "src/error.hpp"
#include <expected>
#include <filesystem>

namespace webserver {
struct CmdArgs {
	std::filesystem::path dir;
};

auto to_string(CmdArgs args) noexcept -> std::string;

auto parse_args(int argc, char *argv[]) noexcept
	-> std::expected<CmdArgs, Error>;
} // namespace webserver
