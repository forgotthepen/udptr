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

#include "sender.hpp"
#include <string> // std::to_string
#include <cerrno> // errno
#include <stdexcept>


namespace udptr {
    sender::sender(adapter &my_adapter, const t_endpoint &server) noexcept(false):
        my_adapter_(my_adapter),
        server_( ensure_compat_server(server) )
    {
        switch (server_.get_mode()) {
        case e_mode::v4: {
            auto &underlying_addr = server_.get_underlying_addr_v4();
            server_addr = reinterpret_cast<const struct sockaddr *>(&underlying_addr);
            server_addr_len = sizeof(underlying_addr);
        }
        break;

        case e_mode::v6: {
            auto &underlying_addr = server_.get_underlying_addr_v6();
            server_addr = reinterpret_cast<const struct sockaddr *>(&underlying_addr);
            server_addr_len = sizeof(underlying_addr);
        }
        break;
        }
    }

    
    t_endpoint sender::ensure_compat_server(const t_endpoint &server) noexcept(false) {
        const auto my_adapter_mode = my_adapter_.get_config().get_mode();
        const auto server_mode = server.get_mode();
        if (my_adapter_mode == server_mode) {
            return server;
        }

        if (e_mode::v4 == my_adapter_mode && e_mode::v6 == server_mode) {
            throw std::runtime_error("Adapter is using IPv4, but target server is using IPv6, no conversion is possible");
        }

        t_endpoint compat_server(e_mode::v6);
        compat_server.use_port(server.get_port());
        common::map_ipv4_to_ipv6(compat_server.get_underlying_addr_v6(), server.get_underlying_addr_v4());
        return compat_server;
    }

    void sender::send(const char *data, std::size_t size) noexcept(false) {
        long long sent_bytes = ::sendto(
            my_adapter_.get_underlying_socket(),
            data, size, 0,
            server_addr, server_addr_len
        );

        if (sent_bytes < 0) {
            throw std::runtime_error("Failed to send udp packet, errno=" + std::to_string(errno));
        } else if (sent_bytes != size) {
            throw std::runtime_error("Sent only " + std::to_string(sent_bytes) + " out of " + std::to_string(size) + " bytes");
        }
    }

}
