/* SPDX-License-Identifier: MIT */
//
// Copyright (c) 2024 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

#pragma once

#include <atomic>
#include <vector>

namespace cppsl::container {

template <typename T>
/**
 * @brief A thread-safe circular buffer implementation.
 *
 * The CircularBuffer class provides a fixed-size circular buffer with thread-safe push and pop operations. The buffer supports a single producer and a single consumer thread.
 *
 * @tparam T The type of items stored in the buffer.
 *
 * @ctor
 * CircularBuffer(size_t size)
 * Constructs a CircularBuffer object with the specified size.
 *
 * @param size The size of the circular buffer. Must be a power of 2.
 * @throws std::invalid_argument if the size is not a power of 2.
 */
class CircularBuffer {
  const size_t m_capacity;            ///< The size of the circular buffer.
  std::vector<T> m_buffer;            ///< The buffer storing the items.
  std::atomic<size_t> m_readIndex;    ///< The read index for the circular buffer.
  std::atomic<size_t> m_writeIndex;   ///< The write index for the circular buffer.

 public:
  /**
   * @brief Constructor: Ensures the size is a power of 2 and initializes the buffer and indices.
   * @param size The size of the circular buffer. Must be a power of 2.
   * @throws std::invalid_argument if the size is not a power of 2.
   * @return An instance of CircularBuffer.
   */
  explicit CircularBuffer(size_t size = 16)
      : m_capacity(nextPowerOf2(size)), m_buffer(m_capacity), m_readIndex(0), m_writeIndex(0) {
    if ((m_capacity & (m_capacity - 1)) != 0) {
      throw std::invalid_argument("Size must be a power of 2");
    }
  }

  /**
   * @brief Adds an item to the buffer.
   * @tparam T The type of items stored in the buffer.
   * @param item The item to be added to the circular buffer.
   * @return true if the item was successfully added to the buffer, false if the buffer is full.
   */
  [[nodiscard]] bool push(const T& item) {
    auto currentWrite = m_writeIndex.load(std::memory_order_relaxed);
    auto nextWrite = increment(currentWrite);

    if (nextWrite == m_readIndex.load(std::memory_order_acquire)) {
      // Buffer is full
      return false;
    }

    m_buffer[currentWrite] = item;
    m_writeIndex.store(nextWrite, std::memory_order_release);

    return true;
  }

  /**
   * @brief Removes the next item from the circular buffer.
   * @tparam T The type of items stored in the buffer.
   * @param item A reference to the variable where the popped item will be stored.
   * @return true if an item was successfully popped from the buffer, false if the buffer is empty.
   */
  [[nodiscard]] bool pop(T& item) {
    auto currentRead = m_readIndex.load(std::memory_order_relaxed);

    if (currentRead == m_writeIndex.load(std::memory_order_acquire)) {
      // Buffer is empty
      return false;
    }

    item = m_buffer[currentRead];
    m_readIndex.store(increment(currentRead), std::memory_order_release);

    return true;
  }

  /**
   * @brief Clears the circular buffer by resetting the read and write indices to 0.
   */
  void clear() {
    m_readIndex.store(0, std::memory_order_release);
    m_writeIndex.store(0, std::memory_order_release);
  }

  /**
   * @brief Checks if the circular buffer is empty.
   * @return true if the circular buffer is empty, false otherwise.
   */
  [[nodiscard]] bool empty() const {
    return m_readIndex.load(std::memory_order_acquire) == m_writeIndex.load(std::memory_order_acquire);
  }

  /**
   * @brief Checks if the circular buffer is full.
   * @tparam T The type of items stored in the buffer.
   * @return true if the circular buffer is full, false otherwise.
   */
  [[nodiscard]] bool full() const {
    return increment(m_writeIndex.load(std::memory_order_acquire)) == m_readIndex.load(std::memory_order_acquire);
  }

  /**
   * @brief Returns the capacity of the circular buffer.
   * @return The capacity of the circular buffer.
   */
  [[nodiscard]] size_t capacity() const { return m_capacity; }

  /**
   * @brief Returns the number of items currently in the circular buffer.
   * @return The number of items currently in the circular buffer.
   */
  [[nodiscard]] size_t size() const {
    auto currentWrite = m_writeIndex.load(std::memory_order_acquire);
    auto currentRead = m_readIndex.load(std::memory_order_acquire);
    return (currentWrite - currentRead + m_capacity) & (m_capacity - 1);
  }

 private:
  /**
   * @brief Determines the next power of 2 for a given number.
   * @param num The number to find the next power of 2 for.
   * @return The next power of 2 for the given number.
   */
  static size_t nextPowerOf2(size_t num) {
    if (num == 0)
      return 1;
    --num;
    num |= num >> 1;
    num |= num >> 2;
    num |= num >> 4;
    num |= num >> 8;
    num |= num >> 16;
#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
    num |= num >> 32;
#endif
    return num + 1;
  }

  /**
   * @brief Increments the given index by 1 and applies bitwise AND operation with size_ - 1.
   * @param index The index to be incremented.
   * @return The incremented index within the circular buffer.
   */
  [[nodiscard]] size_t increment(size_t index) const { return (index + 1) & (m_capacity - 1); }
};

}   // namespace cppsl::container
