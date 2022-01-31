
#include <cstdlib>
#include <string>
#include <unistd.h>

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------

#include "cppsl/threading/thread_realtime.h"

//----------------------------------------------------------------------------
// Private Defines and Macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Private typedefs, enums, unions, variables
//----------------------------------------------------------------------------

using namespace std;
using namespace cppsl::threading;

/**
 * @brief Returns true if the thread was successfully started, false if there was an error starting the thread
 * @return true if successfully, otherwise - false
 */
bool tmq4_thread::thread_start(driving how_driving, detached_state how_detached) {

   pthread_attr_t attr;

   pthread_attr_init(&attr);

   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

   // --------------------------------
   // start as self driven
   // --------------------------------
   if (pthread_create(&m_threadid, &attr, tmq4_thread::threadFunc, this) != 0) {
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

/**
 * @brief self driving with while(true)
 * @param pvThis
 */
void* tmq4_thread::threadFunc(void* pvThis) {

   tmq4_thread* thisThread = (tmq4_thread*) pvThis;

   if (thisThread->thread_setUp()) {

      tmq4log::info("cppsl::threading: thread \"{0}\" starting..", thisThread->m_name.c_str());

      /// set state
      thisThread->thread_set_state(tmq4_thread::thread_state::RUNNING);

      // thread specific loop function
      thisThread->thread_func();

      /// set state
      thisThread->thread_set_state(tmq4_thread::thread_state::STOPPING);

      /// before exit we have to clean all allocated resources
      thisThread->thread_cleanUp();

      thisThread->thread_set_state(tmq4_thread::thread_state::STOPPED);

      tmq4log::info("cppsl::threading: thread \"{0}\" exit..", thisThread->m_name.c_str());

      // pthread_cancel(thisThread->m_threadid);
      pthread_exit(NULL);
   }
   else {
      tmq4log::critical("cppsl::threading: thread \"{0}\" cannot start because of failed startup", thisThread->m_name.c_str());
   }

   return NULL;
}
