#include "data_reader.h"

#include <stdexcept>
#include <utility>

namespace tiny_flink {


StreamMessage<std::string> FileDataReader::poll() {
    if (finished_) {
        return EndOfInput{};
    }
    if (!file_stream_.is_open()) {
        if (data_source_ == nullptr) {
            throw std::invalid_argument("FileDataReader requires a data source");
        }
        file_stream_.open(data_source_->file_path());
        if (!file_stream_.is_open()) {
            throw std::runtime_error("Cannot open input file: " + data_source_->file_path());
        }
    }
    std::string line;
    if (std::getline(file_stream_, line)) {
        return Record<std::string>(std::move(line));
    }
    if (!file_stream_.bad() && file_stream_.eof()) {
        finished_ = true;
        file_stream_.close();
        return EndOfInput{};
    }
    throw std::runtime_error("Cannot read input file: " + data_source_->file_path());
}

} // namespace tiny_flink
