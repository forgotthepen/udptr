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

#include "common.hpp"
#include <cstring> // std::memset, std::memcpy
#include <stdexcept>


#ifdef _MSC_VER
    // link with ws2_32.lib
    #pragma comment(lib, "Ws2_32.lib")
#endif


namespace udptr {
namespace common {
    std::string get_ip(const struct sockaddr_in &addr) noexcept(false) {
        char ip[INET_ADDRSTRLEN]{};
        auto ret = ::inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));
        if (nullptr == ret) {
            throw std::runtime_error("IPv4 address conversion failed, error code=" + std::to_string(LAST_NET_ERR()));
        }
        return ip;
    }

    std::string get_ip(const struct sockaddr_in6 &addr) noexcept(false) {
        char ip[INET6_ADDRSTRLEN]{};
        auto ret = ::inet_ntop(AF_INET6, &addr.sin6_addr, ip, sizeof(ip));
        if (nullptr == ret) {
            throw std::runtime_error("IPv6 address conversion failed, error code=" + std::to_string(LAST_NET_ERR()));
        }
        return ip;
    }

    uint16_t get_port(const struct sockaddr_in &addr) noexcept {
        return ::ntohs(addr.sin_port);
    }

    uint16_t get_port(const struct sockaddr_in6 &addr) noexcept {
        return ::ntohs(addr.sin6_port);
    }

    void set_ip(struct sockaddr_in &addr, const std::string &ip) noexcept(false) {
        auto ret = ::inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
        if (ret <= 0) {
            throw std::runtime_error("IPv4 address conversion failed, error code=" + std::to_string(LAST_NET_ERR()));
        }
    }

    void set_ip(struct sockaddr_in6 &addr, const std::string &ip) noexcept(false) {
        auto ret = ::inet_pton(AF_INET6, ip.c_str(), &addr.sin6_addr);
        if (ret <= 0) {
            throw std::runtime_error("IPv6 address conversion failed, error code=" + std::to_string(LAST_NET_ERR()));
        }
    }

    void map_ipv4_to_ipv6(struct sockaddr_in6 &addr_v6, const sockaddr_in &addr_v4) noexcept {
        // https://learn.microsoft.com/en-us/dotnet/api/system.net.ipaddress.isipv4mappedtoipv6
        std::memset(&addr_v6.sin6_addr, 0, sizeof(addr_v6.sin6_addr));
        addr_v6.sin6_addr.s6_addr[10] = 0xFF;
        addr_v6.sin6_addr.s6_addr[11] = 0xFF;
        std::memcpy(&addr_v6.sin6_addr.s6_addr[12], &addr_v4.sin_addr, sizeof(addr_v4.sin_addr));
    }

    void set_port(struct sockaddr_in &addr, uint16_t port) noexcept {
        addr.sin_port = ::htons(port);
    }

    void set_port(struct sockaddr_in6 &addr, uint16_t port) noexcept {
        addr.sin6_port = ::htons(port);
    }

}
}
