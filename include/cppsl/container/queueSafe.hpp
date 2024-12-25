//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************/ /**
* @file    queue_threadsafe.h
* @brief   thread safe queue with locking.
* @author  Alexander Sacharov <a.sacharov@asoft-labs.de>
* @ingroup Containers
*****************************************************************************/

#pragma once

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

//----------------------------------------------------------------------------
// Public Prototypes
//----------------------------------------------------------------------------

namespace cppsl::container {

/**
 * @class QueueSafe
 * @brief A thread-safe implementation of a queue with a mutex and condition variable.
 * @tparam T The type of elements to be stored in the queue.
 */
template <typename T>
class QueueSafe {
 private:
  /// private variables
  mutable std::mutex mut;
  std::queue<T> data_queue;
  std::condition_variable data_cond;

 public:
  /**
   * @brief Default constructor.
   */
  QueueSafe() {}

  /**
   * @brief copy constructor
   */
  QueueSafe(QueueSafe const& other) {
    std::lock_guard lk(other.mut);
    data_queue = other.data_queue;
  }

  /**
   * @brief Inserts a new value into the queue.
   * @tparam T The type of elements in the queue.
   * @param new_value The new value to insert.
   */
  void push(T new_value) {
    std::lock_guard lk(mut);
    data_queue.push(new_value);
    data_cond.notify_one();
  }

  /**
   * @brief Waits until the queue is not empty, pops the front element from the queue, and assigns it to the specified value.
   * @tparam T The type of elements in the queue.
   * @param value A reference to a variable where the popped element will be assigned.
   */
  void wait_and_pop(T& value) {
    std::unique_lock lk(mut);
    data_cond.wait(lk, [this] { return !data_queue.empty(); });
    value = data_queue.front();
    data_queue.pop();
  }

  /**
   * @brief Waits until the queue is not empty, pops the front element from the queue, and returns it as a shared pointer.
   * @tparam T The type of elements in the queue.
   * @return A shared pointer to the popped element.
   */
  std::shared_ptr<T> wait_and_pop() {
    std::unique_lock lk(mut);
    data_cond.wait(lk, [this] { return !data_queue.empty(); });
    std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
    data_queue.pop();
    return res;
  }

  /**
   * @brief immediately return false if no data, or pop first
   * @tparam T The type of elements to be stored in the queue.
   */
  bool try_pop(T& value) {
    std::lock_guard lk(mut);
    if (data_queue.empty()) {
      return false;
    }
    value = data_queue.front();
    data_queue.pop();
    return (true);
  }

  /**
   * @brief Immediately returns an empty shared pointer or pops the front element from the queue and returns it as a shared pointer.
   * @tparam T The type of elements in the queue.
   * @return A shared pointer to the popped element, or an empty shared pointer if the queue is empty.
   */
  std::shared_ptr<T> try_pop() {
    std::lock_guard lk(mut);
    if (data_queue.empty()) {
      return std::shared_ptr<T>();
    }
    std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
    data_queue.pop();
    return res;
  }

  /**
   * @brief Checks whether the queue is empty.
   * @tparam T The type of elements in the queue.
   * @return True if the queue is empty, false otherwise.
   */
  bool empty() const {
    std::lock_guard lk(mut);
    return data_queue.empty();
  }

  /**
   * @brief Returns the size of the queue.
   * @tparam T The type of elements in the queue.
   * @return The size of the queue.
   */
  size_t size() const {
    std::lock_guard lk(mut);
    return data_queue.size();
  }
};

}   // namespace cppsl::container
