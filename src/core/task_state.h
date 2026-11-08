namespace tiny_flink {

enum class TaskState {
    CREATED,
    RUNNING,
    FINISHED,
    CANCELLING,
    CANCELLED,
    FAILED
};

constexpr bool is_terminal_state(TaskState state) noexcept {
    return state == TaskState::FINISHED ||
           state == TaskState::CANCELLED ||
           state == TaskState::FAILED;
}
} // namespace tiny_flink