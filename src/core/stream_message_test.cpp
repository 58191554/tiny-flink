// Keep these first to verify that the header is self-contained and repeatable.
#include "core/stream_message.h"
#include "core/stream_message.h"

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <type_traits>
#include <utility>

namespace {

using tiny_flink::EndOfInput;
using tiny_flink::Record;
using tiny_flink::StreamMessage;
using MoveOnlyMessage = StreamMessage<std::unique_ptr<int>>;

TEST(StreamMessageTest, HoldsRecordWithItsPayload) {
    const StreamMessage<int> message = Record<int>{42};

    ASSERT_TRUE(std::holds_alternative<Record<int>>(message));
    EXPECT_FALSE(std::holds_alternative<EndOfInput>(message));
    EXPECT_EQ(std::get<Record<int>>(message).value(), 42);
}

TEST(StreamMessageTest, HoldsEndOfInputWithoutConstructingPayload) {
    const MoveOnlyMessage message = EndOfInput{};

    EXPECT_TRUE(std::holds_alternative<EndOfInput>(message));
    EXPECT_EQ(std::get_if<Record<std::unique_ptr<int>>>(&message), nullptr);
}

TEST(StreamMessageTest, NullPayloadIsARecordRatherThanEndOfInput) {
    const MoveOnlyMessage message = Record<std::unique_ptr<int>>{nullptr};

    ASSERT_TRUE(std::holds_alternative<Record<std::unique_ptr<int>>>(message));
    EXPECT_EQ(std::get<Record<std::unique_ptr<int>>>(message).value(), nullptr);
    EXPECT_FALSE(std::holds_alternative<EndOfInput>(message));
}

TEST(StreamMessageTest, CopyConstructionPreservesRecordAndCopiesPayload) {
    const StreamMessage<std::string> original = Record<std::string>{"payload"};
    const auto copied = original;

    ASSERT_TRUE(std::holds_alternative<Record<std::string>>(copied));
    const auto& original_value = std::get<Record<std::string>>(original).value();
    const auto& copied_value = std::get<Record<std::string>>(copied).value();
    EXPECT_EQ(copied_value, "payload");
    EXPECT_EQ(original_value, copied_value);
    EXPECT_NE(std::addressof(original_value), std::addressof(copied_value));
}

TEST(StreamMessageTest, CopyConstructionPreservesEndOfInput) {
    const StreamMessage<int> original = EndOfInput{};
    const auto copied = original;

    EXPECT_TRUE(std::holds_alternative<EndOfInput>(copied));
    EXPECT_TRUE(std::holds_alternative<EndOfInput>(original));
}

TEST(StreamMessageTest, CopyAssignmentCanSwitchBetweenAlternatives) {
    const StreamMessage<int> record = Record<int>{42};
    const StreamMessage<int> end = EndOfInput{};
    StreamMessage<int> message = end;

    message = record;
    ASSERT_TRUE(std::holds_alternative<Record<int>>(message));
    EXPECT_EQ(std::get<Record<int>>(message).value(), 42);

    message = end;
    EXPECT_TRUE(std::holds_alternative<EndOfInput>(message));
}

TEST(StreamMessageTest, MoveConstructionTransfersMoveOnlyPayload) {
    auto payload = std::make_unique<int>(42);
    const auto* original_pointer = payload.get();
    MoveOnlyMessage original = Record<std::unique_ptr<int>>{std::move(payload)};
    EXPECT_EQ(payload, nullptr);

    const MoveOnlyMessage moved(std::move(original));

    ASSERT_TRUE(std::holds_alternative<Record<std::unique_ptr<int>>>(moved));
    const auto& value = std::get<Record<std::unique_ptr<int>>>(moved).value();
    ASSERT_EQ(value.get(), original_pointer);
    EXPECT_EQ(*value, 42);
}

TEST(StreamMessageTest, MoveAssignmentTransfersMoveOnlyPayload) {
    MoveOnlyMessage original = Record<std::unique_ptr<int>>{std::make_unique<int>(42)};
    const auto* original_pointer =
        std::get<Record<std::unique_ptr<int>>>(original).value().get();
    MoveOnlyMessage assigned = EndOfInput{};

    assigned = std::move(original);

    ASSERT_TRUE(std::holds_alternative<Record<std::unique_ptr<int>>>(assigned));
    const auto& value = std::get<Record<std::unique_ptr<int>>>(assigned).value();
    ASSERT_EQ(value.get(), original_pointer);
    EXPECT_EQ(*value, 42);
}

TEST(StreamMessageTest, CanEmplaceRecordAfterEndOfInput) {
    MoveOnlyMessage message = EndOfInput{};

    message.emplace<Record<std::unique_ptr<int>>>(std::make_unique<int>(42));

    ASSERT_TRUE(std::holds_alternative<Record<std::unique_ptr<int>>>(message));
    const auto& value = std::get<Record<std::unique_ptr<int>>>(message).value();
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(*value, 42);
}

TEST(StreamMessageTest, ReplacingRecordWithEndOfInputReleasesPayload) {
    auto payload = std::make_shared<int>(42);
    const std::weak_ptr<int> observer = payload;
    StreamMessage<std::shared_ptr<int>> message =
        Record<std::shared_ptr<int>>{std::move(payload)};
    ASSERT_FALSE(observer.expired());

    message = EndOfInput{};

    EXPECT_TRUE(std::holds_alternative<EndOfInput>(message));
    EXPECT_TRUE(observer.expired());
}

TEST(StreamMessageTest, AccessingInactiveAlternativeThrows) {
    const StreamMessage<int> record = Record<int>{42};
    const StreamMessage<int> end = EndOfInput{};

    EXPECT_THROW(std::get<EndOfInput>(record), std::bad_variant_access);
    EXPECT_THROW(std::get<Record<int>>(end), std::bad_variant_access);
}

TEST(StreamMessageTest, VisitorDistinguishesDataFromEndOfInput) {
    const auto describe = [](const auto& value) -> std::string {
        if constexpr (std::is_same_v<std::decay_t<decltype(value)>, Record<int>>) {
            return std::to_string(value.value());
        } else {
            return "end";
        }
    };
    const StreamMessage<int> record = Record<int>{42};
    const StreamMessage<int> end = EndOfInput{};

    EXPECT_EQ(std::visit(describe, record), "42");
    EXPECT_EQ(std::visit(describe, end), "end");
}

} // namespace
