#include "udptr/endpoint.hpp"
#include "udptr/adapter.hpp"
#include "udptr/sender.hpp"
#include "udptr/receiver.hpp"
#include <cstdint>
#include <iostream>
#include <exception>
#include <string>
#include <thread>
#include <chrono>


udptr::adapter my_sender_adapter(udptr::e_mode::v6);

udptr::adapter my_recv_adapter({
    udptr::e_mode::v6,
    "::1", // "127.0.0.1",
    6056
});

int64_t get_now() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

void recv_th() {
    udptr::receiver recc(my_recv_adapter);
    while (true) {
        try {
            auto ret = recc.receive(500);
            if (!ret.timeout) {
                auto t2 = get_now();
                auto t1 = *reinterpret_cast<decltype(t2) *>(ret.data.data());
                auto dd = t2 - t1;
                auto len = ret.data.size() - sizeof(t2);

                if (len > 0) {
                    std::string ss(ret.data.data() + sizeof(t1), len);
                    std::cout
                        << "<< received data size=" << ss.size() << " '" << ss << "'\n"
                        << "   from: " << ret.sender_details.get_ip() << '|' << ret.sender_details.get_port() << '\n'
                        << "   latency = " << dd << "us\n"
                        ;
                }
            }
        } catch (const std::exception &ex) {
            std::cerr << "Error: " << ex.what();
            break;
        }
    }
}

int main() {
    std::cout << "hello" << '\n';

    try {
        my_sender_adapter.open();
        my_recv_adapter.open();
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what();
        return 1;
    }

    udptr::sender sender(my_sender_adapter, my_recv_adapter.get_config());

    auto recv_ttt = std::thread(recv_th);

    while (true) {
        std::cout << "\nType anything and press Enter >> ";

        std::string ss{};
        std::getline(std::cin, ss);

        if ("q" == ss ||"x" == ss ||"quit" == ss ||"exit" == ss) {
            break;
        }

        try {
            auto t1 = get_now();
            ss.insert(0, reinterpret_cast<const char *>(&t1), sizeof(t1));
            sender.send(ss.data(), ss.size());
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        } catch (const std::exception &ex) {
            std::cerr << "Error: " << ex.what();
        }
    }

    my_sender_adapter.close();
    my_recv_adapter.close();

    recv_ttt.join();

    std::cout << "\n\nbye" << '\n';
    return 0;
}
