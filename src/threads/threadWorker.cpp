//
// Copyright (c) 2021 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************/ /**
 * @file
 * \brief       contains base thread worker class in Linux with POSIX pthread.
 * \author      Alexander Sacharov <a.sacharov@gmx.de>
 * \date        2021-10-17
 *****************************************************************************/

#include <fmt/format.h>
#include <unistd.h>
#include <sstream>
#include <thread>

#include <cppsl/threads/threadWorker.hpp>

using namespace std;
using namespace cppsl;
using namespace cppsl::concurency;

/// set new thread interval
bool ThreadWorker::setTimeout(const std::chrono::microseconds &newTimeout) {
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
  setState(thread_state::STOPPING);

  /// wake-up
  m_condVar.notify_all();
}

/// set new state
bool ThreadWorker::setState(ThreadWorker::thread_state newState) {
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
void *ThreadWorker::internalEventDrivingFunc(void *pvThis) {
  ThreadWorker *thisThread = (ThreadWorker *) pvThis;

  if (!thisThread->onTestSetup()) {
    thisThread->m_logPtr->warn(
       "Test setup failed for intern event-driven thread:{}. Maybe a timeout has to be increased?",
       thisThread->m_name.c_str());
    thisThread->setState(thread_state::STOPPED);
  } else {
    /// set state
    thisThread->setState(thread_state::RUNNING);

    while (thisThread->getState() == thread_state::RUNNING) {
      // wait
      thisThread->waitEvent(thisThread->getTimeout());

      // thread specific loop function
      thisThread->threadFunction();
    }

    (static_cast<ThreadWorker *>(pvThis))->exitThreadFunc(pvThis);
  }

  return nullptr;
}

/// inside thread function
/// \param pvThis
/// \return
void *ThreadWorker::internalSelfDrivingFunc(void *pvThis) {
  ThreadWorker *thisThread = (ThreadWorker *) pvThis;

  if (!thisThread->onTestSetup()) {
    thisThread->m_logPtr->warn("Test setup failed for self-driven thread:{}. Maybe a timeout has to be increased?",
                               thisThread->m_name.c_str());
    thisThread->setState(thread_state::STOPPED);
  } else {

    /// set state
    thisThread->setState(thread_state::RUNNING);

    // thread specific loop function
    thisThread->threadFunction();
  }
  (static_cast<ThreadWorker *>(pvThis))->exitThreadFunc(pvThis);
  return nullptr;
}

/// Exit
void ThreadWorker::exitThreadFunc(void *pvThis) {
  ThreadWorker *thisThread = (ThreadWorker *) pvThis;

  /// set state
  thisThread->setState(thread_state::STOPPING);

  /// before exit we have to clean all allocated resources
  thisThread->onCleanupExit();

  thisThread->setState(thread_state::STOPPED);

  pthread_exit(NULL);
}
