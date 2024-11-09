#include <array>
#include <chrono>
#include <cppsl/container/circularBuffer.hpp>
#include <cstring>
#include <memory>
#include <thread>

#include "cppsl/container/listSafe.hpp"

namespace param {
static const size_t initialCapacity = 1024;
static const size_t messageCount = 100;
static const size_t meassageSize = 128;
static const size_t numTries = 1000;
static const double messageTimeout = 1.0;
}   // namespace param

std::atomic_int diff_message{0};
uint32_t number_sent{0};
uint32_t number_recv{0};

struct TestCase {
  TestCase(std::string name) : m_name{std::move(name)} {}
  virtual ~TestCase() = default;
  virtual void perform();
  const std::string m_name;
};

struct Message {
  std::array<uint8_t, 256> data{};
  std::chrono::time_point<std::chrono::system_clock> ts{};
};

void TestCase::perform() {

  cppsl::container::CyclicBuffer<std::shared_ptr<Message>, param::meassageSize> buffer{};

  /*
   * PROVIDER
   */
  auto thread_provider = [&buffer]() {
    const size_t message_count = param::messageCount;

    std::printf("-->> begin send\n");

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point time_of_warning;
    bool is_first_warning = true;

    for (size_t i = 0; i < message_count;) {
      std::memcpy(message_buffer.get(), &i, sizeof(size_t));
      size_t oldcap = buffer.capacity();
      if (!buffer.put(message_buffer.get(), message_size)) {
        if (TypeOfBuffer::can_extend()) {
          std::printf("-->> buffer did not extend\n");   // must never happen
          std::terminate();
        } else {
          std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
          double elapsed = std::chrono::duration<double>(now - start).count();
          if (is_first_warning || std::chrono::duration<double>(now - time_of_warning).count() > 1) {
            std::printf("-->> message not sent yet (%zu)...\n", i);
            time_of_warning = now;
            is_first_warning = false;
          }
          if (elapsed > param::messageTimeout) {
            std::printf("-->> message sending timeout\n");
            std::terminate();
          }
        }
      } else {
        ++diff_message;
        number_sent++;

        size_t newcap = buffer.capacity();
        if (oldcap != newcap) {
          std::printf("-->> growth (%zu -> %zu)\n", oldcap, newcap);
        }
        ++i;
        start = std::chrono::steady_clock::now();
        is_first_warning = true;
      }
    }

    std::printf("-->> sent (%zu)\n", message_count);
  };

  /*
   * CONSUMER
   */
  auto thread_consumer = [&buffer]() {
    const size_t message_size = param::meassageSize;
    const size_t message_count = param::messageCount;
    std::unique_ptr<uint8_t[]> message_buffer(new uint8_t[message_size]);

    std::printf("--<< begin receive\n");

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point time_of_warning;
    bool is_first_warning = true;

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
}

/*************************************************************************/ /**
 * main
 * @return
 */
int main() {
  {   // 12/2/2020 -> 737761
    std::unique_ptr<BaseTestCase> test;
    test.reset(new TheTestCase<Ring_Buffer>{"Hard"});
    test->perform();
  }

  {   // 12/2/2020 -> 737761
    std::unique_ptr<BaseTestCase> test;
    test.reset(new TheTestCase<Soft_Ring_Buffer>{"Soft"});
    test->perform();
  }
  return 0;
}
