//
// Copyright (c) 2021 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************//**
 * \file        basePThread.cpp
 * \brief       contains base thread worker class in Linux with POSIX pthread.
 * \author      Alexander Sacharov <as@asoft-labs.de>
 * \date        2021-10-17
 *****************************************************************************/

#include <unistd.h>
#include <string>
#include <cstdlib>
#include <sstream>

#include "cppsl/threading/threadWorker.hpp"

using namespace std;
using namespace cppsl;
using namespace cppsl::threading;

/// The following is the default constructor if no arguments are supplied.
/// 1000 ms is default
ThreadWorker::ThreadWorker(std::shared_ptr<cppsl::log::writer> logPtr,
                           unsigned long number,
                           const std::string &name,
                           const std::chrono::microseconds &timeout)
   : cppsl::anyWithLog(logPtr),
     m_number(number),
     m_state(thread_state::IDLE),
     m_threadid(0),
     m_timeout(timeout) {

   m_name.assign(name);

   m_logPtr->trace("Created thread {}", m_name.c_str());
}

/// set new thread interval
bool ThreadWorker::set_timeout(const std::chrono::microseconds &newTimeout) {
   if (m_state == thread_state::STOPPED)
      return (false);
   else {
      std::unique_lock<std::mutex> lock(m_mutexState);
      m_timeout = newTimeout;
   }
   /// wake-up
   m_condVar.notify_all();
   return (true);
}

/// terminate thread
void ThreadWorker::terminate(void) {
   /// set state
   set_state(thread_state::STOPPING);

   /// wake-up
   m_condVar.notify_all();
}

/// set new state
bool ThreadWorker::set_state(ThreadWorker::thread_state newState) {
   if (m_state == thread_state::STOPPED)
      return (false);
   else {
      std::unique_lock<std::mutex> lock(m_mutexState);
      m_state = newState;
   }
   /// wake-up
   m_condVar.notify_all();

   return true;
}

/// internal thread function
/// \param pvThis
/// \return
void *ThreadWorker::internal_eventDriving_Func(void *pvThis) {
   ThreadWorker *thisThread = (ThreadWorker *) pvThis;

   if (!thisThread->on_test_setup()) {
      thisThread->m_logPtr->warn("Test setup failed for intern event-driven thread:{}. Maybe a timeout has to be increased?",
                                 thisThread->m_name.c_str());
      thisThread->set_state(thread_state::STOPPED);
   }
   else {
      /// set state
      thisThread->set_state(thread_state::RUNNING);

      while (thisThread->get_state() == thread_state::RUNNING) {
         // wait
         thisThread->wait_event(thisThread->get_timeout());

         // thread specific loop function
         thisThread->thread_function();
      }

      (static_cast<ThreadWorker *>(pvThis))->exit_thread_func(pvThis);
   }

   return nullptr;
}

/// inside thread function
/// \param pvThis
/// \return
void *ThreadWorker::internal_selfDriving_func(void *pvThis) {
   ThreadWorker *thisThread = (ThreadWorker *) pvThis;

   if (!thisThread->on_test_setup()) {
      thisThread->m_logPtr->warn("Test setup failed for self-driven thread:{}. Maybe a timeout has to be increased?",
                                 thisThread->m_name.c_str());
      thisThread->set_state(thread_state::STOPPED);
   }
   else {

      /// set state
      thisThread->set_state(thread_state::RUNNING);

      // thread specific loop function
      thisThread->thread_function();
   }
   (static_cast<ThreadWorker *>(pvThis))->exit_thread_func(pvThis);
   return nullptr;
}

/// Exit
void ThreadWorker::exit_thread_func(void *pvThis) {
   ThreadWorker *thisThread = (ThreadWorker *) pvThis;

   /// set state
   thisThread->set_state(thread_state::STOPPING);

   /// before exit we have to clean all allocated resources
   thisThread->on_cleanup_exit();

   thisThread->set_state(thread_state::STOPPED);

   pthread_exit(NULL);
}

/// Returns true if the thread was successfully started, false if there was an error starting the thread
/// realtime priority (1 = low, 99 = max)
bool ThreadWorker::start_detached(driving_state howToDrive, int realtimePriority) {
   pthread_attr_t attr;

   pthread_attr_init(&attr);

   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

   if ((realtimePriority > 0) && (realtimePriority < 100)) {
      /*
      * NOTE: This is a realtime application!
      * -------------------------------------
      *
      * We have some timing constrains here so solve:
      * - it seems there is no other process in the system that uses realtime
      *   priority. So we can use the smalles possible relatime priority here: 1
      * - to achive a true realtime behaviour also all used memory must be
      *   locked when run ( refer mlock() and mlockall() )
      * - no glibc calls shoud be made that can be blocked!
      * - Priority inheritance mutexes must be used to syncronize reatime and
      *   non realtime processes/threads (refer pthread_mutexattr_setprotocol()
      *   and PTHREAD_PRIO_INHERIT ).
      *
      */
      set_rt_attr(realtimePriority, &attr);
   }

   // --------------------------------
   // start as self driven
   // --------------------------------
   if (howToDrive == driving_state::SELF_DRIVING) {
      if (pthread_create(&m_threadid, &attr, ThreadWorker::internal_selfDriving_func, this) != 0) {
         pthread_attr_destroy(&attr);
         return (false);
      }
      if (pthread_detach(m_threadid) != 0) {
         pthread_attr_destroy(&attr);
         return (false);
      }
      pthread_attr_destroy(&attr);
      return (true);
   }
   else {
      // --------------------------------
      // start as condition driven
      // --------------------------------
      if (pthread_create(&m_threadid, &attr, ThreadWorker::internal_eventDriving_Func, this)
          != 0) {
         pthread_attr_destroy(&attr);
         return (false);
      }
      if (pthread_detach(m_threadid) != 0) {
         pthread_attr_destroy(&attr);
         return (false);
      }
      pthread_attr_destroy(&attr);
      return (true);
   }
}

/// Returns true if the thread was successfully started, false if there was an error starting the thread
/// realtime priority (1 = low, 99 = max)
bool ThreadWorker::start_joinable(driving_state howToDrive, int realtimePriority) {
   pthread_attr_t attr;

   pthread_attr_init(&attr);

   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   if ((realtimePriority > 0) && (realtimePriority < 100)) {
      set_rt_attr(realtimePriority, &attr);
   }

   // --------------------------------
   // start as self driven
   // --------------------------------
   if (howToDrive == driving_state::SELF_DRIVING) {
      if (pthread_create(&m_threadid, &attr, ThreadWorker::internal_selfDriving_func, this) != 0) {
         pthread_attr_destroy(&attr);
         return (false);
      }
      pthread_attr_destroy(&attr);
      return (true);
   }
   else {
      // --------------------------------
      // start as condition driven
      // --------------------------------
      if (pthread_create(&m_threadid, &attr, ThreadWorker::internal_eventDriving_Func, this)
          != 0) {
         pthread_attr_destroy(&attr);
         return (false);
      }
      pthread_attr_destroy(&attr);
      return (true);
   }
}

/// Prepare a pthread_attr_t for realtime usage
/// @param[in] realtimePriority realtime priority (1 = low, 99 = max)
/// @return a preapred pthread_attr_t to be used in a pthread_create call
void ThreadWorker::set_rt_attr(int realtimePriority, pthread_attr_t *attr) {
   struct sched_param param;

   param.__sched_priority = realtimePriority;

   /* defaults */
   pthread_attr_init(attr);
   /* prepare for realtime */
   pthread_attr_setinheritsched(attr, PTHREAD_EXPLICIT_SCHED);
   /* set up realtime scheduler: SCHED_FIFO or SCHED_RR */

   /*
   * schedule politic is  harmful under heavy load of CPU.
   * Linux Kernel activates throttling all RT scheduled threads
   * up to CPU will be again in normal operation:
   * Kernel prints:
   * [sched_delayed] sched: RT throttling activated
   */

   pthread_attr_setschedpolicy(attr, SCHED_FIFO);
   /* set up realtime priority */
   pthread_attr_setschedparam(attr, &param);
}

/// \brief get state of detached
/// \return legal values are: DETACHED or JOINABLE
//
ThreadWorker::detach_state ThreadWorker::bpt_getDetachState() {
   pthread_attr_t bpt_attr;
   int detachstate;
   int rc;

   rc = pthread_attr_getdetachstate(&bpt_attr, &detachstate);
   if (rc != 0) {
      return detach_state::UNKNOWN;
   }
   else {
      /* legal values for detachstate are:
       * PTHREAD_CREATE_DETACHED or PTHREAD_CREATE_JOINABLE
       */
      switch (detachstate) {
         case PTHREAD_CREATE_DETACHED:
            return detach_state::DETACHED;
         case PTHREAD_CREATE_JOINABLE:
            return detach_state::JOINABLE;
         default:
            return detach_state::UNKNOWN;
      }
   }
}

