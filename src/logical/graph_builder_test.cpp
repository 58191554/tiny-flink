#include "logical/graph_builder.h"

#include <cstdint>
#include <memory>
#include <unordered_set>

#include <gtest/gtest.h>

namespace tiny_flink {
namespace {

TEST(GraphBuilderTest, BuildsEmptyGraph) {
    GraphBuilder builder;
    auto graph = builder.build();

    ASSERT_TRUE(graph.ok()) << graph.status();
    EXPECT_TRUE(graph->nodes().empty());
    EXPECT_TRUE(graph->edges().empty());
    EXPECT_TRUE(graph->validate().ok());
}

TEST(GraphBuilderTest, BuildsSourceAndTransfersDefinitionOwnership) {
    auto definition = std::make_unique<SourceOperator<int>>();
    const auto* original_definition = definition.get();
    LogicalGraph graph;
    OperatorId source_id{0};
    {
        GraphBuilder builder;
        source_id = builder.addSource(std::move(definition));
        auto result = builder.build();
        ASSERT_TRUE(result.ok()) << result.status();
        graph = std::move(*result);
    }

    ASSERT_EQ(graph.nodes().size(), 1u);
    EXPECT_EQ(graph.nodes()[0].id(), source_id);
    EXPECT_TRUE(graph.nodes()[0].is_source());
    EXPECT_EQ(graph.nodes()[0].operator_(), original_definition);
    EXPECT_NE(dynamic_cast<SourceOperator<int>*>(graph.nodes()[0].operator_()), nullptr);
    EXPECT_TRUE(graph.edges().empty());
    EXPECT_TRUE(graph.validate().ok());
}

TEST(GraphBuilderTest, BuildsSourceMapFilterSinkPipeline) {
    GraphBuilder builder;
    const auto source = builder.addSource(std::make_unique<SourceOperator<int>>());
    const auto map = builder.addUnaryOperator(source, std::make_unique<MapOperator<int>>());
    const auto filter = builder.addUnaryOperator(map, std::make_unique<FilterOperator<int>>());
    const auto sink = builder.addUnaryOperator(filter, std::make_unique<SinkOperator<int>>());

    auto graph = builder.build();
    ASSERT_TRUE(graph.ok()) << graph.status();
    ASSERT_EQ(graph->nodes().size(), 4u);
    ASSERT_EQ(graph->edges().size(), 3u);
    const OperatorId ids[] = {source, map, filter, sink};
    for (std::size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(graph->nodes()[i].id(), ids[i]);
        EXPECT_EQ(graph->nodes()[i].is_source(), i == 0);
    }
    EXPECT_NE(dynamic_cast<MapOperator<int>*>(graph->nodes()[1].operator_()), nullptr);
    EXPECT_NE(dynamic_cast<FilterOperator<int>*>(graph->nodes()[2].operator_()), nullptr);
    EXPECT_NE(dynamic_cast<SinkOperator<int>*>(graph->nodes()[3].operator_()), nullptr);
    for (std::size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(graph->edges()[i].from(), ids[i]);
        EXPECT_EQ(graph->edges()[i].to(), ids[i + 1]);
    }
    EXPECT_TRUE(graph->validate().ok());
}

TEST(GraphBuilderTest, BuildsBranchesWithSharedSource) {
    GraphBuilder builder;
    const auto source = builder.addSource(std::make_unique<SourceOperator<int>>());
    const auto map = builder.addUnaryOperator(source, std::make_unique<MapOperator<int>>());
    const auto filter = builder.addUnaryOperator(source, std::make_unique<FilterOperator<int>>());

    auto graph = builder.build();
    ASSERT_TRUE(graph.ok()) << graph.status();
    ASSERT_EQ(graph->nodes().size(), 3u);
    ASSERT_EQ(graph->edges().size(), 2u);
    EXPECT_EQ(graph->edges()[0].from(), source);
    EXPECT_EQ(graph->edges()[0].to(), map);
    EXPECT_EQ(graph->edges()[1].from(), source);
    EXPECT_EQ(graph->edges()[1].to(), filter);
}

TEST(GraphBuilderTest, AssignsUniqueIdsAcrossSourcesAndUnaryOperators) {
    GraphBuilder builder;
    std::unordered_set<std::uint64_t> ids;
    for (int i = 0; i < 32; ++i) {
        const auto source = builder.addSource(std::make_unique<SourceOperator<int>>());
        const auto map = builder.addUnaryOperator(source, std::make_unique<MapOperator<int>>());
        EXPECT_TRUE(ids.insert(source.value()).second);
        EXPECT_TRUE(ids.insert(map.value()).second);
    }

    auto graph = builder.build();
    ASSERT_TRUE(graph.ok()) << graph.status();
    EXPECT_EQ(graph->nodes().size(), 64u);
    EXPECT_EQ(graph->edges().size(), 32u);
}

TEST(GraphBuilderTest, BuildRejectsMissingInput) {
    GraphBuilder builder;
    builder.addSource(std::make_unique<SourceOperator<int>>());
    builder.addUnaryOperator(OperatorId{999}, std::make_unique<MapOperator<int>>());

    auto graph = builder.build();
    ASSERT_FALSE(graph.ok());
    EXPECT_EQ(graph.status().code(), absl::StatusCode::kInvalidArgument);
    EXPECT_EQ(graph.status().message(),
              "Logical graph edge references a nonexistent node");
}

TEST(GraphBuilderTest, BuildRejectsCycleFromForwardReference) {
    GraphBuilder builder;
    const auto source = builder.addSource(std::make_unique<SourceOperator<int>>());
    // Point the first unary operator at the ID allocated to the following one.
    const auto map = builder.addUnaryOperator(
        OperatorId{source.value() + 2}, std::make_unique<MapOperator<int>>());
    builder.addUnaryOperator(map, std::make_unique<FilterOperator<int>>());

    auto graph = builder.build();
    ASSERT_FALSE(graph.ok());
    EXPECT_EQ(graph.status().code(), absl::StatusCode::kInvalidArgument);
    EXPECT_EQ(graph.status().message(), "Logical graph is not a DAG");
}

} // namespace
} // namespace tiny_flink
