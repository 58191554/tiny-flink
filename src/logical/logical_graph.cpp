#include "logical_graph.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace tiny_flink {

namespace {

struct OperatorIdHash {
    std::size_t operator()(const OperatorId& id) const noexcept {
        return std::hash<std::uint64_t>{}(id.value());
    }
};

struct EdgeHash {
    std::size_t operator()(
        const std::pair<OperatorId, OperatorId>& edge) const noexcept {
        const auto from = OperatorIdHash{}(edge.first);
        const auto to = OperatorIdHash{}(edge.second);
        return from ^ (to + 0x9e3779b9U + (from << 6) + (from >> 2));
    }
};

} // namespace

void LogicalGraph::addNode(LogicalNode node) {
    nodes_.push_back(std::move(node));
}

void LogicalGraph::addEdge(LogicalEdge edge) {
    edges_.push_back(std::move(edge));
}

absl::Status LogicalGraph::validate() const {
    // check no repeated node ids
    std::unordered_set<OperatorId, OperatorIdHash> node_ids;
    for (const auto& node : nodes_) {
        if (!node_ids.insert(node.id()).second) {
            return absl::InvalidArgumentError("Logical graph has repeated node ids");
        }
    }
    // Check endpoints before running the DAG check.
    std::unordered_set<std::pair<OperatorId, OperatorId>, EdgeHash> edge_ids;
    for (const auto& edge : edges_) {
        if (!node_ids.contains(edge.from()) || !node_ids.contains(edge.to())) {
            return absl::InvalidArgumentError(
                "Logical graph edge references a nonexistent node");
        }
        if (!edge_ids.insert({edge.from(), edge.to()}).second) {
            return absl::InvalidArgumentError("Logical graph has repeated edges");
        }
    }
    // check no cycles
    if (!IsDag()) {
        return absl::InvalidArgumentError("Logical graph is not a DAG");
    }
    return absl::OkStatus();
}

bool LogicalGraph::IsDag() const {
    std::unordered_map<OperatorId, std::vector<OperatorId>, OperatorIdHash> adj;
    std::unordered_map<OperatorId, std::size_t, OperatorIdHash> indegree;
    for (const auto& edge : edges_) {
        adj[edge.from()].push_back(edge.to());
        indegree[edge.to()]++;
    }
    std::queue<OperatorId> queue;
    for (const auto& node : nodes_) {
        if (indegree[node.id()] == 0) {
            queue.push(node.id());
        }
    }
    std::size_t visited = 0;
    while (!queue.empty()) {
        const OperatorId id = queue.front();
        queue.pop();
        visited++;
        for (const OperatorId next : adj[id]) {
            if (--indegree[next] == 0) {
                queue.push(next);
            }
        }
    }
    return visited == nodes_.size();
}

} // namespace tiny_flink
