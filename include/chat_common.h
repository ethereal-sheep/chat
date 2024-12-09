#ifndef CHAT_COMMON_H
#define CHAT_COMMON_H

#include "message.h"
#include "terminal.h"

#include <boost/asio.hpp>
#include <deque>
#include <iostream>
#include <string>
#include <string_view>

namespace chat_common {

// aliases
using tcp = boost::asio::ip::tcp;
using socket = tcp::socket;
using acceptor = tcp::acceptor;
using resolver = tcp::resolver;
using endpoint = tcp::endpoint;
using endpoints = resolver::results_type;
using buffer = boost::asio::streambuf;
using error_code = boost::system::error_code;
using io_context = boost::asio::io_context;

// listening port
constexpr uint16_t SERVER_PORT = 1234;

/**
base class which handles all message sending between sockets
and handles writing to the terminal
 */
class chat_base {
public:
    explicit chat_base(io_context& io_context,
                       std::string_view this_name,
                       std::string_view other_name);
    virtual ~chat_base();

    // closes the socket
    virtual void close();

    // checks if socket has been closed
    bool is_closed() const;

    /**
    chat writing functions;
    main utility is to make sure our current message written in terminal
    is not affected by incoming messages, and vice versa;
    we do this by saving, clearing, and restoring our current message
    when an incoming message is received and written to the terminal
    */
    // append char to current message buffer
    void append_to_buffer(char c);

    // erase last char in current message buffer
    void pop_from_buffer();

    // sends the current message
    // TODO refactor commands into class; extend possible commands
    void enter();

protected:
    // writes information to chat
    void info(std::string_view info);

    // receives a message from the socket
    void receive_message();

    // connect to an end point and listen asynchronously
    void connect_endpoint(endpoints endpoint);

    // hook to handle errors
    virtual void on_error(const error_code& ec);

private:
    // writes an incoming message to terminal
    void write_incoming_message(std::string_view msg);

    // writes a message to be sent asynchronously
    void prepare_message(message message);

    // sends a message to the socket
    void send_message();

    // respond to an incoming message with an acknowledgement
    void acknowledge();

protected:
    io_context& io_context_;
    socket socket_;

private:
    buffer buffer_;
    std::deque<message> pending_send_msgs_;
    std::deque<message> pending_ack_msgs_;
    std::string current_msg_;
    std::mutex mutex_; // locks current_msg_
    std::atomic_bool closed_;

    const std::string this_name_;
    const std::string other_name_;
};

} // namespace chat_common
#endif // CHAT_COMMON_H
