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

#include "adapter.hpp"
#include "endpoint.hpp"
#include <vector>


namespace udptr {
    class receiver {
        private:
            const adapter *my_adapter_;
            unsigned long recv_buff_size = 0;

        public:
            constexpr static unsigned long MIN_RECEIVE_BUFFER_SIZE = 8192;
            constexpr static unsigned long MAX_RECEIVE_BUFFER_SIZE = 65536;

            struct t_data {
                bool timeout = false;
                std::vector<char> data{};
                t_endpoint sender_details;

                t_data(e_mode mode) noexcept(false);
            };

            receiver(const adapter &my_adapter) noexcept(false);
            t_data receive(int millisec = -1) noexcept(false);
        };
}
