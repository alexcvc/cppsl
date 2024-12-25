//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************/ /**
 * @file
 * \brief   contains lock free queue.
 * \author  Alexander Sacharov
 * \date    2022-01-11
 * \ingroup
 *****************************************************************************/

#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <atomic>
#include <memory>

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
 * @class QueueLockFree
 * @brief A lock-free queue data structure implementation.
 *
 * This class provides a lock-free queue data structure, where multiple threads can
 * simultaneously push and pop elements without encountering any races or deadlocks.
 *
 * @tparam T The type of the elements stored in the queue.
 */
template <typename T>
class QueueLockFree {
 private:
  /**
     * @class QueueLockFree
     * @brief A lock-free queue data structure implementation.
     *
     * This class provides a lock-free queue data structure, where multiple threads can
     * simultaneously push and pop elements without encountering any races or deadlocks.
     *
     * @tparam T The type of the elements stored in the queue.
     */
  QueueLockFree(const QueueLockFree& other);
  /**
     * @class QueueLockFree
     * @brief A lock-free queue data structure implementation.
     *
     * This class provides a lock-free queue data structure, where multiple threads can
     * simultaneously push and pop elements without encountering any races or deadlocks.
     *
     * @tparam T The type of the elements stored in the queue.
     */
  QueueLockFree& operator=(const QueueLockFree& other);

  /**
     * @class node
     * @brief A node used in the QueueLockFree class.
     *
     * This class represents a node in the lock-free queue data structure.
     * It contains a shared pointer to the data and a pointer to the next node.
     *
     * @tparam T The type of the elements stored in the queue.
     */
  struct node {
    std::shared_ptr<T> m_dataSp;
    node* m_next;

    node() : m_next(nullptr) {}
  };

  /// atomic head and tail of queue
  std::atomic<node*> m_head;
  std::atomic<node*> m_tail;

  /**
     * @brief Removes and returns the first node from the head of the lock-free queue.
     *
     * This function pops the first node from the head of the lock-free queue (FIFO).
     * If the queue is empty, it returns a nullptr.
     *
     * @return A pointer to the first node on success, or nullptr if the queue is empty.
     */
  node* pop_head() {
    node* const old_head = m_head.load();
    if (old_head == m_tail.load()) {
      return nullptr;
    }
    m_head.store(old_head->m_next);
    return old_head;
  }

 public:
  /**
     * @class QueueLockFree
     * @brief A lock-free queue data structure implementation.
     *
     * This class provides a lock-free queue data structure, where multiple threads can
     * simultaneously push and pop elements without encountering any races or deadlocks.
     *
     * @tparam T The type of the elements stored in the queue.
     */
  QueueLockFree() : m_head(new node), m_tail(m_head.load()) {}

  /**
     * @class QueueLockFree
     * @brief A lock-free queue data structure implementation.
     *
     * This class provides a lock-free queue data structure, where multiple threads can
     * simultaneously push and pop elements without encountering any races or deadlocks.
     *
     * @tparam T The type of the elements stored in the queue.
     */
  ~QueueLockFree() {
    while (node* const old_head = m_head.load()) {
      m_head.store(old_head->m_next);
      delete old_head;
    }
  }

  /**
    * @brief Try to pop an element from the front of the lock-free queue.
    *
    * This function tries to pop an element from the front of the lock-free queue.
    * If the queue is empty, it returns an empty shared pointer.
    *
    * @tparam T The type of the elements stored in the queue.
    *
    * @return std::shared_ptr<T> A shared pointer to the popped element if successful,
    * or an empty shared pointer if the queue is empty.
    */
  std::shared_ptr<T> try_pop() {
    std::unique_ptr<node> old_head = pop_head();
    return old_head ? old_head->m_dataSp : std::shared_ptr<T>();
  }

  /**
     * @brief Pop an element from the front of the lock-free queue.
     *
     * This function pops an element from the front of the lock-free queue.
     * If the queue is empty, it returns an empty shared pointer.
     *
     * @tparam T The type of the elements stored in the queue.
     *
     * @return std::shared_ptr<T> A shared pointer to the popped element.
     */
  std::shared_ptr<T> pop() {
    node* old_head = pop_head();
    if (!old_head) {
      return std::shared_ptr<T>();
    }
    std::shared_ptr<T> const res(old_head->m_dataSp);
    delete old_head;
    return res;
  }

  /**
   * @brief Pushes a new value into the lock-free queue.
   *
   * This function pushes a new value into the lock-free queue. It creates a new node with
   * the given value, and updates the tail of the queue to point to this new node.
   *
   * @tparam T The type of the value to be pushed into the queue.
   * @param new_value The new value to be pushed into the queue.
   */
  void push(T new_value) {
    std::shared_ptr<T> new_data(std::make_shared<T>(new_value));
    node* p = new node;
    node* const old_tail = m_tail.load();
    old_tail->m_dataSp.swap(new_data);
    old_tail->m_next = p;
    m_tail.store(p);
  }
};

}   //namespace cppsl::container
