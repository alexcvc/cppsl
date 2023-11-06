//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************//**
 * @file
 * \brief   contains lock free queue.
 * \author  Alexander Sacharov
 * \date    2022-01-11
 * \ingroup
 *****************************************************************************/

#ifndef INCLUDE_CPPSL_CONTAINER_QUEUE_LOCKFREE_H
#define INCLUDE_CPPSL_CONTAINER_QUEUE_LOCKFREE_H

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <memory>
#include <atomic>

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

  /// @brief lock free queue starting C++11
  /// @author Alexander Sacharov (AS)
  /// @date   2016/02/26

  template<typename T>
  class QueueLockFree {
   private:
    /// copy constructor and =operation
    QueueLockFree(const QueueLockFree &other);
    QueueLockFree &operator=(const QueueLockFree &other);

    /// queue node
    struct node {
      std::shared_ptr<T> m_dataSp;
      node              *m_next;

      node() : m_next(nullptr) {}
    };

    /// atomic head and tail of queue
    std::atomic<node *> m_head;
    std::atomic<node *> m_tail;

    /// pop from m_head (FIFO)
    node *pop_head() {
      node *const old_head = m_head.load();
      if (old_head == m_tail.load()) {
        return nullptr;
      }
      m_head.store(old_head->m_next);
      return old_head;
    }

   public:
    /// default constructor
    QueueLockFree() :
       m_head(new node), m_tail(m_head.load()) {}

    /// destructor
    ~QueueLockFree() {
      while (node *const old_head = m_head.load()) {
        m_head.store(old_head->m_next);
        delete old_head;
      }
    }

    std::shared_ptr<T> try_pop() {
      std::unique_ptr<node> old_head = pop_head();
      return old_head ? old_head->m_dataSp : std::shared_ptr<T>();
    }

    /// pop from queue
    std::shared_ptr<T> pop() {
      node *old_head = pop_head();
      if (!old_head) {
        return std::shared_ptr<T>();
      }
      std::shared_ptr<T> const res(old_head->m_dataSp);
      delete old_head;
      return res;
    }

    /// push in back
    void push(T new_value) {
      std::shared_ptr<T> new_data(std::make_shared<T>(new_value));
      node *p = new node;
      node *const old_tail = m_tail.load();
      old_tail->m_dataSp.swap(new_data);
      old_tail->m_next = p;
      m_tail.store(p);
    }
  };

} //namespace cppsl::container

#endif /* INCLUDE_CPPSL_CONTAINER_QUEUE_LOCKFREE_H */
