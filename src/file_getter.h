#pragma once

#include <filesystem>
#include <fstream>
#include <stacktrace>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#include <spdlog/spdlog.h>

namespace file_getter
{

// options
static std::filesystem::path root_dir;
static bool single_file_mode = false;

static std::move_only_function<bool()> build_getter(const char file_name[], char* buffer, const int buf_size)
{
    const auto p = single_file_mode ? root_dir : root_dir / file_name;
    if (std::filesystem::exists(p))
    {
        try
        {
            std::ifstream fs(p, std::ios::binary);
            return [fs = std::move(fs), file_name, buffer, buf_size] mutable // NOLINT(*-exception-escape)
            {
                try
                {
                    fs.read(buffer, buf_size);

                }
                catch (const std::exception& e)
                {
                    SPDLOG_ERROR("Error reading file [{}]: {}\n{}", // NOLINT(*-lambda-function-name)
                                 file_name,
                                 e.what(),
                                 std::to_string(std::stacktrace::current()));
                    return true; // stop further reading
                }
                return fs.eof();
            };
        }
        catch (const std::exception& e)
        {
            SPDLOG_ERROR("Error opening file [{}]: {}\n{}", file_name, e.what(), std::to_string(std::stacktrace::current()));
        }
    }

    return [] { return true; };
}

} // namespace file_getter
