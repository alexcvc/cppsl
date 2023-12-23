#include <cppsl/buffer/cycle_buffer.hpp>
#include <fmt/format.h>
#include <cstring>
#include <memory>
#include <thread>

using namespace cppsl::buffer;

struct TestMessage {
  uint32_t msg_cnt{0};
  std::string msg_text{};

  TestMessage() = default;
  TestMessage(int cnt, std::string text): msg_cnt(cnt), msg_text(text) {}
};

constexpr uint32_t numbThreads{10};

/*************************************************************************/ /**
 * main
 * @return
 */
int main() {
  int message_count{100};
  Cycle_Buffer<TestMessage, 32> buffer;

  /*
   * PROVIDER
   */
  auto thread_provider = [&]() {
    std::printf("-->> begin send\n");

    for (size_t i = 0; i < message_count; ++i) {
      TestMessage msg(i, fmt::format("Long message with counter {0} & Long message with counter {0}", i));
      if (!buffer.insert(msg)) {
        std::printf("-->> message not sent yet (%zu)...\n", i);
        std::terminate();
      }
    }
  };

  /*
   * CONSUMER
   */
  auto thread_consumer = [&]() {
    std::printf("--<< begin receive\n");
    for (size_t i = 0; i < message_count; ++i) {
      if(buffer.readAvailable()) {
        TestMessage msg;
        if (!buffer.readBuff(&msg, 1)) {
          std::printf("-->> cannot read message (%zu)...\n", i);
          std::terminate();
        } else {
          std::printf("-->> message %d read %s\n", i, msg.msg_text.c_str());
        }
      }
    }
  };

  for (size_t i = 0; i < numbThreads; ++i) {
    std::printf("---------------- %4zu/%4zu ----------------\n", i + 1, numbThreads);
    std::thread t1(thread_provider);
    std::thread t2(thread_consumer);
    t1.join();
    t2.join();
  }
  std::printf("-------------------------------------------\n");
  std::printf("success!\n");

  std::printf("<<< TestCase end >>>\n");

  return 0;
}
