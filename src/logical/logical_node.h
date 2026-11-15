#pragma once

#include <memory>
#include <utility>

#include "core/id.h"
#include "operater_definition.h"

namespace tiny_flink {

class LogicalNode {
public:
    LogicalNode(OperatorId id, std::unique_ptr<OperatorDefinition> operator_):
        id_(id), operator_definition_(std::move(operator_)) {}

    OperatorId id() const { return id_; }
    OperatorDefinition* operator_() const { return operator_definition_.get(); }

private:
    OperatorId id_;
    std::unique_ptr<OperatorDefinition> operator_definition_;
};

} // namespace tiny_flink
