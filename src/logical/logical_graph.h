#pragma once

#include "absl/status/status.h"

#include <vector>

#include "logical_edge.h"
#include "logical_node.h"

namespace tiny_flink {

class LogicalGraph {
public:
    LogicalGraph() {}
    void addNode(LogicalNode node);
    void addEdge(LogicalEdge edge);
    absl::Status validate() const;
    
private:
    std::vector<LogicalNode> nodes_;
    std::vector<LogicalEdge> edges_;
    bool IsDag() const;
};
} // namespace tiny_flink