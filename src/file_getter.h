#pragma once
#include <filesystem>

namespace file_getter
{

// options
static std::filesystem::path root_dir;

static int get_file_data(const char file_name[], char* buffer, const int buf_size)
{
    if (const auto p = root_dir / file_name; std::filesystem::exists(p)) {}
    return 0;
}
} // namespace file_getter
