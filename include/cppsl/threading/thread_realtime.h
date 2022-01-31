//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

#ifndef CA336C67_EEED_47AC_8225_ED06D56F8B0B
#define CA336C67_EEED_47AC_8225_ED06D56F8B0B

#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <string>
#include <thread>
#include <utility>

namespace cppsl::threading {

/**
   * @brief : Class base thread is parent for all worker threads
   **/
class RtThreadWrapper {
public:
   /// How to drive thread in main
   enum class driving {
      inside_condition,    ///< good default for threads without IO and self waiting
      self_driving,        ///< good default for all threads with IO and for self wait processing
   } ;

   /// How to drive thread in main
   enum class detached_state {
      joinable,            ///< thread starts as join able
      detached,            ///< thread starts as detached
   } ;

protected:
   /// states
   typedef enum class thread_state {
      IDLE,     /* state idle */
      RUNNING,  /* processing */
      STOPPING, /* stopping before exit */
      STOPPED,  /* stopped */
      QUITTING  /* allow thread to quit did by himself */
   } thread_state;

public:
   /// The following is the default constructor if no arguments are supplied.
   /// 1000 ms is default
   explicit RtThreadWrapper(const std::string& name, const std::chrono::microseconds& timeout = std::chrono::seconds(1))
       : m_name(name),
         m_state(RtThreadWrapper::thread_state::IDLE),
         m_threadid(0),
         m_timeout(timeout) {}

   /// virtual destructor
   virtual ~RtThreadWrapper() = default;

   /// @brief     start thread
   /// @param[in] bool=false - as condition driving, true - self driving
   /// @return    true if the thread started successfully, false - in error case
   bool thread_start(driving how_driving, detached_state how_detached);

   /// terminate thread
   void thread_terminate(void) {

      /// set state
      thread_set_state(RtThreadWrapper::thread_state::STOPPING);

      /// wake-up
      m_condVar.notify_all();
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
   }

   /// Returns true if the thread is running
   inline bool thread_isRunning(void) {
      return m_state == RtThreadWrapper::thread_state::RUNNING ? true : false;
   }

   /// Returns true if the thread is running
   inline bool thread_isStopped(void) {
      return m_state == RtThreadWrapper::thread_state::STOPPED ? true : false;
   }

   /// get interval of thread
   const std::chrono::microseconds& thread_timeout(void) const {
      return m_timeout;
   }

   /// set new thread interval
   bool thread_set_timeout(const std::chrono::microseconds& newTimeout) {

      if (thread_isStopped())
         return (false);
      else {
         std::unique_lock<std::mutex> lock(m_mutexState);
         m_timeout = newTimeout;
      }
      /// wake-up
      m_condVar.notify_all();
      return (true);
   }

   /// wake-up thread and force to do something
   inline void thread_wakeUp(void) {
      std::unique_lock<std::mutex> lock(m_mutexCond);
      m_condVar.notify_one();
   }

   /// get thread id
   inline const std::string& thread_name(void) {
      return m_name;
   }

   /// Will not return until the internal thread has exited, but doesn't close handles
   inline void thread_wait_for_exit(void) {
      pthread_exit(NULL);
      // pthread_join(m_threadid, NULL);
   }

protected:
   /// Implement this method in your subclass with the code you want your thread to run.
   virtual void thread_func() = 0;

   /** Implement this method in your subclass to setup something before thread_Execute will run */
   virtual bool thread_setUp() = 0;

   /** Implement this method in your subclass to clean all before the thread will exit */
   virtual void thread_cleanUp() = 0;

   /// get thread handle
   pthread_t thread_handle() {
      return m_threadid;
   }

   /// set new state
   void thread_set_state(const RtThreadWrapper::thread_state& newState) {
      if (m_state == RtThreadWrapper::thread_state::STOPPED)
         return;
      else {
         std::unique_lock<std::mutex> lock(m_mutexState);
         m_state = newState;
      }
      /// wake-up
      m_condVar.notify_all();
   }

   /// Will not return until timer_reply expired or event happened
   std::cv_status thread_wait_for(const std::chrono::microseconds& timeout = std::chrono::microseconds(0)) {
      if (m_state != RtThreadWrapper::thread_state::STOPPED) {
         /// wait
         std::unique_lock<std::mutex> lock(m_mutexCond);
         return m_condVar.wait_for(lock, timeout == std::chrono::microseconds(0) ? m_timeout : timeout);
      }
      return std::cv_status::no_timeout;
   }

private:

   /// thread main function intern
   static void* threadFunc(void* pvThis);

private:
   /// Private variables
   std::string m_name;                          ///< thread user land name
   RtThreadWrapper::thread_state m_state;           ///< thread state
   pthread_t m_threadid;                        ///< thread handle
   std::chrono::microseconds m_timeout{0};      ///< thread timeout
   std::mutex m_mutexState;                     ///< thread mutex
   std::mutex m_mutexCond;                      ///< thread mutex condition
   std::condition_variable m_condVar;           ///< thread wake-up event
};
}// namespace cppsl::threading

#endif /* CA336C67_EEED_47AC_8225_ED06D56F8B0B */
