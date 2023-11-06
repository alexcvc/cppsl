/************************************************************************/ /**
* @file
* @brief   general math functions
* @author A.Sacharov <a.sacharov@gmx.de>
****************************************************************************/

#ifndef INCLUDE_CPPSL_MATH_DSP_FUNCTIONS_HPP
#define INCLUDE_CPPSL_MATH_DSP_FUNCTIONS_HPP

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include <algorithm>
#include <complex>
#include <cstdint>
#include <numeric>
#include <string>
#include <vector>

//-----------------------------------------------------------------------------
// global Defines and Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// global Structures, Typedefs, Enums, Unions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// global Variables Declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// global Function Prototypes
//-----------------------------------------------------------------------------

namespace cppsl::math {

/**
  * @brief compute max in vector
  * @param data - vector values
  * @return maximum absolut value
  * @exception range_error
  */
  template <typename NumericType>
  inline double max(const std::vector<NumericType>& data) {
    if (data.empty()) {
      return 0.;
    } else {
      auto iter = std::max_element(data.begin(), data.end(), [&](const NumericType& a, const NumericType& b) {
        return std::abs(a) < std::abs(b);
      });
      // get real in max. element
      return std::abs(*iter);
    }
  }

  /**
  * @brief compute max in vector
  * @param data - vector values
  * @param firstIndex - first index in search
  * @param lastIndex - last index in search
  * @return maximum absolut value
  * @exception range_error
  */
  template <typename NumericType>
  inline double max(const std::vector<NumericType>& data, size_t firstIndex, size_t lastIndex) {
    if (data.empty()) {
      return 0.;
    } else if ((lastIndex - firstIndex) >= data.size()) {
      throw std::range_error("compute complex mag from " + std::to_string(firstIndex) + " up to " +
                             std::to_string(lastIndex));
    } else {
      auto iter = std::max_element(data.begin() + firstIndex, data.begin() + lastIndex,
                                   [&](const NumericType& a, const NumericType& b) {
                                     return std::abs(a) < std::abs(b);
                                   });
      // get real in max. element
      return std::abs(*iter);
    }
  }

  /**
  * @brief compute max real value in vector complex values
  * @param data - vector complex values
  * @return max real value
  * @exception range_error
  */
  template <typename NumericType>
  double maxReal(const std::vector<std::complex<NumericType>>& data) {
    if (data.empty()) {
      return 0.;
    } else {
      auto iter = std::max_element(data.begin(), data.end(),
                                   [&](const std::complex<NumericType>& a, const std::complex<NumericType>& b) {
                                     return std::abs(a.real()) < std::abs(b.real());
                                   });
      // get real in max. element
      return std::abs(iter->real());
    }
  }

  /**
  * @brief compute max real value in vector complex values
  * @param data - vector complex values
  * @param firstIndex - first index in search
  * @param lastIndex - last index in search
  * @return max real value
  * @exception range_error
  */
  template <typename NumericType>
  inline double maxReal(const std::vector<std::complex<NumericType>>& data, size_t firstIndex, size_t lastIndex) {
    if (data.empty()) {
      return 0.;
    } else if ((lastIndex - firstIndex) >= data.size()) {
      throw std::range_error("compute complex mag from " + std::to_string(firstIndex) + " up to " +
                             std::to_string(lastIndex));
    } else {
      auto iter = std::max_element(data.begin() + firstIndex, data.begin() + firstIndex,
                                   [&](const std::complex<NumericType>& a, const std::complex<NumericType>& b) {
                                     return std::abs(a.real()) < std::abs(b.real());
                                   });
      // get real in max. element
      return std::abs(iter->real());
    }
  }

  /**
  * @brief compute Root Mean Square in vector
  * @param data - vector values
  * @return RMS value
  * @exception range_error
  */
  template <typename NumericType>
  inline double rms(const std::vector<NumericType>& data) {
    if (data.empty()) {
      return 0.;
    } else {
      double square =
          std::accumulate(data.begin(), data.end(), (double)0.0, [&](const double& s, const NumericType& n) {
            return (s + (double)n * n / data.size());
          });
      return std::sqrt(square);
    }
  }

  /**
  * @brief compute Root Mean Square in vector
  * @param data - vector values
  * @param firstIndex - first index
  * @param lastIndex - last index
  * @return RMS value
  * @exception range_error
  */
  template <typename NumericType>
  inline double rms(const std::vector<NumericType>& data, size_t firstIndex, size_t lastIndex) {
    if (data.empty()) {
      return 0.;
    } else if ((lastIndex - firstIndex) >= data.size()) {
      throw std::range_error("compute complex mag from " + std::to_string(firstIndex) + " up to " +
                             std::to_string(lastIndex));
    } else {
      size_t nn = lastIndex - firstIndex;
      double square = std::accumulate(data.begin() + firstIndex, data.begin() + lastIndex, (double)0.0,
                                      [&](const double& s, const NumericType& n) {
                                        return (s + (double)n * n / nn);
                                      });
      return std::sqrt(square);
    }
  }

  /**
  * @brief compute Root Mean Square in vector
  * @param data - vector values
  * @return RMS value
  * @exception range_error
  */
  template <typename NumericType>
  inline double rmsReal(const std::vector<std::complex<NumericType>>& data) {
    if (data.empty()) {
      return 0.;
    } else {
      double square = std::accumulate(data.begin(), data.end(), (double)0.0,
                                      [&](const std::complex<double>& s, const std::complex<NumericType>& n) {
                                        return (s.real() + n.real() * n.real() / data.size());
                                      });
      return std::sqrt(square);
    }
  }

  /**
  * @brief compute Root Mean Square in vector
  * @param data - vector values
  * @param firstIndex - first index
  * @param lastIndex - last index
  * @return RMS value
  * @exception range_error
  */
  template <typename NumericType>
  inline double rmsReal(const std::vector<std::complex<NumericType>>& data, size_t firstIndex, size_t lastIndex) {
    if (data.empty()) {
      return 0.;
    } else if ((lastIndex - firstIndex) >= data.size()) {
      throw std::range_error("compute complex mag from " + std::to_string(firstIndex) + " up to " +
                             std::to_string(lastIndex));
    } else {
      size_t nn = lastIndex - firstIndex;
      double square = std::partial_sum(data.begin() + firstIndex, data.begin() + lastIndex, (double)0.0,
                                       [&](const std::complex<double>& s, const std::complex<NumericType>& n) {
                                         return (s.real() + n.real() * n.real() / nn);
                                       });
      return std::sqrt(square);
    }
  }

}   // namespace cppsl::math

#endif /* INCLUDE_CPPSL_MATH_DSP_FUNCTIONS_HPP */
