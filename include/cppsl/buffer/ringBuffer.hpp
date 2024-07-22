//          Copyright Jean Pierre Cimalando 2018-2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// SPDX-License-Identifier: BSL-1.0

/**
 * @file
 * \brief  contains the ring buffer classes for use with C++17.
 *
 * ## Description
 *
 * The purpose of a ring buffer is to provide a unidirectional FIFO communication from a thread to another.
 * The implementation of this data structure can be lock-free, making it suitable for real-time programming.
 *
 * Ther are two variants of the ring buffer.
 *
 * - **Ring_Buffer** is a *bounded*, *lock-free* ring buffer. The capacity is fixed and determined at instantiation.
 *   It can only store messages up to the capacity of the buffer.
 *
 * - **Soft_Ring_Buffer** is an *unbounded*, *mostly lock-free* ring buffer.
 *   The storage expands as write operations require it, by a factor of 1.5. The access is protected by
 *   a shared-exclusive lock, and only blocks while the buffer expands. This object is adequate for soft real-time,
 *   when communicating all the messages is more important than missing a few deadlines.
 *
 * ## Programming interface
 *
 * `Ring_Buffer(size_t capacity);`
 *
 *    Instantiate the ring buffer with the given capacity.
 *
 * `size_t capacity() const;`
 *
 *    Return the capacity.
 *
 * ### Writer interface
 *
 * `size_t size_free() const;`
 *
 *    Return the number of bytes available to write to.
 *
 * `template <class T> bool put(const T &x);`
 *
 *    If the buffer has enough room (or the buffer is *unbounded*), store the `sizeof(T)` bytes of `x`,
 *    then return `true`. Otherwise, return `false`.
 *
 * `template <class T> bool put(const T *x, size_t n);`
 *
 *    Similar to `put` above, except it stores an array of `n` consecutive `T` elements.
 *
 * ### Reader interface
 *
 * `size_t size_used() const;`
 *
 *    Return the number of bytes available to read.
 *
 * `bool discard(size_t len);`
 *
 *    If the ringbuffer contains at least `len` bytes, extract them and ignore them, then return `true`.
 *    Otherwise, return `false`.
 *
 * `template <class T> bool get(T &x);`
 *
 *    If the buffer contains enough data, extract `sizeof(T)` bytes and assign this data to `x`,
 *    then return `true`. Otherwise, return `false`.
 *
 * `template <class T> bool get(T *x, size_t n);`
 *
 *    Similar to `get` above, except it extracts an array of `n` consecutive `T` elements.
 *
 * `template <class T> bool peek(T &x);`
 *
 *    If the buffer contains enough data, extract `sizeof(T)` bytes without removing them from storage,
 *    assign this data to `x`, then return `true`. Otherwise, return `false`.
 *
 * `template <class T> bool peek(T *x, size_t n);`
 *
 *    Similar to `peek` above, except it extracts an array of `n` consecutive `T` elements..
 *
 * @author Jean Pierre Cimalando
 * @date   2018-2022
 *****************************************************************************/

#ifndef INCLUDE_CPPSL_RING_BUFFER_RING_BUFFER_HPP
#define INCLUDE_CPPSL_RING_BUFFER_RING_BUFFER_HPP

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <shared_mutex>
#include <type_traits>

#if defined(__cpp_lib_atomic_is_always_lock_free)
static_assert(std::atomic<size_t>::is_always_lock_free, "atomic<size_t> must be lock free");
#endif

//------------------------------------------------------------------------------
template <bool>
class Ring_Buffer_Ex;
typedef Ring_Buffer_Ex<true> Ring_Buffer;

template <class>
class Soft_Ring_Buffer_Ex;
#if defined(__cpp_lib_shared_mutex)
typedef Soft_Ring_Buffer_Ex<std::shared_mutex> Soft_Ring_Buffer;
#else
typedef Soft_Ring_Buffer_Ex<std::shared_timed_mutex> Soft_Ring_Buffer;
#endif

//------------------------------------------------------------------------------
template <class RB>
class Basic_Ring_Buffer {
 public:
  // read operations
  template <class T>
  bool get(T& x);
  template <class T>
  bool get(T* x, size_t n);
  template <class T>
  bool peek(T& x);
  template <class T>
  bool peek(T* x, size_t n);
  // write operations
  template <class T>
  bool put(const T& x);
  template <class T>
  bool put(const T* x, size_t n);
};

//------------------------------------------------------------------------------
template <bool Atomic>
class Ring_Buffer_Ex final : private Basic_Ring_Buffer<Ring_Buffer_Ex<Atomic>> {
 private:
  typedef Basic_Ring_Buffer<Ring_Buffer_Ex<Atomic>> Base;

 public:
  // initialization and cleanup
  explicit Ring_Buffer_Ex(size_t capacity);
  ~Ring_Buffer_Ex();
  // attributes
  size_t capacity() const;
  // read operations
  size_t size_used() const;
  bool discard(size_t len);
  using Base::get;
  using Base::peek;
  // write operations
  size_t size_free() const;
  using Base::put;

  static constexpr bool can_extend() { return false; }

 private:
  size_t cap_{0};
  std::conditional_t<Atomic, std::atomic<size_t>, size_t> rp_{0}, wp_{0};
  std::unique_ptr<uint8_t[]> rbdata_{};
  friend Base;
  template <class>
  friend class Soft_Ring_Buffer_Ex;
  bool getbytes_(void* data, size_t len);
  bool peekbytes_(void* data, size_t len) const;
  bool getbytes_ex_(void* data, size_t len, bool advp);
  bool putbytes_(const void* data, size_t len);
};

//------------------------------------------------------------------------------
template <class Mutex>
class Soft_Ring_Buffer_Ex final : private Basic_Ring_Buffer<Soft_Ring_Buffer_Ex<Mutex>> {
 private:
  typedef Basic_Ring_Buffer<Soft_Ring_Buffer_Ex<Mutex>> Base;

 public:
  // initialization and cleanup
  explicit Soft_Ring_Buffer_Ex(size_t capacity);
  ~Soft_Ring_Buffer_Ex();
  // attributes
  size_t capacity() const;
  // read operations
  size_t size_used() const;
  bool discard(size_t len);
  using Base::get;
  using Base::peek;
  // write operations
  size_t size_free() const;
  using Base::put;

  static constexpr bool can_extend() { return true; }

 private:
  Ring_Buffer_Ex<false> rb_;
  mutable Mutex shmutex_;
  friend Base;
  bool getbytes_(void* data, size_t len);
  bool peekbytes_(void* data, size_t len) const;
  bool putbytes_(const void* data, size_t len);
  void grow_(size_t newcap);
};

//------------------------------------------------------------------------------
#include <cppsl/buffer/ringBuffer.tcc>

#endif /* INCLUDE_CPPSL_RING_BUFFER_RING_BUFFER_HPP */
