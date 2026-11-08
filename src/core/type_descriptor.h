#pragma once

#include <typeindex>
#include <type_traits>

namespace tiny_flink {

class TypeDescriptor {
public:
    template <typename T>
    static TypeDescriptor of() {
        using U = std::remove_cvref_t<T>;
        return TypeDescriptor{std::type_index(typeid(U))};
    }

    bool operator==(const TypeDescriptor& other) const noexcept {
        return type_index_ == other.type_index_;
    }

    const char* name() const noexcept {
        return type_index_.name();
    }

private:
    explicit TypeDescriptor(std::type_index type_index)
        : type_index_(type_index) {}

    std::type_index type_index_;
};

} // namespace tiny_flink
