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

// ReSharper disable once CppDFAUnreachableFunctionCall
static std::filesystem::path combine_path(const char file_name[])
{ return single_file_mode ? root_dir : root_dir / file_name; }

/**
 * @return a function that, reads bytes from file into <buffer> and returns the number of read bytes, at every invocation
 */
static std::move_only_function<long long()> build_getter(const char file_name[], char* buffer, const long long buf_size)
{
    if (const auto path = combine_path(file_name); std::filesystem::exists(path))
    {
        try
        {
            std::ifstream fs(path, std::ios::binary);
            SPDLOG_INFO("Ready to read [{}]", path.string());
            return [fs = std::move(fs), path, buffer, buf_size] mutable -> long long // NOLINT(*-exception-escape)
            {
                try
                {
                    fs.read(buffer, buf_size);
                    const long long c = fs.gcount();
                    if (c == 0)
                        SPDLOG_INFO("Finish reading file [{}]", path.string()); // NOLINT(*-lambda-function-name)
                    return c;
                }
                catch (const std::exception& e)
                {
                    SPDLOG_ERROR("Error reading file [{}]: {}\n{}", // NOLINT(*-lambda-function-name)
                                 path.string(),
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
