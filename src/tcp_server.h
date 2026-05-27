#pragma once

#include <vector>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#include <spdlog/spdlog.h>

#include <sockpp/tcp_acceptor.h>

#include "constant.h"
#include "file_getter.h"

namespace tcp_server
{

// options
static int port;
static std::chrono::seconds socket_timeout;
static int max_connections;

static void write_file_to_socket(sockpp::tcp_socket& socket, const char* file_name)
{
    char data_buf[1024];
    // send the file
    auto getter = file_getter::build_getter(file_name, data_buf, 1024);
    long long data_read_len = 0;
    do // NOLINT(*-avoid-do-while)
    {
        data_read_len = getter();
        socket.write(data_buf, data_read_len);
    } while (data_read_len != 0);
}

static void socket_thread(sockpp::tcp_socket socket, const std::string& peer, const std::chrono::seconds sock_timeout)
{
    socket.read_timeout(sock_timeout);

    if (file_getter::single_file_mode)
        // start transferring immediately after connection established
        write_file_to_socket(socket, "");

    char buf[TCP_SERVER_BUFFER_SIZE];
    auto* const received = reinterpret_cast<char*>(&buf);
    std::fill_n(received, TCP_SERVER_BUFFER_SIZE, 0);

    auto* tail = received;
    unsigned long long total_read_len = 0;

    // keep connection alive, but check port changing every TCP_SOCKET_TIMEOUT
    while (true)
    {
        // blocking I/O until socket timeout
        // accumulate reading, maybe client can't send / server can't read the whole filename
        // within single socket.read()
        if (const sockpp::result<size_t> r = socket.read(tail, TCP_SERVER_BUFFER_SIZE - total_read_len); r.value() > 0)
        {
            SPDLOG_DEBUG("[{}] sends: [{}]", peer, buf);
            const auto read_len = r.value();
            total_read_len += read_len;
            tail = tail + read_len;
            if (std::filesystem::exists(file_getter::combine_path(received)))
                write_file_to_socket(socket, received);
        }
        else if (r.error().value() == 0 && r.value() == 0)
        {
            // a successful read that returns a value of zero indicates that
            // the peer actively closed the connection
            // ref: https://github.com/fpagliughi/sockpp/issues/99#issuecomment-4263496155
            SPDLOG_INFO("Connection with [{}] closed by peer", peer);
            break; // to listening new connection
        }
        else if (r.error().value() == 138)
        {
            SPDLOG_ERROR("Inactive connection with [{}] timed out, close it", peer);
            socket.close();
            break;
        }
        else
        {
            SPDLOG_ERROR("Error reading from connection with [{}], close it: {}", peer, r.error().message());
            socket.close();
            break;
        }
    }
}

static void tcp_server_thread()
{
    sockpp::initialize();

    const in_port_t port_ = port;
    std::error_code ec;
    sockpp::tcp_acceptor server = {port_, 4, sockpp::tcp_acceptor::REUSE, ec};
    if (ec)
    {
        SPDLOG_ERROR("Error creating TCP server: {}", ec.message());
        return;
    }
    SPDLOG_INFO("TCP server listening on port [{}]", port);

    std::vector<std::jthread> socket_threads = {};

    while (true)
    {
        static bool warn_once = true;
        if (socket_threads.size() >= max_connections)
        {
            if (warn_once)
            {
                SPDLOG_WARN("Maximum number of connections ({}) reached, waiting for a connection to close", max_connections);
                warn_once = false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        warn_once = true;

        // waiting for a new client connection
        sockpp::inet_address peer;
        if (auto conn_result = server.accept(TCP_SERVER_TIMEOUT, &peer); !conn_result)
        {
            if (conn_result != std::errc::timed_out)
            {
                SPDLOG_ERROR("Error accepting connection: {}", conn_result.error_message());
            }
            // continue waiting for next connection
        }
        else
        {
            auto peer_addr = peer.to_string();
            SPDLOG_INFO("Accept connection with [{}]", peer_addr);
            // start another sub thread to handle the socket
            socket_threads.emplace_back(socket_thread, conn_result.release(), peer_addr, socket_timeout);
        }
    }
}

} // namespace tcp_server
