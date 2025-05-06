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

#include "endpoint.hpp"
#include "common.hpp"
#include <cstring> // std::memset
#include <stdexcept>


namespace udptr {
    t_endpoint::t_endpoint(e_mode mode) noexcept(false):
        mode_(mode)
    {
        std::memset(&addr_, 0, sizeof(addr_));
        switch (mode) {
        case e_mode::ip_v4: {
            addr_.v4_.sin_family = AF_INET;
        }
        break;

        case e_mode::ip_v6: {
            addr_.v6_.sin6_family = AF_INET6;
        }
        break;
        
        default:
            throw std::runtime_error("Invalid mode");
        break;
        }
    }

    t_endpoint::t_endpoint(e_mode mode, const std::string &ip, uint16_t port) noexcept(false):
        t_endpoint(mode)
    {
        use_ip(ip);
        use_port(port);
    }

    e_mode t_endpoint::get_mode() const noexcept {
        return mode_;
    }

    std::string t_endpoint::get_ip() const noexcept(false) {
        switch (mode_) {
        case e_mode::ip_v4: {
            return common::get_ip(addr_.v4_);
        }
        break;

        case e_mode::ip_v6: {
            return common::get_ip(addr_.v6_);
        }
        break;
        }

        // unreachable
        return {};
    }

    uint16_t t_endpoint::get_port() const noexcept {
        switch (mode_) {
        case e_mode::ip_v4: {
            return common::get_port(addr_.v4_);
        }
        break;

        case e_mode::ip_v6: {
            return common::get_port(addr_.v6_);
        }
        break;
        }

        // unreachable
        return {};
    }

    const sockaddr_in& t_endpoint::get_underlying_addr_v4() const noexcept(false) {
        return const_cast<t_endpoint *>(this)->get_underlying_addr_v4();
    }

    sockaddr_in& t_endpoint::get_underlying_addr_v4() noexcept(false) {
        if (mode_ != e_mode::ip_v4) {
            throw std::runtime_error("Invalid mode for underlying v4 addr");
        }

        return addr_.v4_;
    }

    const sockaddr_in6& t_endpoint::get_underlying_addr_v6() const noexcept(false) {
        return const_cast<t_endpoint *>(this)->get_underlying_addr_v6();
    }

    sockaddr_in6& t_endpoint::get_underlying_addr_v6() noexcept(false) {
        if (mode_ != e_mode::ip_v6) {
            throw std::runtime_error("Invalid mode for underlying v6 addr");
        }

        return addr_.v6_;
    }

    t_endpoint& t_endpoint::use_ip(const std::string &ip) noexcept(false) {
        switch (mode_) {
        case e_mode::ip_v4: {
            common::set_ip(addr_.v4_, ip);
        }
        break;

        case e_mode::ip_v6: {
            common::set_ip(addr_.v6_, ip);
        }
        break;
        }

        return *this;
    }

    t_endpoint& t_endpoint::use_port(uint16_t port) noexcept {
        switch (mode_) {
        case e_mode::ip_v4: {
            common::set_port(addr_.v4_, port);
        }
        break;

        case e_mode::ip_v6: {
            common::set_port(addr_.v6_, port);
        }
        break;
        }

        return *this;
    }

}
