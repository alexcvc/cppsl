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
 * \file        threadBase.hh
 * \brief       contains wrapper for std::thread.
 * \author      Alexander Sacharov <as@asoft-labs.de>
 * \date        2021-10-24
 *****************************************************************************/

#ifndef B43F8E70_ECFE_4125_83CB_6E44E220E3F8
#define B43F8E70_ECFE_4125_83CB_6E44E220E3F8

// In an application create a derived class and overrides the virtual function
//  ThreadWrapper::Body. When the instance is already constructed, the thread
//  is not yet started. It can be started any time later, on the call to the
//  separate function, ThreadWrapper::Start(bool). The parameter offers the
//  option to specify detached (background) mode.
//  A separate start is a very important flexibility feature missing in raw
//  std::thread.

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <thread>
#include <memory>
#include <condition_variable>
#include <exception>

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------
#include "cppsl/threading/delegate.h"

//----------------------------------------------------------------------------
// Public defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public Function Prototypes
//----------------------------------------------------------------------------

namespace cppsl::threading {

  //
  /// Thread wrapper abstract class
  //
  class ThreadWrapper {
   public:

    /// start a thread as joinable or detach it after start
    void start(bool is_detached = false) {
      // create thread
      std::thread tmpThread(&ThreadWrapper::thread_function_wrapper, this);
      m_thread.swap(tmpThread);
      // detach?
      if (is_detached) { m_thread.detach(); }
    }

    /// put thread to sleep
    void suspend() {
      std::lock_guard<std::mutex> lock(m_stateMutex);
      m_state = thread_action::sleep;
      m_stateEvent.notify_one();
    }

    /// wakeup
    void wakeUp() {
      std::lock_guard<std::mutex> lock(m_stateMutex);
      m_state = thread_action::wakeup;
      m_stateEvent.notify_one();
    }

    /// abort
    void abort() {
      // works only at the point where thread body calls syncPoint
      set_abort(true);
    }

    /// join the thread and wait its termination
    void join() {
      if (m_thread.joinable()) { m_thread.join(); }
      else {
        // fallback for the case the thread was detached:
        std::unique_lock<std::mutex> ul(m_joinMutex);
        m_joinEvent.wait(ul, [=] { return m_done; });
      }
    }

   private:

    enum class thread_action {
      sleep = 0,
      wakeup = 1,
      shallowAbort = wakeup | 2,
      deepAbort = wakeup | 4
    };

    class thread_abort_exception : std::exception {
    };

    /// wrapper of thread function
    void thread_function_wrapper() {
      try {
        thread_function();
      }
      catch (thread_abort_exception &) {
        on_aborted();
      }
      catch (std::exception &ex) {
        on_exception_caught(ex);
      }
      catch (...) {
        on_unknown_exception_caught();
      } //exception
      std::lock_guard<std::mutex> lock(m_joinMutex);
      m_done = true;
      m_joinEvent.notify_all();
      on_terminated();
    }

   private:
    std::thread m_thread;                            ///< thread
    std::condition_variable m_stateEvent;            ///< condition state
    std::mutex m_stateMutex;                         ///< state mutex
    std::condition_variable m_joinEvent;             ///< joinable condition
    std::mutex m_joinMutex;                          ///< joinable mutex
    thread_action m_state{thread_action::wakeup};    ///< state of thread
    bool m_done = false;                             ///< is done after

   protected:

    /// thread and thread_wrapper object synchronization point
    void synchronization(bool autoReset = false) {
      std::unique_lock<std::mutex> ul(m_stateMutex);
      m_stateEvent.wait(ul, [=] {
        auto result = ((int) m_state & (int) thread_action::wakeup) > 0;

        if (m_state == thread_action::deepAbort) {
          throw thread_abort_exception();
        } else if (m_state == thread_action::shallowAbort) {
          throw shallow_thread_abort_exception();
        }
        if (autoReset) {
          m_state = thread_action::sleep;
        }

        return result;
      });
    }

    /// exposed to conveyor thread, instead of exposing action state:
    class shallow_thread_abort_exception : thread_abort_exception {
    };

    ///
    void set_abort(bool set, bool shallow = true) {
      std::lock_guard<std::mutex> lock(m_stateMutex);
      if (set) {
        if (shallow) {
          m_state = thread_action::shallowAbort;
        } else {
          m_state = thread_action::deepAbort;
        }
      } else {
        //clear abort
        m_state = thread_action::wakeup;
      }
      m_stateEvent.notify_one();
    } //SetAbort

    static void initialize_synchronization_delegate(cppsl::threading::Delegate<void(bool)> &del,
                                                    cppsl::threading::ThreadWrapper *instance) {
      del = cppsl::threading::Delegate<void(bool)>::create
         <cppsl::threading::ThreadWrapper, &cppsl::threading::ThreadWrapper::synchronization>(instance);
    }

    /// abstract thread main function
    virtual void thread_function() = 0;

    /// on event function
    virtual void on_aborted() {}

    /// on event function
    virtual void on_terminated() {}

    /// on event function
    virtual void on_exception_caught(std::exception &) {}

    /// on event function
    virtual void on_unknown_exception_caught() {}

  };

} // namespace cppsl::threading

#endif /* B43F8E70_ECFE_4125_83CB_6E44E220E3F8 */
