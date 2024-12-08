#include "message.h"

#include <gtest/gtest.h>
#include <string_view>

constexpr auto SAMPLE_MESSAGE = "sample message";

void ASSERT_MESSAGE(const chat_common::message& message) {
    ASSERT_EQ(SAMPLE_MESSAGE, message.to_string());
    ASSERT_EQ(chat_common::message::MSG, message.type());
    ASSERT_EQ(std::string(SAMPLE_MESSAGE).append(chat_common::DELIMETER), message.payload());
}

void ASSERT_ACKNOWLEDGEMENT(const chat_common::message& message) {
    ASSERT_TRUE(message.to_string().empty());
    ASSERT_EQ(chat_common::message::ACK, message.type());
    ASSERT_EQ(std::string(chat_common::ACKNOWLEDGE).append(chat_common::DELIMETER),
              message.payload());
}

TEST(TestMessage, TestMakeMessage) {
    const auto message = chat_common::message::make_message(SAMPLE_MESSAGE);
    ASSERT_MESSAGE(message);
}

TEST(TestMessage, TestMakeAcknowledgement) {
    const auto message = chat_common::message::make_acknowledgement();
    ASSERT_ACKNOWLEDGEMENT(message);
}

TEST(TestMessage, TestDecodeMessage) {
    const auto payload = std::string(SAMPLE_MESSAGE).append(chat_common::DELIMETER);
    const auto length = payload.length();
    const auto message = chat_common::message::decode(payload, length);
    ASSERT_MESSAGE(message);
}

TEST(TestMessage, TestDecodeAcknowledgement) {
    const auto payload = std::string(chat_common::ACKNOWLEDGE).append(chat_common::DELIMETER);
    const auto length = payload.length();
    const auto message = chat_common::message::decode(payload, length);
    ASSERT_ACKNOWLEDGEMENT(message);
}