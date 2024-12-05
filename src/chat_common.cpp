#include "chat_common.h"
#include <sstream>

namespace chat_common {

constexpr std::string_view DELIMETER = "\r\n";

chat_base::chat_base(io_context& io_context,
                     bool keep_alive_on_error /*= false*/)
    : io_context_{io_context}, socket_{io_context}, keep_alive_on_error_{
                                                        keep_alive_on_error} {}

void chat_base::write_message(std::string_view message) {
    std::ostringstream oss;
    oss << message << DELIMETER;
    boost::asio::post(io_context_, [this, msg = oss.str()]() {
        bool write_in_progress = !pending_msgs_.empty();
        pending_msgs_.emplace_back(std::move(msg));
        if (!write_in_progress) {
            send_message();
        }
    });
}

void chat_base::connect_endpoint(endpoints endpoints) {
    boost::asio::async_connect(
        socket_, endpoints, [this](boost::system::error_code ec, endpoint ep) {
            if (!ec) {
                std::cout << "Successfully connected to " << ep << std::endl;
                receive_message();
            } else {
                std::cerr << "Failed to connect"
                          << ", Error code: " << ec.message() << std::endl;
            }
        });
}

void chat_base::set_socket(socket socket) { socket_ = std::move(socket); }

void chat_base::close() {
    boost::asio::post(io_context_, [this]() { socket_.close(); });
}

void chat_base::receive_message() {
    boost::asio::async_read_until(
        socket_, buffer_, "\r\n",
        [this](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                std::istream is(&buffer_);
                std::string line;
                std::getline(is, line);
                std::cout.write(line.data(), length);
                std::cout << "\n";
                receive_message();
            } else {
                if (!keep_alive_on_error_) {
                    socket_.close();
                }
                std::cerr << "Failed to receive: " << ec.message() << std::endl;
            }
        });
}

void chat_base::send_message() {
    boost::asio::async_write(
        socket_,
        boost::asio::buffer(pending_msgs_.front().data(),
                            pending_msgs_.front().length()),
        [this](boost::system::error_code ec, std::size_t) {
            const std::string msg = std::move(pending_msgs_.front());
            if (!ec) {
                pending_msgs_.pop_front();
                if (!pending_msgs_.empty()) {
                    send_message();
                }
                std::cout << "Successfully sent: " << msg << std::endl;
            } else {
                if (!keep_alive_on_error_) {
                    socket_.close();
                }
                std::cerr << "Failed to send: " << msg << std::endl;
            }
        });
}

} // namespace chat_common