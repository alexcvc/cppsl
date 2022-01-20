
/*************************************************************************//**
* @file    threadSafeQueueT.hh
* @brief   thread safe queue with locking.
* @author  Alexander Sacharov <a.sacharov@asoft-labs.de>
* @ingroup COM4CPP Library
*****************************************************************************/

#ifndef A8C6771A_3F8D_46A5_8962_3D92D963C0C5
#define A8C6771A_3F8D_46A5_8962_3D92D963C0C5

#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>

namespace cppsl {
   namespace threading {

      template<typename T>
      class threadSafeQueue
      {
      private:
          /// private variables
          mutable std::mutex mut;
          std::queue<T> data_queue;
          std::condition_variable data_cond;

      public:
          /// default constructor
          threadSafeQueue() {}

          /// copy constructor
          threadSafeQueue(threadSafeQueue const& other)
          {
              std::lock_guard<std::mutex> lk(other.mut);
              data_queue = other.data_queue;
          }

          /// push in queue
          void push(T new_value)
          {
              std::lock_guard<std::mutex> lk(mut);
              data_queue.push(new_value);
              data_cond.notify_one();
          }

          /// wait condition "not empty" and pop
          void wait_and_pop(T& value)
          {
              std::unique_lock<std::mutex> lk(mut);
              data_cond.wait(lk, [this] {return !data_queue.empty();});
              value = data_queue.front();
              data_queue.pop();
          }

          /// wait condition "not empty" and pop with return shared pointer
          std::shared_ptr<T> wait_and_pop()
          {
              std::unique_lock<std::mutex> lk(mut);
              data_cond.wait(lk, [this] {return !data_queue.empty();});
              std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
              data_queue.pop();
              return res;
          }

          /// immediately return false if no data, or pop first
          bool try_pop(T& value)
          {
              std::lock_guard<std::mutex> lk(mut);
              if (data_queue.empty()) {
                  return false;
              }
              value = data_queue.front();
              data_queue.pop();
              return ( true );
          }

          /// immediately return empty shared pointer or pop data
          std::shared_ptr<T> try_pop()
          {
              std::lock_guard<std::mutex> lk(mut);
              if (data_queue.empty()) {
                  return std::shared_ptr<T>();
              }
              std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
              data_queue.pop();
              return res;
          }

          /// return true if empty
          bool empty() const
          {
             std::lock_guard<std::mutex> lk(mut);
             return data_queue.empty();
          }

          /// return size
          size_t size() const
          {
             std::lock_guard<std::mutex> lk(mut);
             return data_queue.size();
          }
      };

   } // end of threading
} //namespace cppsl

#endif /* A8C6771A_3F8D_46A5_8962_3D92D963C0C5 */
