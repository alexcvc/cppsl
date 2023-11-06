/* SPDX-License-Identifier: MIT */
//
// Copyright (c) 2023 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/************************************************************************/ /**
 * @file
 * @brief class called "StopTimer" that allows you to set a timeout and check
 * the elapsed time of a timer. It uses the chrono library to handle time-
 * related operations.
 *
 * The class provides various functionalities such as setting a timeout
 * duration, starting and stopping the timer, checking if the timer is running,
 * and getting the elapsed and remaining time.
 * It also includes useful typedefs for different time units (seconds, milliseconds,
 * and microseconds).
 *
 * Overall, this class provides a convenient way to measure and manage time
 * intervals in C++ programs.
 *
 ****************************************************************************/

#ifndef INCLUDE_CPPSL_TIME_TIMER_HPP
#define INCLUDE_CPPSL_TIME_TIMER_HPP

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include <chrono>
#include <optional>

//----------------------------------------------------------------------------
// Public Prototypes
//----------------------------------------------------------------------------

namespace cppsl::time {

template <class TDuration = std::chrono::milliseconds>
class StopTimer {
 public:
  /** types */
  using Clock = std::chrono::steady_clock;
  using TimePoint = std::chrono::time_point<StopTimer::Clock, TDuration>;

  /**
   * @brief constructors
   */
  StopTimer() = default;
  StopTimer(TDuration timeout) : m_timeout_duration(timeout) {}

  /**
   * @brief get timeout
   * @tparam TUnit - to-duration unit type
   * @return duration in to-duration unit type
   */
  template <typename TUnit = TDuration>
  [[nodiscard]] TUnit Timeout() const noexcept {
    return std::chrono::duration_cast<TUnit>(m_timeout_duration);
  }

  /**
   * @brief set timeout in duration unit
   * @tparam TUnit - from-duration unit type
   * @param timeout
   */
  template <typename TUnit = TDuration>
  void SetTimeout(const TUnit& timeout) noexcept {
    m_timeout_duration = std::chrono::duration_cast<TDuration>(timeout);
  }

  /**
   * @brief checks whether the program is currently running.
   * @return It returns a boolean value indicating the running status.
   */
  [[nodiscard]] bool IsRunning() const noexcept { return m_is_running; }

  /**
   * @brief used to reset the state of an object. It sets the "m_is_running"
   * variable to false and "m_start_point" variable to its default value.
   * The "noexcept" keyword indicates that this function does not throw any exceptions.
   */
  void Reset() noexcept {
    m_is_running = false;
    m_start_point = {};
  }

  /**
   * @brief This function is used to stop the execution of a process or task.
   * It sets the value of the variable "m_is_running" to false, indicating
   * that the process should stop.
   * The function is declared with the "noexcept" specifier, which means
   * that it does not throw any exceptions.
   */
  void Stop() noexcept { m_is_running = false; }

  /**
   * @brief starts the stopwatch by setting the "m_is_running" flag to true and
   * capturing the current time as the start point.
   * @return the start point time.
   */
  TimePoint Start() noexcept {
    m_is_running = true;
    m_start_point = StopTimer::CurrentTime();
    return m_start_point;
  }

  /**
   * Starts the timer with the specified timeout duration.
   *
   * @tparam TUnit The type of the timeout duration. Defaults to TDuration.
   * @param new_timeout The new timeout duration to set.
   * @return The time point when the timer was started.
   * @remarks The timeout duration is set using the SetTimeout method. The timer
   *          is marked as running and the current time point is recorded as the
   *          start point. The start point is then returned.
   * @note The noexcept specifier indicates that this function does not throw any exceptions.
   */
  template <typename TUnit = TDuration>
  TimePoint Start(TUnit new_timeout) noexcept {
    SetTimeout<TUnit>(new_timeout);
    m_is_running = true;
    m_start_point = StopTimer::CurrentTime();
    return m_start_point;
  }

  /**
   * @brief checks if the timer has elapsed or not. It returns an optional boolean value,
   * indicating whether the timer has elapsed or not.  If the timer is not running, it
   * returns std::nullopt, indicating that the elapsed state cannot be determined.
   *
   * If the timer is running with a timeout duration of 0 (no timeout), it returns true,
   * indicating that the timer has elapsed.  Otherwise, it compares the elapsed time with
   * the timeout duration and returns the result as a boolean value.
   *
   * @return If the elapsed time is greater than the timeout duration, it returns true, indicating
   * that the timer has elapsed. Otherwise, it returns false, indicating that the timer
   * has not yet elapsed.
   */
  [[nodiscard]] std::optional<bool> IsElapsed() noexcept {
    if (!m_is_running) {
      // timer is not running
      return std::nullopt;
    } else if (m_timeout_duration.count() == 0) {
      // is running with 0 timeout
      return true;
    } else {
      return (ElapsedTime() > m_timeout_duration);
    }
  }

  /**
   * @brief calculates and returns the elapsed time of the stopwatch in the specified
   * unit of time. The template parameter  TUnit  represents the type of time unit to
   * be used for measuring the elapsed time. By default, it is set to  TDuration , which
   * is another template parameter representing the duration type of the stopwatch.
   *
   * The function first checks if the stopwatch is currently running by calling the
   * IsRunning()  function. If it is running, it calculates the duration between the
   * current time and the start point of the stopwatch using the  StopTimer::CurrentTime()
   * function. The elapsed time is then obtained by casting this duration to the specified
   * time unit  TUnit  using  std::chrono::duration_cast.
   * If the stopwatch is not running, it returns a default-constructed  TUnit  object,
   * indicating that no time has elapsed.
   *
   * @tparam TUnit - duration unit
   * @return Returns the elapsed time in the specified time unit
   */
  template <typename TUnit = TDuration>
  [[nodiscard]] TUnit ElapsedTime() noexcept {
    if (IsRunning()) {
    // Calculate the duration between the current time and the start point
    auto elapsedTime = StopTimer::CurrentTime() - m_start_point;
    return std::chrono::duration_cast<TUnit>(elapsedTime);
    } else {
    // Return a default-constructed TUnit if the stopwatch is not running
      return TUnit{};
    }
  }

  /**
   * @brief returns the remaining time in the specified unit (defaulting to TDuration).
   * The function first checks if the timer is currently running by calling the "IsRunning"
   * function. If it is running, it calculates the remaining time by subtracting the elapsed
   * time (obtained from the "ElapsedTime" function) from the total timeout duration
   * (stored in the "m_timeout_duration" variable). The result is then casted to the specified
   * unit using "std::chrono::duration_cast".
   * If the timer is not running, the function simply returns TUnit{}, which is the
   * default-constructed value of the specified unit.
   * Return value should not be ignored.
   * @tparam TUnit - duration unit
   * @return left time
   */
  template <typename TUnit = TDuration>
  [[nodiscard]] TUnit LeftTime() noexcept {
    if (IsRunning()) {
      return std::chrono::duration_cast<TUnit>(m_timeout_duration - ElapsedTime());
    } else {
      return TUnit{};
    }
  }

  /**
   * @brief returns the current time as a time point. The function
   * uses the "now" function of the "Clock" class from the standard
   * library's "chrono" namespace to get the current time. It then
   * casts the obtained time point to the specified duration type
   * "TDuration" and returns it.
   * Return value of this function should not be ignored.
   * @return timepoint
   */
  [[nodiscard]] static inline TimePoint CurrentTime() noexcept {
    return std::chrono::time_point_cast<TDuration>(Clock::now());
  }

 private:
  StopTimer::TimePoint m_start_point{};   ///< start time point
  TDuration m_timeout_duration{};         ///< timeout
  bool m_is_running{false};               ///< is running
};

/**
 * @brief useful types
 * @details useful types for replace template type like standard library std::string
 * TimerSec timerSec;
 * TimerMs timerMs;
 * TimerUs timerUs;
 */
using StopTimerSec = cppsl::time::StopTimer<std::chrono::seconds>;
using StopTimerMs = cppsl::time::StopTimer<std::chrono::milliseconds>;
using StopTimerUs = cppsl::time::StopTimer<std::chrono::microseconds>;

}   // namespace cppsl::time

#endif /* INCLUDE_CPPSL_TIME_TIMER_HPP */
