#include "chat_client.h"
#include "chat_common.h"
#include "chat_server.h"
#include "message.h"
#include "terminal.h"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <thread>

constexpr std::string_view USAGE = "Usage: chat <server|client [ip]>";
constexpr std::string_view DEFAULT_HOST = "127.0.0.1";

std::unique_ptr<chat_common::chat_base> handle_clargs(chat_common::io_context& io_context,
                                                      int argc,
                                                      char** argv) {
    if (argc < 2) {
        return {};
    }

    const std::string_view mode{argv[1]};
    if (mode == "server") {
        return std::make_unique<chat_server>(io_context, chat_common::SERVER_PORT);
    }

    if (mode == "client") {
        std::string_view host = DEFAULT_HOST;
        if (argc == 3) {
            host = argv[2];
        }
        return std::make_unique<chat_client>(io_context, host, chat_common::SERVER_PORT);
    }

    return {};
}

int main(int argc, char** argv) {
    try {
        terminal::enable_raw_mode();

        chat_common::io_context io_context;
        const auto chat = handle_clargs(io_context, argc, argv);
        if (!chat) {
            terminal::writeln(USAGE);
            return 1;
        }

        std::thread t([&io_context]() { io_context.run(); });

        terminal::poll_for_input(
            [&](terminal::input_event event) {
                if (event.backspace) {
                    chat->pop_from_buffer();
                } else if (event.enter) {
                    chat->enter();
                } else if (event.ctrl) {
                } else {
                    chat->append_to_buffer(event.c);
                }
            },
            [&]() { return chat->is_closed(); });

        chat->close();
        t.join();
    } catch (const std::exception& e) {
        // TODO handle "address in use" error
        terminal::clearln();
        terminal::writeln(e.what());
        return 1;
    }

    return 0;
}
