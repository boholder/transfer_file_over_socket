#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#include <spdlog/spdlog.h>

#include <sockpp/tcp_acceptor.h>

#include "constant.h"

namespace tcp_server
{

static bool start_server(sockpp::tcp_acceptor& server, const int port)
{
    const in_port_t port_ = port;
    std::error_code ec;
    server = {port_, 4, sockpp::tcp_acceptor::REUSE, ec};
    if (ec)
    {
        SPDLOG_ERROR("Error creating TCP server: {}", ec.message());
        return false;
    }
    SPDLOG_INFO("TCP server listens on port [{}]", port);
    return true;
}

static void tcp_server_thread(const int port)
{
    sockpp::initialize();

    sockpp::tcp_acceptor server;
    if (!start_server(server, port))
        return;

    while (true)
    {
        // accept a new client connection
        sockpp::inet_address peer;
        if (auto conn_result = server.accept(TCP_SERVER_TIMEOUT, &peer); !conn_result)
        {
            if (conn_result != std::errc::timed_out)
            {
                SPDLOG_ERROR("Error accepting connection: {}", conn_result.error_message());
                return;
            }
        }
        else
        {
            auto peer_addr = peer.to_string();
            SPDLOG_INFO("Accept connection with [{}]", peer_addr);

            sockpp::tcp_socket socket = conn_result.release();
            socket.read_timeout(TCP_SOCKET_TIMEOUT);
            sockpp::result<size_t> r;

            char buf[TCP_SERVER_BUFFER_SIZE];
            auto* const begin = reinterpret_cast<char*>(&buf);
            std::fill_n(begin, TCP_SERVER_BUFFER_SIZE, 0);
            unsigned long long last_len = 0;

            // keep connection alive, but check port changing every TCP_SOCKET_TIMEOUT
            while (true)
            {
                // only zeroing non-zero (used for storing last data) parts in every loop
                std::fill_n(begin, last_len, 0);

                r = socket.read(buf, sizeof(buf)); // blocking I/O until socket timeout

                if (r.value() > 0)
                {
                    SPDLOG_DEBUG("[{}] sends: [{}]", peer_addr, buf);
                    last_len = r.value();
                    Subtitle::append(buf);
                }
                else if (r.error().value() == 0 && r.value() == 0)
                {
                    // a successful read that returns a value of zero indicates that
                    // the peer actively closed the connection
                    // ref: https://github.com/fpagliughi/sockpp/issues/99#issuecomment-4263496155
                    SPDLOG_INFO("Connection with [{}] closed by peer", peer_addr);
                    break; // to listening new connection
                }
                else
                {
                    SPDLOG_ERROR("Error reading from connection with [{}], close it: {}", peer_addr, r.error().message());
                    socket.close();
                    break; // to listening new connection
                }
            }
        }
    }
}

} // namespace tcp_server
