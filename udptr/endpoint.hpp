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


namespace udptr {
    enum class e_mode {
        ip_v4,
        ip_v6,
    };

    class t_endpoint {
    private:
        e_mode mode_;
        union {
            struct sockaddr_in v4_;
            struct sockaddr_in6 v6_;
        } addr_;

    public:
        t_endpoint(e_mode mode) noexcept(false);
        t_endpoint(e_mode mode, const std::string &ip, uint16_t port) noexcept(false);

        e_mode get_mode() const noexcept;
        std::string get_ip() const noexcept(false);
        uint16_t get_port() const noexcept;

        const sockaddr_in& get_underlying_addr_v4() const noexcept(false);
        sockaddr_in& get_underlying_addr_v4() noexcept(false);

        const sockaddr_in6& get_underlying_addr_v6() const noexcept(false);
        sockaddr_in6& get_underlying_addr_v6() noexcept(false);

        t_endpoint& use_ip(const std::string &ip) noexcept(false);
        t_endpoint& use_port(uint16_t port) noexcept;
    };
}
