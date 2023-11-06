/************************************************************************/ /**
* @file
* @brief   Sampling Rate Converter with linear interpolation
* @details Sampling rate (sometimes called sampling frequency or Fs) is
* the number of data points acquired per second. A sampling rate of
* 4000 samples/second means that 4000 discrete data points are acquired every
* second. This can be referred to as 4000 Hertz sample frequency.
* The sampling rate is important for determining the maximum amplitude and
* correct waveform of the signal.
* More information about sample rate conversion in
* 1) "Efficient Algorithms for Arbitrary Sample Rate Conversion with
* Application to Wave Field Synthesis" Andreas Franck
* 2) For simulation:
* https://docs.octave.org/v7.1.0/Signal-Processing.html
* 3) Harris, Fredric J. (Jan 1978). "On the use of Windows for
* Harmonic Analysis with the Discrete Fourier Transform"
* 4) https://www.hpmemoryproject.org/an/pdf/an_243.pdf
* 5) https://en.wikipedia.org/wiki/Window_function
*
* @author Alexander Sacharov <a.sacharov@gmx.de>
*
* Project : Digital Signal Processing
****************************************************************************/

#ifndef INCLUDE_CPPSL_MATH_DSP_SRC_LINEAR_HPP
#define INCLUDE_CPPSL_MATH_DSP_SRC_LINEAR_HPP

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
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

/// The sampling rate converter with linear interpolation method.
  /// The output signal will have a 1-sample delay

  template <typename T>
  class SmpRateConvLinear {
   public:
    /**
   * @brief default constructor
   */
    SmpRateConvLinear() {
      Initialize(1, 1, 16);
    }

    /**
     * @brief constructor
     * @param in_sampling_rate - input sampling rate (Fs in) or Number of data samples acquired
     * per second
     * @param out_sampling_rate - outgoing sampling rate (Fs out) or Number of data samples
     * acquired per second
     * @param bits_accuracy - increasing the accuracy by shifting N bits to the left
     */
    SmpRateConvLinear(int in_sampling_rate, int out_sampling_rate, unsigned char bits_accuracy = 16) {
      Initialize(in_sampling_rate, out_sampling_rate, bits_accuracy);
    }

    /** @brief initialize
     * @param in_sampling_rate - input sampling rate (Fs in) or Number of data samples acquired
     * per second
     * @param out_sampling_rate - outgoing sampling rate (Fs out) or Number of data samples
     * acquired per second
     * @param bits_accuracy - increasing the accuracy by shifting N bits to the left
     */
    bool Initialize(int in_sampling_rate, int out_sampling_rate, unsigned char bits_accuracy = 16) {
      m_mask = (1 << bits_accuracy) - 1;
      long long quotient = ((long long)in_sampling_rate << bits_accuracy) / (long long)out_sampling_rate;
      m_dTf = (long long)(quotient & m_mask);
      m_dT = (long long)(quotient >> bits_accuracy);
      m_accuracy = bits_accuracy;
      m_Tf = 0;
      m_T = 0;
      m_inT = 0;
      m_previous = 0;
      return (quotient > 0);
    }

    /**
     * @brief Convert sampling rate
     * @param inc_sample_data incoming buffer
     * @param out_waveform the output buffer, will be set to an appropriate size
     * @return number of samples in the output buffer
     */
    int Convert(std::vector<T>& inc_sample_data, std::vector<T>& out_waveform) {
      int out_count = 0;
      int s = 0;

      while (true) {
        while (m_inT <= m_T) {
          // get previous
          m_previous = inc_sample_data[s++];
          m_inT++;
          if (s >= (int)inc_sample_data.size()) {
            if (m_inT < m_T) {
              m_T = m_T - m_inT;
              m_inT = 0;
            } else {
              m_inT = m_inT - m_T;
              m_T = 0;
            }
            return out_count;
          }
        }

        T temp = (T)((long long)m_previous +
                     (long long)(((long long)m_Tf * (long long)((long long)inc_sample_data[s] - (long long)m_previous)) >>
                                 m_accuracy));

        out_waveform.push_back(temp);
        out_count++;
        long long t = m_Tf + m_dTf;
        m_T = m_T + m_dT + (long long)(t >> m_accuracy);
        m_Tf = t & m_mask;
      }
    }

   private:
    unsigned char m_accuracy{1};  ///< accuracy shift bits
    long long m_mask{0xffff};     ///< mask for the shifted approximated number
    long long m_dTf{0};           ///<
    long long m_dT{0};            ///<
    long long m_Tf{0};            ///<
    long long m_T{0};             ///<
    long long m_inT{0};           ///<
    T m_previous;                 ///< previous value
  };

}   // namespace cppsl::math

#endif /* INCLUDE_CPPSL_MATH_DSP_SRC_LINEAR_HPP */
