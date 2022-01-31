//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************//**
 * \file    byteswap.hpp
 * \brief   contains byte swap (as starting C++20).
 * \author  Alexander Sacharov
 * \date    2022-01-11
 * \ingroup
 *****************************************************************************/

#ifndef __INCLUDE_CPPSL_SYS_ENDIAN_H__
#define __INCLUDE_CPPSL_SYS_ENDIAN_H__

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <endian.h>
#include <stdlib.h>
#include <stdint.h>

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
  /// host ByteOrder
  enum class ByteOrder {
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
    struct swap {
      inline T operator()(T val) {
        throw std::out_of_range("data size");
      }
    };

    template<typename T>
    struct swap<T, 1> {
      inline T operator()(T val) {
        return val;
      }
    };

    template<typename T>
    struct swap<T, 2> // for 16 bit
    {
      inline T operator()(T val) {
        return ((((val) >> 8) & 0xff) | (((val) & 0xff) << 8));
      }
    };

    template<typename T>
    struct swap<T, 4> // for 32 bit
    {
      inline T operator()(T val) {
        return ((((val) & 0xff000000) >> 24) |
           (((val) & 0x00ff0000) >> 8) |
           (((val) & 0x0000ff00) << 8) |
           (((val) & 0x000000ff) << 24));
      }
    };

    template<>
    struct swap<float, 4> {
      inline float operator()(float val) {
        uint32_t mem = swap<uint32_t, sizeof(uint32_t)>()(*(uint32_t *) &val);
        return *(float *) &mem;
      }
    };

    template<typename T>
    struct swap<T, 8> // for 64 bit
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
    struct swap<double, 8> {
      inline double operator()(double val) {
        uint64_t mem = swap<uint64_t, sizeof(uint64_t)>()(*(uint64_t *) &val);
        return *(double *) &mem;
      }
    };

    template<class T>
    struct do_swap_always {
      inline T operator()(T value) {
        return swap<T, sizeof(T)>()(value);
      }
    };

    template<ByteOrder from, ByteOrder to, class T>
    struct do_swap {
      inline T operator()(T value) {
        return swap<T, sizeof(T)>()(value);
      }
    };

    // specialisations when attempting to swap to the same byte ordering

    // little -> little
    template<class T>
    struct do_swap<ByteOrder::little_endian, ByteOrder::little_endian, T> { inline T operator()(T value) { return value; }};

    // big -> big
    template<class T>
    struct do_swap<ByteOrder::big_endian, ByteOrder::big_endian, T> { inline T operator()(T value) { return value; }};

  } // namespace detail

  /// swap anyway
  /// \tparam T 
  /// \param value 
  template<class T>
  inline T byteswap(T value) {
    // ensure the data is only 1, 2, 4 or 8 bytes
    static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8, "data is only 1, 2, 4 or 8 bytes");
    // ensure we're only swapping arithmetic types
    static_assert(std::is_arithmetic<T>::value, "only swapping arithmetic types");

    return detail::do_swap_always<T>()(value);
  }

  /// swap from to byte ordering
  /// \tparam T 
  /// \param value 
  template<ByteOrder from, ByteOrder to, class T>
  inline T byteswap(T value) {
    // ensure the data is only 1, 2, 4 or 8 bytes
    static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8, "data is only 1, 2, 4 or 8 bytes");
    // ensure we're only swapping arithmetic types
    static_assert(std::is_arithmetic<T>::value, "only swapping arithmetic types");

    return detail::do_swap<from, to, T>()(value);
  }
}  // namespace cppsl::sys

namespace cppsl {
  using endiannes = cppsl::sys::ByteOrder;
} // end of cppsl

#endif /* __INCLUDE_CPPSL_SYS_ENDIAN_H__ */
