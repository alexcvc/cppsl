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
 * \file    JThread.hpp
 * \brief   contains joinable thread class (wrapper).
 * \author  Alexander Sacharov
 * \date    2022-01-13
 * \ingroup Threading
 *****************************************************************************/

#ifndef __INCLUDE_CPPSL_THREADING_JTHREAD_H__
#define __INCLUDE_CPPSL_THREADING_JTHREAD_H__

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
  class JThread {
    std::thread m_thread;  /// member thread

   public:

    /// default ctor
    JThread() noexcept = default;

    /// explicit universal ctor
    template<typename Callable, typename ... Args>
    explicit JThread(Callable &&func, Args &&... args):
       m_thread(std::forward<Callable>(func), std::forward<Args>(args)...) {}

    /// ctor with thread move
    explicit JThread(std::thread t_) noexcept:
       m_thread(std::move(t_)) {}

    /// copy ctor
    JThread(JThread &&other) noexcept:
       m_thread(std::move(other.m_thread)) {}

    /// assignment
    JThread &operator=(JThread &&other) noexcept {
      if (joinable())
        join();
      m_thread = std::move(other.m_thread);
      return *this;
    }

    /// assignment std::thread
    JThread &operator=(std::thread other) noexcept {
      if (joinable())
        join();
      m_thread = std::move(other);
      return *this;
    }

    /// destructor
    ~JThread() noexcept {
      if (joinable())
        join();
    }

    /// swap (move) other jthread
    void swap(JThread &other) noexcept {
      m_thread.swap(other.m_thread);
    }

    /// get thread id
    std::thread::id get_id() const noexcept {
      return m_thread.get_id();
    }

    /// is joinable
    bool joinable() const noexcept {
      return m_thread.joinable();
    }

    /// join thread
    void join() {
      m_thread.join();
    }

    /// detach thread
    void detach() {
      m_thread.detach();
    }

    /// returns reference to thread
    std::thread &as_thread() noexcept {
      return m_thread;
    }

    /// returns const reference to thread
    const std::thread &as_thread() const noexcept {
      return m_thread;
    }
  };

} //namespace cppsl::threading

#endif /* __INCLUDE_CPPSL_THREADING_JTHREAD_H__ */
