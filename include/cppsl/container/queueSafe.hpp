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

#ifndef INCLUDE_CPPSL_CONTAINER_QUEUE_THREADSAFE_H
#define INCLUDE_CPPSL_CONTAINER_QUEUE_THREADSAFE_H

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

//----------------------------------------------------------------------------
// Defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

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
    std::lock_guard<std::mutex> lk(other.mut);
    data_queue = other.data_queue;
  }

  /**
   * @brief Inserts a new value into the queue.
   *
   * This function locks the mutex, pushes the new value into the data queue, and
   * notifies one waiting thread that new data is available.
   *
   * @tparam T The type of elements in the queue.
   * @param new_value The new value to insert.
   */
  void push(T new_value) {
    std::lock_guard<std::mutex> lk(mut);
    data_queue.push(new_value);
    data_cond.notify_one();
  }

  /**
   * @fn void wait_and_pop(T& value)
   * @brief Waits until the queue is not empty, pops the front element from the queue, and assigns it to the specified value.
   *
   * This function waits until the queue is not empty by using a condition variable. It then locks the mutex, pops the front element
   * from the queue, assigns it to the specified value, and removes it from the queue.
   *
   * @tparam T The type of elements in the queue.
   * @param value A reference to a variable where the popped element will be assigned.
   */
  void wait_and_pop(T& value) {
    std::unique_lock<std::mutex> lk(mut);
    data_cond.wait(lk, [this] { return !data_queue.empty(); });
    value = data_queue.front();
    data_queue.pop();
  }

  /**
   * @fn std::shared_ptr<T> QueueSafe<T>::wait_and_pop()
   * @brief Waits until the queue is not empty, pops the front element from the queue, and returns it as a shared pointer.
   *
   * This function waits until the queue is not empty by using a condition variable. It then locks the mutex, pops the front element
   * from the queue, creates a shared pointer to the popped element, and removes it from the queue. The shared pointer is returned.
   * This function assumes that the underlying type T has a copy constructor.
   *
   * @tparam T The type of elements in the queue.
   * @return A shared pointer to the popped element.
   */
  std::shared_ptr<T> wait_and_pop() {
    std::unique_lock<std::mutex> lk(mut);
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
    std::lock_guard<std::mutex> lk(mut);
    if (data_queue.empty()) {
      return false;
    }
    value = data_queue.front();
    data_queue.pop();
    return (true);
  }

  /**
   * @fn std::shared_ptr<T> try_pop()
   * @brief Immediately returns an empty shared pointer or pops the front element from the queue and returns it as a shared pointer.
   *
   * This function immediately locks the mutex, checks if the queue is empty, and returns an empty shared pointer if it is.
   * Otherwise, it creates a shared pointer to the front element of the queue, removes the front element from the queue,
   * and returns the shared pointer. This function assumes that the underlying type T has a copy constructor.
   *
   * @tparam T The type of elements in the queue.
   * @return A shared pointer to the popped element, or an empty shared pointer if the queue is empty.
   */
  std::shared_ptr<T> try_pop() {
    std::lock_guard<std::mutex> lk(mut);
    if (data_queue.empty()) {
      return std::shared_ptr<T>();
    }
    std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
    data_queue.pop();
    return res;
  }

  /**
   * @fn bool QueueSafe<T>::empty() const
   * @brief Checks whether the queue is empty.
   *
   * This function locks the mutex using std::lock_guard and checks whether the data_queue is empty.
   *
   * @tparam T The type of elements in the queue.
   * @return True if the queue is empty, false otherwise.
   */
  bool empty() const {
    std::lock_guard<std::mutex> lk(mut);
    return data_queue.empty();
  }

  /**
   * @fn size_t QueueSafe<T>::size() const
   * @brief Returns the size of the queue.
   *
   * This function locks the mutex, retrieves the size of the data queue, and returns it.
   *
   * @tparam T The type of elements in the queue.
   * @return The size of the queue.
   */
  size_t size() const {
    std::lock_guard<std::mutex> lk(mut);
    return data_queue.size();
  }
};

}   // namespace cppsl::container

#endif /* INCLUDE_CPPSL_CONTAINER_QUEUE_THREADSAFE_H */
