
#ifndef EF70C160_D689_45AE_AE52_4AD10A99FAC3
#define EF70C160_D689_45AE_AE52_4AD10A99FAC3

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <endian.h>
#include <stdlib.h>

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

namespace cppsl::sys {
  /// host endian
  enum class endian {
    little_endian,
    big_endian,
    network_endian = big_endian,
#if __LITTLE_ENDIAN
    host_endian = little_endian
#elif __BIG_ENDIAN
    host_endian = big_endian
#else
  #error "unable to determine system bytes ordering"
#endif
  };

  namespace detail {

    template<typename T, size_t sz>
    struct SwapBytes {
      inline T operator()(T val) {
        throw std::out_of_range("data size");
      }
    };

    template<typename T>
    struct SwapBytes<T, 1> {
      inline T operator()(T val) {
        return val;
      }
    };

    template<typename T>
    struct SwapBytes<T, 2> // for 16 bit
    {
      inline T operator()(T val) {
        return ((((val) >> 8) & 0xff) | (((val) & 0xff) << 8));
      }
    };

    template<typename T>
    struct SwapBytes<T, 4> // for 32 bit
    {
      inline T operator()(T val) {
        return ((((val) & 0xff000000) >> 24) |
           (((val) & 0x00ff0000) >> 8) |
           (((val) & 0x0000ff00) << 8) |
           (((val) & 0x000000ff) << 24));
      }
    };

    template<>
    struct SwapBytes<float, 4> {
      inline float operator()(float val) {
        uint32_t mem = SwapBytes<uint32_t, sizeof(uint32_t)>()(*(uint32_t *) &val);
        return *(float *) &mem;
      }
    };

    template<typename T>
    struct SwapBytes<T, 8> // for 64 bit
    {
      inline T operator()(T val) {
        return ((((val) & 0xff00000000000000ull) >> 56) |
           (((val) & 0x00ff000000000000ull) >> 40) |
           (((val) & 0x0000ff0000000000ull) >> 24) |
           (((val) & 0x000000ff00000000ull) >> 8) |
           (((val) & 0x00000000ff000000ull) << 8) |
           (((val) & 0x0000000000ff0000ull) << 24) |
           (((val) & 0x000000000000ff00ull) << 40) |
           (((val) & 0x00000000000000ffull) << 56));
      }
    };

    template<>
    struct SwapBytes<double, 8> {
      inline double operator()(double val) {
        uint64_t mem = SwapBytes<uint64_t, sizeof(uint64_t)>()(*(uint64_t *) &val);
        return *(double *) &mem;
      }
    };

    template<endian from, endian to, class T>
    struct DoByteSwap {
      inline T operator()(T value) {
        return SwapBytes<T, sizeof(T)>()(value);
      }
    };

    // specialisations when attempting to swap to the same byte ordering

    // little -> little
    template<class T>
    struct DoByteSwap<endian::little_endian, endian::little_endian, T> { inline T operator()(T value) { return value; }};

    // big -> big
    template<class T>
    struct DoByteSwap<endian::big_endian, endian::big_endian, T> { inline T operator()(T value) { return value; }};

  } // namespace detail

  template<endian from, endian to, class T>
  inline T ByteSwap(T value) {
    // ensure the data is only 1, 2, 4 or 8 bytes
    static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8, "data is only 1, 2, 4 or 8 bytes");
    // ensure we're only swapping arithmetic types
    static_assert(std::is_arithmetic<T>::value, "only swapping arithmetic types");

    return detail::DoByteSwap<from, to, T>()(value);
  }
}  // namespace cppsl::sys

namespace cppsl {
  using endiannes = cppsl::sys::endian;
} // end of cppsl

#endif /* EF70C160_D689_45AE_AE52_4AD10A99FAC3 */
