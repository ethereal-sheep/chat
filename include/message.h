#ifndef MESSAGE_H
#define MESSAGE_H

#include <chrono>
#include <ratio>
#include <string>

namespace chat_common {

constexpr std::string_view DELIMETER = "\r\n";

/**
message class which represents the payload passed between the sockets
currently sent delimetered by a CRLF, but can be extended to include header info
 */
class message {
public:
    // aliases
    using time_point = std::chrono::time_point<std::chrono::steady_clock>;

    // TODO may extend to support cross network commands
    // kill client, queries, healthchecks, etc
    enum message_type { ACK = 1, MSG = 2 };

    /* factory functions */
    static message make_message(std::string_view msg);
    static message make_acknowledgement();
    // purely for debugging purposes, makes a message that will fail decoding
    static message make_unsupported() {
        return message("", static_cast<message_type>(3));
    }
    // decodes a payload into a message
    // TODO extend header info to include message id so we can ACK the correct msg
    static std::optional<message> decode(std::string_view payload, size_t length);

    /* getter functions */
    std::string payload() const;
    std::string to_string() const;
    std::string acked_string() const;
    message_type type() const;
    std::chrono::microseconds elapsed_us() const;

    /* setter functions */
    void set_sent_time();

private:
    message(std::string_view msg, message_type type);

    std::string payload_;
    time_point sent_time_;
    message_type type_;
};

} // namespace chat_common

#endif // MESSAGE_H
