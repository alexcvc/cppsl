
/*************************************************************************//**
* @file    threadSafeListT.hh
* @brief   thread safe list class.
* @author  Alexander Sacharov <a.sacharov@asoft-labs.de>
* @ingroup COM4CPP Library
*****************************************************************************/

#ifndef B2EC764D_9A73_4A09_B31A_7C2A28F15BF8
#define B2EC764D_9A73_4A09_B31A_7C2A28F15BF8

#include <memory>
#include <mutex>

namespace cppsl {
   namespace threading {

      template<typename T>
      class threadSafeList
      {
          struct node {
              std::mutex            mtx;
              std::shared_ptr<T>    data_ptr;
              std::unique_ptr<node> next;

              node(): next() {}

              node(T const& value) : data_ptr(std::make_shared<T>(value)) {}
          };

          node head;

      public:
          threadSafeList() {}

          ~threadSafeList()
          {
              remove_if( [](T const&) {return true;} );
          }

          /// @breaf    clean.
          ///
          /// @details  removes all from list
          ///
          void clean()
          {
              remove_if( [](T const&) {return true;} );
          }

          /// @breaf    pushes in front.
          ///
          /// @details  implementation of threadsafe_list<T>::push_front
          ///

          void push_front(T const& value)
          {
              std::unique_ptr<node> new_node( new node(value) );

              std::lock_guard<std::mutex> lk(head.mtx);
              new_node->next = std::move(head.next);
              head.next      = std::move(new_node);
          }

          /// @breaf    runs for each.
          ///
          /// @details  implementation of for each in stack
          ///
          template<typename Function>
          void for_each(Function f)
          {
              node *current = &head;

              std::unique_lock<std::mutex> lk(head.mtx);
              while (node *const next = current->next.get()) {
                  std::unique_lock<std::mutex> next_lk(next->mtx);
                  lk.unlock();
                  f(*next->data_ptr);
                  current = next;
                  lk = std::move(next_lk);
              }
          }

          /// @breaf    finds first if condition.
          ///
          /// @details  implementation of find first by condition
          ///
          template<typename Predicate>
          std::shared_ptr<T> find_first_if(Predicate p)
          {
              node *current = &head;

              std::unique_lock<std::mutex> lk(head.mtx);
              while (node *const next = current->next.get()) {
                  std::unique_lock<std::mutex> next_lk(next->mtx);
                  lk.unlock();
                  if ( p(*next->data_ptr) ) {
                      return next->data_ptr;
                  }
                  current = next;
                  lk = std::move(next_lk);
              }
              return std::shared_ptr<T>();
          }

          /// @breaf    removes if condition.
          ///
          /// @details  implementation of remove by condition
          ///
          template<typename Predicate>
          void remove_if(Predicate p)
          {
              node *current = &head;
              std::unique_lock<std::mutex> lk(head.mtx);
              while (node *const next = current->next.get()) {
                  std::unique_lock<std::mutex> next_lk(next->mtx);
                  if (p(*next->data_ptr)) {
                      std::unique_ptr<node> old_next = std::move(current->next);
                      current->next = std::move(next->next);
                      next_lk.unlock();
                  } else {
                      lk.unlock();
                      current = next;
                      lk = std::move(next_lk);
                  }
              }
          }
      private:
          threadSafeList(threadSafeList const& other)
          {
              throw std::logic_error("The method not implemented.");
          }
          threadSafeList& operator=(threadSafeList const& other)
          {
              throw std::logic_error("The method not implemented.");
          }
      };

   } // end of threading
} //namespace cppsl

#endif /* B2EC764D_9A73_4A09_B31A_7C2A28F15BF8 */
