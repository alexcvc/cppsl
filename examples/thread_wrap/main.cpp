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

#include <com4cpp/logging/logWriter.hh>
#include "demoThread.h"
#include "demoConveyorThread.h"

int main() {
   mylogPtr.reset(new com4cpp::log::writer("demo_thread_wrapper"));

   mylogPtr->add_console_sink(true, true, spdlog::level::info);
   mylogPtr->open_logger(spdlog::level::debug);

   demoWrapperThread::run_test(1000);

   mylogPtr->info("Thread wrapper demo complete.");
   mylogPtr->info("Now, Conveyor Thread Wrapper demo: enter any character :");

   demoConveyorThread::run_test(1);

   mylogPtr->info("Conveyor Thread Wrapper demo complete");

   return 0;
}
