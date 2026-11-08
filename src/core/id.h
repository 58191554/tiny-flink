#ifndef TINY_FLINK_CORE_ID_H
#define TINY_FLINK_CORE_ID_H

#include <cstdint>

namespace tiny_flink {
template <typename T>
class StrongId {
public:
    explicit StrongId(const std::uint64_t& value) : value_(value) {}
    const std::uint64_t& value() const { return value_; }
    bool operator==(const StrongId<T>& other) const { return value_ == other.value_; }
    bool operator!=(const StrongId<T>& other) const { return value_ != other.value_; }
private:
    std::uint64_t value_;
};

// Logical graph id
struct OperatorIdTag {};
using OperatorId = StrongId<OperatorIdTag>;

struct EdgeIdTag {};
using EdgeId = StrongId<EdgeIdTag>;

// Executable graph id
struct TaskIdTag {};
using TaskId = StrongId<TaskIdTag>;

struct TaskEdgeIdTag {};
using TaskEdgeId = StrongId<TaskEdgeIdTag>;
}

#endif // TINY_FLINK_CORE_ID_H
