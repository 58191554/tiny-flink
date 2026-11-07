#pragma once

#include <exception>
#include <optional>
#include <string>

#include "id.h"

namespace tiny_flink {

enum class ErrorCategory {
    GRAPH,
    COMPILATION,
    EXECUTION
};

enum class ErrorCode {
    GRAPH_CYCLE_DETECTED,
    INVALID_EDGE,

    UNSUPPORTED_OPERATOR,
    TASK_GRAPH_BUILD_FAILED,

    USER_FUNCTION_FAILED,
    CHANNEL_WRITE_FAILED,
    TASK_INITIALIZATION_FAILED
};

struct ExecutionError {
    ErrorCategory category;
    ErrorCode code;
    std::string message;

    std::optional<TaskId> task_id;
    std::optional<OperatorId> operator_id;

    std::exception_ptr cause;
};

} // namespace tiny_flink