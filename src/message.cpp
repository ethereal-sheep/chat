#include "message.h"

#include <chrono>
#include <optional>
#include <sstream>

namespace chat_common {

// for now, just use one byte to store the type
constexpr size_t HEADER_LENGTH = 1;

message message::make_message(std::string_view msg) {
    return message(msg, MSG);
}

message message::make_acknowledgement() {
    return message("", ACK);
}

std::optional<message> message::decode(std::string_view payload, size_t length) {
    payload = payload.substr(0, length - DELIMETER.length());
    const auto header = payload.substr(0, HEADER_LENGTH);
    if (header.length() < HEADER_LENGTH) {
        // corrupted payload
        return {};
    }
    const auto first_byte = static_cast<int>(header.front());
    if (first_byte == ACK) {
        return make_acknowledgement();
    } else if (first_byte == MSG) {
        return make_message(payload.substr(HEADER_LENGTH));
    }

    return {};
}

message::message(std::string_view msg, message_type type)
    : payload_{msg}
    , sent_time_{}
    , type_{type} {
}

std::string message::payload() const {
    std::ostringstream oss;
    oss << static_cast<char>(type_) << payload_ << DELIMETER;
    return oss.str();
}

std::string message::to_string() const {
    if (type_ == MSG) {
        return payload_;
    }
    return {};
}

std::string message::acked_string() const {
    std::ostringstream oss;
    oss << "ACK \"" << to_string() << "\" (" << elapsed_us().count() << "us)";
    return oss.str();
}

message::message_type message::type() const {
    return type_;
}

std::chrono::microseconds message::elapsed_us() const {
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() -
                                                                 sent_time_);
}

void message::set_sent_time() {
    sent_time_ = std::chrono::steady_clock::now();
}

} // namespace chat_common