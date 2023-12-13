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

using namespace std::chrono_literals;

//-----------------------------------------------------------------------------
// local Defines and Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// local Typedefs, Enums, Unions
//-----------------------------------------------------------------------------

/**
 * @brief event plan structure
 */
struct Event {
 public:
  std::mutex event_mutex;
  std::condition_variable event_condition;
};

//-----------------------------------------------------------------------------
// local/global Variables Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// local/global Function Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// local Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// global Function Definitions
//-----------------------------------------------------------------------------

/**
* @brief   initializes and run stuff.
* @details main function
*/
int main(int argc, char** argv) {
  // configuration
  std::stop_source stop_src;   // Create a stop source

  // Create sleeper thread
  std::thread tSleep = std::move(std::thread(
     [](std::stop_token token) {
       std::mutex sem;
       std::condition_variable event;

       std::cout << "Sleeper thread" << std::endl;

       // Register a stop callback
       std::stop_callback stop_cb(token, [&]() {
         // Wake thread on stop request
         event.notify_all();
       });

       while (true) {
         {
           // Start of locked block
           std::unique_lock lck(sem);
           event.wait_for(lck, 10s);
         }
         // Stop if requested to stop
         if (token.stop_requested()) {
           break;
         }
       }   // End of while loop
     },
     stop_src.get_token()));

  // Create sleeper thread
  std::thread tWaker = std::move(std::thread(
     [](std::stop_token token) {
       std::mutex sem;
       std::condition_variable event;

       std::cout << "Task " << std::to_string(number) << std::endl;

       // Register a stop callback
       std::stop_callback stop_cb(token, [&]() {
         // Wake thread on stop request
         event.notify_all();
       });

       while (true) {
         {
           // Start of locked block
           std::unique_lock lck(sem);
           event.wait_for(lck, 10s);
         }
         // Stop if requested to stop
         if (token.stop_requested()) {
           break;
         }
       }   // End of while loop
     },
     stop_src.get_token()));

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
