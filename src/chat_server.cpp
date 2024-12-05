#include "chat_server.h"
#include "chat_common.h"

chat_server::chat_server(chat_common::io_context& io_context, uint16_t port)
    : chat_common::chat_base{io_context, true},
      acceptor_{io_context,
                chat_common::endpoint(boost::asio::ip::tcp::v4(), port)} {
    accept_connection();
}

void chat_server::accept_connection() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, chat_common::socket socket) {
            if (!ec) {
                std::cout << "Accepted connection" << std::endl;
                set_socket(std::move(socket));
                receive_message();
            }
            accept_connection();
        });
}
