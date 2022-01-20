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

#ifndef C7B2A149_2908_4126_8801_E9794AD6B742
#define C7B2A149_2908_4126_8801_E9794AD6B742

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <iostream>
#include <chrono>

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------
#include <com4cpp/logging/logWriter.hh>
#include <com4cpp/threading/threadConveyor.hh>

//----------------------------------------------------------------------------
// Public defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

using com4cpp::threading::ThreadWrapper;
using com4cpp::threading::ThreadConveyor;

extern std::shared_ptr<com4cpp::log::writer> mylogPtr;

//----------------------------------------------------------------------------
// Public Function Prototypes
//----------------------------------------------------------------------------
extern std::string ReadLine(void);

/*************************************************************************//**
 *
 * integer conveyor thread
 */
class demoIntegerConveyorThread : public ThreadConveyor<int> {
protected:
   virtual void on_current_task_aborted(const int &current) override {
      // info
      std::cout << ">> Current integer task aborted: " << current;
      std::cout.flush();
      // on abort
      ThreadConveyor<int>::on_current_task_aborted(current);
   }
};

/*************************************************************************//**
 *
 * demo conveyor thread
 */
class demoConveyorThread : public ThreadConveyor<int> {

   enum class command {
      quit = 'q',       // thread
      abort = 'a',      // current task
      sleep = 's',
      wakeUp = 'w',
   };

   /// output string
   static const char *help() {
      return "a: task abort, q: quit, s: sleep, w: wake up, key: ";
   }

   /// compare command
   static bool is_command(char c, command cmd) { return (int) cmd == (int) c; }

public:
   using natural_number = unsigned long long int;

   static void run_test(natural_number delayMs) {

      // set number
      natural_number mod{1};
      natural_number multiplier{1623425};      // Numerical Recipes
      natural_number increment{1022204453};    // Numerical Recipes
      natural_number max{10};
      demoIntegerConveyorThread conveyorThread;

      conveyorThread.currentTaskAborted = [](int) -> void {
         std::cout << ". Enter any character: ";
         std::cout.flush();
      };

      mod = mod << 32;

      //-------------------------------------
      // sleep func
      //-------------------------------------
      using convType = decltype(conveyorThread);

      auto sleepFunc = [delayMs] {
         std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
      };

      //-------------------------------------
      // Collatz-Problem algorithm
      //-------------------------------------
      auto CollatzFunc = [](natural_number a) -> natural_number {
         if (a % 2 == 0)
            return a / 2;
         else
            return 3 * a + 1;
      };

      //-------------------------------------
      // Collatz calculation stopping times
      //-------------------------------------
      auto CollatzStoppingTime = [CollatzFunc](natural_number n) -> natural_number {
         natural_number stoppingTime = 0;
         // start Collatz-Computation with natural number
         while (n != 1) {
            n = CollatzFunc(n);
            ++stoppingTime;
         } //loop
         return stoppingTime;
      };

      //-------------------------------------
      // Lambda function for Collatz-Problem
      //-------------------------------------
      auto lambdaCollatzFunc = [CollatzStoppingTime, sleepFunc, max](convType::DelegatedSync &sync, int value) {
         for (natural_number count = 1; count < max; ++count) {
            natural_number stoppingTime = CollatzStoppingTime(count + value);
            // synchronize
            sync(false);
            std::cout << count << ": " << help()
               << value << " -> Collatz stopping time=" << stoppingTime << std::endl;
            std::cout.flush();
            sleepFunc();
         }
      };

      //-------------------------------------
      // Local gradient computation (LGC)
      //-------------------------------------
      auto LocalGradCompFunc = [mod, multiplier, increment](natural_number n) -> natural_number {
         return (n * multiplier + increment) % mod;
      };

      auto lambdaLocalGradCompFunc = [LocalGradCompFunc, sleepFunc, max](convType::DelegatedSync &sync, int value) {
         natural_number n = value;

         std::cout << "natural number: " << n << std::endl;
         std::cout.flush();

         for (natural_number count = 0; count < max; count++) {
            n = LocalGradCompFunc(n);
            sync(false);
            std::cout << help() << value << " => LGC " << (n) << std::endl;
            std::cout.flush();
            // sleep
            sleepFunc();
         } //loop
      }; //lambdaLgc

      //----------------------------------------------------------
      // delegation for conveyor
      //----------------------------------------------------------
      com4cpp::threading::delegate<void(convType::DelegatedSync & , int)> delegateCollatzFunc = lambdaCollatzFunc;
      com4cpp::threading::delegate<void(convType::DelegatedSync & , int)> delegateLocalGradFunc = lambdaLocalGradCompFunc;

      //----------------------------------------------------------
      // Start conveyor thread
      //----------------------------------------------------------
      conveyorThread.start();

      //----------------------------------------------------------
      // run endless test
      //----------------------------------------------------------
      char cmd;

      while (true) {
         cmd = std::getchar();

         if (is_command(cmd, command::abort))
            conveyorThread.abort();
         else if (is_command(cmd, command::quit)) {
            conveyorThread.abort(convType::abort_depth::innerThread);
            break;
         }
         else if (is_command(cmd, command::sleep))
            conveyorThread.suspend();
         else if (is_command(cmd, command::wakeUp))
            conveyorThread.wakeUp();
         else {
            int cmdValue = (int) cmd;
            if (cmdValue % 2 == 0)
               conveyorThread.push_in_queue(delegateCollatzFunc, cmdValue);
            else
               conveyorThread.push_in_queue(delegateLocalGradFunc, cmdValue);
         }
      }

      // join
      conveyorThread.join();
   }
};

#endif /* C7B2A149_2908_4126_8801_E9794AD6B742 */
