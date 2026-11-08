// Keep these first to verify that the header is self-contained and repeatable.
#include "core/type_descriptor.h"
#include "core/type_descriptor.h"

#include <gtest/gtest.h>

#include <utility>

namespace {

using tiny_flink::TypeDescriptor;

struct FirstType {
    int value;
};

struct SecondType {
    int value;
};

struct NonConstructible {
    NonConstructible() = delete;
};

static_assert(noexcept(std::declval<const TypeDescriptor&>() ==
                       std::declval<const TypeDescriptor&>()),
              "Comparing descriptors must not throw");
static_assert(noexcept(std::declval<const TypeDescriptor&>().name()),
              "Looking up a diagnostic name must not throw");

TEST(TypeDescriptorTest, SameTypeComparesEqual) {
    const auto descriptor = TypeDescriptor::of<int>();
    EXPECT_EQ(descriptor, descriptor);
    EXPECT_EQ(descriptor, TypeDescriptor::of<int>());
}

TEST(TypeDescriptorTest, DifferentTypesCompareUnequalInBothDirections) {
    const auto integer = TypeDescriptor::of<int>();
    const auto real = TypeDescriptor::of<double>();
    EXPECT_FALSE(integer == real);
    EXPECT_FALSE(real == integer);
    EXPECT_FALSE(integer == TypeDescriptor::of<unsigned int>());
}

TEST(TypeDescriptorTest, RemovesTopLevelCvQualifiersAndReferences) {
    const auto expected = TypeDescriptor::of<int>();
    const TypeDescriptor descriptors[] = {
        TypeDescriptor::of<const int>(),
        TypeDescriptor::of<volatile int>(),
        TypeDescriptor::of<const volatile int>(),
        TypeDescriptor::of<int&>(),
        TypeDescriptor::of<const int&>(),
        TypeDescriptor::of<volatile int&>(),
        TypeDescriptor::of<const volatile int&>(),
        TypeDescriptor::of<int&&>(),
        TypeDescriptor::of<const int&&>(),
        TypeDescriptor::of<volatile int&&>(),
        TypeDescriptor::of<const volatile int&&>()
    };
    for (const auto& descriptor : descriptors) {
        EXPECT_EQ(descriptor, expected);
    }
}

TEST(TypeDescriptorTest, TypeAliasesHaveTheSameIdentity) {
    using Alias = FirstType;
    EXPECT_EQ(TypeDescriptor::of<Alias>(), TypeDescriptor::of<FirstType>());
}

TEST(TypeDescriptorTest, DistinctUserTypesHaveDifferentIdentities) {
    EXPECT_FALSE(TypeDescriptor::of<FirstType>() == TypeDescriptor::of<SecondType>());
}

TEST(TypeDescriptorTest, PreservesPointeeQualifiers) {
    const auto pointer = TypeDescriptor::of<int*>();
    EXPECT_EQ(pointer, TypeDescriptor::of<int* const>());
    EXPECT_EQ(pointer, TypeDescriptor::of<int* const volatile&>());
    EXPECT_FALSE(pointer == TypeDescriptor::of<const int*>());
    EXPECT_FALSE(pointer == TypeDescriptor::of<volatile int*>());
    EXPECT_FALSE(pointer == TypeDescriptor::of<int>());
}

TEST(TypeDescriptorTest, PreservesArrayTypesAndExtents) {
    const auto array = TypeDescriptor::of<int[3]>();
    EXPECT_EQ(array, TypeDescriptor::of<const int (&)[3]>());
    EXPECT_EQ(array, TypeDescriptor::of<int (&&)[3]>());
    EXPECT_FALSE(array == TypeDescriptor::of<int*>());
    EXPECT_FALSE(array == TypeDescriptor::of<int[4]>());
}

TEST(TypeDescriptorTest, PreservesFunctionTypesWithoutDecayingToPointers) {
    using Function = int(double);
    const auto function = TypeDescriptor::of<Function>();
    EXPECT_EQ(function, TypeDescriptor::of<Function&>());
    EXPECT_FALSE(function == TypeDescriptor::of<Function*>());
    EXPECT_FALSE(function == TypeDescriptor::of<int(int)>());
}

TEST(TypeDescriptorTest, SupportsTypesWithoutConstructingValues) {
    EXPECT_EQ(TypeDescriptor::of<NonConstructible>(),
              TypeDescriptor::of<const NonConstructible&>());
    EXPECT_EQ(TypeDescriptor::of<void>(), TypeDescriptor::of<const void>());
    EXPECT_FALSE(TypeDescriptor::of<void>() == TypeDescriptor::of<NonConstructible>());
}

TEST(TypeDescriptorTest, CopyConstructionAndAssignmentPreserveIdentity) {
    const auto original = TypeDescriptor::of<FirstType>();
    const auto copied = original;
    EXPECT_EQ(copied, original);

    auto assigned = TypeDescriptor::of<SecondType>();
    assigned = original;
    EXPECT_EQ(assigned, original);
}

TEST(TypeDescriptorTest, NameMatchesNormalizedTypeInfo) {
    const auto descriptor = TypeDescriptor::of<const FirstType&>();
    ASSERT_NE(descriptor.name(), nullptr);
    EXPECT_NE(descriptor.name()[0], '\0');
    // RTTI names are implementation-defined; avoid assuming a readable spelling.
    EXPECT_STREQ(descriptor.name(), typeid(FirstType).name());
    EXPECT_STREQ(descriptor.name(), TypeDescriptor::of<FirstType>().name());
}

} // namespace
