#include <iostream>
#include <thread>

#include "chat_server.h"

int main() {
    chat_common::io_context io_context;
    chat_server server(io_context, chat_common::SERVER_PORT);

    std::thread t([&io_context]() { io_context.run(); });
    std::string input;
    while (std::getline(std::cin, input)) {
        server.write_message(input);
        input.clear();
    }
    server.close();
    t.join();

    return 0;
}
