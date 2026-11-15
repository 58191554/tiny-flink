#pragma once

#include <string>

#include "data_source.h"


namespace tiny_flink {

template <typename T>
class DataReader {
public:
    virtual ~DataReader() = default;
    virtual StreamMessage<T> poll() = 0;
};

template <typename T>
class FileDataReader : public DataReader<T> {
public:
    FileDataReader(FileDataSource* data_source) : data_source_(data_source) {}
    StreamMessage<T> poll() override;
private:
    FileDataSource* data_source_;
    std::ifstream file_stream_;
}
} // namespace tiny_flink
