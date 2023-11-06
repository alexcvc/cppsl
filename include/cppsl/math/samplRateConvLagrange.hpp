/************************************************************************//**
* @file
* @brief   Sampling Rate Converter with Langrange interpolation.
* The polynomial interpolator of the N-th order is equivalent to the lowpass
* reconstruction filter of the (N+1)th order. The accuracy of the interpolation
* is equal to the amount of the aliased components after the lowpass.
* Considering the worst case you will need the polynom of the ~31th degree.
*
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

#ifndef INCLUDE_CPPSL_MATH_DSP_SRC_LAGRANGE_HPP
#define INCLUDE_CPPSL_MATH_DSP_SRC_LAGRANGE_HPP

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include <cmath>
#include <limits>
#include <vector>
#include <stdexcept>
#include <limits>

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

namespace DSP {
namespace SRC {

  /// A sampling rate converter with Lagrange interpolation.
  /// The output signal is delayed by (N+1)/2 samples.

  template <typename T>
  class SmpRateConvLagrange {
   public:
    SmpRateConvLagrange() {
      Initialize(1, 1, (unsigned char)1);
    }

    /**
   * @brief Class initialization
   * @param in_sampling_rate input sample frequency
   * @param out_sampling_rate output sample frequency
   * @param interpolation_order interpolation order. The value must be odd!
   */
    SmpRateConvLagrange(int in_sampling_rate, int out_sampling_rate, unsigned char interpolation_order = 1) {
      Initialize(in_sampling_rate, out_sampling_rate, interpolation_order);
    }

    /**
   * @brief Parameter initialization
   * @param in_sampling_rate input sample rate
   * @param out_sampling_rate output sample rate
   * @param interpolation interpolation order
   */
    bool Initialize(int in_sampling_rate, int out_sampling_rate, unsigned char interpolation) {
      if ((interpolation & 1) == 0) {
        throw std::invalid_argument("the interpolation value must be odd");
      }
      m_interpolation = interpolation;
      m_Hinterpolation = (unsigned char)((interpolation - 1) >> 1);
      m_pF.resize(m_interpolation + 1);
      m_pLI.resize(m_interpolation + 1);
      m_wrPos = m_interpolation;
      m_dT = (double)in_sampling_rate / (double)out_sampling_rate;
      m_T = 0.0;
      m_inT = 0;
      return (m_dT > 0.0);
    }

    /**
   * @brief Convert sampling rate
   * @param inc_sample_data incoming buffer
   * @param out_waveform the output buffer, must be of an appropriate size
   * @return number of samples in the output buffer
   */
    int Convert(std::vector<T>& inc_sample_data, std::vector<T>& out_waveform) {
      int outCount = 0;
      int s = 0;

      while (true) {
        while (m_inT <= (int)m_T) {
          m_inT++;
          m_pF[m_wrPos++] = inc_sample_data[s++];
          if (m_wrPos > m_interpolation)
            m_wrPos = 0;
          if (s >= (int)inc_sample_data.size()) {
            if (m_inT < m_T) {
              m_T = m_T - (double)m_inT;
              m_inT = 0;
            } else {
              m_inT = m_inT - (int)m_T;
              m_T = m_T - (double)(int)m_T;
            }
            return outCount;
          }
        }

        double D = (double)m_Hinterpolation + m_T - (double)(int)m_T;

        for (unsigned char n = 0; n <= m_interpolation; ++n) {
          m_pLI[n] = 1.0;
          for (unsigned char k = 0; k <= m_interpolation; ++k) {
            if (n != k) {
              m_pLI[n] = m_pLI[n] * (D - (double)k) / (double)(n - k);
            }
          }
        }

        double fout = 0.0;

        for (unsigned char k = 0, index = m_wrPos;;) {
          fout = fout + m_pLI[k++] * m_pF[index];
          if (k > m_interpolation)
            break;
          if (++index > m_interpolation)
            index = 0;
        }

        // check amplitude for limits
        if (fout > std::numeric_limits<T>::max())
          fout = std::numeric_limits<T>::max();

        if (fout < std::numeric_limits<T>::lowest())
          fout = std::numeric_limits<T>::lowest();

        out_waveform.push_back(fout);
        outCount++;
        m_T = m_T + m_dT;
      }
    }


   private:
    double m_dT;
    double m_T;
    int m_inT;
    std::vector<double> m_pF;
    unsigned char m_wrPos;
    std::vector<double> m_pLI;
    unsigned char m_interpolation;
    unsigned char m_Hinterpolation;
  };

}  // namespace SRC

#endif /* INCLUDE_CPPSL_MATH_DSP_SRC_LAGRANGE_HPP */
