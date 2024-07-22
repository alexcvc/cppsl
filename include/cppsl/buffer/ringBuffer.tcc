//          Copyright Jean Pierre Cimalando 2018-2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// SPDX-License-Identifier: BSL-1.0

#ifndef INCLUDE_CPPSL_RING_BUFFER_RING_BUFFER_TCC
#define INCLUDE_CPPSL_RING_BUFFER_RING_BUFFER_TCC

#include <cppsl/buffer/ringBuffer.hpp>

/**
 * @class Ring_Buffer_Ex
 * @brief A ring buffer implementation with extended functionalities.
 *
 * This class provides a template implementation of a ring buffer
 * with additional functionalities. It stores elements of type uint8_t
 * in a circular buffer. The capacity of the buffer is equal to cap_ - 1.
 * The size of the buffer is determined by the number of elements
 * that have been written to the buffer.
 *
 * The buffer supports atomic access if the template parameter Atomic is set to true.
 *
 */
template <bool Atomic>
inline size_t Ring_Buffer_Ex<Atomic>::capacity() const {
  return cap_ - 1;
}

/**
 * @class Soft_Ring_Buffer_Ex
 * @brief A soft ring buffer with extended functionality.
 *
 * This class implements a soft ring buffer with additional operations for reading and writing data.
 *
 * @tparam Mutex The mutex type to use for thread safety.
 */
//------------------------------------------------------------------------------
template <class Mutex>
inline Soft_Ring_Buffer_Ex<Mutex>::Soft_Ring_Buffer_Ex(size_t capacity) : rb_(capacity) {}

/**
 * @class Soft_Ring_Buffer_Ex
 * @brief Extended soft ring buffer implementation
 *
 * This class provides an extended implementation of a soft ring buffer.
 * It is templated on a Mutex type for synchronization between multiple threads.
 *
 * @tparam Mutex The type of mutex used for synchronization
 */
template <class Mutex>
inline Soft_Ring_Buffer_Ex<Mutex>::~Soft_Ring_Buffer_Ex() {}

/**
 * @brief Soft_Ring_Buffer_Ex class represents a soft ring buffer with extended functionality.
 *
 * This class provides a soft ring buffer implementation with extended functionality such as
 * capacity retrieval.
 *
 * @tparam Mutex The mutex type used for thread safety.
 */
template <class Mutex>
inline size_t Soft_Ring_Buffer_Ex<Mutex>::capacity() const {
  return rb_.capacity();
}

/**
 * @brief Read a single element from the ring buffer.
 *
 * This function reads a single element from the ring buffer and stores it in the
 * given variable.
 *
 * @tparam T The type of the element to be read.
 * @param x The variable where the read element will be stored.
 * @return True if an element was successfully read from the ring buffer, false otherwise.
 */
//------------------------------------------------------------------------------
template <class RB>
template <class T>
inline bool Basic_Ring_Buffer<RB>::get(T& x) {
  return get(&x, 1);
}

/******************************************************************************
* \brief Get elements from the ring buffer.
*
* This function retrieves a specified number of elements from the ring buffer.
*
* \tparam RB The ring buffer class.
* \tparam T The type of elements to be retrieved.
*
* \param x Pointer to the memory location to store the retrieved elements.
* \param n The number of elements to retrieve.
*
* \return True if the elements were successfully retrieved, false otherwise.
*
* \remarks The type T must be trivially copyable.
*****************************************************************************/
template <class RB>
template <class T>
inline bool Basic_Ring_Buffer<RB>::get(T* x, size_t n) {
  static_assert(std::is_trivially_copyable<T>::value, "ring_buffer: T must be trivially copyable");
  RB* self = static_cast<RB*>(this);
  return self->getbytes_(x, n * sizeof(T));
}

/**
 * @brief Peek at the next element in the ring buffer without removing it.
 *
 * This function allows you to examine the next element in the ring buffer
 * without removing it from the buffer. The element is copied into the provided
 * variable. If the buffer is empty, the function will return false.
 *
 * @tparam RB The type of the ring buffer.
 * @tparam T The type of the element.
 * @param x The variable to store the next element.
 * @return True if successfully peeked at the next element, false otherwise.
 */
template <class RB>
template <class T>
inline bool Basic_Ring_Buffer<RB>::peek(T& x) {
  return peek(&x, 1);
}

/**
 * @class Basic_Ring_Buffer
 * @brief A template class that represents a basic ring buffer.
 *
 * This class provides read and write operations to a ring buffer.
 *
 * @tparam RB The type of the ring buffer.
 */
template <class RB>
template <class T>
inline bool Basic_Ring_Buffer<RB>::peek(T* x, size_t n) {
  static_assert(std::is_trivially_copyable<T>::value, "ring_buffer: T must be trivially copyable");
  RB* self = static_cast<RB*>(this);
  return self->peekbytes_(x, n * sizeof(T));
}

/**
 * @class Basic_Ring_Buffer
 * @brief A template class that represents a basic ring buffer.
 *
 * This class provides read and write operations for a ring buffer.
 *
 * The ring buffer can store elements of any type, using the template parameter T.
 * It has methods for reading elements from the buffer, writing elements to the buffer,
 * and peeking at elements without removing them from the buffer.
 *
 * @tparam RB The underlying data structure for the ring buffer.
 */
template <class RB>
template <class T>
inline bool Basic_Ring_Buffer<RB>::put(const T& x) {
  return put(&x, 1);
}

/***************************************************************
 * \class Basic_Ring_Buffer
 *
 * \brief The Basic_Ring_Buffer class provides a basic ring buffer implementation.
 *
 * The ring buffer allows for fast input and output operations, and supports
 * both reading and writing of elements.
 *
 * \tparam RB  The derived ring buffer class.
 ****************************************************************/
template <class RB>
template <class T>
inline bool Basic_Ring_Buffer<RB>::put(const T* x, size_t n) {
  static_assert(std::is_trivially_copyable<T>::value, "ring_buffer: T must be trivially copyable");
  RB* self = static_cast<RB*>(this);
  return self->putbytes_(x, n * sizeof(T));
}

#endif /* INCLUDE_CPPSL_RING_BUFFER_RING_BUFFER_TCC */
