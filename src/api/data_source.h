#pragma once

#include <string>
#include <utility>


namespace tiny_flink {
template <typename T>
class DataSource {
};

class FileDataSource : public DataSource<std::string> {
public:
  explicit FileDataSource(std::string file_path) : file_path_(std::move(file_path)) {}
  const std::string& file_path() const noexcept { return file_path_; }

private:
  std::string file_path_;
};

} // namespace tiny_flink
