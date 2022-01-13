//
// Copyright (c) 2018 Anthony Williams.
// The source code was released under the [Boost Software License]
// For a copy, see (https://www.boost.org/LICENSE_1_0.txt).
//
// Advanced from
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************//**
 * \file    concurrency.hpp
 * \brief   contains useful classes for threading and concurrency.
 * \author  Alexander Sacharov
 * \date    2022-01-13
 * \ingroup
 *****************************************************************************/

#ifndef DA7A4988_DEB1_4AA1_9C88_5F756805A1DB
#define DA7A4988_DEB1_4AA1_9C88_5F756805A1DB

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <thread>

//----------------------------------------------------------------------------
// Defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public Prototypes
//----------------------------------------------------------------------------

namespace cppsl::threading {

  //
  /// Class joinable thread
  /// Class is similar to std::thread, except that join is automatically
  /// included in the destructor. std::jthread is in C++20
  /// @author Alexander Sacharov
  //
  class jthread {
    std::thread _thread;  /// member thread

   public:

    /// default ctor
    jthread() noexcept = default;

    /// explicit universal ctor
    template<typename Callable, typename ... Args>
    explicit jthread(Callable &&func, Args &&... args):
       _thread(std::forward<Callable>(func), std::forward<Args>(args)...) {}

    /// ctor with thread move
    explicit jthread(std::thread t_) noexcept:
       _thread(std::move(t_)) {}

    /// copy ctor
    jthread(jthread &&other) noexcept:
       _thread(std::move(other._thread)) {}

    /// assignment
    jthread &operator=(jthread &&other) noexcept {
      if (joinable())
        join();
      _thread = std::move(other._thread);
      return *this;
    }

    /// assignment std::thread
    jthread &operator=(std::thread other) noexcept {
      if (joinable())
        join();
      _thread = std::move(other);
      return *this;
    }

    /// destructor
    ~jthread() noexcept {
      if (joinable())
        join();
    }

    /// swap (move) other jthread
    void swap(jthread &other) noexcept {
      _thread.swap(other._thread);
    }

    /// get thread id
    std::thread::id get_id() const noexcept {
      return _thread.get_id();
    }

    /// is joinable
    bool joinable() const noexcept {
      return _thread.joinable();
    }

    /// join thread
    void join() {
      _thread.join();
    }

    /// detach thread
    void detach() {
      _thread.detach();
    }

    /// returns reference to thread
    std::thread &as_thread() noexcept {
      return _thread;
    }

    /// returns const reference to thread
    const std::thread &as_thread() const noexcept {
      return _thread;
    }
  };

} //namespace cppsl::threading

#endif /* DA7A4988_DEB1_4AA1_9C88_5F756805A1DB */
