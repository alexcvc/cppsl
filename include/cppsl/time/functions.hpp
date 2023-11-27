/* SPDX-License-Identifier: MIT */
//
// Copyright (c) 2020 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/************************************************************************/ /**
 * @file
 * @brief  chrono functions
 *
 *   How to use:
 *   ---
 *
 *   using std::chrono::system_clock;
 *   auto now = system_clock::now();
 *
 *   std::clog << "==== " << toString(now) << std::endl;
 *
 *   using DD = std::chrono::duration<std::size_t, std::ratio<2, 3>>;
 *   using TP = std::chrono::time_point<std::chrono::system_clock, DD>;
 *
 *   for (int i = 0; i <= 3; ++i)
 *       std::clog << "==== " << toString(TP(DD(i))) << std::endl;
 *
 *   for (std::string s: {
 *                "2017-May-01 00:10:15",
 *                "2017-May-01 00:10:15.25",
 *                "2017-Mar-01",
 *                "1969-Dec-31 19:00:00.666666666666667",
 *                "2018",      // underspecified - but succeeds as 2017-12-31
 *                "1752-May-5",// out of range
 *                "not a date",
 *        }) {
 *       try {
 *           std::clog << "---- "
 *                     << toString(fromString<system_clock::time_point>(s))
 *                     << std::endl;
 *       } catch (const std::exception &e) {
 *           std::cerr << e.what() << std::endl;
 *       }
 *   }
 *
 *  ==== 2023-Jun-25 19:27:52.117357015609741
 *  ==== 1970-Jan-01 01:00:00.000000000000000
 *  ==== 1970-Jan-01 01:00:00.666666666666667
 *  ==== 1970-Jan-01 01:00:01.333333333333333
 *  ==== 1970-Jan-01 01:00:02.000000000000000
 *  ---- 2017-May-01 01:10:15.000000000000000
 *  ---- 2017-May-01 01:10:15.249999761581421
 *  ---- 2017-Mar-01 00:00:00.000000000000000
 *  ---- 1969-Dec-31 19:00:00.666666665998491
 *  ---- 2017-Dec-31 00:00:00.000000000000000
 *  ---- get_time
 *  ---- get_time
 *
****************************************************************************/

#ifndef INCLUDE_CPPSL_TIME_CHRONO_FUNC_HPP
#define INCLUDE_CPPSL_TIME_CHRONO_FUNC_HPP

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include <cerrno>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

//----------------------------------------------------------------------------
// Public Prototypes
//----------------------------------------------------------------------------

namespace cppsl::time {

/**
 * to string
 * @tparam Double
 * @tparam Precision
 * @tparam TimePoint
 * @param timePoint
 * @return string
 */
  template<typename Double = double, std::size_t Precision = std::numeric_limits<Double>::digits10, typename TimePoint>
    requires std::is_floating_point_v<Double> && (Precision <= std::numeric_limits<Double>::digits10)
  inline std::string toString(const TimePoint &timePoint) {
    auto seconds = Double(timePoint.time_since_epoch().count()) * TimePoint::period::num / TimePoint::period::den;
    auto const zeconds = std::modf(seconds, &seconds);
    std::time_t tt{static_cast<std::time_t>(seconds)};
    std::ostringstream oss;
    auto const tm = std::localtime(&tt);
    if (!tm)
      throw std::runtime_error(std::strerror(errno));
    oss << std::put_time(tm, "%Y-%b-%d %H:%M:") << std::setw(Precision + 3) << std::setfill('0') << std::fixed
        << std::setprecision(Precision) << tm->tm_sec + zeconds;
    if (!oss)
      throw std::runtime_error("timepoint-to-string");
    return oss.str();
  }

  /**
 * from string to time point
 * @tparam TimePoint
 * @param str
 * @return
 */
  template<typename TimePoint>
  TimePoint fromString(const std::string &str) {
    std::istringstream iss{str};
    std::tm tm{};
    if (!(iss >> std::get_time(&tm, "%Y-%b-%d %H:%M:%S")))
      throw std::invalid_argument("get_time");
    TimePoint timePoint{std::chrono::seconds(std::mktime(&tm))};
    if (iss.eof())
      return timePoint;
    double zz;
    if (iss.peek() != '.' || !(iss >> zz))
      throw std::invalid_argument("decimal");
    using hr_clock = std::chrono::high_resolution_clock;
    std::size_t zeconds = zz * hr_clock::period::den / hr_clock::period::num;
    return timePoint += hr_clock::duration(zeconds);
  }

}// namespace cppsl::chrono::func

#endif /* INCLUDE_CPPSL_TIME_CHRONO_FUNC_HPP */
