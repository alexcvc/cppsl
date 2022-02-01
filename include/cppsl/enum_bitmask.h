//
// Copyright (c) 2015 Andre Haupt, Anthony Williams
//
// This work involved from https://wiggling-bits.net/using-enum-classes-as-type-safe-bitmasks/.
// is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************//**
 * \file    enumBits.hpp
 * \brief   boilerplate code for using enum classes as type-safe bitmasks..
 * \author  Alexander Sacharov
 * \date    2022-12-27
 * \ingroup
 *****************************************************************************/

/**
 * Using:
 *    enum class AnyEnumClass {
 *       Readable   = 0x4,
 *       Writeable  = 0x2,
 *       Executable = 0x1
 *    };
 *    ENABLE_BITMASK_OPERATORS(AnyEnumClass)
 *
 *    // all ok
 *    T_EnumClass p = AnyEnumClass::Readable | AnyEnumClass::Writable;
 *    p |= AnyEnumClass::Executable;
 *    p &= ~AnyEnumClass::Writable;
 *
 */

#ifndef EE3869A1_C8EC_4FF8_9A27_5A710C0133BF
#define EE3869A1_C8EC_4FF8_9A27_5A710C0133BF

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

//----------------------------------------------------------------------------
// Public Prototypes
//----------------------------------------------------------------------------

namespace cppsl {

   /**
    * To enable the bitwise operators, provides a template specialization for bitmask enum
    */
   template<typename TEnum>
   struct EnableBitMaskOperators {
      static const bool enable = false;
   };

   /**
    * Operators
    */
   template<typename TEnum>
   typename std::enable_if<EnableBitMaskOperators<TEnum>::enable, TEnum>::type
   operator&(TEnum lhs, TEnum rhs) {
      using underlying = typename std::underlying_type<TEnum>::type;
      return static_cast<TEnum> (
         static_cast<underlying>(lhs) &
         static_cast<underlying>(rhs)
      );
   }

   template<typename TEnum>
   typename std::enable_if<EnableBitMaskOperators<TEnum>::enable, TEnum>::type
   operator^(TEnum lhs, TEnum rhs) {
      using underlying = typename std::underlying_type<TEnum>::type;
      return static_cast<TEnum> (
         static_cast<underlying>(lhs) ^
         static_cast<underlying>(rhs)
      );
   }

   template<typename TEnum>
   typename std::enable_if<EnableBitMaskOperators<TEnum>::enable, TEnum>::type
   operator~(TEnum rhs) {

      using underlying = typename std::underlying_type<TEnum>::type;
      return static_cast<TEnum> (
         ~static_cast<underlying>(rhs)
      );
   }

   template<typename TEnum>
   typename std::enable_if<EnableBitMaskOperators<TEnum>::enable, TEnum>::type
   operator|(TEnum lhs, TEnum rhs) {
      using underlying = typename std::underlying_type<TEnum>::type;
      return static_cast<TEnum> (
         static_cast<underlying>(lhs) |
         static_cast<underlying>(rhs)
      );
   }

   template<typename TEnum>
   typename std::enable_if<EnableBitMaskOperators<TEnum>::enable, TEnum>::type &
   operator&=(TEnum &lhs, TEnum rhs) {
      using underlying = typename std::underlying_type<TEnum>::type;
      lhs = static_cast<TEnum> (
         static_cast<underlying>(lhs) &
         static_cast<underlying>(rhs)
      );
      return lhs;
   }

   template<typename TEnum>
   typename std::enable_if<EnableBitMaskOperators<TEnum>::enable, TEnum>::type &
   operator^=(TEnum &lhs, TEnum rhs) {
      using underlying = typename std::underlying_type<TEnum>::type;
      lhs = static_cast<TEnum> (
         static_cast<underlying>(lhs) ^
         static_cast<underlying>(rhs)
      );
      return lhs;
   }

   template<typename TEnum>
   typename std::enable_if<EnableBitMaskOperators<TEnum>::enable, TEnum &>::type
   operator|=(TEnum &lhs, TEnum rhs) {
      using underlying = typename std::underlying_type<TEnum>::type;
      lhs = static_cast<TEnum> (
         static_cast<underlying>(lhs) |
         static_cast<underlying>(rhs)
      );
      return lhs;
   }
} // end of cppsl


#define ENABLE_ENUM_BITMASK_OPERATORS(x)  \
template<>                           \
struct cppsl::EnableBitMaskOperators<x>     \
{                                    \
    static const bool enable = true; \
};

#endif /* EE3869A1_C8EC_4FF8_9A27_5A710C0133BF */
