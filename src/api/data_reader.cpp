#include "data_reader.h"

namespace tiny_flink {


FileDataReader::poll() {
    if (!file_stream_.is_open()) {
        file_stream_.open(data_source_->file_path_);
    }
    std::string line = file_stream_.getline();
    if (file_stream_.eof()) {
        file_stream_.close();
        return EndOfStream();
    }
    Record<std::string> record;
    return record;
}
} // namespace tiny_flink