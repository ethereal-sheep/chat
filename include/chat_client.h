#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include "chat_common.h"

#include <atomic>
#include <boost/asio.hpp>
#include <string>

class chat_client : public chat_common::chat_base {
public:
    explicit chat_client(chat_common::io_context& io_context, std::string_view host, uint16_t port);

private:
    // currently assumes all errors are disconnection errors
    // TODO handle each possible error properly
    virtual void on_error(const chat_common::error_code& ec) override;

    chat_common::resolver resolver_;
};

#endif // CHAT_CLIENT_H
