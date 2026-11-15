#include <optional>
#include <string>
#include <utility>

#include "absl/status/status.h"


namespace tiny_flink {
template <typename T>
class DataSource {
};

class FileDataSource : public DataSource<std::string> {
private:
  std::string file_path_;
}

} // namespace tiny_flink