#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <thread>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#include <spdlog/spdlog.h>

#include "constant.h"
#include "tcp_server.h"

#define HELP_TEXT                                                                            \
    "Usage: tfos [OPTIONS]\n\n"                                                              \
    "OPTIONS:\n"                                                                             \
    "  -h, --help      Show this help message\n"                                             \
    "  -r, --root      Where the served files are (default: current working directory)\n"    \
    "  -p, --port      Which port to listen on    (default: 18180)\n"                        \
    "  -v, --verbose   Enable debug log\n"                                                   \
    "  -t, --timeout   Inactive socket timeout (after connected) in seconds (default: 60)\n"

static std::string EMPTY;

// options
static std::filesystem::path root_dir;
static int port;
static bool enable_debug_log = false;
static std::chrono::seconds socket_timeout;

namespace
{
/**
 * ref: https://stackoverflow.com/a/868894/23093084
 *
 * @author iain
 */
class InputParser
{
public:
    InputParser(const int& argc, char** argv)
    {
        for (int i = 1; i < argc; ++i)
            this->tokens.emplace_back(argv[i]);
    }

    [[nodiscard]] const std::string& getCmdOption(const std::string& option) const
    {
        if (auto itr = std::ranges::find(this->tokens, option);
            itr != this->tokens.end() && ++itr != this->tokens.end()) // NOLINT(*-inc-dec-in-conditions)
        {
            return *itr;
        }
        return EMPTY;
    }

    [[nodiscard]] bool cmdOptionExists(const std::string& option) const
    { return std::ranges::find(this->tokens, option) != this->tokens.end(); }

private:
    std::vector<std::string> tokens;
};
} // namespace

static void parse_cmd_args(const int argc, char** argv)
{
    const InputParser input(argc, argv);
    std::string option;

#define OPTION_PASSED(SHORT, LONG) input.cmdOptionExists(SHORT) || input.cmdOptionExists(LONG)

#define ASSIGN_OPTION(SHORT, LONG, VARIABLE, DEFAULT)                                          \
    VARIABLE = DEFAULT;                                                                        \
    if (OPTION_PASSED(SHORT, LONG))                                                            \
    {                                                                                          \
        option = input.getCmdOption(SHORT);                                                    \
        if (option.empty())                                                                    \
            option = input.getCmdOption(LONG);                                                 \
        if (option.empty())                                                                    \
        {                                                                                      \
            SPDLOG_ERROR("option [{}] requires a value but not provided, exit", SHORT);        \
            std::cerr << "option [" << (SHORT) << "] requires a value but not provided, exit"; \
            exit(1); /*NOLINT(*-mt-unsafe)*/                                                   \
        }                                                                                      \
        VARIABLE = option;                                                                     \
    }

    if (OPTION_PASSED("-h", "--help"))
    {
        std::cout << HELP_TEXT;
        exit(0); // NOLINT(*-mt-unsafe)
    }

    ASSIGN_OPTION("-r", "--root", root_dir, std::filesystem::current_path());

    std::string port_str;
    ASSIGN_OPTION("-p", "--port", port_str, "18180");
    port = std::stoi(port_str);

    if (OPTION_PASSED("-v", "--verbose"))
        enable_debug_log = true;

    std::string socket_timeout_str;
    ASSIGN_OPTION("-t", "--timeout", socket_timeout_str, "60");
    socket_timeout = std::chrono::seconds(std::stoi(socket_timeout_str));
}

int main(const int argc, char** argv) // NOLINT(*-exception-escape)
{
    spdlog::set_pattern(LOG_PATTERN);

    parse_cmd_args(argc, argv);

    if (enable_debug_log)
        spdlog::set_level(spdlog::level::debug);

    std::jthread tcp_server_thread(tcp_server::tcp_server_thread, port);

    SPDLOG_DEBUG("Log format: [time] level thread-id source-file-and-line: message");
    SPDLOG_INFO("Successfully initialized");
}
