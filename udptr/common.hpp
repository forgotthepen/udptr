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

#pragma once

#include "net-include.hpp"
#include <string>
#include <cstdint> // uint16_t
#include <string>


namespace udptr {
namespace common {
    constexpr static const int INVALID_SOCKET_FD = -1;

    std::string get_ip(const struct sockaddr_in &addr) noexcept(false);
    std::string get_ip(const struct sockaddr_in6 &addr) noexcept(false);

    uint16_t get_port(const struct sockaddr_in &addr) noexcept;
    uint16_t get_port(const struct sockaddr_in6 &addr) noexcept;

    void set_ip(struct sockaddr_in &addr, const std::string &ip) noexcept(false);
    void set_ip(struct sockaddr_in6 &addr, const std::string &ip) noexcept(false);
    void map_ipv4_to_ipv6(struct sockaddr_in6 &addr_v6, const sockaddr_in &addr_v4) noexcept;

    void set_port(struct sockaddr_in &addr, uint16_t port) noexcept;
    void set_port(struct sockaddr_in6 &addr, uint16_t port) noexcept;
}
}
