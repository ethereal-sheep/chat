#include "chat_common.h"

#include "message.h"
#include "terminal.h"

#include <sstream>
#include <string_view>

namespace chat_common {

constexpr std::string_view INPUT_MARKER = ">> ";

chat_base::chat_base(io_context& io_context,
                     std::string_view this_name,
                     std::string_view other_name)
    : io_context_{io_context}
    , socket_{io_context}
    , closed_{false}
    , this_name_{this_name}
    , other_name_{other_name} {

    info("Use \\exit to exit the application");
}

chat_base::~chat_base() {
    close();
}

void chat_base::close() {
    if (!is_closed()) {
        boost::asio::post(io_context_, [this]() { socket_.close(); });
        closed_.store(true);
    }
}

bool chat_base::is_closed() const {
    return closed_;
}

void chat_base::append_to_buffer(char c) {
    std::scoped_lock lock{mutex_};
    current_msg_.push_back(c);
    terminal::write(c);
}

void chat_base::pop_from_buffer() {
    std::scoped_lock lock{mutex_};
    if (current_msg_.empty()) {
        return;
    }
    current_msg_.pop_back();
    terminal::backspace();
}

void chat_base::enter() {
    std::scoped_lock lock{mutex_};

    // block empty input
    // TODO consider blocking blank input?
    if (current_msg_.empty()) {
        return;
    }

    // clear the current msg in terminal
    terminal::clearln();

    // we can extend checking for more commands here
    // for now we just hardcode "\exit" to be our exit command
    if (current_msg_ == R"(\exit)") {
        close();
    } else if (current_msg_ == R"(\bad)") {
        // debug purposes only, sends a bad message
        // TODO hide behind feature flag
        if (socket_.is_open()) {
            terminal::writeln("[INFO]: Sending bad message!");
            prepare_message(message::make_unsupported());
        }
    } else {
        // if socket is closed, no point preparing a message
        if (socket_.is_open()) {
            terminal::write(this_name_);
            terminal::write(": ");
            terminal::writeln(current_msg_);
            prepare_message(message::make_message(current_msg_));
        } else {
            terminal::writeln("[INFO]: No connection!");
        }
    }

    // we do this here for the case where there are no connections
    // but user tries to send a msg anyways
    terminal::write(INPUT_MARKER);
    current_msg_.clear();
}

void chat_base::info(std::string_view info) {
    std::scoped_lock lock{mutex_};
    terminal::clearln();
    terminal::write("[INFO]: ");
    terminal::writeln(info);
    terminal::write(INPUT_MARKER);
    terminal::write(std::string_view{current_msg_});
}

void chat_base::write_incoming_message(std::string_view msg) {
    std::scoped_lock lock{mutex_};
    terminal::clearln();
    terminal::write(other_name_);
    terminal::write(": ");
    terminal::writeln(msg);
    terminal::write(INPUT_MARKER);
    terminal::write(std::string_view{current_msg_});
}

void chat_base::receive_message() {
    boost::asio::async_read_until(socket_,
                                  buffer_,
                                  DELIMETER,
                                  [this](error_code ec, std::size_t length) {
                                      if (!ec) {
                                          // obtain payload from buffer
                                          std::istream is(&buffer_);
                                          std::string payload;
                                          std::getline(is, payload);
                                          // note: payload has length - 1 bytes, since
                                          // \r (CR) character is culled by std::getline
                                          const auto msg = message::decode(payload, length);
                                          if (!msg) {
                                              // bad message
                                              info("Bad message from sender!");
                                              receive_message();
                                              return;
                                          }

                                          const auto type = msg->type();
                                          if (type == message::ACK) {
                                              // TODO id each sent msg and match ack to msgs by id
                                              if (!pending_ack_msgs_.empty()) {
                                                  info(pending_ack_msgs_.front().acked_string());
                                                  pending_ack_msgs_.pop_front();
                                              } // TODO handle unmatched acknowledgement
                                          } else if (type == message::MSG) {
                                              write_incoming_message(msg->to_string());
                                              acknowledge();
                                          }
                                          receive_message();

                                      } else {
                                          on_error(ec);
                                      }
                                  });
}

void chat_base::connect_endpoint(endpoints endpoints) {
    boost::asio::async_connect(socket_, endpoints, [this](error_code ec, endpoint ep) {
        if (!ec) {
            std::string msg;
            std::ostringstream oss{msg};
            oss << "Successfully connected to " << ep;
            info(oss.str());
            receive_message();
        } else {
            on_error(ec);
        }
    });
}

void chat_base::on_error(const error_code& ec) {
    info(ec.message());
}

void chat_base::acknowledge() {
    boost::asio::async_write(socket_,
                             boost::asio::buffer(message::make_acknowledgement().payload()),
                             [this](error_code ec, std::size_t length) {
                                 if (ec) {
                                     on_error(ec);
                                 }
                             });
}

void chat_base::send_message() {
    boost::asio::async_write(socket_,
                             boost::asio::buffer(pending_send_msgs_.front().payload()),
                             [this](error_code ec, std::size_t) {
                                 if (!ec) {
                                     auto sent_msg = std::move(pending_send_msgs_.front());
                                     pending_send_msgs_.pop_front();
                                     sent_msg.set_sent_time();
                                     pending_ack_msgs_.emplace_back(std::move(sent_msg));
                                     if (!pending_send_msgs_.empty()) {
                                         send_message();
                                     }
                                 } else {
                                     on_error(ec);
                                 }
                             });
}

void chat_base::prepare_message(message message) {
    boost::asio::post(io_context_, [this, msg = std::move(message)]() {
        bool write_in_progress = !pending_send_msgs_.empty();
        pending_send_msgs_.emplace_back(std::move(msg));
        if (!write_in_progress) {
            send_message();
        }
    });
}

} // namespace chat_common