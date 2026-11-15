#pragma once

#include "core/id.h"

namespace tiny_flink {

class LogicalEdge {
public:
    LogicalEdge(OperatorId from, OperatorId to):
        from_(from), to_(to) {}
    OperatorId from() const { return from_; }
    OperatorId to() const { return to_; }
private:
    OperatorId from_;
    OperatorId to_;
};
} // namespace tiny_flink
