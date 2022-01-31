//
// Copyright (c) 2021 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <thread>
#include <memory>
#include <unordered_map>
#include <string>
#include <chrono>

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------
#include "com4cpp/threading/pthreadBase.hh"
#include "com4cpp/logging/logWriter.hh"

using namespace com4cpp::threading;
using namespace com4cpp::log;
using namespace std::chrono;

class Executor;

class Task;

//----------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------
#define BLOBSIZE   1000

//----------------------------------------------------------------------------
// Typedefs, enums, unions, variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Declarations
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Function Definitions
//----------------------------------------------------------------------------

int numberTasks = 0;
int numberExecutor = 0;

/*************************************************************************//**
 * Class implements object blob data .
 */
struct DataBlob {
   int *m_data;

   DataBlob() {
      m_data = new int[BLOBSIZE];
   }

   virtual ~DataBlob() {
      delete[] m_data;
   }
};

/*************************************************************************//**
 * Class Task implements some task.
 */
class Task : public com4cpp::threading::ThreadWorker {
   // Private Variables
   std::shared_ptr<writer> m_logPtr;
   std::weak_ptr<Executor> m_exePtr;

public:
   /// Constructor & Destructor
   Task(std::shared_ptr<writer> logPtr, uint TaskNumb, std::weak_ptr<Executor> exePtr)
      : ThreadWorker(logPtr, TaskNumb, "Task", std::chrono::microseconds(500)),
        m_logPtr(logPtr), m_exePtr(exePtr) {
      m_logPtr->info("++Task number: {:d} created", get_threadNumber());
      ++numberTasks;
   }

   virtual ~Task() {
      m_logPtr->info("--Task number: {:d} says by by!", get_threadNumber());
      --numberTasks;
   }

protected:
   virtual void thread_function() override {
      while (is_running()) {
         if (m_exePtr.expired()) {
            m_logPtr->warn("Task number: {:d} detected an expired executor", get_threadNumber());
            break;
         }
         else {
            if (std::cv_status::no_timeout == wait_event(get_timeout())) {
               m_logPtr->trace("===> Wakeup Task number {:d}", get_threadNumber());
            }
            else {
               m_logPtr->trace("Task number {:d} - timeout", get_threadNumber());
            }
         }
      }
   }

   virtual bool on_test_setup() override {
      m_logPtr->trace("Task number {:d} - test setup", get_threadNumber());
      return true;
   }

   virtual void on_cleanup_exit() override {
      m_logPtr->trace("Task number {:d} - cleanup before exit", get_threadNumber());
   }
};

/*************************************************************************//**
 * Class Executor implements executor tasks
 */
class Executor : public com4cpp::threading::ThreadWorker {
   // Private Variables
   std::shared_ptr<writer> m_logPtr;
   std::unordered_map<uint, std::shared_ptr<Task>> m_mapTasks;
   std::unique_ptr<DataBlob> m_data;
   DataBlob *m_pData{nullptr};

public:
   /// Constructor & Destructor
   Executor(std::shared_ptr<writer> logPtr) : ThreadWorker(logPtr, 1, "Executor", std::chrono::microseconds(1000)), m_logPtr(logPtr) {
      m_logPtr->info("++Executor");
      ++numberExecutor;
      // create data
      m_data.reset(new DataBlob);
      m_pData = new DataBlob;
   }

   virtual ~Executor() {
      m_logPtr->info("--Executor");
      --numberExecutor;
      delete m_pData;
   }

   std::unordered_map<uint, std::shared_ptr<Task>> &mapTasks() {
      return m_mapTasks;
   }

   void startTask() {
      for (auto &item: m_mapTasks) {
         item.second->start_joinable(ThreadWorker::driving_state::SELF_DRIVING);
      }
   }

   void terminateTasks() {
      for (auto &item: m_mapTasks) {
         item.second->terminate();
         item.second->join();
      }
      m_mapTasks.clear();
   }

   void terminate() {
      terminate();
   }

   void join() {
      join();
   }

protected:
   virtual void thread_function() override {
      while (is_running()) {
         if (std::cv_status::no_timeout == wait_event(get_timeout())) {
            m_logPtr->info(">>> Executor <<< Wakeup Executor");
         }
         else {
            m_logPtr->trace(">>> Executor <<< Executor timeout");
         }
      }
      m_logPtr->info(">>> Executor <<< Executor terminated");
   }

   virtual bool on_test_setup() override {
      m_logPtr->trace(">>> Executor <<< is testing setup");
      return true;
   }

   virtual void on_cleanup_exit() override {
      m_logPtr->info(">>> Executor <<< is cleaning");
   }

private:
};

// ///////////////////////////////////////////////////////////////////////////////////////////////

int main() {
   uint nCount{0};
   std::shared_ptr<com4cpp::log::writer> logPtr(new com4cpp::log::writer("test"));

   logPtr->add_console_sink(true, true, spdlog::level::info);
   logPtr->open_logger(spdlog::level::debug);

   numberExecutor = 0;
   numberTasks = 0;
   {
      std::shared_ptr<Executor> exePtr(new Executor(logPtr));

      exePtr->start_joinable(com4cpp::threading::ThreadWorker::driving_state::SELF_DRIVING);

      // test 1
      logPtr->info("====== TEST 1 ======");
      {
         do {
            std::shared_ptr<Task> taskPtr(std::make_shared<Task>(logPtr, nCount, exePtr));
            exePtr->mapTasks().insert({nCount, taskPtr});
         } while (nCount++ < 10);

         logPtr->info("executor starts all tasks");
         exePtr->startTask();

         logPtr->info(".. executor terminates all tasks");
         exePtr->terminateTasks();

         if (numberTasks != 0) {
            logPtr->error("some {:d} tasks are deallocated", numberTasks);
         }
         else {
            logPtr->info("TEST 1 PASSED!");
         }

      }

      // test 2
      logPtr->info("====== TEST 2 ======");
      {
         do {
            std::shared_ptr<Task> taskPtr(std::make_shared<Task>(logPtr, nCount, exePtr));
            exePtr->mapTasks().insert({nCount, taskPtr});
         } while (nCount++ < 20);

         logPtr->info("executor starts all tasks");
         exePtr->startTask();

         logPtr->info(".. executor terminates all tasks");
         exePtr->terminateTasks();

         if (numberTasks != 0) {
            logPtr->error("some {:d} tasks are deallocated", numberTasks);
         }
         else {
            logPtr->info("TEST 2 PASSED!");
         }

      }

      std::this_thread::sleep_for(seconds(1));

      logPtr->info("main terminates executor and joined to  executor .. ");

      exePtr->terminate();
      exePtr->join();

   }

   // test 2
   logPtr->info("====== TEST 3 ======");

   if (numberExecutor != 0) {
      logPtr->error("executor {:d} tasks is not deleted!", numberExecutor);
   }
   else {
      logPtr->info("TEST 3 PASSED!");
   }

   logPtr->info("main sleeps 2 sec (executor should be deleted) ...");
   std::this_thread::sleep_for(seconds(2));

   logPtr->info("main says : by by!");
   return 0;
}