/************************************************************************/ /**
* @file
* @brief   math constants
* @author Alexander Sacharov <a.sacharov@gmx.de>
* Project : DSP
****************************************************************************/

#ifndef INCLUDE_CPPSL_MATH_DSP_CONST_HPP
#define INCLUDE_CPPSL_MATH_DSP_CONST_HPP

#include <numbers>

namespace cppsl::math {
constexpr double DSP_PI_2       =  1.570796326794896619231321691639751442L;   ///< pi / 2
constexpr double DSP_PI         =  std::numbers::pi;                          ///< pi
constexpr double DSP_2PI        =  6.283185307179586476925286766559005770L;   ///< 2xPI
constexpr double DSP_SQRT2      =  1.414213562373095048801688724209698079L;   ///< sqrt(2)
constexpr double DSP_SQRT3      =  1.732050807568877293527446341505872367L;   ///< sqrt(3)
constexpr double DSP_RADIAN     = 57.295779513082320876798154814105170332L;   ///< one radian in degrees
constexpr double DSP_30_DEGREE  =  0.523598775598298873077107230546583814L;   ///< radians in 30 degrees
constexpr double DSP_120_DEGREE =  2.094395102393195492308428922186335256L;   ///< radians in 120 degrees
constexpr double DSP_150_DEGREE =  2.617993877991494365385536152732919070L;   ///< radians in 150 degrees
}   // namespace cppsl::math

#endif /* INCLUDE_CPPSL_MATH_DSP_CONST_HPP */
