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
 * \file        ThreadConveyor.hpp
 * \brief       thread conveyor.
 * \date        2021-10-24
 *****************************************************************************/

#ifndef __INCLUDE_CPPSL_THREADING_THREAD_CONVEYOR_H__
#define __INCLUDE_CPPSL_THREADING_THREAD_CONVEYOR_H__

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <queue>

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------
#include "cppsl/threading/thread_synchro.h"
#include "cppsl/threading/delegate.h"

//----------------------------------------------------------------------------
// Public defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

namespace cppsl::threading {

   template<typename Parameter_T>
   class ThreadConveyor : public ThreadWrapper {
    public:
      using DelegatedSync = cppsl::threading::Delegate<void(bool)>;
      using DelegatedTask = cppsl::threading::Delegate<void(DelegatedSync &, Parameter_T)>;

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

      /// enqueue task
      void push_in_queue(DelegatedTask &task, Parameter_T argument) {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_queue.push(new TaskItem(task, argument));
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

    public:
      // public member
      cppsl::threading::Delegate<void(const Parameter_T &)> currentTaskAborted;

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
          TaskItem task;
          // get
          getTask(task);

          try {
            (*task.Delegate)(m_syncDelegate, task.parameter);
          }
          catch (shallow_thread_abort_exception &) {
            set_abort(false);
            on_current_task_aborted(task.parameter);
          }
        }
      }

    private:
      /// task item in queue
      struct TaskItem {
        TaskItem() = default;

        TaskItem(DelegatedTask &del, Parameter_T param) {
          Delegate = &del;
          parameter = param;
        } //QueueItem
        DelegatedTask *Delegate;
        Parameter_T parameter;
      }; //struct QueueItem


      /// delegeted
      DelegatedSync m_syncDelegate;

      /// get task from front
      void getTask(TaskItem &itemCopy) {
        std::unique_lock<std::mutex> ul(m_queueMutex);
        m_queueCondition.wait(ul, [=] {
          return m_queue.size() > 0;
        });
        auto front = m_queue.front();
        itemCopy.Delegate = front->Delegate;
        itemCopy.parameter = front->parameter;
        m_queue.pop();
        delete front;
      }

      std::queue<TaskItem *> m_queue;           ///< queue task item
      std::condition_variable m_queueCondition;   ///< queue condition
      std::mutex m_queueMutex;                    ///< queue mutex


   };

} /* namespace cppsl::threading */

#endif /* __INCLUDE_CPPSL_THREADING_THREAD_CONVEYOR_H__ */
