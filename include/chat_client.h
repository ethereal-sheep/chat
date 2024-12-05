#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include <boost/asio.hpp>
#include <string>

#include "chat_common.h"

class chat_client : public chat_common::chat_base {
public:
    explicit chat_client(chat_common::io_context& io_context,
                         std::string_view host, uint16_t port);

private:
    chat_common::resolver resolver_;
};

#endif // CHAT_CLIENT_H
