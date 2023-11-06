#include <iomanip>
#include "cppsl/file/fileBaseAppender.hpp"
#include "cppsl/file/fileRollAppender.hpp"
#include "gtest/gtest.h"

const std::filesystem::path fileLogPath{"/tmp/test.log"};

std::string generate(int n)// n is size of the random string
{
  std::srand(time(0)); //seeding random generator
  std::string result{};
  result.reserve(n);
  for (int i = 0; i < n; i++)
  {
    int g = std::rand()%25;
    char d = static_cast<char> (g + 'a'); // generating random char
    result.push_back(d);
  }
  return result;
}

TEST(TestFileAppender, TestBaseAppender) {

  std::srand(time(0)); //seeding random generator
  cppsl::log::log_appenderPtr logPtr = cppsl::log::create_log_appender("test");
  if (!logPtr->add_console_sink(false, true, spdlog::level::trace)) {
    std::cerr << "cannot add a console sink" << std::endl;
  }
  // open writer
  if (!logPtr->open_logger(spdlog::level::trace)) {
    std::cerr << "cannot open logger" << std::endl;
  }

  cppsl::file::FileBaseAppender file(logPtr, fileLogPath, true);

  for (auto i = 0; i < 500; ++i) {
    file.writeMessage(std::string(generate(i)) + "\n");
  }

  EXPECT_TRUE(file.reopenFile());
}

TEST(FileRollAppender, TestCreate) {

  std::srand(time(0)); //seeding random generator
  cppsl::log::log_appenderPtr logPtr = cppsl::log::create_log_appender("test");
  if (!logPtr->add_console_sink(false, true, spdlog::level::trace)) {
    std::cerr << "cannot add a console sink" << std::endl;
  }
  // open writer
  if (!logPtr->open_logger(spdlog::level::trace)) {
    std::cerr << "cannot open logger" << std::endl;
  }

  cppsl::file::FileRollAppender file(logPtr, fileLogPath, 6666, 10, true);

  for (auto i = 0; i < 111; ++i) {
    file.writeMessage(std::string(453, 'a' + i%10));
    file.writeMessage("\n");
  }

  EXPECT_TRUE(file.reopenFile());
}
