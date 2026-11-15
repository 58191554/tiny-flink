#include "logical/logical_graph.h"

#include <cstdint>
#include <memory>

#include <gtest/gtest.h>

namespace tiny_flink {
namespace {

void AddNode(LogicalGraph& graph, std::uint64_t id) {
    graph.addNode(LogicalNode(
        OperatorId(id), std::make_unique<MapOperator<int>>()));
}

void AddEdge(LogicalGraph& graph, std::uint64_t from, std::uint64_t to) {
    graph.addEdge(LogicalEdge(OperatorId(from), OperatorId(to)));
}

TEST(LogicalGraphTest, AcceptsEmptyGraph) {
    EXPECT_TRUE(LogicalGraph().validate().ok());
}

TEST(LogicalGraphTest, AcceptsDisconnectedDagAndIsolatedNode) {
    LogicalGraph graph;
    for (auto id : {10, 20, 30, 40, 50, 60, 70}) {
        AddNode(graph, id);
    }
    AddEdge(graph, 10, 20);
    AddEdge(graph, 10, 30);
    AddEdge(graph, 20, 40);
    AddEdge(graph, 30, 40);
    AddEdge(graph, 50, 60);
    EXPECT_TRUE(graph.validate().ok());
    EXPECT_TRUE(graph.validate().ok()); // Validation must not mutate the graph.
}

TEST(LogicalGraphTest, RejectsDuplicateNodeIds) {
    LogicalGraph graph;
    AddNode(graph, 1);
    AddNode(graph, 1);
    EXPECT_EQ(graph.validate().code(), absl::StatusCode::kInvalidArgument);
}

TEST(LogicalGraphTest, RejectsDuplicateEdges) {
    LogicalGraph graph;
    AddNode(graph, 1);
    AddNode(graph, 2);
    AddEdge(graph, 1, 2);
    AddEdge(graph, 1, 2);
    EXPECT_EQ(graph.validate().message(), "Logical graph has repeated edges");
}

TEST(LogicalGraphTest, RejectsMissingSource) {
    LogicalGraph graph;
    AddNode(graph, 1);
    AddEdge(graph, 2, 1);
    EXPECT_EQ(graph.validate().message(),
              "Logical graph edge references a nonexistent node");
}

TEST(LogicalGraphTest, RejectsMissingTargetAndExternalSelfLoop) {
    LogicalGraph graph;
    AddNode(graph, 1);
    AddNode(graph, 2);
    AddEdge(graph, 1, 3);
    AddEdge(graph, 3, 3);
    EXPECT_EQ(graph.validate().message(),
              "Logical graph edge references a nonexistent node");
}

TEST(LogicalGraphTest, RejectsEdgesInEmptyGraph) {
    LogicalGraph graph;
    AddEdge(graph, 1, 2);
    EXPECT_EQ(graph.validate().code(), absl::StatusCode::kInvalidArgument);
}

TEST(LogicalGraphTest, RejectsSelfLoop) {
    LogicalGraph graph;
    AddNode(graph, 1);
    AddEdge(graph, 1, 1);
    EXPECT_EQ(graph.validate().message(), "Logical graph is not a DAG");
}

TEST(LogicalGraphTest, RejectsCycleInDisconnectedComponent) {
    LogicalGraph graph;
    for (auto id : {1, 2, 3, 4, 5}) {
        AddNode(graph, id);
    }
    AddEdge(graph, 1, 2);
    AddEdge(graph, 3, 4);
    AddEdge(graph, 4, 5);
    AddEdge(graph, 5, 3);
    EXPECT_EQ(graph.validate().message(), "Logical graph is not a DAG");
}

} // namespace
} // namespace tiny_flink
