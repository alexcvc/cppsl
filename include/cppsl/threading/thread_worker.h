//
// Copyright (c) 2021 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************//**
 * \file   ThreadBase.hpp
 * \brief  contains base thread worker class.
 * Class base thread is parent for all worker threads
 * \author Alexander Sacharov <as@asoft-labs.de>
 * \date 2021-10-17
 *****************************************************************************/

#ifndef __INCLUDE_CPPSL_THREADING_THREAD_WORKER_H__
#define __INCLUDE_CPPSL_THREADING_THREAD_WORKER_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <mutex>
#include <thread>
#include <string>
#include <condition_variable>
#include <cppsl/log/some_withlog.h>

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

   //
   /// Class implements Class base thread is parent for all worker threads
   /// @author Alexander Sacharov
   //
   class ThreadWorker : public cppsl::log::SomeWithLog {
    public:
      /// states
      enum class thread_state {
        IDLE,          /* state idle */
        RUNNING,       /* processing */
        STOPPING,      /* stopping before exit */
        STOPPED,       /* stopped */
        QUITTING       /* allow thread to quit did by himself */
      };

      /// detached states
      enum class detach_state {
        UNKNOWN,       /* unknown */
        DETACHED,      /* detached */
        JOINABLE,      /* joinable */
      };

      /// driving states
      enum class driving_state {
        EVENT_DRIVING, /* thread with waiting for wakeup event with timeout for the thread */
        SELF_DRIVING,  /* thread with self-driving routines */
      };

      /// The following is the default constructor if no arguments are supplied.
      /// 1000 ms is default
      ThreadWorker(std::shared_ptr<cppsl::log::SinkAppender> logPtr,
                   unsigned long number,
                   const std::string &name,
                   const std::chrono::microseconds &timeout = std::chrono::seconds(1));

      /// virtual destructor
      virtual ~ThreadWorker() {}

      //----------------------------------------------------------------------------
      /// Create a detached thread when you know you won't want to wait for it with join().
      /// The only performance benefit is that when a detached thread terminates,
      /// its resources can be released immediately instead of having to wait for
      /// the thread to be joined before the resources can be released.
      ///
      /// It is 'legal' not to join a joinable thread; but it is not usually advisable
      /// because (as previously noted) the resources won't be released until the thread
      /// is joined, so they'll remain tied up indefinitely (until the program exits)
      /// if you don't join it.
      ///
      /// When should I create a thread as detached, right from the outset?
      /// Whenever the application doesn't care when that thread completes and doesn't
      /// care about its return value of a thread, either (a thread may communicate
      /// a value back to other thread/application via pthread_exit).
      ///
      /// Performance-wise, there's no difference between joinable threads vs detached
      /// threads. The only difference is that with detached threads, its resources
      /// (such as thread stack and any associated heap memory, and so on - exactly
      /// what constitutes those "resources" are implementation-specific).
      ///
      //----------------------------------------------------------------------------

      /// @brief     start joinable thread
      /// @param[in] bool=false - as condition driving, true - self driving
      /// @param[in] int - realtime priority (1 = low, 99 = max), if 0 - default
      /// @return    true if the thread started successfully, false - in error case
      bool startJoinable(driving_state howToDrive, int realtimePriority = 0);

      /// @brief     start detached thread
      /// @param[in] bool=false - as condition driving, true - self driving
      /// @param[in] int - realtime priority (1 = low, 99 = max), if 0 - default
      /// @return    true if the thread started successfully, false - in error case
      bool startDetached(driving_state howToDrive, int realtimePriority = 0);

      //----------------------------------------------------------------------------
      // Process function
      //----------------------------------------------------------------------------
      /// wake-up thread and force to do something
      inline void wakeUp(void) {
        std::unique_lock<std::mutex> lock(m_mutexCond);
        m_condVar.notify_one();
      }

      /// Will not return until the internal thread has exited, but doesn't close handles
      inline void join(void) {
        if (!is_detached()) {
          join();
        }
      }

      //----------------------------------------------------------------------------
      /// Send a cancellation request to a thread with  pthread_testcancel.
      /// Important: pthread_testcancel interrupt a thread function!
      ///
      /// A thread's cancellation type, determined by pthread_setcanceltype(3),
      /// may be either asynchronous or deferred (the default for new threads).
      /// - Asynchronous cancellability means that the thread can be canceled at any time
      /// (usually immediately, but the system does not guarantee this).
      /// - Deferred cancellability means that cancellation will be delayed until the thread
      /// next calls a function that is a cancellation point.
      /// A list of functions that are or may be cancellation points:
      ///         https://linux.die.net/man/7/pthreads
      //----------------------------------------------------------------------------
      void cancel(void) {
        int status = m_thread.(m_threadid);
        if (status != 0)
          m_logPtr->error("Cancel thread failed with status: {:d}", status);
      }

      //----------------------------------------------------------------------------
      // Auxiliary
      //----------------------------------------------------------------------------

      /// Returns true if the thread is running
      inline bool is_running(void) {
        return (m_state == thread_state::RUNNING);
      }

      /// Returns true if the thread is running
      inline bool is_done(void) {
        return (m_state == thread_state::STOPPED);
      }

      /// get interval of thread
      const std::chrono::microseconds &get_timeout(void) const {
        return m_timeout;
      }

      /// set new thread interval
      bool set_timeout(const std::chrono::microseconds &newTimeout);

      /// get thread id
      inline unsigned long get_threadNumber(void) {
        return m_number;
      }

      /// get thread id
      inline const std::string &get_name(void) {
        return m_name;
      }

      /// terminate thread
      void terminate(void);

      /// Will not return until the internal thread has exited, but doesn't close handles
      detach_state bpt_getDetachState();

      /// is thread detached?
      /// \return
      inline bool is_detached() {
        return m_thread.joinable();
      }

      /// get thread referred handle
      std::thread::native_handle_type get_nativeId() {
        return m_thread.native_handle();
      }

      /// set and get state
      thread_state get_state(void) const {
        return m_state;
      }

    protected:

      /// Implement this method in your subclass with the code you want your thread to run.
      virtual void thread_function() = 0;

      /// Implement this method in your subclass to setup something before ThreadExecute will run
      virtual bool on_test_setup() { return true; };

      /// Implement this method in your subclass to clean all before the thread will exit
      virtual void on_cleanup_exit() {};

      /// Will not return until timer expired or event happened
      std::cv_status wait_event(const std::chrono::microseconds &timeout = std::chrono::microseconds(0)) {
        if (m_state != thread_state::STOPPED) {
          /// wait
          std::unique_lock<std::mutex> lock(m_mutexCond);
          return m_condVar.wait_for(lock, timeout == std::chrono::microseconds(0) ? m_timeout : timeout);
        }
        return std::cv_status::no_timeout;
      }

      /// set new state
      bool set_state(ThreadWorker::thread_state newState);

      /// Self termination
      void exit(void) {
        pthread_exit(NULL);
      }

    private:
      /// inside thread function
      static void *internal_selfDriving_func(void *pvThis);

      /// inside function for task with wait data from que etc
      /// without IO-operation from sockets and serial
      static void *internal_eventDriving_Func(void *pvThis);

      /// Exit function
      static void exit_thread_func(void *pvThis);

      /// Prepare a pthread_attr_t for realtime usage
      /// @param[in] prio realtime priority (1 = low, 99 = max)
      /// @return a prepared pthread_attr_t to be used in a pthread_create call
      void set_rt_attr(int prio, pthread_attr_t *attr);

    private:
      /// Private variables
      unsigned long m_number;                ///< thread user land ID
      std::thread m_thread;                  ///< thread
      std::string m_name;                    ///< thread user land name
      thread_state m_state;                  ///< thread state
      std::chrono::microseconds m_timeout;   ///< thread timeout
      std::mutex m_mutexState;               ///< thread mutex
      std::mutex m_mutexCond;                ///< thread mutex condition
      std::condition_variable m_condVar;     ///< thread wake-up event
   };
}

#endif /* __INCLUDE_CPPSL_THREADING_THREAD_WORKER_H__ */
