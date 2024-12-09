#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include "chat_common.h"

#include <atomic>
#include <boost/asio.hpp>
#include <functional>
#include <memory>

class chat_server : public chat_common::chat_base {
public:
    explicit chat_server(chat_common::io_context& io_context, uint16_t port);

    virtual void close() override;

protected:
    // currently assumes all errors are disconnection errors
    // TODO handle each possible error properly
    virtual void on_error(const chat_common::error_code& ec) override;

private:
    // accepts an incoming connection
    void accept_connection();

    chat_common::acceptor acceptor_;
};

#endif // CHAT_SERVER_H
