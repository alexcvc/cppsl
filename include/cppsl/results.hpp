//
// Author: Alexander Sacharov <a.sacharov@gmx.de>
// Year:   2022
//
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <https://unlicense.org>

#ifndef A7CBD896_42BA_4096_837C_F23EFB813E1C
#define A7CBD896_42BA_4096_837C_F23EFB813E1C

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <optional>
#include <variant>

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
  class result_optval {
   public:
    constexpr result_optval(T const &&t) noexcept
       : _opt_result_value{t} {
    }

    explicit constexpr result_optval() noexcept = default;

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
  struct result_var : protected std::variant<std::monostate, T> {
    explicit constexpr result_var() noexcept = default;

    constexpr result_var(T const &&t) noexcept
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
  struct result_opt : protected std::optional<T> {
    explicit constexpr result_opt() noexcept = default;

    constexpr result_opt(T const &&t) noexcept
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

}

#endif /* A7CBD896_42BA_4096_837C_F23EFB813E1C */
