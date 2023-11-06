//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************//**
 * @file
 * \brief   contains a safe dual queue container using fixed size memory 
 * allocation and handling of elements at both ends in constant time..
 * \author  Alexander Sacharov
 * \date    2022-01-29
 * \ingroup Container
 *****************************************************************************/

#ifndef INCLUDE_CPPSL_CONCURENCY_DEQUE_LOCK_HPP
#define INCLUDE_CPPSL_CONCURENCY_DEQUE_LOCK_HPP

//-----------------------------------------------------------------------------
// includes 
//-----------------------------------------------------------------------------

#include <condition_variable>
#include <deque>
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

namespace cppsl::concurency {

//----------------------------------------------------------------------------
// Public defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public Function Prototypes
//----------------------------------------------------------------------------

   /// A safe dual queue container using fixed size memory allocation and handling
   /// of elements at both ends in constant time.

   template<typename T, typename _TAlloc = std::allocator<T>>
   class DequeSafe {

      private:
      mutable std::mutex m_semaphore;
      std::condition_variable m_condition;
      std::deque<T> m_container;

      public:
      /// default constructor
       DequeSafe() {}

      /// copy constructor
      /// \param rvalue dequeue
       DequeSafe(DequeSafe const &other) {
         std::lock_guard<std::mutex> lk(other.m_semaphore);
         m_container = other.m_container;
      }

      /// clear
      void clear() {
         std::lock_guard<std::mutex> lk(m_semaphore);
         m_container.clear();
      }

      /// push in queue front
      /// \param pushed value
      void push_front(T new_value) {
         std::lock_guard<std::mutex> lk(m_semaphore);
         m_container.push_front(new_value);
         m_condition.notify_one();
      }

      /// push in queue back
      /// \param pushed value
      void push_back(T new_value) {
         std::lock_guard<std::mutex> lk(m_semaphore);
         m_container.push_back(new_value);
         m_condition.notify_one();
      }

      /// wait condition "not empty" and pop
      /// \param value - pop value from
      void wait_and_pop_front(T &value) {
         std::unique_lock<std::mutex> lk(m_semaphore);
         m_condition.wait(lk, [this] { return !m_container.empty(); });
         value = m_container.front();
         m_container.pop_front();
      }

      /// wait condition "not empty" and pop
      void wait_and_pop_back(T &value) {
         std::unique_lock<std::mutex> lk(m_semaphore);
         m_condition.wait(lk, [this] { return !m_container.empty(); });
         value = m_container.back();
         m_container.pop_back();
      }

      /// wait condition "not empty" and pop with return shared pointer
      std::shared_ptr<T> wait_and_pop_front() {
         std::unique_lock<std::mutex> lk(m_semaphore);
         m_condition.wait(lk, [this] { return !m_container.empty(); });
         std::shared_ptr<T> res(std::make_shared<T>(m_container.front()));
         m_container.pop_front();
         return res;
      }

      /// wait condition "not empty" and pop with return shared pointer
      std::shared_ptr<T> wait_and_pop_back() {
         std::unique_lock<std::mutex> lk(m_semaphore);
         m_condition.wait(lk, [this] { return !m_container.empty(); });
         std::shared_ptr<T> res(std::make_shared<T>(m_container.back()));
         m_container.pop_back();
         return res;
      }

      /// immediately return false if no data, or pop first
      bool try_pop_back(T &value) {
         std::lock_guard<std::mutex> lk(m_semaphore);
         if (m_container.empty()) {
            return false;
         }
         value = m_container.back();
         m_container.pop_back();
         return (true);
      }

      /// immediately return empty shared pointer or pop data
      std::shared_ptr<T> try_pop_back() {
         std::lock_guard<std::mutex> lk(m_semaphore);
         if (m_container.empty()) {
            return std::shared_ptr<T>();
         }
         std::shared_ptr<T> res(std::make_shared<T>(m_container.back()));
         m_container.pop_back();
         return res;
      }

      /// immediately return false if no data, or pop first
      bool try_pop_front(T &value) {
         std::lock_guard<std::mutex> lk(m_semaphore);
         if (m_container.empty()) {
            return false;
         }
         value = m_container.front();
         m_container.pop_front();
         return (true);
      }

      /// immediately return empty shared pointer or pop data
      std::shared_ptr<T> try_pop_front() {
         std::lock_guard<std::mutex> lk(m_semaphore);
         if (m_container.empty()) {
            return std::shared_ptr<T>();
         }
         std::shared_ptr<T> res(std::make_shared<T>(m_container.front()));
         m_container.pop_front();
         return res;
      }

      /// return true if empty
      bool empty() const {
         std::lock_guard<std::mutex> lk(m_semaphore);
         return m_container.empty();
      }

      /// return size
      size_t size() const {
         std::lock_guard<std::mutex> lk(m_semaphore);
         return m_container.size();
      }
   };

} // end of namespace

#endif /* INCLUDE_CPPSL_CONCURENCY_DEQUE_LOCK_HPP */
