// Keep these first to verify that the header is self-contained and repeatable.
#include "core/record.h"
#include "core/record.h"

#include <gtest/gtest.h>

#include <limits>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace {

using tiny_flink::Record;
using MoveOnlyRecord = Record<std::unique_ptr<int>>;

static_assert(std::is_constructible_v<Record<int>, int>,
              "Records must be constructible from a payload");
static_assert(!std::is_convertible_v<int, Record<int>>,
              "Construction from a payload must be explicit");
static_assert(std::is_same_v<decltype(std::declval<Record<int>&>().value()), const int&>,
              "Mutable records must expose a const payload reference");
static_assert(std::is_same_v<decltype(std::declval<const Record<int>&>().value()), const int&>,
              "Const records must expose a const payload reference");
static_assert(noexcept(std::declval<const Record<int>&>().value()),
              "Accessing a payload must not throw");
static_assert(!std::is_copy_constructible_v<MoveOnlyRecord> &&
              !std::is_copy_assignable_v<MoveOnlyRecord>,
              "Records with move-only payloads must not be copyable");
static_assert(std::is_move_constructible_v<MoveOnlyRecord> &&
              std::is_move_assignable_v<MoveOnlyRecord>,
              "Records with move-only payloads must be movable");

struct Payload {
    Payload() = delete;
    explicit Payload(int value) : number(value) {}

    int number;
};

TEST(RecordTest, PreservesScalarValues) {
    const int values[] = {0, 1, -1, std::numeric_limits<int>::min(),
                          std::numeric_limits<int>::max()};
    for (const auto value : values) {
        const Record<int> record(value);
        EXPECT_EQ(record.value(), value);
    }
}

TEST(RecordTest, SupportsUserDefinedPayloadWithoutDefaultConstructor) {
    const Record<Payload> record(Payload{42});
    EXPECT_EQ(record.value().number, 42);
}

TEST(RecordTest, OwnsIndependentCopyOfLvaluePayload) {
    std::string source = "original";
    const Record<std::string> record(source);
    EXPECT_EQ(source, "original");

    source = "changed";
    EXPECT_EQ(record.value(), "original");
}

TEST(RecordTest, AccessorReturnsTheStoredObject) {
    Record<std::string> record("payload");
    const auto& const_record = record;
    EXPECT_EQ(std::addressof(record.value()), std::addressof(const_record.value()));
    EXPECT_EQ(std::addressof(record.value()), std::addressof(record.value()));
    EXPECT_EQ(const_record.value(), "payload");
}

TEST(RecordTest, ConstructsFromMoveOnlyPayload) {
    auto payload = std::make_unique<int>(42);
    const auto* original = payload.get();

    const MoveOnlyRecord record(std::move(payload));

    EXPECT_EQ(payload, nullptr);
    ASSERT_EQ(record.value().get(), original);
    EXPECT_EQ(*record.value(), 42);
}

TEST(RecordTest, CopyConstructionCopiesPayload) {
    const Record<std::vector<int>> original(std::vector<int>{1, 2, 3});
    const auto copied = original;

    EXPECT_EQ(copied.value(), (std::vector<int>{1, 2, 3}));
    EXPECT_EQ(original.value(), copied.value());
    EXPECT_NE(original.value().data(), copied.value().data());
}

TEST(RecordTest, CopyAssignmentReplacesPayload) {
    const Record<std::vector<int>> original(std::vector<int>{1, 2, 3});
    Record<std::vector<int>> assigned(std::vector<int>{99});

    assigned = original;

    EXPECT_EQ(assigned.value(), (std::vector<int>{1, 2, 3}));
    EXPECT_EQ(original.value(), assigned.value());
    EXPECT_NE(original.value().data(), assigned.value().data());
}

TEST(RecordTest, MoveConstructionTransfersMoveOnlyPayload) {
    MoveOnlyRecord original(std::make_unique<int>(42));
    const auto* payload = original.value().get();

    const MoveOnlyRecord moved(std::move(original));

    EXPECT_EQ(original.value(), nullptr);
    ASSERT_EQ(moved.value().get(), payload);
    EXPECT_EQ(*moved.value(), 42);
}

TEST(RecordTest, MoveAssignmentTransfersMoveOnlyPayload) {
    MoveOnlyRecord original(std::make_unique<int>(42));
    const auto* payload = original.value().get();
    MoveOnlyRecord assigned(std::make_unique<int>(99));

    assigned = std::move(original);

    EXPECT_EQ(original.value(), nullptr);
    ASSERT_EQ(assigned.value().get(), payload);
    EXPECT_EQ(*assigned.value(), 42);
}

} // namespace
