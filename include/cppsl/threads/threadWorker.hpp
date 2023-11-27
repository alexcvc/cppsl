//
// Copyright (c) 2021 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************//**
 * @file
 * \brief  contains base thread worker class.
 * Class base thread is parent for all worker threads
 * \author Alexander Sacharov <a.sacharov@gmx.de>
 * \date 2021-10-17
 *****************************************************************************/

#ifndef INCLUDE_CPPSL_THREADING_THREAD_WORKER_HPP
#define INCLUDE_CPPSL_THREADING_THREAD_WORKER_HPP

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <condition_variable>
#include <cppsl/log/baseLogAppender.hpp>
#include <mutex>
#include <string>
#include <thread>

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

   //
   /// Class implements Class base thread is parent for all worker threads
   /// @author Alexander Sacharov
   //
   class ThreadWorker : public cppsl::log::BaseLogAppender {
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
      ThreadWorker(cppsl::log::log_appenderPtr logPtr, std::string_view name, const std::chrono::microseconds& timeout)
          : cppsl::log::BaseLogAppender(logPtr), m_name(name), m_state(thread_state::IDLE), m_timeout(timeout) {}

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
        if (!isDetached()) {
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
//        int status = m_thread.;
//        if (status != 0)
//          m_logPtr->error("Cancel thread failed with status: {:d}", status);
      }

      //----------------------------------------------------------------------------
      // Auxiliary
      //----------------------------------------------------------------------------

      /// Returns true if the thread is running
      inline bool isRunning(void) {
        return (m_state == thread_state::RUNNING);
      }

      /// Returns true if the thread is running
      inline bool isDone(void) {
        return (m_state == thread_state::STOPPED);
      }

      /// get interval of thread
      const std::chrono::microseconds & getTimeout(void) const {
        return m_timeout;
      }

      /// set new thread interval
      bool setTimeout(const std::chrono::microseconds &newTimeout);

      /// get thread name
      inline const std::string & getName(void) {
        return m_name;
      }

      /// terminate thread
      void terminate(void);

      /// is thread detached?
      /// \return
      inline bool isDetached() {
        return m_thread.joinable();
      }

      /// get thread referred handle
      std::thread::native_handle_type getNativeHandle() {
        return m_thread.native_handle();
      }

      /// set and get state
      thread_state getState(void) const {
        return m_state;
      }

    protected:

      /// Implement this method in your subclass with the code you want your thread to run.
      virtual void threadFunction() = 0;

      /// Implement this method in your subclass to setup something before ThreadExecute will run
      virtual bool onTestSetup() { return true; };

      /// Implement this method in your subclass to clean all before the thread will exit
      virtual void onCleanupExit() {};

      /// Will not return until timer expired or event happened
      std::cv_status waitEvent(const std::chrono::microseconds &timeout = std::chrono::microseconds(0)) {
        if (m_state != thread_state::STOPPED) {
          /// wait
          std::unique_lock<std::mutex> lock(m_mutexCond);
          return m_condVar.wait_for(lock, timeout == std::chrono::microseconds(0) ? m_timeout : timeout);
        }
        return std::cv_status::no_timeout;
      }

      /// set new state
      bool setState(ThreadWorker::thread_state newState);

      /// Self termination
      void exit(void) {
        pthread_exit(NULL);
      }

    private:
      /// inside thread function
      static void * internalSelfDrivingFunc(void *pvThis);

      /// inside function for task with wait data from que etc
      /// without IO-operation from sockets and serial
      static void * internalEventDrivingFunc(void *pvThis);

      /// Exit function
      static void exitThreadFunc(void *pvThis);

    private:
      /// Private variables
      std::thread m_thread;                  ///< thread
      std::string m_name;                    ///< thread user land name
      thread_state m_state;                  ///< thread state
      std::chrono::microseconds m_timeout;   ///< thread timeout
      std::mutex m_mutexState;               ///< thread mutex
      std::mutex m_mutexCond;                ///< thread mutex condition
      std::condition_variable m_condVar;     ///< thread wake-up event
   };
}

#endif /* INCLUDE_CPPSL_THREADING_THREAD_WORKER_HPP */
