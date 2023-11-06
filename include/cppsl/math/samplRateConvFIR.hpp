/************************************************************************//**
* @file
* @brief   Sampling Rate Converter with a finite impulse response (FIR).
 * FIR is a filter whose impulse response (or response to any finite length input)
 * is of finite duration, because it settles to zero in finite time.
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

#ifndef INCLUDE_CPPSL_MATH_DSP_SRCFIR_HPP
#define INCLUDE_CPPSL_MATH_DSP_SRCFIR_HPP

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include <cmath>
#include <limits>
#include <vector>
#include <stdexcept>

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

/// Sample rate converter with low pass FIR filter
/// https://www.mathworks.com/help/dsp/ug/designing-low-pass-fir-filters.html.
  /// The output signal has a delay depending on the length of the filter.
  /// To save memory and increase quality at low oversampling values, you can
  /// add a linear interpolation calculation of the missing FIR values of the filter.
  
  template <typename T>
  class SmpRateConvFIR {
    const double PI_RAD = 3.141592653589793238463;
   public:
    /**
   * @brief Default constructor
   */
    SmpRateConvFIR() {
      Initialize(1, 1, 1, 1);
    }

    /**
   * @brief Initialization
   * @param in_sampling_rate the input sample rate
   * @param out_sample_rate the output sample rate
   * @param oversampling the filter sample rate, determines the time accuracy.
   * If the interpolation of filter coefficients is not used, it is no good to use
   * a value of at least 1024.
   * The table of filter coefficients has the size of oversampling.
   * The oversampling mul product must be even to compute an even-order filter,
   * which allows only half of the coefficients to be stored and saves memory.
   * @param mul - the desired number of multiplications when filtering, determines
   * the processor load.
   */
    SmpRateConvFIR(int in_sampling_rate, int out_sample_rate, int oversampling, unsigned char mul) {
      Initialize(in_sampling_rate, out_sample_rate, oversampling, mul);
    }

    /**
   * @brief Parameter initialization
   * @param in_sampling_rate input sample rate
   * @param out_sample_rate output sample rate
   * @param oversampling filter sample rate, determines the accuracy on the time axis.
   * if the interpolation of filter coefficients is not used, it is worth using a value of at least 1024.
   * The table of filter coefficients has the size of oversampling mul.
   * @param mul - the desired number of multiplications when filtering, determines the processor load.
   * @return true if successful, otherwise false
   */
    bool Initialize(int in_sampling_rate, int out_sample_rate, int oversampling, unsigned char mul) {
      if ((mul & oversampling & 1) != 0)
        return false;

      m_oversampling = oversampling;
      m_mul_count = mul;
      m_fir_length1 = m_oversampling * m_mul_count;

      m_fir.resize((m_fir_length1 >> 1));

      /*
     * we calculate the filter for half of the sampling frequency,
     * since the operating frequency is increased by oversampling times,
     * the filter frequency should be oversampling times less:
     * f = 1, fd = 2
     * w = 2 * pi * f / fd / oversampling
     */
      double w = PI_RAD / m_oversampling;

      double sum = 0.0;
      /*
     * for symmetrical characteristic
     */
      double c = (double)m_fir.size() - 0.5;
      for (size_t i = 0; i < m_fir.size(); ++i) {
        double d = (double)i - c;
        m_fir[i] = std::sin(w * d) / d * GetWindowBlackman(i, m_fir_length1);
        sum += m_fir[i] + m_fir[i];
      }

      m_fir_length1--;

      /*
     * normalization and scaling
     */
      sum /= oversampling;
      for (size_t i = 0; i < m_fir.size(); ++i)
        m_fir[i] /= sum;

      m_buff.resize(m_mul_count + 1);

      for (int i = m_mul_count; i-- > 0;)
        m_buff[i] = 0;
      m_dT = (double)in_sampling_rate / (double)out_sample_rate;
      m_T = 0.0;
      m_inT = 0;

      return true;
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
        while (m_inT <= m_T) {
          m_inT++;

          /*
         * filtering buffer, delay line
         */
          for (int i = m_mul_count; --i > 0;)
            m_buff[i] = m_buff[i - 1];

          m_buff[0] = inc_sample_data[s++];
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

        /*
       * the nearest initial displacement on the time axis
       */
        int shift = (int)(0.5 + m_oversampling * (m_T - (double)(int)m_T));
        /*
       * To increase accuracy at small oversampling value you can
       * additionally calculate by interpolation value of the filter coefficient at the required point
       */
        double fout = 0.0;
        int k, i;
        // first half of the FIR filter
        for (k = shift, i = 0; k < (int)m_fir.size(); k += m_oversampling)
          fout += m_fir[k] * m_buff[i++];
        // second half of the FIR filter
        for (k = m_fir_length1 - k; k >= 0; k -= m_oversampling)
          fout += m_fir[k] * m_buff[i++];

        // amplitude limit
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

    /** @brief Windowing. Gets Blackman window element
   * @details in
   * 1) Harris, Fredric J. (Jan 1978). "On the use of Windows for
   * Harmonic Analysis with the Discrete Fourier Transform"
   * 2) https://www.hpmemoryproject.org/an/pdf/an_243.pdf
   * 3) https://en.wikipedia.org/wiki/Window_function
   * @param elem_numb element number
   * @param window_length window length
   * @return value of Blackman window element
   */
    double GetWindowBlackman(int elem_numb, int window_length) {
      const double A0 = 0.42;
      const double A1 = 0.5;
      const double A2 = 0.08;
      double N_SUB1 = window_length - 1;
      return A0 - (A1 * std::cos((2.0 * PI_RAD * (double)elem_numb) / N_SUB1)) +
             (A2 * std::cos((4.0 * PI_RAD * (double)elem_numb) / N_SUB1));
    }

   private:
    unsigned char m_mul_count;
    int m_oversampling;
    int m_fir_length1;
    std::vector<double> m_fir;
    std::vector<double> m_buff;
    double m_dT;
    double m_T;
    int m_inT;
  };

}   // namespace cppsl::math

#endif /* INCLUDE_CPPSL_MATH_DSP_SRCFIR_HPP */
