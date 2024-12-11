#include "message.h"

#include <gtest/gtest.h>
#include <string_view>

constexpr auto SAMPLE_MESSAGE = "sample message";

void ASSERT_MESSAGE(const chat_common::message& message) {
    ASSERT_EQ(SAMPLE_MESSAGE, message.to_string());
    ASSERT_EQ(chat_common::message::MSG, message.type());
    ASSERT_EQ(std::string("\2").append(SAMPLE_MESSAGE).append(chat_common::DELIMETER),
              message.payload());
}

void ASSERT_ACKNOWLEDGEMENT(const chat_common::message& message) {
    ASSERT_TRUE(message.to_string().empty());
    ASSERT_EQ(chat_common::message::ACK, message.type());
    ASSERT_EQ(std::string("\1").append(chat_common::DELIMETER), message.payload());
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
    const auto payload = chat_common::message::make_message(SAMPLE_MESSAGE).payload();
    const auto length = payload.length();
    const auto message = chat_common::message::decode(payload, length);
    ASSERT_TRUE(message);
    ASSERT_MESSAGE(message.value());
}

TEST(TestMessage, TestDecodeAcknowledgement) {
    const auto payload = chat_common::message::make_acknowledgement().payload();
    const auto length = payload.length();
    const auto message = chat_common::message::decode(payload, length);
    ASSERT_TRUE(message);
    ASSERT_ACKNOWLEDGEMENT(message.value());
}

TEST(TestMessage, TestDecodeUnsupported) {
    const auto payload = chat_common::message::make_unsupported().payload();
    const auto length = payload.length();
    const auto message = chat_common::message::decode(payload, length);
    ASSERT_FALSE(message);
}

TEST(TestMessage, TestDecodeUnsupportedString) {
    const auto payload = std::string("\3SOMETHINGWRONG").append(chat_common::DELIMETER);
    const auto length = payload.length();
    const auto message = chat_common::message::decode(payload, length);
    ASSERT_FALSE(message);
}