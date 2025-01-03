//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************/ /**
 * @file
 * \brief   contains a safe dual queue container using fixed size memory 
 * allocation and handling of elements at both ends in constant time..
 * \author  Alexander Sacharov
 * \date    2022-01-29
 * \ingroup Container
 *****************************************************************************/

#pragma once

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>

//----------------------------------------------------------------------------
// Public Prototypes
//----------------------------------------------------------------------------

namespace cppsl::container {

/**
 * @class DequeSafe
 * @brief A thread-safe implementation of deque container.
 *
 * The DequeSafe class implements a thread-safe version of the deque container,
 * which allows multiple threads to concurrently perform operations on the container
 * without causing any data race. It uses a mutex and condition variable to ensure
 * synchronized access to the underlying deque.
 *
 * @tparam T The type of elements stored in the deque.
 * @tparam TAlloc The allocator type for element construction, defaults to std::allocator<T>.
 */
template <typename T, class TAlloc = std::allocator<T>>
class DequeSafe {

  mutable std::mutex m_semaphore;
  std::condition_variable m_condition;
  std::deque<T> m_container;

 public:
  /**
    * @brief Default constructor for the DequeSafe class.
    */
  DequeSafe() = default;

  /**
   * @brief Constructs a DequeSafe object by copying the contents of another DequeSafe object.
   * @param other The DequeSafe object to copy from.
   */
  DequeSafe(DequeSafe const& other) {
    std::lock_guard lk(other.m_semaphore);
    m_container = other.m_container;
  }

  /**
    * @brief Removes all elements from the container.
    */
  void clear() {
    std::lock_guard lk(m_semaphore);
    m_container.clear();
  }

  /**
    * @brief Adds a new element to the front of the deque container.
    * @tparam T The type of elements stored in the deque.
    * @param new_value The value to be added to the deque.
    */
  void push_front(T new_value) {
    std::lock_guard lk(m_semaphore);
    m_container.push_front(new_value);
    m_condition.notify_one();
  }

  /**
   * @brief Adds a new element to the end of the deque container.
   * @tparam T The type of elements stored in the deque.
   * @param new_value The value to be added to the deque.
   */
  void push_back(T new_value) {
    std::lock_guard lk(m_semaphore);
    m_container.push_back(new_value);
    m_condition.notify_one();
  }

  /**
   * @brief Waits until the deque is not empty and pops the first element from the deque.
   * @tparam T The type of elements stored in the deque.
   * @param value A reference to store the popped element.
   */
  void wait_and_pop_front(T& value) {
    std::unique_lock<std::mutex> lk(m_semaphore);
    m_condition.wait(lk, [this] { return !m_container.empty(); });
    value = m_container.front();
    m_container.pop_front();
  }

  /**
   * @brief Waits until the deque is not empty and pops the last element.
   * @tparam T The type of elements stored in the deque.
   * @param value A reference to store the popped element.
   */
  void wait_and_pop_back(T& value) {
    std::unique_lock lk(m_semaphore);
    m_condition.wait(lk, [this] { return !m_container.empty(); });
    value = m_container.back();
    m_container.pop_back();
  }

  /**
   * @brief Waits until the deque is not empty and pops the first element from the deque.
   * @tparam T The type of elements stored in the deque.
   * @return A shared pointer to the first element of the deque.
   */
  std::shared_ptr<T> wait_and_pop_front() {
    std::unique_lock lk(m_semaphore);
    m_condition.wait(lk, [this] { return !m_container.empty(); });
    std::shared_ptr<T> res(std::make_shared<T>(m_container.front()));
    m_container.pop_front();
    return res;
  }

  /**
    * @brief Waits until the deque is not empty and pops the last element from the deque.
    * @tparam T The type of elements stored in the deque.
    * @return A shared pointer to the last element of the deque.
    */
  std::shared_ptr<T> wait_and_pop_back() {
    std::unique_lock lk(m_semaphore);
    m_condition.wait(lk, [this] { return !m_container.empty(); });
    std::shared_ptr<T> res(std::make_shared<T>(m_container.back()));
    m_container.pop_back();
    return res;
  }

  /**
  * @brief Attempts to pop the last element from the deque if it is not empty.
  * @tparam T The type of elements stored in the deque.
  * @param value A reference to store the popped element.
  * @return True if an element was successfully popped, false otherwise.
  */
  bool try_pop_back(T& value) {
    std::lock_guard lk(m_semaphore);
    if (m_container.empty()) {
      return false;
    }
    value = m_container.back();
    m_container.pop_back();
    return (true);
  }

  /**
   * @brief Tries to pop the last element from the deque if it is not empty.
   * @return A shared pointer to the popped element, or nullptr if the deque is empty.
   */
  std::shared_ptr<T> try_pop_back() {
    std::lock_guard lk(m_semaphore);
    if (m_container.empty()) {
      return std::shared_ptr<T>();
    }
    std::shared_ptr<T> res(std::make_shared<T>(m_container.back()));
    m_container.pop_back();
    return res;
  }

  /**
   * @brief Tries to pop the first element from the deque if it is not empty.
   * @param value The variable to store the popped element.
   * @return True if an element was successfully popped, false otherwise.
   */
  bool try_pop_front(T& value) {
    std::lock_guard lk(m_semaphore);
    if (m_container.empty()) {
      return false;
    }
    value = m_container.front();
    m_container.pop_front();
    return true;
  }

  /**
   * @brief Tries to pop the first element from the deque if it is not empty.
   * @return A shared pointer to the popped element, or nullptr if the deque is empty.
   */
  std::shared_ptr<T> try_pop_front() {
    std::lock_guard lk(m_semaphore);
    if (m_container.empty()) {
      return std::shared_ptr<T>();
    }
    std::shared_ptr<T> res(std::make_shared<T>(m_container.front()));
    m_container.pop_front();
    return res;
  }

  /**
   * @brief Check if the container is empty.
   * @return True if the container is empty, False otherwise.
   */
  bool empty() const {
    std::lock_guard lk(m_semaphore);
    return m_container.empty();
  }

  /**
   * @brief Returns the size of the container.
   * @return The size of the container.
   */
  size_t size() const {
    std::lock_guard lk(m_semaphore);
    return m_container.size();
  }
};

}   // namespace cppsl::container
