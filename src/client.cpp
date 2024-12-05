#include "chat_client.h"
#include "chat_common.h"
#include <iostream>
#include <string_view>

int main() {
    constexpr std::string_view host_ip = "127.0.0.1";
    chat_common::io_context io_context;
    chat_client client(io_context, host_ip, chat_common::SERVER_PORT);

    std::thread t([&io_context]() { io_context.run(); });
    std::string input;
    while (std::getline(std::cin, input)) {
        if (input == "exit")
            break;
        client.write_message(input);
        input.clear();
    }
    client.close();
    t.join();

    return 0;
}
