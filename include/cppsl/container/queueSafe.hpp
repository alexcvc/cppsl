//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************//**
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
#include <mutex>
#include <condition_variable>
#include <queue>
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

   template<typename T>
   class QueueSafe {
      private:
      /// private variables
      mutable std::mutex      mut;
      std::queue<T>           data_queue;
      std::condition_variable data_cond;

      public:
      /// default constructor
      QueueSafe() {}

      /// copy constructor
      QueueSafe(QueueSafe const &other) {
         std::lock_guard<std::mutex> lk(other.mut);
         data_queue = other.data_queue;
      }

      /// push in queue
      void push(T new_value) {
         std::lock_guard<std::mutex> lk(mut);
         data_queue.push(new_value);
         data_cond.notify_one();
      }

      /// wait condition "not empty" and pop
      void wait_and_pop(T &value) {
         std::unique_lock<std::mutex> lk(mut);
         data_cond.wait(lk, [this] { return !data_queue.empty(); });
         value = data_queue.front();
         data_queue.pop();
      }

      /// wait condition "not empty" and pop with return shared pointer
      std::shared_ptr<T> wait_and_pop() {
         std::unique_lock<std::mutex> lk(mut);
         data_cond.wait(lk, [this] { return !data_queue.empty(); });
         std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
         data_queue.pop();
         return res;
      }

      /// immediately return false if no data, or pop first
      bool try_pop(T &value) {
         std::lock_guard<std::mutex> lk(mut);
         if (data_queue.empty()) {
            return false;
         }
         value = data_queue.front();
         data_queue.pop();
         return (true);
      }

      /// immediately return empty shared pointer or pop data
      std::shared_ptr<T> try_pop() {
         std::lock_guard<std::mutex> lk(mut);
         if (data_queue.empty()) {
            return std::shared_ptr<T>();
         }
         std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
         data_queue.pop();
         return res;
      }

      /// return true if empty
      bool empty() const {
         std::lock_guard<std::mutex> lk(mut);
         return data_queue.empty();
      }

      /// return size
      size_t size() const {
         std::lock_guard<std::mutex> lk(mut);
         return data_queue.size();
      }
   };

} // namespace cppsl::container

#endif /* INCLUDE_CPPSL_CONTAINER_QUEUE_THREADSAFE_H */
