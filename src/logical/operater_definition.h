#pragma once

namespace tiny_flink {

enum class OperatorType {
    Source,
    Map,
    Filter,
    Sink
};

class OperatorDefinition {
public:
    OperatorDefinition(OperatorType type): type_(type) {}
    virtual ~OperatorDefinition() = default;
private:
    OperatorType type_;
};

template<typename T>
class MapOperator : public OperatorDefinition {
public:
    MapOperator(): OperatorDefinition(OperatorType::Map) {}
};

template<typename T>
class FilterOperator : public OperatorDefinition {
public:
    FilterOperator(): OperatorDefinition(OperatorType::Filter) {}
};

template<typename T>
class SinkOperator : public OperatorDefinition {
public:
    SinkOperator(): OperatorDefinition(OperatorType::Sink) {}
};

template<typename T>
class SourceOperator : public OperatorDefinition {
public:
    SourceOperator(): OperatorDefinition(OperatorType::Source) {}
};
} // namespace tiny_flink