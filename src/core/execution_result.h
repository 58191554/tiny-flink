#pragma once

#include <optional>
#include <vector>

#include "execution_error.h"
#include "job_status.h"

namespace tiny_flink {

enum class CleanupStatus {
    COMPLETED,
    INCOMPLETE
};

struct ExecutionResult {
    JobStatus job_status;
    CleanupStatus cleanup_status;
    std::optional<ExecutionError> primary_error;
    std::vector<ExecutionError> secondary_errors;
};

} // namespace tiny_flink