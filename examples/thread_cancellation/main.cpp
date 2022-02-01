//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/// Using std::future, you can block in limited intervals and eventually decide to stop
/// waiting and detach the thread.
///
/// Leaking a running thread won't matter if your going to exit anyway. If not exiting,
/// then either the blocking call will return naturally and the thread will be destroyed,
/// or it will be blocked for the rest of the process' lifetime anyway.
///
/// In the following example MAX_WAIT_TIME is used as a condition for detaching and exiting.

#include <chrono>
#include <future>
#include <thread>

using namespace std::literals;

const auto BLOCK_DURATION = 100ms;
const auto MAX_WAIT_TIME = 3s;

int main() {
  // Some blocking call.
  auto blocking_call = [] { std::this_thread::sleep_for(1min); };

  // Wrap the callable in a packaged task and get future.
  std::packaged_task<void()> task{blocking_call};
  auto future = task.get_future();

  auto start_time = std::chrono::steady_clock::now();
  std::thread t{std::move(task)};

  // Continually check if task is finished, i.e., if blocking call has returned.
  auto status = future.wait_for(0s);
  while (status != std::future_status::ready) {
    status = future.wait_for(BLOCK_DURATION);

    // Room for condition to stop blocking, e.g., check some signaling etc.
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start_time);
    if (duration > MAX_WAIT_TIME) {
      break;
    }
  }

  if (status == std::future_status::ready) {
    future.get(); // Optionally get return value.
    t.join();
  } else {
    t.detach();
  }

  /* And we're on our way... */


  /// std::future::wait_for will immediately return when the call returns,
  /// even if the full wait duration have not yet come to pass.
  ///
  /// If time is the limit for how long the call may block, then there's
  /// even the neat std::future::wait_until. E.g.:

  //  auto start_time = std::chrono::steady_clock::now();
  //  std::thread t{std::move(task)};
  //
  //  auto status = future.wait_until(start_time + MAX_WAIT_TIME);
  //
  //  if (status == std::future_status::ready) {
  //    t.join();
  //  } else {
  //    t.detach();
  //  }
}


