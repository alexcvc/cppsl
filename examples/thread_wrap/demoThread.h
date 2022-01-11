//
// Based on ideas Thread Wrapper
//	Copyright (C) 2017 by Sergey A Kryukov
//	http://www.SAKryukov.org
//	http://www.codeproject.com/Members/SAKryukov
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//
// Improved and changed from : Alexander Sacharov <a.sacharov@gmx.de>

/*************************************************************************//**
 * \file        demoWrapperThread.h
 * \brief       contains demo classes.
 * \author      Alexander Sacharov <as@asoft-labs.de>
 * \date        2021-10-24
 *****************************************************************************/

#ifndef C670E879_858B_414E_815B_95432E883043
#define C670E879_858B_414E_815B_95432E883043

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <iostream>
#include <chrono>
#include <stdio.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <com4cpp/logging/logWriter.hh>
#include <com4cpp/threading/interlockProperty.hh>
#include <com4cpp/threading/threadBase.hh>

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

using natural_number = unsigned long long int;
using com4cpp::threading::inter_lock_property;
using com4cpp::threading::ThreadWrapper;

std::shared_ptr<com4cpp::log::writer> mylogPtr;
bool main_exit = false;

//----------------------------------------------------------------------------
// Public Function Prototypes
//----------------------------------------------------------------------------

class testWrapperThread : public ThreadWrapper {
public:
   testWrapperThread() : id(m_mutex, 2), name(m_mutex) {}

   inter_lock_property<int> id;
   inter_lock_property<const char *> name, help;
   inter_lock_property<natural_number> delayMs;

protected:
   virtual void thread_function() override {

      auto sleepFunc = [=] {
         std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
      }; //sleep
      int count = 0;

      name = m_oldName;
      while (!main_exit) {
         this->synchronization();
         mylogPtr->info("count {} {} change property: id:{:d} name:{}", count++, help, id, name);
         sleepFunc();
      }
   }

private:
   const char *m_oldName = "noName";
   std::mutex m_mutex;
};

/**
 *
 * Demo thread
 *
 */
class demoWrapperThread {
   enum class command {
      abort = 'a', // abort thread
      quit = 'q', // also abort thread
      sleep = 's',
      wakeUp = 'w',
   };

   static const char *help() {
      return " a, q: quit, s: sleep, w: wake-up, else:";
   }

   static bool command_is(char c, command cmd) {
      return (int) cmd == (int) c;
   }

public:

   static void run_test(natural_number delayMs) {

      const char *newName = "NewName";
      testWrapperThread thread;

      thread.help = help();
      thread.delayMs = delayMs;

      thread.start();

      while (true) {

         char cmd;
         std::cout << "=========================================================\n";
         std::cout << "Enter command (a,q,s,w) either integer ID or string name\n";

         cmd = getchar();

         if (command_is(cmd, command::abort) || command_is(cmd, command::quit)) {
            thread.abort();
            break;
         }
         else if (command_is(cmd, command::sleep))
            thread.suspend();
         else if (command_is(cmd, command::wakeUp))
            thread.wakeUp();
         else {
            thread.id = thread.id + 1; // no ++ defined
            thread.name = newName;
         }
      }

      thread.join();
   }

};


#endif /* C670E879_858B_414E_815B_95432E883043 */
