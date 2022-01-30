//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************/ /**
 * \file    menu.hpp
 * \brief   contains console menu in C++17 style.
 * \author  Alexander Sacharov
 * \date    2022-12-27
 * \ingroup C++ support library
 *****************************************************************************/

/**
 * \section ex1 Example of use console menu
 *
 *  --------------------------------
 *  #include "cppsl/console/menu.hh"
 *
 *  // define namespaces here
 *  using namespace cppsl::console;
 *  using varVal = std::variant<size_t, int, double, char, std::string>;
 *  --------------------------------
 *  Now define all menu items functions
 *
 *  void menu_func_test1(std::any &param) {
 *     auto &v = std::any_cast<std::vector<varVal> &>(param);
 *     // useful stuff here
 *     ...
 *     // push in stack of calls
 *     v.push_back(menu::get_number_from_stream<size_t>("Enter a number ..."));
 *  }
 *
 *  void menu_func_test2(std::any &param) {
 *     auto &v = std::any_cast<std::vector<varVal> &>(param);
*     // useful stuff here
*     ...
*     // push in stack of calls
 *     v.push_back(menu::get_line_from_stream<size_t>("Enter a text ..."));
 *  }
 *  ...
 *  void menu_func_show(std::any &param) {
 *     auto &v = std::any_cast<std::vector<varVal> &>(param);
 *
 *     std::cout << "Entered data is \n";
 *
 *     for (const auto &d : v)
 *        if (auto pvi = std::get_if<int>(&d))
 *           std::cout << *pvi << std::endl;
 *        else if (auto pvd = std::get_if<double>(&d))
 *           std::cout << *pvd << std::endl;
 *        else if (auto pvc = std::get_if<char>(&d))
 *           std::cout << *pvc << std::endl;
 *        else if (auto pvs = std::get_if<std::string>(&d))
 *           std::cout << *pvs << std::endl;
 *        else if (auto pvu = std::get_if<size_t>(&d))
 *           std::cout << *pvu << std::endl;
 *        else
 *           std::cout << "Unknown type" << std::endl;
 *
 *     std::cout << std::endl;
 *  }
 *  ...
 *
 *  In main function:
 *  ...
 *  printf("Open test Menu\n");
 *
 *  std::any vect_params = std::vector<varVal>{};
 *
 *  // self menu
 *  menu::console_menu consoleMenu{
 *     "Test Menu ...", {
 *        { "Test menu 1", menu_func_test1},
 *        { "Test menu 2", menu_func_test2},
 *        { "Show menu", menu_func_show }
 *     }
 *  };
 *
 *  // view menu
 *  // Remark (mail remains in menu up to exit from menu)
 *  main_menu.menu(vect_params);
 *  ...
 */

#ifndef EA3C8146_3CEF_4C3B_9789_14772CA8FD66
#define EA3C8146_3CEF_4C3B_9789_14772CA8FD66

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <cctype>
#include <iostream>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>
#include <any>
#include <variant>
#include <vector>

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public defines and macros
//----------------------------------------------------------------------------
// ::max used for numeric_limits!
#ifdef max
#undef max
#endif

//----------------------------------------------------------------------------
// Public typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public Function Prototypes
//----------------------------------------------------------------------------

/*
 * Console menu with number items
 */
namespace cppsl::console::menu {
  /**
   * Removes leading and trailing white-space chars fromstd::string s
   * @param s -std::string to use (not changed)
   * @return updatedstd::string
   */
  inline std::string trim_whitespaces(const std::string &s) {
    constexpr char whitespace[] = " \t\n\r";
    const size_t first = s.find_first_not_of(whitespace);

    return (first != std::string::npos) ? s.substr(first, (s.find_last_not_of(whitespace) - first + 1)) : std::string{};
  }

  /**
   * Converts a text number to specified type.
   * All of the text must be a valid number of the specified type. eg 345Mq is invalid
   * @tparam T - type
   * @param st -std::string to convert
   * @return returns either value of converted number or no value if text number cannot be converted
   */
  template<typename T = int>
  std::optional<T> str_to_number(const std::string &st) {
    const auto s = trim_whitespaces(st);
    bool ok = s.empty() ? false : (std::isdigit(s.front())
       || (((std::is_signed<T>::value && (s.front() == '-')) || (s.front() == '+')) && ((s.size() > 1) && std::isdigit(s[1]))));

    auto v = T{};

    if (ok) {
      std::istringstream ss(s);

      ss >> v;
      ok = (ss.peek() == EOF);
    }

    return ok ? v : std::optional<T>{};
  }

  /**
   * Obtain a line of text from specified stream. Removes any existing data from input buffer
   * @param is - input stream
   * @param def - optional default text if no text entered
   * @return either valid input line or no value if problem obtaining input
   */
  std::optional<std::string> get_line_from_stream(std::istream &is, const std::string &def = "") {
    for (auto no = is.rdbuf()->in_avail(); no && is && std::isspace(is.peek()); is.ignore(), --no);

    std::string ln;

    return std::getline(is, ln) ? (ln.empty() && !def.empty() ? def : ln) : (is.clear(), std::optional<std::string>{});
  }

  /**
   * Obtain a line of text from console. First displays prompt text. If default text provided display within [..] after prompt
   * @param prm - optional prompt text to display first
   * @param def - optional default text if no text entered
   * @return entered text as typestd::string. No error conditions. Only returns when valid data entered
   */
  auto get_line_from_stream(const std::string &prm = "", const std::string &def = "") {
    std::optional<std::string> o;

    do {
      std::cout << prm;
      if (!def.empty())
        std::cout << " [" << def << "]";

      std::cout << " :";
      o = get_line_from_stream(std::cin, def);
    } while (!o.has_value() && (std::cout << "Invalid input" << std::endl));

    return *o;
  }

  /**
   * Retrieves the next data item from the specified stream. The data must end with a space.
   * @tparam T - type
   * @param is - stream from which data should be extracted
   * @return either the correctly extracted data or no value, if there are problems with data extraction
   */
  template<typename T = std::string>
  std::optional<T> get_next_item(std::istream &is) {
    auto i = T{};
    const bool b = (is >> i) && std::isspace(is.peek());

    for (is.clear(); is && !std::isspace(is.peek()); is.ignore());
    return b ? i : std::optional<T>{};
  }

  /**
   * Retrieves a number from the specified stream in the specified type. By default, number type is int.
   * @tparam T
   * @param is - stream from which the number is to be retrieved
   * @param oneline - true if only one number per line (default), false if can have multiple numbers per line.
   * @return either a valid number of the required type or no value if there are problems during data extraction
   */
  template<typename T = int>
  auto get_number_from_stream(std::istream &is, bool oneline = true) {
    if (oneline) {
      const auto o = get_line_from_stream(is);
      return o.has_value() ? str_to_number<T>(*o) : std::optional<T>{};
    }

    return get_next_item<T>(is);
  }

  /**
   * Obtains a number from the console.
   * First displays prompt text. If specified, number must be within the specified min..max range
   * and range displayed as (...) after prompt prm
   * @tparam T - type
   * @param prm - optional prompt text to display first
   * @param nmin - optional minimum valid value
   * @param nmax - optional maximum valid value
   * @param oneline- true if only one number per line (default), false if can have multiple numbers per line.
   * @return
   */
  template<typename T = int>
  auto get_number_from_stream(const std::string &prm = "",
                              T nmin = std::numeric_limits<T>::lowest(),
                              T nmax = std::numeric_limits<T>::max(),
                              bool oneline = true) {
    const auto showdefs = [nmin, nmax]() {
      std::cout << " (";

      if (nmin != std::numeric_limits<T>::lowest() || std::is_unsigned<T>::value)
        std::cout << nmin;

      std::cout << " - ";

      if (nmax != std::numeric_limits<T>::max())
        std::cout << nmax;

      std::cout << ")";
    };

    std::optional<T> o;

    do {
      std::cout << prm;

      if ((nmin != std::numeric_limits<T>::lowest()) || (nmax != std::numeric_limits<T>::max()))
        showdefs();

      std::cout << " :";

      o = get_number_from_stream<T>(std::cin, oneline);
    } while ((!o.has_value() || (((*o < nmin) || (*o > nmax)))) && (std::cout << "Invalid input" << std::endl));

    return *o;
  }

  /**
   * Resolve a char from the specified stream
   * @param  is - stream from which to obtain number
   * @param  def - default char to return if no character obtained
   * @param  oneline - true if only one char per line (default), false if can have multiple chars per line
   * @return either valid character or no value if problem extracting data
   */
  std::optional<char> get_char_stream(std::istream &is, char def = 0, bool oneline = true) {
    if (oneline) {
      if (auto o = get_line_from_stream(is); o.has_value())
        return (o->empty() && def ? def : ((o->size() == 1) ? o->front() : std::optional<char>{}));
      else
        return {};
    }

    return get_next_item<char>(is);
  }

  /**
   * Obtains a char from the console. First displays prompt text
   * @param  prm - optional prompt text to display first
   * @param  valid - optionalstd::string containing valid values for the char. Displayed within (...)
   * @param  def - optional default char to use if none entered. Displayed within [...]
   * @param  oneline - true if only one char per line (default), false if can have multiple chars per line
   * @return returns valid char. No error conditions. Only returns when valid char entered
   */
  auto get_char_console(const std::string &prm = "", const std::string &valid = "", char def = 0, bool oneline = true) {
    const auto showopt = [&valid, def]() {
      std::cout << " (";

      for (size_t i = 0, s = valid.size(); i < s; ++i)
        std::cout << (i ? "/" : "") << valid[i];

      std::cout << ")";

      if (def)
        std::cout << " [" << def << "]";
    };

    std::optional<char> o;

    do {
      std::cout << prm;

      if (!valid.empty())
        showopt();

      std::cout << " :";
      o = get_char_stream(std::cin, def, oneline);
    } while ((!o.has_value() || ((!valid.empty()) && (valid.find(*o) == std::string::npos))) &&
       (std::cout << "\nInvalid input!" << std::endl));

    return *o;
  }

  /**
   * Class implements console menu. A menu consists of one or more menu_item items – which can
   * 1) either be a function pointer
   * 2) or a pointer to another menu_item.
   *
   * As two different types of entry are to be stored in a vector of variant.
   * The type of pointer to menu_item is certainly known.
   * The functions called have one parameter - but which may be defined by the show_menu.
   * So std::any type is used for the function parameter. The type of entry for the function is known:
   *
   * void any_your_menu_function(std::any& your_param);
   *
   * The function type is void(*)(std::any& param);
   *
   * The two types are mandatory in the vector show_menu, therefore f_type and enu*. See struct menu_item.
   *
   */
  class console_menu {
   private:
    using func_type = void (*)(std::any &param);

    struct menu_item {
      std::string name;
      std::variant<func_type, console_menu *> func;
    };

   public:
    console_menu() {}

    console_menu(const std::string &t, const std::vector<menu_item> &vm) : m_title(t), m_items(vm) {}

    auto title() const noexcept {
      return m_title;
    }

    void title(const std::string &t) {
      m_title = t;
    }

    void menu(std::any &param) {
      show_menu(*this, param);
    }

    bool erase(size_t indx) {
      if (indx < m_items.size()) {
        m_items.erase(m_items.begin() + indx);
        return true;
      }

      return false;
    }

    bool append(const menu_item &mi) {
      m_items.emplace_back(mi);
      return true;
    }

    bool insert(size_t indx, const menu_item &mi) {
      if (indx < m_items.size()) {
        m_items.insert(m_items.begin() + indx, mi);
        return true;
      }

      return false;
    }

   private:
    /**
     * recursive show menu for items
     * @param m - console menu
     * @param param - parameter in console menu
     */
    void show_menu(const console_menu &m, std::any &param) {
      const auto show = [nom = m.m_items.size()](const console_menu &mu) {
        std::ostringstream oss;

        oss << mu.title() << std::endl;

        for (size_t i = 0U; i < nom; ++i) {
          oss << i + 1 << ")  " << mu.m_items[i].name << std::endl;
        }

        oss << "0)  Exit menu\n\n"
               "Enter menu option number";
        return get_number_from_stream<size_t>(oss.str(), 0, nom);
      };

      for (size_t opt = 0U; (opt = show(m)) > 0;)
        if (const auto &mi = m.m_items[opt - 1]; std::holds_alternative<console_menu::func_type>(mi.func))
          std::get<console_menu::func_type>(mi.func)(param);
        else
          show_menu(*std::get<console_menu *>(mi.func), param);
    }

    std::string m_title;
    std::vector<menu_item> m_items;
  };

}// namespace cppsl::console::menu

#endif /* EA3C8146_3CEF_4C3B_9789_14772CA8FD66 */
