/*
	Thread Wrapper v.2.0
	Delegates

	Copyright (C) 2017 by Sergey A Kryukov
   The article, along with any associated source code and files, is licensed under
   The Code Project Open License (CPOL)

   http://www.SAKryukov.org
	http://www.codeproject.com/Members/SAKryukov

	Original publications:
	https://www.codeproject.com/Articles/1177478/Thread-Wrapper-CPP
	https://www.codeproject.com/Articles/1177869/Conveyor-Thread-Wrapper-CPP
*/

/*************************************************************************//**
 * \file        threadConveyor.hh
 * \brief       thread conveyor.
 * \date        2021-10-24
 *****************************************************************************/

#ifndef AAC4E950_759E_4CB0_BA18_F7DE1C46C859
#define AAC4E950_759E_4CB0_BA18_F7DE1C46C859

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <queue>

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------
#include "cppsl/threading/threadWrapper.hpp"
#include "cppsl/threading/delegate.hpp"

//----------------------------------------------------------------------------
// Public defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

namespace cppsl {
   namespace threading {

      template<typename Parameter_T>
      class ThreadConveyor : public ThreadWrapper {
      public:

         ThreadConveyor() {
            ThreadWrapper::initialize_synchronization_delegate(m_syncDelegate, this);
         }

         ~ThreadConveyor() {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            // loop
            while (!m_queue.empty()) {
               auto element = m_queue.front();
               m_queue.pop();
               delete element;
            }
         };

         using DelegatedSync = cppsl::threading::delegate<void(bool)>;
         using DelegatedTask = cppsl::threading::delegate<void(DelegatedSync &, Parameter_T)>;

         /// enqueue task
         void push_in_queue(DelegatedTask &task, Parameter_T argument) {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            m_queue.push(new queue_item(task, argument));
            m_queueCondition.notify_one();
         }

         /// abortion deep
         enum class abort_depth {
            currentTask,
            innerThread,
         };

         /// abort
         /// \param depth - how deep to  abort
         void abort(abort_depth depth = abort_depth::currentTask) {
            set_abort(true, depth == abort_depth::currentTask);
         }

         cppsl::threading::delegate<void(const Parameter_T &)> currentTaskAborted;

      protected:

         /// on current task was aborted
         /// \param current task
         virtual void on_current_task_aborted(const Parameter_T &current) {
            if (!currentTaskAborted.is_null())
               currentTaskAborted(current);
         }

      protected:

         /// thread function for thread conveyor
         void thread_function() override final {
            while (true) {
               queue_item task;
               // get
               getTask(task);

               try {
                  (*task.delegate)(m_syncDelegate, task.parameter);
               }
               catch (shallow_thread_abort_exception &) {
                  set_abort(false);
                  on_current_task_aborted(task.parameter);
               }
            }
         }

      private:

         /// delegeted
         DelegatedSync m_syncDelegate;

         struct queue_item {
            queue_item() = default;

            queue_item(DelegatedTask &del, Parameter_T param) {
               delegate = &del;
               parameter = param;
            } //QueueItem
            DelegatedTask *delegate;
            Parameter_T parameter;
         }; //struct QueueItem

         std::queue<queue_item *> m_queue;            // queue task item
         std::condition_variable m_queueCondition;
         std::mutex m_queueMutex;

         /// get task from front
         void getTask(queue_item &itemCopy) {
            std::unique_lock<std::mutex> ul(m_queueMutex);
            m_queueCondition.wait(ul, [=] {
               return m_queue.size() > 0;
            });
            auto front = m_queue.front();
            itemCopy.delegate = front->delegate;
            itemCopy.parameter = front->parameter;
            m_queue.pop();
            delete front;
         }

      };

   } // end of threading
} /* namespace cppsl */

#endif /* AAC4E950_759E_4CB0_BA18_F7DE1C46C859 */
