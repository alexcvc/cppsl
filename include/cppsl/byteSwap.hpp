//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************/ /**
 * @file
 * @brief  contains byte swap class.
 * @ingroup CPPSL
 *****************************************************************************/

#pragma once

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <bit>
#include <cstdint>
#include <cstddef>

//----------------------------------------------------------------------------
// Public Function Prototypes
//----------------------------------------------------------------------------

namespace cppsl {

/**
 * @class ByteSwapper
 * @brief A class for swapping bytes based on the endianness
 *
 * This class provides a static method for swapping bytes in a value based on the endianness of the machine,
 * as well as an enum class for specifying the swap type.
 */
class ByteSwapper {
 public:
  enum class SwapType {
    BE,   ///< swap for big endian machines
    LE,   ///< swap for little endian machines
    AX,   ///< swap always
    NX    ///< swap never
  };

  /**
   * @brief A class for swapping bytes based on the endianness
   *
   * This class provides a static method for swapping bytes in a value based on the endianness of the machine,
   * as well as an enum class for specifying the swap type.
   */
  template <typename T>
  static T Swap(T val, SwapType swapType) {
    if (swapType == SwapType::NX) {
      return val;
    }

    T ret = val;
    auto ptr = reinterpret_cast<uint8_t*>(&ret);
    const auto size = sizeof(T);

    if constexpr (std::endian::native == std::endian::big) {
      if (swapType == SwapType::BE || swapType == SwapType::AX)
        SwapBytes(ptr, size);
    } else if constexpr (std::endian::native == std::endian::little) {
      if (swapType == SwapType::LE || swapType == SwapType::AX)
        SwapBytes(ptr, size);
    }

    return ret;
  }

  /**
   * @fn swapBytes
   * @brief A helper method to swap bytes
   * Swaps the bytes in the given buffer.
   *
   * This function swaps the bytes in the given buffer of the specified size.
   *
   * @param ptr The pointer to the buffer of bytes to be swapped.
   * @param size The size of the buffer in bytes.
   * @return None.
   */
  static void SwapBytes(uint8_t* ptr, size_t size) {
    for (size_t i = 0; i < size / 2; ++i) {
      std::swap(ptr[i], ptr[size - i - 1]);
    }
  }
};

}   // namespace cppsl
