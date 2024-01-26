/* SPDX-License-Identifier: MIT */
//
// Copyright (c) 2024 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************/ /**
 * \file
 * \brief  contains cyclic buffer template lock free thread-safe.
 * \author A.Sacharov <a.sacharov@gmx.de> 26.01.24
 * \date    2024-01-26
 * \ingroup C++ support library
 *****************************************************************************/

/**
 * @details
 *
 * Buffer:
 *
 * 1) Elements placed one after the other as one piece : Last Index > First Index
 *     First Element                   Last Element
 *          |--------------------------------|
 * Cyclic Buffer
 * |-------------------------------------------------------|
 *
 * 2) Elements placed as two pieces : Last Index < First Index
 *      Last Element                   First Element
 *  --------|                                |-------------
 * Cyclic Buffer
 * |-------------------------------------------------------|
 *
 * 3) No elements placed : Last Index == First Index
 *      Last Element == First Element
 *                    |
 * Cyclic Buffer
 * |-------------------------------------------------------|
 *
 * 4) Added one element: Last Index++ % BufferSize
 *                    |-|
 * Cyclic Buffer
 * |-------------------------------------------------------|
 *
 * 4) Added one element: Last Index++ % BufferSize
 * |-|
 * Cyclic Buffer
 * |-------------------------------------------------------|
 *
 * Index Rules:
 * 1) Number elements = (Last Index + BufferSize - First Index) & BufferSizeMask  or: & BufferSize
 * 2) If Last Index == First Index and First Index == BufferSize - 1 than set Last Index = 0 and First Index = 0
 *
 */

#ifndef INCLUDE_CPPSL_BUFFER_CYCLIC_BUFFER_LOCK_FREE_HPP
#define INCLUDE_CPPSL_BUFFER_CYCLIC_BUFFER_LOCK_FREE_HPP

#include <array>
#include <atomic>
#include <cstddef>   // For std::ptrdiff_t
#include <cstddef>
#include <cstdint>
#include <iterator>   // For std::forward_iterator_tag
#include <limits>

namespace cppsl::buffer {

template <typename _Tp, std::size_t _Nm>
struct __cyclic_buffer_lock_free_traits {
  typedef _Tp _Type[_Nm];
  static constexpr _Tp& _S_ref(const _Type& __t, std::size_t __n) noexcept { return const_cast<_Tp&>(__t[__n]); }
  static constexpr _Tp* _S_ptr(const _Type& __t) noexcept { return const_cast<_Tp*>(__t); }
};

template <typename _Tp>
struct __cyclic_buffer_lock_free_traits<_Tp, 0> {
  struct _Type {};
  static constexpr _Tp& _S_ref(const _Type&, std::size_t) noexcept { return *static_cast<_Tp*>(nullptr); }
  static constexpr _Tp* _S_ptr(const _Type&) noexcept { return nullptr; }
};

/**
 * \class cyclic_buffer_lock_free
 *
 * \brief A lock-free cyclic buffer implementation.
 *
 * This class provides a lock-free cyclic buffer, which allows non-blocking insertion and removal of elements.
 *
 * \tparam _Type The type of elements stored in the buffer.
 * \tparam _BufferSize The maximum number of elements that can be stored in the buffer.
 * \tparam UseRelaxedMemOrder Flag indicating whether to use relaxed memory ordering for atomic operations.
 * \tparam IndexType The type used for buffer indexes.
 */
template <typename _Type, size_t _BufferSize = 16, bool _UseRelaxed = false, typename IndexType = size_t>
class cyclic_buffer_lock_free {
 public:
    struct Iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = _Type;
        using pointer           = value_type*;
        using reference         = value_type&;

        Iterator(pointer ptr) : m_ptr(ptr) {}

        reference operator*() const { return *m_ptr; }
        pointer operator->() { return m_ptr; }
        Iterator& operator++() { m_ptr++; return *this; }
        Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
        friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
        friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };

    private:
        pointer m_ptr;
    };

  // types
  using value_type = _Type;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = value_type*;
  using const_iterator = const value_type*;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  void fill(const value_type& __u) { std::fill_n(begin(), size(), __u); }

  void swap(cyclic_buffer_lock_free& __other) noexcept { std::swap_ranges(begin(), end(), __other.begin()); }

  // Iterators.
  [[nodiscard]] iterator begin() noexcept { return iterator(data()); }

  [[nodiscard]] const_iterator begin() const noexcept { return const_iterator(data()); }

  [[nodiscard]] iterator end() noexcept { return static_cast<iterator>(mDataBuffer.end()); }

  [[nodiscard]] const_iterator end() const noexcept { return static_cast<const_iterator>(mDataBuffer.end()); }

  [[nodiscard]] const_iterator cbegin() const noexcept {

    return static_cast<const_iterator>(mDataBuffer.end());
  }

  [[nodiscard]] const_iterator cend() const noexcept { return static_cast<const_iterator>(mDataBuffer.end()); }

  // Capacity.
  [[nodiscard]] constexpr size_type size() const noexcept { return available_to_read(); }

  [[nodiscard]] constexpr size_type max_size() const noexcept { return _BufferSize - size(); }

  [[nodiscard]] constexpr bool empty() const noexcept { return size() == 0; }

  //  // Element access.
  //  [[nodiscard]] reference operator[](size_type __n) noexcept { return _AT_Type::_S_ref(_M_elems, __n); }
  //
  //  [[nodiscard]] constexpr const_reference operator[](size_type __n) const noexcept {
  //    return _AT_Type::_S_ref(_M_elems, __n);
  //  }
  //
  //  reference at(size_type __n) {
  //    if (__n >= _Nm)
  //      std::__throw_out_of_range_fmt(__N("array::at: __n (which is %zu) "
  //                                        ">= _Nm (which is %zu)"),
  //                                    __n, _Nm);
  //    return _AT_Type::_S_ref(_M_elems, __n);
  //  }
  //
  //  constexpr const_reference at(size_type __n) const {
  //    // Result of conditional expression must be an lvalue so use
  //    // boolean ? lvalue : (throw-expr, lvalue)
  //    return __n < _Nm ? _AT_Type::_S_ref(_M_elems, __n)
  //                     : (std::__throw_out_of_range_fmt(__N("array::at: __n (which is %zu) "
  //                                                          ">= _Nm (which is %zu)"),
  //                                                      __n, _Nm),
  //                        _AT_Type::_S_ref(_M_elems, 0));
  //  }
  //

  [[nodiscard]] reference front() noexcept { return *begin(); }

  //  [[nodiscard]] constexpr const_reference front() const noexcept { return _AT_Type::_S_ref(_M_elems, 0); }
  //
  //  [[nodiscard]] reference back() noexcept {
  //    __glibcxx_requires_nonempty();
  //    return _Nm ? *(end() - 1) : *end();
  //  }
  //
  //  [[nodiscard]] constexpr const_reference back() const noexcept {
  //    return _Nm ? _AT_Type::_S_ref(_M_elems, _Nm - 1) : _AT_Type::_S_ref(_M_elems, 0);
  //  }

  [[nodiscard]] pointer data() noexcept {
    if (empty())
      return end();
    else
      return &at(mFirstIndex.load(std::memory_order_relaxed));
  }

  [[nodiscard]] const_iterator data() const noexcept {
    if (empty())
      return end();
    else
      return &at(mFirstIndex.load(std::memory_order_relaxed));
  }

  void clear() {
    if( IndexType last = mLastIndex.load(std::memory_order_relaxed); last == _BufferSize - 1 ) {
     mLastIndex.store( 0, std::memory_order_relaxed);
    }
    mFirstIndex.store( mLastIndex.load(std::memory_order_relaxed), std::memory_order_relaxed);
  }

  IndexType available_to_read() const {
    return ((mLastIndex.load(mMemOrderBarrierToEntry) + _BufferSize) - mFirstIndex.load(std::memory_order_relaxed)) &
           mBufferSizeMask;
  }

  IndexType available_to_write() const { return _BufferSize - available_to_read(); }

  bool try_to_insert(_Type data) {
    if (available_to_write() == 0) {
      return false;
    } else {
      IndexType tmp_head = mLastIndex.load(std::memory_order_relaxed);

      mDataBuffer[tmp_head++ & mBufferSizeMask] = data;
      std::atomic_signal_fence(std::memory_order_release);
      mLastIndex.store(tmp_head, mMemOrderBarrierToRelease);

      return true;
    }
  }

  bool try_to_insert(const _Type* data) {
    IndexType tmp_head = mLastIndex.load(std::memory_order_relaxed);

    if ((tmp_head - mFirstIndex.load(mMemOrderBarrierToEntry)) == _BufferSize) {
      return false;
    } else {
      mDataBuffer[tmp_head++ & mBufferSizeMask] = *data;
      std::atomic_signal_fence(std::memory_order_release);
      mLastIndex.store(tmp_head, mMemOrderBarrierToRelease);
    }
    return true;
  }

  bool find(const _Type* data, bool (*compare_func)(const _Type& a)) {
    IndexType tmp_head = mLastIndex.load(std::memory_order_relaxed);
    // first try to find in available to read

    // second try to insert
    return try_to_insert(data);
  }

  bool try_to_upgrade_or_insert(const _Type* data, bool (*compare_func)(const _Type& a)) {
    IndexType tmp_head = mLastIndex.load(std::memory_order_relaxed);
    // first try to find in available to read

    // second try to insert
    return try_to_insert(data);
  }

  bool try_to_insert(_Type (*get_data_callback)()) {
    IndexType tmp_head = mLastIndex.load(std::memory_order_relaxed);

    if ((tmp_head - mFirstIndex.load(mMemOrderBarrierToEntry)) == _BufferSize) {
      return false;
    } else {
      //execute callback only when there is space in buffer
      mDataBuffer[tmp_head++ & mBufferSizeMask] = get_data_callback();
      std::atomic_signal_fence(std::memory_order_release);
      mLastIndex.store(tmp_head, mMemOrderBarrierToRelease);
    }
    return true;
  }

  /**
   * \brief Removes single element without reading
   * \return True if one element was removed
   */
  bool remove() {
    IndexType tmp_tail = mFirstIndex.load(std::memory_order_relaxed);

    if (tmp_tail == mLastIndex.load(std::memory_order_relaxed))
      return false;
    else
      mFirstIndex.store(++tmp_tail, mMemOrderBarrierToRelease);   // release in case data was loaded/used before

    return true;
  }

  /**
   * \brief Removes multiple elements without reading and storing it elsewhere
   * \param cnt Maximum number of elements to remove
   * \return Number of removed elements
   */
  size_t remove(size_t cnt) {
    IndexType tmp_tail = mFirstIndex.load(std::memory_order_relaxed);
    IndexType avail = mLastIndex.load(std::memory_order_relaxed) - tmp_tail;

    cnt = (cnt > avail) ? avail : cnt;

    mFirstIndex.store(tmp_tail + cnt, mMemOrderBarrierToRelease);
    return cnt;
  }

  /**
   * \brief Reads one element from internal buffer without blocking
   * \param[out] data Reference to memory location where removed element will be stored
   * \return True if data was fetched from the internal buffer
   */
  bool remove(_Type& data) {
    return remove(&data);   // references are anyway implemented as pointers
  }

  /**
   * \brief Reads one element from internal buffer without blocking
   * \param[out] data Pointer to memory location where removed element will be stored
   * \return True if data was fetched from the internal buffer
   */
  bool remove(_Type* data) {
    IndexType tmp_tail = mFirstIndex.load(std::memory_order_relaxed);

    if (tmp_tail == mLastIndex.load(mMemOrderBarrierToEntry))
      return false;
    else {
      *data = mDataBuffer[tmp_tail++ & mBufferSizeMask];
      std::atomic_signal_fence(std::memory_order_release);
      mFirstIndex.store(tmp_tail, mMemOrderBarrierToRelease);
    }
    return true;
  }

  _Type* peek() {
    IndexType tmp_tail = mFirstIndex.load(std::memory_order_relaxed);

    if (tmp_tail == mLastIndex.load(mMemOrderBarrierToEntry))
      return nullptr;
    else
      return &mDataBuffer[tmp_tail & mBufferSizeMask];
  }

  _Type* at(size_t index) {
    IndexType tmp_tail = mFirstIndex.load(std::memory_order_relaxed);

    if ((mLastIndex.load(mMemOrderBarrierToEntry) - tmp_tail) <= index)
      return nullptr;
    else
      return &mDataBuffer[(tmp_tail + index) & mBufferSizeMask];
  }

  _Type& operator[](size_t index) {
    return mDataBuffer[(mFirstIndex.load(std::memory_order_relaxed) + index) & mBufferSizeMask];
  }

  size_t WriteToBuffer(const _Type* elements, size_t num_elements);

  size_t WriteToBuffer(const _Type* buff, size_t count, size_t count_to_callback, void (*execute_data_callback)(void));

  size_t readBuff(_Type* buff, size_t count);

  size_t readBuff(_Type* buff, size_t count, size_t count_to_callback, void (*execute_data_callback)(void));

 private:
  constexpr static IndexType mBufferSizeMask = _BufferSize - 1;   ///< bitwise mask for a given buffer size
  constexpr static std::memory_order mMemOrderBarrierToEntry =
     _UseRelaxed
        ? std::memory_order_relaxed
        : std::memory_order_acquire;   ///< do not load from, or store to buffer before confirmed by the opposite side
  constexpr static std::memory_order mMemOrderBarrierToRelease =
     _UseRelaxed
        ? std::memory_order_relaxed
        : std::memory_order_release;   ///< do not update own side before all operations on mDataBuffer committed

  // Buffer indexes and self-buffering
  std::atomic<IndexType> mLastIndex{0};           ///< head index
  std::atomic<IndexType> mFirstIndex{0};          ///< first element index
  std::array<_Type, _BufferSize> mDataBuffer{};   ///< cyclic buffer

  // static asserts to make compiler happy
  static_assert((_BufferSize != 0), "buffer cannot be of zero size");
  static_assert((_BufferSize & mBufferSizeMask) == 0, "buffer size is not a power of 2");
  static_assert(sizeof(IndexType) <= sizeof(size_t),
                "indexing type size is larger than size_t, operation is not lock free and doesn't make sense");
  static_assert(std::numeric_limits<IndexType>::is_integer, "indexing type is not integral type");
  static_assert(!(std::numeric_limits<IndexType>::is_signed), "indexing type shall not be signed");
  static_assert(mBufferSizeMask <= ((std::numeric_limits<IndexType>::max)() >> 1),
                "buffer size is too large for a given indexing type (maximum size for n-bit type is 2^(n-1))");
};

template <typename _Type, size_t _BufferSize, bool _UseRelaxed, typename IndexType>
size_t cyclic_buffer_lock_free<_Type, _BufferSize, _UseRelaxed, IndexType>::WriteToBuffer(const _Type* elements,
                                                                                       size_t num_elements) {
  IndexType isAvailable = 0;
  IndexType tmp_head = mLastIndex.load(std::memory_order_relaxed);
  size_t to_write = num_elements;

  isAvailable = _BufferSize - (tmp_head - mFirstIndex.load(mMemOrderBarrierToEntry));

  if (isAvailable < num_elements) {
    // do not write more than we can
    to_write = isAvailable;
  }

  // maybe divide it into 2 separate writes
  for (size_t i = 0; i < to_write; i++) {
    mDataBuffer[tmp_head++ & mBufferSizeMask] = elements[i];
  }

  // store
  std::atomic_signal_fence(std::memory_order_release);
  mLastIndex.store(tmp_head, mMemOrderBarrierToRelease);

  return to_write;
}

template <typename _Type, size_t _BufferSize, bool _UseRelaxed, size_t align_size, typename IndexType>
size_t cyclic_buffer_lock_free<_Type, _BufferSize, _UseRelaxed, align_size, IndexType>::WriteToBuffer(
   const _Type* buff, size_t count, size_t count_to_callback, void (*execute_data_callback)()) {
  size_t written = 0;
  IndexType available = 0;
  IndexType tmp_head = mLastIndex.load(std::memory_order_relaxed);
  size_t to_write = count;

  if (count_to_callback != 0 && count_to_callback < count)
    to_write = count_to_callback;

  while (written < count) {
    available = _BufferSize - (tmp_head - mFirstIndex.load(mMemOrderBarrierToEntry));

    if (available == 0)   // less than ??
      break;

    if (to_write > available)   // do not write more than we can
      to_write = available;

    while (to_write--)
      mDataBuffer[tmp_head++ & mBufferSizeMask] = buff[written++];

    std::atomic_signal_fence(std::memory_order_release);
    mLastIndex.store(tmp_head, mMemOrderBarrierToRelease);

    if (execute_data_callback != nullptr)
      execute_data_callback();

    to_write = count - written;
  }

  return written;
}

template <typename _Type, size_t _BufferSize, bool _UseRelaxed, size_t align_size, typename IndexType>
size_t cyclic_buffer_lock_free<_Type, _BufferSize, _UseRelaxed, align_size, IndexType>::readBuff(_Type* buff,
                                                                                              size_t count) {
  IndexType available = 0;
  IndexType tmp_tail = mFirstIndex.load(std::memory_order_relaxed);
  size_t to_read = count;

  available = mLastIndex.load(mMemOrderBarrierToEntry) - tmp_tail;

  if (available < count)   // do not read more than we can
    to_read = available;

  // maybe divide it into 2 separate reads
  for (size_t i = 0; i < to_read; i++)
    buff[i] = mDataBuffer[tmp_tail++ & mBufferSizeMask];

  std::atomic_signal_fence(std::memory_order_release);
  mFirstIndex.store(tmp_tail, mMemOrderBarrierToRelease);

  return to_read;
}

template <typename _Type, size_t _BufferSize, bool _UseRelaxed, typename IndexType>
size_t cyclic_buffer_lock_free<_Type, _BufferSize, _UseRelaxed, IndexType>::readBuff(_Type* buff, size_t count,
                                                                                  size_t count_to_callback,
                                                                                  void (*execute_data_callback)()) {
  size_t read = 0;
  IndexType available = 0;
  IndexType tmp_tail = mFirstIndex.load(std::memory_order_relaxed);
  size_t to_read = count;

  if (count_to_callback != 0 && count_to_callback < count)
    to_read = count_to_callback;

  while (read < count) {
    available = mLastIndex.load(mMemOrderBarrierToEntry) - tmp_tail;

    if (available == 0)   // less than ??
      break;

    if (to_read > available)   // do not write more than we can
      to_read = available;

    while (to_read--)
      buff[read++] = mDataBuffer[tmp_tail++ & mBufferSizeMask];

    std::atomic_signal_fence(std::memory_order_release);
    mFirstIndex.store(tmp_tail, mMemOrderBarrierToRelease);

    if (execute_data_callback != nullptr)
      execute_data_callback();

    to_read = count - read;
  }

  return read;
}

}   // namespace cppsl::buffer

#endif /* INCLUDE_CPPSL_BUFFER_CYCLIC_BUFFER_LOCK_FREE_HPP */
