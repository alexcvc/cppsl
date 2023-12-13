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
    /**
     * @class ResultOptVal
     * @brief A class that represents a result using std::optional. It provides methods for checking if the result is valid or invalid, retrieving the result value, and getting a default
    * value if no result is present.
     *
     * It is important to note that this class does not inherit from a standard library class to avoid incorrect behavior. It is marked as final so that it cannot be used as a base class
    *. Instead, it contains an std::optional member for storing the result value.
     *
     * @tparam T The type of the result value.
     */
    constexpr ResultOptVal(T const &&t) noexcept
       : _opt_result_value{t} {
    }

    /**
     * @class ResultOptVal
     * @brief A class that represents a result using std::optional.
     *
     * The ResultOptVal class provides methods for checking if the result is valid or invalid,
     * retrieving the result value, and getting a default value if no result is present.
     *
     * @tparam T The type of the result value.
     */
    explicit constexpr ResultOptVal() noexcept = default;

    /**
     * @brief Check if the result value is valid.
     *
     * This method determines if the result value is valid by checking if `_opt_result_value` has a value.
     *
     * @return true if the result value is valid, false otherwise.
     */
    [[nodiscard]] constexpr bool valid() const noexcept {
      return _opt_result_value.has_value();
    }

    /**
     * @brief Check if the result value is invalid.
     *
     * This method determines if the result value is invalid by checking if `_opt_result_value` does not have a value.
     *
     * @return true if the result value is invalid, false otherwise.
     */
    [[nodiscard]] constexpr bool invalid() const noexcept {
      return !valid();
    }

    /**
     * @class ResultOptVal
     * @brief A class that represents a result using std::optional. It provides methods for checking if the result is valid or invalid, retrieving the result value, and getting a default
    * value if no result is present.
     *
     * It is important to note that this class does not inherit from a standard library class to avoid incorrect behavior. It is marked as final so that it cannot be used as a base class
    *. Instead, it contains an std::optional member for storing the result value.
     *
     * @tparam T The type of the result value.
     */
    [[nodiscard]] constexpr auto get() const -> T {
      return _opt_result_value.value();
    }

    /**
     * @brief Retrieves the result value or a default value if no result is present.
     *
     * This method returns the result value stored in `_opt_result_value`. If there is no result value, it returns a default-constructed value of type `T`.
     *
     * @return The result value, or a default-constructed value of type `T` if no result is present.
     */
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
    /**
     * @class ResultVarVal
     * Represents a result value that can hold a variable or void.
     */
    explicit constexpr ResultVarVal() noexcept = default;

    /**
     * @class ResultVarVal
     * Represents a result value that can hold a variable or void.
     */
    constexpr ResultVarVal(T const &&t) noexcept
       : std::variant<std::monostate, T>{t} {
    }

    /**
     * @brief Check if the ResultVarVal object holds a valid value.
     *
     * @return true if the ResultVarVal object holds a valid value, false otherwise.
     */
    constexpr bool valid() const noexcept {
      return std::holds_alternative<T>(*this);
    }

    /**
     * @brief Check if the ResultVarVal object holds an invalid value.
     *
     * @return true if the ResultVarVal object holds an invalid value, false otherwise.
     */
    constexpr bool invalid() const noexcept {
      return !valid();
    }

    /**
     * @brief Get the value stored in the ResultVarVal object.
     *
     * If the ResultVarVal object holds a valid value, returns that value.
     * If the ResultVarVal object holds an invalid value, returns a default-constructed value of type T.
     *
     * @return The value stored in the ResultVarVal object.
     */
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

    /**
     * @class ResultOptional
     *
     * @brief Result based on std::optional
     *
     * The ResultOptional class is a templated class that is based on std::optional. It provides a result type that either contains a valid value of type T or is empty. This class is designed
    * to be a wrapper around std::optional and adds some convenience methods for checking the validity of the result and accessing the value.
     *
     * Usage:
     * ```
     * ResultOptional<int> result(42);
     * if (result.valid()) {
     *     int value = result.get();
     *     // Do something with value
     * } else {
     *     // Handle error case
     * }
     * ```
     *
     * @tparam T The type of the value contained in the result
     */
    constexpr ResultOptional(T const &&t) noexcept
       : std::optional<T>{t} {
    }

    /**
     * @brief Check if the ResultOptional object is valid.
     *
     * This function checks if the ResultOptional object contains a valid value.
     * It returns true if the object has a value, and false otherwise.
     *
     * @return True if the object has a value, false otherwise.
     */
    [[nodiscard]] constexpr bool valid() const noexcept {
      //        return bool( *this);
      return std::optional<T>::has_value();
    }

    /**
     * @brief Check if the ResultOptional object is invalid.
     *
     * This function checks if the ResultOptional object is invalid. An object is considered
     * invalid if it does not contain a valid value. It returns true if the object is invalid,
     * and false otherwise.
     *
     * @return True if the object is invalid, false otherwise.
     */
    [[nodiscard]] constexpr bool invalid() const noexcept {
      return !valid();
    }

    /**
     * @brief Get the value contained in the ResultOptional object.
     *
     * This member function returns the value contained in the ResultOptional object. If the object has a value, that value is returned. If the object is empty, a default-constructed value
    * of type T is returned.
     *
     * @tparam T The type of the value.
     * @return The value contained in the ResultOptional object.
     */
    [[nodiscard]] constexpr auto get() const noexcept -> T {
      return std::optional<T>::value_or(T());
    }
  };

}   // namespace cppsl

#endif /* INCLUDE_CPPSL_RESULT_HPP */
