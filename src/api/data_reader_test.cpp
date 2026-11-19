#include "api/data_reader.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <variant>

namespace {

using tiny_flink::DataReader;
using tiny_flink::EndOfInput;
using tiny_flink::FileDataReader;
using tiny_flink::FileDataSource;
using tiny_flink::Record;

class FileDataReaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        directory_ = std::filesystem::temp_directory_path() /
                     ("tiny-flink-reader-" + std::to_string(std::random_device{}()));
        ASSERT_TRUE(std::filesystem::create_directory(directory_));
        path_ = directory_ / "input.txt";
    }

    void TearDown() override {
        if (!path_.empty()) {
            std::filesystem::remove(path_);
            std::filesystem::remove(directory_);
        }
    }

    void WriteFile(const std::string& contents) {
        std::ofstream file(path_, std::ios::binary);
        ASSERT_TRUE(file.is_open());
        file << contents;
        file.close();
        ASSERT_FALSE(file.fail());
    }

    std::filesystem::path directory_;
    std::filesystem::path path_;
};

TEST_F(FileDataReaderTest, ReadsLinesInOrderThroughBaseInterface) {
    ASSERT_NO_FATAL_FAILURE(WriteFile("first\nsecond\n"));
    FileDataSource source(path_.string());
    FileDataReader file_reader(&source);
    DataReader<std::string>& reader = file_reader;

    for (const std::string expected : {"first", "second"}) {
        auto message = reader.poll();
        ASSERT_TRUE(std::holds_alternative<Record<std::string>>(message));
        EXPECT_EQ(std::get<Record<std::string>>(message).value(), expected);
    }
    EXPECT_TRUE(std::holds_alternative<EndOfInput>(reader.poll()));
}

TEST_F(FileDataReaderTest, EmptyFileImmediatelyEnds) {
    ASSERT_NO_FATAL_FAILURE(WriteFile(""));
    FileDataSource source(path_.string());
    FileDataReader reader(&source);

    EXPECT_TRUE(std::holds_alternative<EndOfInput>(reader.poll()));
}

TEST_F(FileDataReaderTest, PreservesEmptyLinesAsRecords) {
    ASSERT_NO_FATAL_FAILURE(WriteFile("\nvalue\n\n"));
    FileDataSource source(path_.string());
    FileDataReader reader(&source);

    for (const std::string expected : {"", "value", ""}) {
        auto message = reader.poll();
        ASSERT_TRUE(std::holds_alternative<Record<std::string>>(message));
        EXPECT_EQ(std::get<Record<std::string>>(message).value(), expected);
    }
    EXPECT_TRUE(std::holds_alternative<EndOfInput>(reader.poll()));
}

TEST_F(FileDataReaderTest, EmitsLastLineWithoutTrailingNewline) {
    ASSERT_NO_FATAL_FAILURE(WriteFile("last line"));
    FileDataSource source(path_.string());
    FileDataReader reader(&source);

    auto message = reader.poll();
    ASSERT_TRUE(std::holds_alternative<Record<std::string>>(message));
    EXPECT_EQ(std::get<Record<std::string>>(message).value(), "last line");
    EXPECT_TRUE(std::holds_alternative<EndOfInput>(reader.poll()));
}

TEST_F(FileDataReaderTest, RemainsFinishedOnSubsequentPolls) {
    ASSERT_NO_FATAL_FAILURE(WriteFile("once\n"));
    FileDataSource source(path_.string());
    FileDataReader reader(&source);

    auto message = reader.poll();
    ASSERT_TRUE(std::holds_alternative<Record<std::string>>(message));
    EXPECT_EQ(std::get<Record<std::string>>(message).value(), "once");
    EXPECT_TRUE(std::holds_alternative<EndOfInput>(reader.poll()));
    EXPECT_TRUE(std::holds_alternative<EndOfInput>(reader.poll()));
}

} // namespace
