#pragma once

#include <variant>
#include "record.h"

namespace tiny_flink {

struct EndOfInput {};

// StreamMessage can either be Record or EndOfInput
template <typename T>
using StreamMessage = std::variant<Record<T>, EndOfInput>;

} // namespace tiny_flink