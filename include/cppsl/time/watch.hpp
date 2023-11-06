/* SPDX-License-Identifier: MIT */
//
// Copyright (c) 2023 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

#ifndef INCLUDE_CPPSL_TIME_WATCH_HPP
#define INCLUDE_CPPSL_TIME_WATCH_HPP

#include <chrono>
#include <vector>

namespace cppsl::time {

using namespace std::chrono;

/**
 * This class provides C++ template class called "StopWatch" that implements a stopwatch functionality.
 * It uses the  <chrono>  library to measure time accurately. The class has the following member functions:
 *
 * -  Reset() : Resets the stopwatch to its initial state.
 * -  Go() : Starts the stopwatch and returns the current time point.
 * -  Stop() : Stops the stopwatch and records the stop time point.
 * -  ElapsedTime() : Returns the elapsed time between the start and stop points.
 *
 * The class also has a protected enum called  State , which represents the current state of the stopwatch (idle, running, or stopped). Additionally, there is a static member function called  CurrentTime()  that returns the current time point using the specified duration.
 */
template<typename duration = std::chrono::milliseconds>
class StopWatch {
 public:
   using clock = std::chrono::steady_clock;
   using time_point = std::chrono::time_point<clock, duration>;

   StopWatch() = default;

   /**
    * Resets the stopwatch to its initial state.
    */
   virtual void Reset() noexcept {
     m_clock_state = State::idle;
   }

   /**
    * Starts the stopwatch and returns the current time point.
    * @return returns the current time point
    */
   time_point Go() noexcept {
     if (m_clock_state != State::stopped)
       m_start_point = CurrentTime();

     m_clock_state = State::running;
     return m_start_point;
   }

   /**
    * Stops the stopwatch and records the stop time point.
    */
   void Stop() noexcept {
     if (m_clock_state == State::running) {
       m_stop_point = CurrentTime();
       m_clock_state = State::stopped;
     }
   }

   /**
    * Returns the elapsed time between the start and stop points.
    * @return elapsed time between the start and stop points.
    */
   duration ElapsedTime() const noexcept {
     switch (m_clock_state) {
       case State::idle:
         return std::chrono::duration_cast<duration>(clock::duration::zero());

       case State::running:
         return std::chrono::duration_cast<duration>(CurrentTime() - m_start_point);

       case State::stopped:
         return std::chrono::duration_cast<duration>(m_stop_point - m_start_point);

       default:
         return std::chrono::duration_cast<duration>(clock::duration::zero());
         break;
     }
   }

   /**
    * These two functions are accessors for the private member variables  m_start_point  and  m_stop_point .
    * @return time point
    */
   // This function returns the start point of time.
   const time_point& GetStartPoint() const { return m_start_point; }

   // This function returns the stop point of time.
   const time_point& GetStopPoint() const { return m_stop_point; }

  protected:
   enum class State : unsigned char { idle, running, stopped };

   static time_point CurrentTime() noexcept {
     return std::chrono::time_point_cast<duration>(clock::now());
   }

   time_point m_start_point;
   time_point m_stop_point;
   State m_clock_state{State::idle};
};

/**
 * The C++ template class "TimerWatch" extends the functionality of a StopWatch. It introduces an additional
 * feature called a "lap" function, which allows for tracking multiple intervals of time.
 * The template parameter, "duration," is used to define the unit of time measurement.
 *
 * The TimerWatch class inherits from the StopWatch class and includes the necessary type aliases for the clock
 * and time_point types. It also inherits the State enum from the StopWatch class.
 *
 * The TimerWatch class provides the following member functions:
 * -  IsElapsed() : Checks if the elapsed time has exceeded the specified timeout duration.
 * -  Timeout() : Returns a constant reference to the timeout duration.
 * -  SetTimeout() : Sets the timeout duration to the specified value.
 *
 * Additionally, the TimerWatch class has a private member variable called  m_timeout , which represents the
 * deadline duration for the TimerWatch.
 * Please let me know if you have any further questions or need additional assistance.
 */
template<typename duration = std::chrono::milliseconds>
class TimerWatch : public StopWatch<duration> {

   using clock = typename StopWatch<duration>::clock;
   using time_point = typename StopWatch<duration>::clock;
   using StopWatch<duration>::State;

 public:
   TimerWatch() = default;

   /// is timeout?
   bool IsElapsed() {
     return (StopWatch<duration>::ElapsedTime() > m_timeout);
   }

   const duration& Timeout() const {
     return m_timeout;
   }

   void SetTimeout(const duration& timeout) {
     m_timeout = timeout;
   }

 private:

   duration m_timeout;  // dead line duration
};

/**
 * This is a C++ template class called "RoundWatch" that extends the "StopWatch" class with a default template parameter
 * of "std::chrono::milliseconds".
 * The "RoundWatch" class has a few type aliases for convenience:
 * "clock" and "time_point" are aliases for the clock and time_point types from the "StopWatch" class.
 * The "State" alias is for the "State" enum from the "StopWatch" class.
 *
 * The class also has a nested struct called "LapDurations" which contains two durations: "total_time" and "split_time".
 *
 * The class provides a method called "StoreLap" which stores the lap durations if the clock state is running.
 * The lap durations are calculated by subtracting the previous lap's total time from the current total time.
 * The lap durations are then stored in a vector.
 *
 * The class also provides a method called "Reset" which clears the vector of lap durations.
 * Lastly, the class has a method called "Laps" which returns a const reference to the vector of lap durations.
 */
template<typename duration = std::chrono::milliseconds>
class RoundWatch : public StopWatch<duration> {

   using clock = typename StopWatch<duration>::clock;
   using time_point = typename StopWatch<duration>::clock;
   using StopWatch<duration>::State;

 public:
   RoundWatch() = default;
   struct LapDurations {
     duration total_time;
     duration split_time;
   };

   void StoreLap() {
     if (StopWatch<duration>::m_clock_state != StopWatch<duration>::State::running)
       return;

     LapDurations current;
     if (m_laps.size() == 0)
       current.total_time = current.split_time = StopWatch<duration>::ElapsedTime();
     else {
       current.total_time = StopWatch<duration>::ElapsedTime();
       current.split_time = (current.total_time - m_laps[m_laps.size() - 1].total_time);
     }
     m_laps.push_back(std::move(static_cast<LapDurations &&>(current)));
   }

   void Reset() noexcept {
     m_laps.clear();
   }

   const std::vector<LapDurations> &Laps() const {
     return m_laps;
   }

 private:

   std::vector<LapDurations> m_laps;  // Lap times
};

}   // namespace cppsl::time

#endif /* INCLUDE_CPPSL_TIME_WATCH_HPP */
