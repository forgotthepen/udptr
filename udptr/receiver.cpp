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

#include "receiver.hpp"
#include "common.hpp"
#include "net-include.hpp"
#include <utility>
#include <string>
#include <stdexcept>


#ifdef _MSC_VER
    // link with ws2_32.lib
    #pragma comment(lib, "Ws2_32.lib")
#endif


namespace udptr {
    receiver::t_data::t_data(e_mode mode) noexcept(false):
        sender_details(mode)
    { }

    receiver::receiver(adapter &my_adapter) noexcept(false):
        my_adapter_(my_adapter)
    {
        auto opt_len = static_cast<socklen_t>(sizeof(max_recv_buff_size));

        auto val_ptr =
#ifdef _MSC_VER
            reinterpret_cast<char *>(&max_recv_buff_size)
#else
            reinterpret_cast<void *>(&max_recv_buff_size)
#endif
        ;

        ::getsockopt(my_adapter_.get_underlying_socket(), SOL_SOCKET, SO_RCVBUF, val_ptr, &opt_len);
        if (max_recv_buff_size < MIN_RECEIVE_BUFFER_SIZE) {
            max_recv_buff_size = MIN_RECEIVE_BUFFER_SIZE;
        }

        if (max_recv_buff_size > MAX_RECEIVE_BUFFER_SIZE) {
            max_recv_buff_size = MAX_RECEIVE_BUFFER_SIZE;
        }
    }

    receiver::t_data receiver::receive(int millisec) noexcept(false) {
        t_data ret(my_adapter_.get_config().get_mode());

        struct sockaddr *sender_addr{};
        socklen_t sender_addr_len = 0;
        switch (my_adapter_.get_config().get_mode()) {
        case e_mode::v4: {
            auto &underlying_addr = ret.sender_details.get_underlying_addr_v4();
            sender_addr = reinterpret_cast<struct sockaddr *>(&underlying_addr);
            sender_addr_len = static_cast<socklen_t>(sizeof(underlying_addr));
        }
        break;

        case e_mode::v6: {
            auto &underlying_addr = ret.sender_details.get_underlying_addr_v6();
            sender_addr = reinterpret_cast<struct sockaddr *>(&underlying_addr);
            sender_addr_len = static_cast<socklen_t>(sizeof(underlying_addr));
        }
        break;
        }

        fd_set read_fd_set;
        FD_ZERO(&read_fd_set);
        FD_SET(my_adapter_.get_underlying_socket(), &read_fd_set);

        struct timeval timeout;
        struct timeval *timeout_ptr = nullptr;
        if (millisec >= 0) {
            timeout_ptr = &timeout;

            timeout.tv_sec = millisec / 1000;
            timeout.tv_usec = static_cast<decltype(timeout.tv_usec)>(millisec % 1000) * 1000;
        }

        int select_ret = ::select(my_adapter_.get_underlying_socket() + 1, &read_fd_set, nullptr, nullptr, timeout_ptr);
        if (select_ret < 0) {
            throw std::runtime_error("Failed to poll on the socket file descriptor, error code=" + std::to_string(LAST_NET_ERR()));
        } else if (0 == select_ret) {
            ret.timeout = true;
            return ret;
        }
    
        std::vector<char> data(max_recv_buff_size);
        long long received_bytes = ::recvfrom(
            my_adapter_.get_underlying_socket(),
            data.data(), data.size(), 0,
            sender_addr, &sender_addr_len
        );

        if (received_bytes < 0) {
            throw std::runtime_error("Failed to receive udp packet, error code=" + std::to_string(LAST_NET_ERR()));
        } else {
            if (received_bytes > 0) {
                ret.data.assign(data.begin(), data.begin() + received_bytes);
            }
        }

        return ret;
    }

}
