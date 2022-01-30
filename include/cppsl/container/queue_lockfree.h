//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************//**
 * \file    LockFreeQueue.hpp
 * \brief   contains lock free queue.
 * \author  Alexander Sacharov
 * \date    2022-01-11
 * \ingroup
 *****************************************************************************/

#ifndef A15BA77F_6AEC_4A8D_ADC4_5162C993D493
#define A15BA77F_6AEC_4A8D_ADC4_5162C993D49

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

namespace cppsl::threading {

  /// @brief lock free queue starting C++11
  /// @author Alexander Sacharov (AS)
  /// @date   2016/02/26

  template<typename T>
  class LockFreeQueue {
   private:
    /// copy constructor and =operation
    LockFreeQueue(const LockFreeQueue &other);
    LockFreeQueue &operator=(const LockFreeQueue &other);

    /// queue node
    struct node {
      std::shared_ptr<T> data;
      node *next;
      node() : next(nullptr) {}
    };

    /// atomic head and tail
    std::atomic<node *> head;
    std::atomic<node *> tail;

    /// pop from head (FIFO)
    node *pop_head() {
      node *const old_head = head.load();
      if (old_head == tail.load()) {
        return nullptr;
      }
      head.store(old_head->next);
      return old_head;
    }

   public:
    /// default constructor
    LockFreeQueue() :
       head(new node), tail(head.load()) {}

    /// destructor
    ~LockFreeQueue() {
      while (node *const old_head = head.load()) {
        head.store(old_head->next);
        delete old_head;
      }
    }

    std::shared_ptr<T> try_pop() {
      std::unique_ptr<node> old_head = pop_head();
      return old_head ? old_head->data : std::shared_ptr<T>();
    }

    /// pop from queue
    std::shared_ptr<T> pop() {
      node *old_head = pop_head();
      if (!old_head) {
        return std::shared_ptr<T>();
      }
      std::shared_ptr<T> const res(old_head->data);
      delete old_head;
      return res;
    }

    /// push in back
    void push(T new_value) {
      std::shared_ptr<T> new_data(std::make_shared<T>(new_value));
      node *p = new node;
      node *const old_tail = tail.load();
      old_tail->data.swap(new_data);
      old_tail->next = p;
      tail.store(p);
    }
  };

} //namespace cppsl::threading

#endif /* A15BA77F_6AEC_4A8D_ADC4_5162C993D493 */
