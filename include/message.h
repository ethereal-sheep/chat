#ifndef MESSAGE_H
#define MESSAGE_H

#include <chrono>
#include <ratio>
#include <string>

namespace chat_common {

constexpr std::string_view DELIMETER = "\r\n";
constexpr std::string_view ACKNOWLEDGE = "ACK\1";

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
    enum message_type { ACK = 0, MSG };

    /* factory functions */
    static message make_message(std::string_view msg);
    static message make_acknowledgement();
    // decodes a payload into a message,
    // TODO extend to support header info
    static message decode(std::string_view payload, size_t length);

    /* getter functions */
    // TODO extend to support header info
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
