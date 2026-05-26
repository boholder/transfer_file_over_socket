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

/**
 * @return a function that, reads bytes from file into <buffer> and returns the number of read bytes, at every invocation
 */
static std::move_only_function<long long()> build_getter(const char file_name[], char* buffer, const long long buf_size)
{
    if (const auto p = single_file_mode ? root_dir : root_dir / file_name; std::filesystem::exists(p))
    {
        try
        {
            std::ifstream fs(p, std::ios::binary);
            return [fs = std::move(fs), file_name, buffer, buf_size] mutable -> long long // NOLINT(*-exception-escape)
            {
                try
                {
                    fs.read(buffer, buf_size);
                    return fs.gcount();
                }
                catch (const std::exception& e)
                {
                    SPDLOG_ERROR("Error reading file [{}]: {}\n{}", // NOLINT(*-lambda-function-name)
                                 file_name,
                                 e.what(),
                                 std::to_string(std::stacktrace::current()));
                    return 0; // stop further reading
                }
            };
        }
        catch (const std::exception& e)
        {
            SPDLOG_ERROR("Error opening file [{}]: {}\n{}", file_name, e.what(), std::to_string(std::stacktrace::current()));
        }
    }

    return [] { return 0; };
}

} // namespace file_getter
