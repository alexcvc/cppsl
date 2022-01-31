//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************//**
 * \file    tnterval_timer.h
 * \brief   contains .
 * \author  Alexander Sacharov
 * \date    2022-01-30
 * \ingroup
 *****************************************************************************/

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public Function Prototypes
//----------------------------------------------------------------------------



#ifndef D32E958A_4A3F_44DB_9095_E73C222B8679
#define D32E958A_4A3F_44DB_9095_E73C222B8679

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <chrono>

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public Function Prototypes
//----------------------------------------------------------------------------

using namespace std::chrono;

namespace tmq4::time {

inline std::chrono::high_resolution_clock::time_point TimeNow() {
   return std::chrono::high_resolution_clock::now();
}

inline std::chrono::microseconds IntervalMks(const std::chrono::steady_clock::time_point& t1,
                                            const std::chrono::steady_clock::time_point& t0) {
   return std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
}

inline std::chrono::milliseconds IntervalMs(const std::chrono::steady_clock::time_point& t1,
                                            const std::chrono::steady_clock::time_point& t0) {
   return std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
}

inline std::chrono::seconds IntervalSec(const std::chrono::steady_clock::time_point& t1,
                                        const std::chrono::steady_clock::time_point& t0) {
   return std::chrono::duration_cast<std::chrono::seconds>(t1 - t0);
}

static
#if defined(__GNUC__)
   __attribute__((used))
#endif
   void
   to_timeval(const system_clock::time_point& tp, timeval& tv) {
   auto s = std::chrono::time_point_cast<seconds>(tp);
   if (s > tp) s = s - seconds{1};
   auto us = std::chrono::duration_cast<microseconds>(tp - s);

   tv.tv_sec = (long) s.time_since_epoch().count();
   tv.tv_usec = (long) us.count();
}

static
#if defined(__GNUC__)
   __attribute__((used))
#endif
   void
   to_time_point(const timeval& tv, system_clock::time_point& dest) {
   dest = std::chrono::system_clock::time_point{seconds{tv.tv_sec} + microseconds{tv.tv_usec}};
}

/**
 * \class stopwatch
 * \brief This is a easy timer_reply.
 *
 * Timer allows to set interval and to check timeot of timer_reply
 *
 */
class IntervalTimer {

public:
   IntervalTimer()
       : m_timeoutValue(std::chrono::milliseconds(0)), m_startingTimePoint(std::chrono::steady_clock::now()),
         m_isStopwatchRunning(false) {}

   IntervalTimer(std::chrono::milliseconds ms)
       : m_timeoutValue(ms), m_startingTimePoint(std::chrono::steady_clock::now()), m_isStopwatchRunning(false) {}

   /// timer_reply has been started (pushed)
   bool is_running() const {
      return m_isStopwatchRunning;
   }

   /// stop
   void stop() {
      m_isStopwatchRunning = false;
      m_startingTimePoint = std::chrono::steady_clock::time_point();
   }

   /// start timer_reply
   std::chrono::steady_clock::time_point start() {
      m_isStopwatchRunning = true;
      m_startingTimePoint = std::chrono::steady_clock::now();
      return m_startingTimePoint;
   }

   /// start timer_reply
   std::chrono::steady_clock::time_point start_for(std::chrono::milliseconds& ms) {
      m_timeoutValue = ms;
      m_startingTimePoint = std::chrono::steady_clock::now();
      m_isStopwatchRunning = true;
      return m_startingTimePoint;
   }

   /// elapsed timeout since start
   std::chrono::microseconds has_elapsedUs() {
      return IntervalMks(std::chrono::steady_clock::now(), m_startingTimePoint);
   }

   /// elapsed timeout since start
   std::chrono::milliseconds has_elapsedMs() {
      return IntervalMs(std::chrono::steady_clock::now(), m_startingTimePoint);
   }

   /// elapsed timeout since start
   std::chrono::seconds has_elapsedSec() {
      return IntervalSec(std::chrono::steady_clock::now(), m_startingTimePoint);
   }

   /// set timeout
   void set_tmoMs(const std::chrono::milliseconds& ms) {
      m_timeoutValue = ms;
   }

   void set_tmoMs(unsigned long ms) {
      m_timeoutValue = std::chrono::milliseconds(ms);
   }

   void set_tmoSec(const std::chrono::seconds& sec) {
      m_timeoutValue = std::chrono::duration_cast<std::chrono::milliseconds>(sec);
   }

   void set_tmoSec(unsigned long sec) {
      m_timeoutValue = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::seconds(sec));
   }

   /// get timeout
   unsigned long get_tmoMs() const {
      return m_timeoutValue.count();
   }

   /// get timeout
   unsigned long get_tmoSec() const {
      return m_timeoutValue.count() / 1000;
   }

   /// is timeout?
   bool is_elapsed() {
      if (!m_isStopwatchRunning) {
         // timer_reply is not running
         return true;
      }
      else if (m_timeoutValue.count() == 0) {
         // is running with 0 timeout
         return true;
      }
      else {
         return (has_elapsedMs() > m_timeoutValue) ? true : false;
      }
   }

private:
   /// start timepoint
   std::chrono::milliseconds m_timeoutValue;

   /// timeout
   std::chrono::steady_clock::time_point m_startingTimePoint;

   /// is running
   bool m_isStopwatchRunning{false};
};

}// namespace tmq4::time


#endif /* D32E958A_4A3F_44DB_9095_E73C222B8679 */
