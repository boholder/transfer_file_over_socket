#pragma once

#define APP_VERSION "0.1.0"
#define APP_LINK "https://github.com/boholder/transfer_file_over_socket"

// [26-10-31 23:46:59.678] shorten-level thread-id source-file-and-line: message
// ref: https://github.com/gabime/spdlog/wiki/Custom-formatting
#define LOG_PATTERN "%^[%C-%m-%d %T.%e] %L %-5t %-8!s:%-4#: %v%$"

#define TCP_SERVER_TIMEOUT std::chrono::seconds(60)
#define TCP_SOCKET_TIMEOUT std::chrono::seconds(3)
#define TCP_SERVER_BUFFER_SIZE 10240
