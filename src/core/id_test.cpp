// Keep these first to verify that the header is self-contained and repeatable.
#include "core/id.h"
#include "core/id.h"

#include <gtest/gtest.h>

#include <limits>
#include <type_traits>
#include <utility>

namespace {

template <typename Left, typename Right>
auto can_equal(int) -> decltype(
    std::declval<const Left&>() == std::declval<const Right&>(), std::true_type{});
template <typename, typename>
std::false_type can_equal(...);

template <typename Left, typename Right>
auto can_differ(int) -> decltype(
    std::declval<const Left&>() != std::declval<const Right&>(), std::true_type{});
template <typename, typename>
std::false_type can_differ(...);

template <typename Left, typename Right>
void check_distinct_types() {
    static_assert(!std::is_constructible<Left, Right>::value,
                  "Different ID categories must not be interchangeable");
    static_assert(!std::is_constructible<Right, Left>::value,
                  "Different ID categories must not be interchangeable");
    static_assert(!std::is_assignable<Left&, Right>::value,
                  "Different ID categories must not be assignable");
    static_assert(!std::is_assignable<Right&, Left>::value,
                  "Different ID categories must not be assignable");
    static_assert(!decltype(can_equal<Left, Right>(0))::value &&
                  !decltype(can_equal<Right, Left>(0))::value,
                  "Different ID categories must not support equality");
    static_assert(!decltype(can_differ<Left, Right>(0))::value &&
                  !decltype(can_differ<Right, Left>(0))::value,
                  "Different ID categories must not support inequality");
}

template <typename Id>
class StrongIdTest : public testing::Test {};

using IdTypes = testing::Types<tiny_flink::OperatorId, tiny_flink::EdgeId,
                              tiny_flink::TaskId, tiny_flink::TaskEdgeId>;

// Supply the optional argument explicitly for strict C++17 variadic-macro checks.
TYPED_TEST_SUITE(StrongIdTest, IdTypes, );

constexpr std::uint64_t values[] = {
    0, 1, std::uint64_t{1} << 32, std::numeric_limits<std::uint64_t>::max()
};

TYPED_TEST(StrongIdTest, RequiresExplicitConstructionAndReadOnlyValue) {
    using Id = TypeParam;
    static_assert(std::is_constructible<Id, std::uint64_t>::value,
                  "IDs must support explicit construction from a value");
    static_assert(!std::is_convertible<std::uint64_t, Id>::value,
                  "Construction from a value must be explicit");
    static_assert(!std::is_convertible<Id, std::uint64_t>::value,
                  "IDs must not implicitly convert to their underlying value");
    static_assert(std::is_same<decltype(std::declval<const Id&>().value()),
                               const std::uint64_t&>::value,
                  "The stored value must be exposed as a const reference");

    SUCCEED();
}

TYPED_TEST(StrongIdTest, PreservesBoundaryValues) {
    for (const auto value : values) {
        const TypeParam id(value);
        EXPECT_EQ(id.value(), value);
    }
}

TYPED_TEST(StrongIdTest, ComparesStoredValues) {
    for (const auto value : values) {
        SCOPED_TRACE(value);
        const TypeParam id(value);
        EXPECT_TRUE(id == id);
        EXPECT_FALSE(id != id);
        for (const auto other_value : values) {
            SCOPED_TRACE(other_value);
            const TypeParam other(other_value);
            EXPECT_EQ(id == other, value == other_value);
            EXPECT_EQ(id != other, value != other_value);
        }
    }
}

TYPED_TEST(StrongIdTest, CopyConstructionPreservesIdentity) {
    for (const auto value : values) {
        const TypeParam id(value);
        const TypeParam copied(id);
        EXPECT_EQ(copied, id);
    }
}

TYPED_TEST(StrongIdTest, CopyAssignmentPreservesIdentity) {
    for (const auto value : values) {
        const TypeParam id(value);
        TypeParam assigned(value == 0 ? 1 : 0);
        assigned = id;
        EXPECT_EQ(assigned, id);
    }
}

TYPED_TEST(StrongIdTest, OwnsCopyOfInputValue) {
    std::uint64_t source = 42;
    const TypeParam id(source);
    source = 99;
    EXPECT_EQ(id.value(), 42u);
}

TEST(StrongIdTypeSafetyTest, CategoriesAreNotInterchangeable) {
    using namespace tiny_flink;

    check_distinct_types<OperatorId, EdgeId>();
    check_distinct_types<OperatorId, TaskId>();
    check_distinct_types<OperatorId, TaskEdgeId>();
    check_distinct_types<EdgeId, TaskId>();
    check_distinct_types<EdgeId, TaskEdgeId>();
    check_distinct_types<TaskId, TaskEdgeId>();

    SUCCEED();
}

} // namespace
