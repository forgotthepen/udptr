/*
MIT License

Copyright (c) 2025 forgotthepen (https://github.com/forgotthepen/udptr)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "adapter.hpp"
#include "net-include.hpp"
#include <utility> // std::move
#include <cstdlib> // std::size_t
#include <cstring> // std::memcmp
#include <stdexcept>


#ifdef _MSC_VER
    // link with ws2_32.lib
    #pragma comment(lib, "Ws2_32.lib")
#endif


namespace udptr {
    adapter::adapter(t_endpoint endpoint) noexcept:
        endpoint_( std::move(endpoint) )
    { }

    adapter::~adapter() noexcept {
        close();
    }

    void adapter::use_config(t_endpoint endpoint) noexcept(false) {
        if (common::INVALID_SOCKET_FD != socket_fd_) {
            throw std::runtime_error("Adapter is already opened, close it first");
        }

        endpoint_ = std::move(endpoint);
    }

    const t_endpoint& adapter::get_config() const noexcept {
        return endpoint_;
    }

    int adapter::get_underlying_socket() const noexcept {
        return socket_fd_;
    }

    void adapter::open() noexcept(false) {
        if (socket_fd_ != common::INVALID_SOCKET_FD) {
            return;
        }

#ifdef _MSC_VER
        {
            WSADATA wsa_data{};
            int wsaerr = ::WSAStartup(MAKEWORD(2, 2), &wsa_data);
            if (0 != wsaerr) {
                close();
                throw std::runtime_error("Call to WSAStartup() failed, error code=" + std::to_string(wsaerr));
            }
        }
#endif

        const sockaddr *addr{};
        std::size_t addr_len = 0;
        bool has_ip = false;
        bool has_port = false;

        switch (endpoint_.get_mode()) {
        case e_mode::ip_v4: {
            socket_fd_ = ::socket(AF_INET, SOCK_DGRAM, 0);

            const auto &underlying_addr = endpoint_.get_underlying_addr_v4();
            addr = reinterpret_cast<const sockaddr *>(&underlying_addr);
            addr_len = sizeof(underlying_addr);

            has_ip = INADDR_ANY != underlying_addr.sin_addr.s_addr;
            has_port = 0 != underlying_addr.sin_port;
        }
        break;

        case e_mode::ip_v6: {
            socket_fd_ = ::socket(AF_INET6, SOCK_DGRAM, 0);

            const auto &underlying_addr = endpoint_.get_underlying_addr_v6();
            addr = reinterpret_cast<const sockaddr *>(&underlying_addr);
            addr_len = sizeof(underlying_addr);

            struct in6_addr any_ipv6 = IN6ADDR_ANY_INIT;
            has_ip = 0 != std::memcmp(&any_ipv6, &underlying_addr.sin6_addr, sizeof(underlying_addr.sin6_addr));
            has_port = 0 != underlying_addr.sin6_port;
        }
        break;
        }

        if (socket_fd_ < 0) {
            close();
            throw std::runtime_error("Failed to create a socket file descriptor, error code=" + std::to_string(LAST_NET_ERR()));
        }

        {
            int allow_multicast = 1;
            
            auto val_ptr =
#ifdef _MSC_VER
                reinterpret_cast<const char *>(&allow_multicast)
#else
                reinterpret_cast<const void *>(&allow_multicast)
#endif
            ;

            int socket_opt = ::setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, val_ptr, static_cast<socklen_t>(sizeof(allow_multicast)));
            if (socket_opt < 0) {
                close();
                throw std::runtime_error("Failed to set SO_REUSEADDR option, error code=" + std::to_string(LAST_NET_ERR()));
            }
        }

        if (has_ip || has_port) {
            auto ret = ::bind(socket_fd_, addr, static_cast<socklen_t>(addr_len));
            if (0 != ret) {
                close();
                throw std::runtime_error("Failed to bind a socket file descriptor to adapter, error code=" + std::to_string(LAST_NET_ERR()));
            }
        }
    }

    void adapter::close() noexcept {
        if (common::INVALID_SOCKET_FD != socket_fd_) {
#ifdef _MSC_VER
            ::closesocket(socket_fd_);
#else
            ::close(socket_fd_);
#endif
        }

        socket_fd_ = common::INVALID_SOCKET_FD;
    }
}
