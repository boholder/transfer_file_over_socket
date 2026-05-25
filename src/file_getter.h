#pragma once
#include <filesystem>
#include <fstream>

namespace file_getter
{

// options
static std::filesystem::path root_dir;
static bool single_file_mode = false;

static std::function<bool()> build_getter(const char file_name[], char* buffer, const int buf_size)
{
    if (const auto p = single_file_mode ? root_dir : root_dir / file_name; std::filesystem::exists(p))
    {
        std::ifstream fs(p, std::ios::binary);
        return [&fs, buffer, buf_size]
        {
            fs.read(buffer, buf_size);
            return fs.eof();
        };
    }

    return [] { return true; };
}

} // namespace file_getter
