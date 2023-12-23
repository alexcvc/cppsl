#include <algorithm>
#include <array>
#include <condition_variable>
#include <cppsl/buffer/cycle_buffer.hpp>
#include <iostream>
#include <memory>
#include <random>
#include <syncstream>
#include <thread>

using namespace std::chrono_literals;
using namespace cppsl::buffer;

//-----------------------------------------------------------------------------
// local Typedefs, Enums, Unions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// local/global Variables Definitions
//-----------------------------------------------------------------------------
const int total_threads = 10;
std::array<std::pair<std::mutex, std::condition_variable>, total_threads> thread_sync_array;
std::atomic<uint32_t> msg_counter{0};

//-----------------------------------------------------------------------------
// local/global Function Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// local Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// global Function Definitions
//-----------------------------------------------------------------------------

struct Message {
  uint32_t id{0};
  std::string data;
};

int main() {
  // configuration
  std::stop_source stop_src;   // Create a stop source
  std::array<std::jthread, total_threads> threads;
  cppsl::buffer::Cycle_Buffer<Message, 64> cycle_buffer;

  //----------------------------------------------------------
  // go to idle in main
  //----------------------------------------------------------
  // Create all workers and pass stop tokens
  for (uint i = 0; i < threads.size(); ++i) {
    std::osyncstream(std::cout) << "Start Provider No." << i << std::endl;

    threads[i] = std::move(std::jthread(
       [&cycle_buffer](int number, std::stop_token token) {
         std::osyncstream(std::cout) << "Task " << std::to_string(number) << std::endl;

         // Register a stop callback
         std::stop_callback stop_cb(token, [&]() {
           // Wake thread on stop request
           thread_sync_array[number].second.notify_all();
         });

         while (true) {
           {
             // Start of locked block
             std::unique_lock lck(thread_sync_array[number].first);
             thread_sync_array[number].second.wait_for(lck, 1ms);
           }
           // Stop if requested to stop
           if (token.stop_requested()) {
             break;
           }
           // even
           if (number % 2 == 0) {
             if (auto write_available = cycle_buffer.writeAvailable(); write_available > 0) {
               Message message{0, std::string(50, ' ')};
               std::iota(message.data.begin(), message.data.end(), 0x32);
               std::shuffle(message.data.begin(), message.data.end(), std::mt19937{std::random_device{}()});
               message.id = msg_counter++;

               std::osyncstream(std::cout)
                  << "Task " << std::to_string(number) << " id=" << message.id << /*" :" << message.data
                  << */"   : messages in buffer " << cycle_buffer.readAvailable() << std::endl;
               cycle_buffer.insert(message);
             } else {
               std::osyncstream(std::cout)
                  << "Task " << std::to_string(number) << " cannot insert message" << std::endl;
             }
           } else {
             Message message;
             if (cycle_buffer.readBuff(&message, 1) == 1) {
               std::osyncstream(std::cout)
                  << "==> Task " << std::to_string(number) << " id=" << message.id /*" << :" << message.data*/ << std::endl;
             } else {
               std::osyncstream(std::cout) << "Task " << std::to_string(number) << " cannot read message" << std::endl;
             }
           }
         }   // End of while loop
       },
       i, stop_src.get_token()));

    std::this_thread::sleep_for(10ms);
  }

  std::osyncstream(std::cout) << "==> ALL THREADS STARTED ======================================\n" << std::endl;
  //----------------------------------------------------------
  // Start application stuff
  //----------------------------------------------------------
  // main loop or sleep
  while (true) {
    if (msg_counter >= 300) {
      break;
    }
    std::this_thread::sleep_for(1s);
  }

  std::osyncstream(std::cout) << "==> START TERMINATION ======================================\n" << std::endl;
  // set token to stop all worker
  stop_src.request_stop();

  // wakeup all tasks
  for (uint i = 0; i < thread_sync_array.size(); ++i) {
    std::unique_lock lck(thread_sync_array[i].first);
    thread_sync_array[i].second.notify_all();
  }

  for (uint i = 0; i < threads.size(); ++i) {
    // Join threads
    threads[i].join();
  }

  return EXIT_SUCCESS;
}
