//
// Copyright (c) 2015 Andre Haupt, Anthony Williams
//
// This work involved from https://wiggling-bits.net/using-enum-classes-as-type-safe-bitmasks/.
// is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************/ /**
 * @file
 * \brief   boilerplate code for using enum classes as type-safe bitmasks..
 * \author  Alexander Sacharov
 * \date    2018-12-27
 * \ingroup
 *****************************************************************************/

/**
 * Using:
 *
 *  enum class Permissions
 *  {
 *      Readable   = 0x4,
 *      Writable   = 0x2,
 *      Executable = 0x1
 *  };
 *  ENABLE_BITMASK_OPERATORS(Permissions)
 *
 *
 *  int main()
 *  {
 *    Permissions p = Permissions::Readable | Permissions::Writable; // Set default permissions
 *    p |= Permissions::Executable; // Add permission
 *    p &= ~Permissions::Writable;  // Remove permission
 *    bool hasReadPermission = static_cast<bool>(p & Permissions::Readable);
 *    bool hasWritePermission = static_cast<bool>(p & Permissions::Writable);
 *    bool hasExecutePermission = static_cast<bool>(p & Permissions::Executable);
 *    if (hasReadPermission) {
 *        std::cout << "Has read permission" << "\n";
 *    }
 *    if (hasWritePermission) {
 *        std::cout << "Has write permission" << "\n";
 *    }
 *    if (hasExecutePermission) {
 *        std::cout << "Has execute permission" << "\n";
 *    }
 *  }
 */

#ifndef INCLUDE_CPPSL_ENUM_BITMASK_H
#define INCLUDE_CPPSL_ENUM_BITMASK_H

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <type_traits>

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------
#define ENABLE_BITMASK_OPERATORS(x)         \
  template <>                               \
  struct EnableBitMaskOperators<x> { \
    static const bool enable = true;        \
  };

//----------------------------------------------------------------------------
// Public Prototypes
//----------------------------------------------------------------------------

template <typename Enum>
struct EnableBitMaskOperators {
  static const bool enable = false;
};

template <typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type operator&(Enum lhs, Enum rhs) {
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

template <typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type operator^(Enum lhs, Enum rhs) {
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum>(static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
}

template <typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type operator~(Enum rhs) {

  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum>(~static_cast<underlying>(rhs));
}

template <typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type operator|(Enum lhs, Enum rhs) {
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

template <typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type& operator&=(Enum& lhs, Enum rhs) {
  using underlying = typename std::underlying_type<Enum>::type;
  lhs = static_cast<Enum>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
  return lhs;
}

template <typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type& operator^=(Enum& lhs, Enum rhs) {
  using underlying = typename std::underlying_type<Enum>::type;
  lhs = static_cast<Enum>(static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
  return lhs;
}

template <typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum&>::type operator|=(Enum& lhs, Enum rhs) {
  using underlying = typename std::underlying_type<Enum>::type;
  lhs = static_cast<Enum>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
  return lhs;
}

#endif /* INCLUDE_CPPSL_ENUM_BITMASK_H */
