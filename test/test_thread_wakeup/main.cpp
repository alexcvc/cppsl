/* SPDX-License-Identifier: MIT */
//
// Copyright (c) 2023 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include <array>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <stop_token>
#include <string>
#include <thread>
#include <stop_token>

using namespace std::chrono_literals;

//-----------------------------------------------------------------------------
// local Defines and Macros
//-----------------------------------------------------------------------------


/**
* @brief   initializes and run stuff.
* @details main function
*/
int main(int argc, char** argv) {
  // configuration
  std::stop_source stop_src;  // Create a stop source
  std::array<std::thread, numberTasks> task_workers;

  //----------------------------------------------------------
  // go to idle in main
  //----------------------------------------------------------
  // Create all workers and pass stop tokens
  for (uint i = 0; i < task_workers.size(); ++i) {
    std::cout << "Start Task=" << i << std::endl;

    task_workers[i] = std::move(std::thread(
        [](int number, std::stop_token token) {
          std::cout << "Task " << std::to_string(number) << std::endl;

          // Register a stop callback
          std::stop_callback stop_cb(token, [&]() {
            // Wake thread on stop request
            task_events[number].event_condition.notify_all();
          });

          while (true) {
            {
              // Start of locked block
              std::unique_lock lck(task_events[number].event_mutex);
              task_events[number].event_condition.wait_for(lck, 2s);
            }
            // Stop if requested to stop
            if (token.stop_requested()) {
              break;
            }
          }  // End of while loop
        },
        i, stop_src.get_token()));

    std::this_thread::sleep_for(10ms);
  }

  //----------------------------------------------------------
  // Start application stuff
  //----------------------------------------------------------
  // main loop or sleep
  while (true) {
    std::string cmd;
    std::cout << "\nInput command (q - exit, h - help):" << std::endl;
    std::cin >> cmd;

    if (cmd.compare(0, 1, "q") == 0) {
      std::cout << "close all and quit .." << std::endl;
      break;
    } else if (cmd.starts_with("h")) {
      std::cout << "h     - help" << std::endl;
      std::cout << "q     - exit" << std::endl;
    }
  }

  // set token to stop all worker
  stop_src.request_stop();

  // wakeup all tasks
  for (uint i = 0; i < task_events.size(); ++i) {
    std::unique_lock lck(task_events[i].event_mutex);
    task_events[i].event_condition.notify_all();
  }

  for (uint i = 0; i < task_workers.size(); ++i) {
    // Join threads
    task_workers[i].join();
  }

  return EXIT_SUCCESS;
}
