#pragma once

#include "src/error.hpp"
#include <expected>
#include <filesystem>
#include <vector>

namespace webserver {
auto get_file(std::filesystem::path base_dir) noexcept
	-> std::expected<std::vector<std::byte>, Error>;
};
