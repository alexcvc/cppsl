/* SPDX-License-Identifier: MIT */
//
// Copyright (c) 2023 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

#ifndef INCLUDE_CPPSL_RESULT_HPP
#define INCLUDE_CPPSL_RESULT_HPP

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <optional>
#include <variant>

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

  // ////////////////////////////////////////////////////////////////////////////
  /// result using std::optional
  /// It is incorrect to inherit from a standard library class.
  /// The result must be marked as final so that it cannot serve as a base class. 
  /// Returning the constructed default value removes the decision from the user
  /// of the class. The following is an implementation that does not use inheritance. 
  /// Instead, std::optional is a member of the class. The get_or() method is added,
  /// which returns the default type value if there is no data. The get() method will
  /// throw an exception if there is no data. The Result user must perform the check.
  // ////////////////////////////////////////////////////////////////////////////
  template<typename T>
  class ResultOptVal {
   public:
    constexpr ResultOptVal(T const &&t) noexcept
       : _opt_result_value{t} {
    }

    explicit constexpr ResultOptVal() noexcept = default;

    [[nodiscard]] constexpr bool valid() const noexcept {
      return _opt_result_value.has_value();
    }

    [[nodiscard]] constexpr bool invalid() const noexcept {
      return !valid();
    }

    [[nodiscard]] constexpr auto get() const -> T {
      return _opt_result_value.value();
    }

    [[nodiscard]] auto get_or() const noexcept -> T {
      return _opt_result_value.value_or(T());
    }

   private:
    std::optional<T> _opt_result_value;
  };

  ///////////////////////////////////////////////////////////////////////////////
  /// Result Based on std::variant class
  /// With the std::any version of result requiring a type specification.
  /// That left std::variant as a possibility instead of using std::optional.
  // ////////////////////////////////////////////////////////////////////////////
  template<typename T>
  struct ResultVarVal : protected std::variant<std::monostate, T> {
    explicit constexpr ResultVarVal() noexcept = default;

    constexpr ResultVarVal(T const &&t) noexcept
       : std::variant<std::monostate, T>{t} {
    }

    constexpr bool valid() const noexcept {
      return std::holds_alternative<T>(*this);
    }

    constexpr bool invalid() const noexcept {
      return !valid();
    }

    constexpr auto get() const noexcept -> T {
      return (valid() ? std::get<T>(*this) : T());
    }
  };

  // ////////////////////////////////////////////////////////////////////////////
  /// Result based on std::optional
  /// Basing result on std::optional always was the top choice.
  /// So here is the version using the top choice.
  /// The other implementations except using different methods for the internals.
  /// An interesting method in std::optional is the operator bool conversion method as an alternative to the has_value() method.
  /// I find it strange or inconsistent not to provide that method in all these classes.
  /// Another interesting method is value_or() which handles the test used in the other implementations.
  // ////////////////////////////////////////////////////////////////////////////
  template<typename T>
  struct ResultOptional : protected std::optional<T> {
    explicit constexpr ResultOptional() noexcept = default;

    constexpr ResultOptional(T const &&t) noexcept
       : std::optional<T>{t} {
    }

    [[nodiscard]] constexpr bool valid() const noexcept {
      //        return bool( *this);
      return std::optional<T>::has_value();
    }

    [[nodiscard]] constexpr bool invalid() const noexcept {
      return !valid();
    }

    [[nodiscard]] constexpr auto get() const noexcept -> T {
      return std::optional<T>::value_or(T());
    }
  };

}   // namespace cppsl

#endif /* INCLUDE_CPPSL_RESULT_HPP */
