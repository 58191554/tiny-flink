#pragma once

#include <utility>

namespace tiny_flink {
    template <typename T>
    class Record {
    public:
        explicit Record(T value) : value_(std::move(value)) {}
        const T& value() const noexcept { return value_; }
    private:
        T value_;
    };
} // namespace tiny_flink
