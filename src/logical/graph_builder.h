#pragma once

#include <cstdint>

#include "absl/status/statusor.h"
#include "logical_graph.h"

namespace tiny_flink {

class GraphBuilder {
public:

    OperatorId addSource(std::unique_ptr<OperatorDefinition> definition);
    OperatorId addUnaryOperator(OperatorId input, std::unique_ptr<OperatorDefinition> definition);
    absl::StatusOr<LogicalGraph> build();

private:
    LogicalGraph logical_graph_;
    std::uint64_t next_id_ = 0;

};

} // namespace tiny_flink
