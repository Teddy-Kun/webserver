#pragma once

#include "src/error.hpp"
#include <expected>
#include <vector>

namespace webserver {
auto get_file() -> std::expected<std::vector<std::byte>, Error>;
};
