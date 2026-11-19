#pragma once

#include <fstream>
#include <string>

#include "core/stream_message.h"
#include "data_source.h"


namespace tiny_flink {

template <typename T>
class DataReader {
public:
    virtual ~DataReader() = default;
    virtual StreamMessage<T> poll() = 0;
};

class FileDataReader : public DataReader<std::string> {
public:
    FileDataReader(FileDataSource* data_source) : data_source_(data_source) {}
    StreamMessage<std::string> poll() override;
private:
    FileDataSource* data_source_;
    std::ifstream file_stream_;
    bool finished_ = false;
};
} // namespace tiny_flink
