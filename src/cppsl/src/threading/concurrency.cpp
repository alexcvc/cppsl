
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include "cppsl/threading/concurrency.hpp"
#include "cppsl/string/charString.hh"

using namespace cppsl;
using namespace cppsl::threading;
using namespace cppsl::string;

extern "C" int pthread_mutexattr_setkind_np(pthread_mutexattr_t *, int);

baseMutex::baseMutex() {
   pthread_mutexattr_t initializer;

   m_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
   assert(m_mutex);

   pthread_mutexattr_init(&initializer);
   pthread_mutexattr_setkind_np(&initializer, PTHREAD_MUTEX_RECURSIVE_NP);
   pthread_mutex_init(m_mutex, &initializer);
   pthread_mutexattr_destroy(&initializer);
}

baseMutex::~baseMutex() {
   free(m_mutex);
}

bool baseMutex::operator==(const baseMutex &rhs) const {
   return m_mutex == rhs.m_mutex;
}

bool baseMutex::operator!=(const baseMutex &rhs) const {
   return !(rhs == *this);
}

baseMutex::baseMutex(pthread_mutex_t *mMutex) : m_mutex(mMutex) {}

baseEvent::baseEvent() : baseMutex() {

   pthread_cond_init(&m_cond, NULL);
   m_oldval = 0;
   m_newval = 0;
}

/// \brief destructor
baseEvent::~baseEvent() {

}

/// reset in initial state
void baseEvent::reset() {

   pthread_mutex_lock(m_mutex);
   m_oldval = 0;
   m_newval = 0;
   pthread_cond_broadcast(&m_cond);
   pthread_mutex_unlock(m_mutex);
}

/// send wakeup
void baseEvent::signal() {

   pthread_mutex_lock(m_mutex);
   m_newval++;
   pthread_cond_broadcast(&m_cond);
   pthread_mutex_unlock(m_mutex);

}

/// wait baseEvent
bool baseEvent::wait(unsigned long ms) {

   struct timeval now;
   struct timespec timeout;
   int retcode;
   unsigned long sec, usec;

   pthread_mutex_lock(m_mutex);
   sec = ms / 1000;
   ms = ms - (sec * 1000);

   if (gettimeofday(&now, NULL)) {
      perror("baseEvent::wait gettimeofday failed");
   }

   timeout.tv_sec = now.tv_sec + sec;

   /* wait event! */
   usec = (ms * 1000L) + now.tv_usec;
   if (usec >= 1000000L) {
      usec -= 1000000L;
      timeout.tv_sec++;
   }
   timeout.tv_nsec = usec * 1000;

   if (m_oldval == m_newval)
      retcode = pthread_cond_timedwait(&m_cond, m_mutex, &timeout) != ETIMEDOUT;
   if (m_oldval != m_newval) {
      m_oldval = m_newval;
      retcode = 1;
   }
   pthread_mutex_unlock(m_mutex);

   return (retcode);

}

/// wait baseEvent
bool baseEvent::waitUsec(unsigned long mks) {

   struct timespec now;
   struct timespec timeout;
   int retcode;
   unsigned long sec, usec;

   pthread_mutex_lock(m_mutex);

   // get time
   if (clock_gettime(CLOCK_REALTIME, &now)) {
      perror("baseEvent::waitUsec clock_gettime failed");
   }

   sec = mks / 1000000ul;
   usec = mks - sec * 1000000ul;
   //
   timeout.tv_sec = now.tv_sec + sec;
   timeout.tv_nsec = now.tv_nsec + usec * 1000;

   if (timeout.tv_nsec > 1000000000l) {
      timeout.tv_nsec -= 1000000000ul;
      timeout.tv_sec++;
   }

   if (m_oldval == m_newval)
      retcode = pthread_cond_timedwait(&m_cond, m_mutex, &timeout) != ETIMEDOUT;
   if (m_oldval != m_newval) {
      m_oldval = m_newval;
      retcode = 1;
   }
   pthread_mutex_unlock(m_mutex);

   return (retcode);
}

