namespace tiny_flink {

enum class JobStatus {
    CREATED,
    RUNNING,
    FINISHED,
    FAILING,
    FAILED,
    CANCELLING,
    CANCELLED
};

constexpr bool is_terminal_status(JobStatus status) noexcept {
    return status == JobStatus::FINISHED ||
           status == JobStatus::CANCELLED ||
           status == JobStatus::FAILED;
}
} // namespace tiny_flink