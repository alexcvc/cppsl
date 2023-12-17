#include <cppsl/buffer/cycle_buffer.hpp>
#include <cstring>
#include <memory>
#include <thread>

using namespace cppsl::buffer;

struct TestMessage {
  uint32_t msg_cnt{0};
  std::string msg_text{};
};

/*************************************************************************/ /**
 * main
 * @return
 */
int main() {
  int message_count{0};
  Cycle_Buffer<TestMessage, 32> buffer;

  /*
   * PROVIDER
   */
  auto thread_provider = [&]() {
    std::printf("-->> begin send\n");

    for (size_t i = 0; i < message_count;) {
      TestMessage msg;
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

    for (size_t i = 0; i < message_count;) {
      if (!buffer.get(message_buffer.get(), message_size)) {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - start).count();
        if (is_first_warning || std::chrono::duration<double>(now - time_of_warning).count() > 1) {
          std::printf("--<< message not arrived yet (%zu)...\n", i);
          time_of_warning = now;
          is_first_warning = false;
        }
        if (elapsed > param::messageTimeout) {
          std::printf("--<< message receiving timeout\n");
          std::terminate();
        }
      } else {
        diff_message--;
        number_recv++;

        size_t msg;
        std::memcpy(&msg, message_buffer.get(), sizeof(size_t));
        if (msg != i) {
          std::printf("--<< message (%zu) != expected (%zu)\n", msg, i);
          std::terminate();
        }
        ++i;
        start = std::chrono::steady_clock::now();
        is_first_warning = true;
      }
    }

    std::printf("--<< received (%zu)\n", message_count);
  };

  for (size_t i = 0; i < param::numTries; ++i) {
    diff_message = 0;

    std::printf("---------------- %4zu/%4zu ----------------\n", i + 1, param::numTries);
    std::thread t1(thread_provider);
    std::thread t2(thread_consumer);
    t1.join();
    t2.join();
  }
  std::printf("-------------------------------------------\n");
  std::printf("success!\n");

  std::printf("<<< TestCase end: %s >>>\n", m_name.c_str());

  return 0;
}
