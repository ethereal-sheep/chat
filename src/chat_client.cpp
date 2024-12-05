#include "chat_client.h"
#include "chat_common.h"
#include <iostream>

chat_client::chat_client(chat_common::io_context& io_context_,
                         std::string_view host_ip, uint16_t port)
    : chat_common::chat_base{io_context_}, resolver_{io_context_} {
    connect_endpoint(resolver_.resolve(host_ip, std::to_string(port)));
}
