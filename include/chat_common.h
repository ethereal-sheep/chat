#ifndef CHAT_COMMON_H
#define CHAT_COMMON_H

#include <boost/asio.hpp>
#include <deque>
#include <iostream>
#include <string>

namespace chat_common {

using tcp = boost::asio::ip::tcp;
using socket = tcp::socket;
using acceptor = tcp::acceptor;
using resolver = tcp::resolver;
using endpoint = tcp::endpoint;
using endpoints = resolver::results_type;
using buffer = boost::asio::streambuf;
using io_context = boost::asio::io_context;

// Listening port
constexpr uint16_t SERVER_PORT = 1234;

class chat_base {
public:
    explicit chat_base(io_context& io_context,
                       bool keep_alive_on_error = false);

    // Closes the socket
    virtual void close();

    // Writes a message to be sent asynchronously
    void write_message(std::string_view message);

protected:
    // Connect to an end point and listen asynchronously
    void connect_endpoint(endpoints endpoint);

    // Receives a message from the socket
    void receive_message();

    // Sets the current socket
    void set_socket(socket socket);

private:
    // Sends a message to the socket
    void send_message();

    io_context& io_context_;
    socket socket_;
    buffer buffer_;
    std::deque<std::string> pending_msgs_;
    bool keep_alive_on_error_;
};

} // namespace chat_common
#endif // CHAT_COMMON_H
