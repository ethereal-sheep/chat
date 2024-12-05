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

private:
    void accept_connection();

    chat_common::acceptor acceptor_;
};

#endif // CHAT_SERVER_H
