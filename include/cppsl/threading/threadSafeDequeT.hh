
/*************************************************************************//**
* @file    ThreadSafeDequeT.hh
* @brief   contains A safe double queue container using fixed-size memory allocation
* and constant-time manipulation of elements at either end.
* @author  Alexander Sacharov <a.sacharov@asoft-labs.de>
* @ingroup COM4CPP Library
*****************************************************************************/

#ifndef B827DD47_E2B4_44D3_9C55_2B76CDFBED03
#define B827DD47_E2B4_44D3_9C55_2B76CDFBED03

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------

#include <mutex>
#include <condition_variable>
#include <deque>
#include <memory>

//----------------------------------------------------------------------------
// Public defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public Function Prototypes
//----------------------------------------------------------------------------

namespace cppsl {
   namespace threading {

      /// Class A safe double queue container using fixed-size memory allocation and
      /// constant-time manipulation of elements at either end.
      ///
      /// @author AS
      /// @date 2020-03-05

      template<typename T, typename _TAlloc = std::allocator<T>>
      class ThreadSafeDeque {
      private:
         /// private variables
         mutable std::mutex      mut;
         std::deque<T>           data_deque;
         std::condition_variable data_cond;

      public:
         /// default constructor
         ThreadSafeDeque() {}

         /// copy constructor
         ThreadSafeDeque(ThreadSafeDeque const &other) {
            std::lock_guard<std::mutex> lk(other.mut);
            data_deque = other.data_deque;
         }

         /// clear
         void clear() {
            std::lock_guard<std::mutex> lk(mut);
            data_deque.clear();
         }

         /// push in queue
         void push_front(T new_value) {
            std::lock_guard<std::mutex> lk(mut);
            data_deque.push_front(new_value);
            data_cond.notify_one();
         }

         /// push in queue
         void push_back(T new_value) {
            std::lock_guard<std::mutex> lk(mut);
            data_deque.push_back(new_value);
            data_cond.notify_one();
         }

         /// wait condition "not empty" and pop
         void wait_and_pop_front(T &value) {
            std::unique_lock<std::mutex> lk(mut);
            data_cond.wait(lk, [this] { return !data_deque.empty(); });
            value = data_deque.front();
            data_deque.pop_front();
         }

         /// wait condition "not empty" and pop
         void wait_and_pop_back(T &value) {
            std::unique_lock<std::mutex> lk(mut);
            data_cond.wait(lk, [this] { return !data_deque.empty(); });
            value = data_deque.back();
            data_deque.pop_back();
         }

         /// wait condition "not empty" and pop with return shared pointer
         std::shared_ptr<T> wait_and_pop_front() {
            std::unique_lock<std::mutex> lk(mut);
            data_cond.wait(lk, [this] { return !data_deque.empty(); });
            std::shared_ptr<T> res(std::make_shared<T>(data_deque.front()));
            data_deque.pop_front();
            return res;
         }

         /// wait condition "not empty" and pop with return shared pointer
         std::shared_ptr<T> wait_and_pop_back() {
            std::unique_lock<std::mutex> lk(mut);
            data_cond.wait(lk, [this] { return !data_deque.empty(); });
            std::shared_ptr<T> res(std::make_shared<T>(data_deque.back()));
            data_deque.pop_back();
            return res;
         }

         /// immediately return false if no data, or pop first
         bool try_pop_back(T &value) {
            std::lock_guard<std::mutex> lk(mut);
            if (data_deque.empty()) {
               return false;
            }
            value = data_deque.back();
            data_deque.pop_back();
            return (true);
         }

         /// immediately return empty shared pointer or pop data
         std::shared_ptr<T> try_pop_back() {
            std::lock_guard<std::mutex> lk(mut);
            if (data_deque.empty()) {
               return std::shared_ptr<T>();
            }
            std::shared_ptr<T> res(std::make_shared<T>(data_deque.back()));
            data_deque.pop_back();
            return res;
         }

         /// immediately return false if no data, or pop first
         bool try_pop_front(T &value) {
            std::lock_guard<std::mutex> lk(mut);
            if (data_deque.empty()) {
               return false;
            }
            value = data_deque.front();
            data_deque.pop_front();
            return (true);
         }

         /// immediately return empty shared pointer or pop data
         std::shared_ptr<T> try_pop_front() {
            std::lock_guard<std::mutex> lk(mut);
            if (data_deque.empty()) {
               return std::shared_ptr<T>();
            }
            std::shared_ptr<T> res(std::make_shared<T>(data_deque.front()));
            data_deque.pop_front();
            return res;
         }

         /// return true if empty
         bool empty() const {
            std::lock_guard<std::mutex> lk(mut);
            return data_deque.empty();
         }

         /// return size
         size_t size() const {
            std::lock_guard<std::mutex> lk(mut);
            return data_deque.size();
         }
      };

   } // end of threading
} //namespace cppsl

#endif /* B827DD47_E2B4_44D3_9C55_2B76CDFBED03 */
