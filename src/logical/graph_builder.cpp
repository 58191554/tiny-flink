

#include "graph_builder.h"

namespace tiny_flink {

OperatorId GraphBuilder::addSource(std::unique_ptr<OperatorDefinition> definition) {
    OperatorId id{next_id_++};
    logical_graph_.addNode(LogicalNode(id, std::move(definition), true));
    return id;
}

OperatorId GraphBuilder::addUnaryOperator(OperatorId input, std::unique_ptr<OperatorDefinition> definition) {
    OperatorId id{next_id_++};
    logical_graph_.addNode(LogicalNode(id, std::move(definition), false));
    logical_graph_.addEdge(LogicalEdge(input, id));
    return id;
}

absl::StatusOr<LogicalGraph> GraphBuilder::build() {
    absl::Status status = logical_graph_.validate();
    if (!status.ok()) {
        return status;
    }
    return std::move(logical_graph_);
}

} // namespace tiny_flink
