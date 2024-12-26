//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************/ /**
* @file    list_threadsafe.h
* @brief   thread safe list class.
* @author  Alexander Sacharov <a.sacharov@asoft-labs.de>
* @ingroup COM4CPP Library
*****************************************************************************/

#pragma once

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <memory>
#include <mutex>

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
 * @class ListSafe
 * @brief Thread-safe linked list class
 *
 * This class is designed to provide a thread-safe linked list that can be accessed concurrently by multiple threads.
 * It uses locks to ensure that only one thread can modify the list at a time, preventing race conditions.
 *
 * @tparam T The type of elements in the list
 */
template <typename T>
class ListSafe {
  struct node {
    std::mutex m_mutex;
    std::shared_ptr<T> m_dataSp;
    std::unique_ptr<node> m_next;

    node() : m_next() {}

    node(T const& value) : m_dataSp(std::make_shared<T>(value)) {}
  };

  node head;

 public:
  ListSafe() = default;
  ~ListSafe() {
    remove_if([](T const&) { return true; });
  }

  /**
   * @class ListSafe
   * @brief Thread-safe linked list class
   * @tparam T The type of elements in the list
   */
  void clean() {
    remove_if([](T const&) { return true; });
  }

  /**
   * @class ListSafe
   * @brief Thread-safe linked list class
   * @param T The type of elements in the list
   */
  void push_front(T const& value) {
    std::unique_ptr<node> new_node(new node(value));

    std::lock_guard<std::mutex> lk(head.m_mutex);
    new_node->m_next = std::move(head.m_next);
    head.m_next = std::move(new_node);
  }

  /**
   * @brief This file contains the implementation of a thread-safe linked list class.
   */
  template <typename Function>
  void for_each(Function f) {
    node* current = &head;

    std::unique_lock<std::mutex> lk(head.m_mutex);
    while (node* const m_next = current->m_next.get()) {
      std::unique_lock<std::mutex> next_lk(m_next->m_mutex);
      lk.unlock();
      f(*m_next->m_dataSp);
      current = m_next;
      lk = std::move(next_lk);
    }
  }

  /**
   * @brief This file contains the implementation of a thread-safe linked list class.
   */
  template <typename Predicate>
  std::shared_ptr<T> find_first_if(Predicate p) {
    node* current = &head;

    std::unique_lock<std::mutex> lk(head.m_mutex);
    while (node* const m_next = current->m_next.get()) {
      std::unique_lock<std::mutex> next_lk(m_next->m_mutex);
      lk.unlock();
      if (p(*m_next->m_dataSp)) {
        return m_next->m_dataSp;
      }
      current = m_next;
      lk = std::move(next_lk);
    }
    return std::shared_ptr<T>();
  }

  /**
    * @brief This file contains the implementation of a thread-safe linked list class.
    */
  template <typename Predicate>
  void remove_if(Predicate p) {
    node* current = &head;
    std::unique_lock<std::mutex> lk(head.m_mutex);
    while (node* const m_next = current->m_next.get()) {
      std::unique_lock<std::mutex> next_lk(m_next->m_mutex);
      if (p(*m_next->m_dataSp)) {
        std::unique_ptr<node> old_next = std::move(current->m_next);
        current->m_next = std::move(m_next->m_next);
        next_lk.unlock();
      } else {
        lk.unlock();
        current = m_next;
        lk = std::move(next_lk);
      }
    }
  }

 private:
  ListSafe(ListSafe const& other) { throw std::logic_error("The method not implemented."); }
  ListSafe& operator=(ListSafe const& other) { throw std::logic_error("The method not implemented."); }
};

}   // namespace cppsl::container
