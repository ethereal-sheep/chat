#include "chat_server.h"

#include "chat_common.h"

#include <sstream>

chat_server::chat_server(chat_common::io_context& io_context, uint16_t port)
    : chat_common::chat_base{io_context, "SERVER", "CLIENT"}
    , acceptor_{io_context, chat_common::endpoint(boost::asio::ip::tcp::v4(), port)} {
    std::ostringstream oss;
    oss << "Listening on port " << port << "; Waiting for connection...";
    info(oss.str());
    accept_connection();
}

void chat_server::close() {
    if (!is_closed()) {
        chat_common::chat_base::close();
        boost::asio::post(io_context_, [this]() { acceptor_.close(); });
    }
}

void chat_server::on_error(const chat_common::error_code&) {
    // assume error occurs on disconnect, extend robustness in future
    if (!is_closed()) {
        boost::asio::post(io_context_, [this]() {
            socket_.close();
            info("Client disconnected; Waiting for connection...");
        });
    }
}

void chat_server::accept_connection() {
    if (is_closed()) {
        info("Goodbye!");
        return;
    }
    acceptor_.async_accept([this](boost::system::error_code ec, chat_common::socket new_client) {
        if (!ec) {
            if (socket_.is_open()) {
                // connection already open, do not handle new client
                // TODO send error message to client
                new_client.close();
            } else {
                info("Accepted connection; Starting chat...");
                socket_ = std::move(new_client);
                receive_message();
            }
        }
        accept_connection();
    });
}
