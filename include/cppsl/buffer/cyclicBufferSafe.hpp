/*************************************************************************/ /**
 * @file    
 * @brief   contains Simple SPSC cycle buffer implementation .
 * @author  Jan Oleksiewicz <jnk0le@hotmail.com>
 * @date    22 Jun 2017
 * @license SPDX-License-Identifier: MIT
 * @ingroup 
 *****************************************************************************/

/**
 * # Ring Buffer
 * 
 * - pure C++11, no OS dependency
 * - no exceptions, RTTI, virtual functions and dynamic memory allocation
 * - designed for compile time (static) allocation and type evaluation
 * - no wasted slots (in powers of 2 granularity)
 * - lock and wait operation
 * - under-run and overrun checks in insert/remove functions
 * - highly efficient on most microcontroller architectures (nearly equal performance as in 'wasted-slot' implemetation)
 * 
 * ## notes
 * 
 * - index_t of size less than architecture reg size (size_t) might not be most efficient
 *   ([known gcc bug](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=71942))
 * - Only lamda expressions or functor callbacks can be inlined into `buffWrite`/`buffRead` functions
 * - 8 bit architectures are not supported in master branch at the moment. Broken code is likely to be generated
 * - relaxed atomic stores on RISC-V gcc port [may be inefficient](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=89835)
 * - the DEC Alpha ultra-weak memory model is not supported
 * 
 * ## example
 * 
 * ```
 * cppsl::buffer::CycleBuffer<const char*, 256> message;
 * 
 * int main()
 * {
 *   //...
 *   while(1)
 *   {
 *     const char* tmp = nullptr;
 *     while(!message.remove(tmp));
 *     printf("%s fired\n", tmp);
 *     //...
 *   }
 * }
 * 
 * extern "C" void SysTick_Handler(void)
 * {
 *   message.insert("SysTick_Handler");
 * }
 * 
 */

#ifndef INCLUDE_CPPSL_BUFFER_RINGBUFFER_HPP
#define INCLUDE_CPPSL_BUFFER_RINGBUFFER_HPP

#include <stddef.h>
#include <stdint.h>
#include <atomic>
#include <limits>

namespace cppsl::buffer {

/**
 * \class CycleBuffer
 * \brief A cyclic buffer implementation for storing elements of type T
 *
 * The CycleBuffer class provides a thread-safe cyclic buffer for storing elements of type T. The buffer has a fixed size
 * specified by the buffer_size template parameter. The buffer uses atomic operations to ensure thread safety and provides
 * methods for inserting and removing elements from the buffer.
 *
 * \tparam T The type of the elements stored in the buffer
 * \tparam buffer_size The size of the buffer (default: 16)
 * \tparam use_relaxed_mem_order A flag indicating whether to use a fake total store order (default: false)
 * \tparam align_size The size of the cache line (default: 0)
 * \tparam index_t The type used for indexing the buffer (default: size_t)
 */
template <typename T, size_t buffer_size = 16, bool use_relaxed_mem_order = false, size_t align_size = 0,
          typename index_t = size_t>
class Cycle_Buffer {
 public:
  /**
   * \brief Default constructor, will initialize head and tail indexes
   */
  Cycle_Buffer() : head(0), tail(0) {}

  /**
   * \brief Special case constructor to premature out unnecessary initialization code when object is
   * instantiated in .bss section
   * \warning If object is instantiated on stack, heap or inside section then the contents have to be
   * explicitly cleared before use
   * \param dummy Ignored
   */
  Cycle_Buffer(int dummy) { (void)(dummy); }

  /**
   * \brief Clear buffer from producer side
   * \warning function may return without performing any action if consumer tries to read data at the same time
   */
  void producerClear(void) {
    // head modification will lead to underflow if cleared during consumer read
    // doing this properly with CAS is not possible without modifying the consumer code
    consumerClear();
  }

  /**
   * \brief Clear buffer from consumer side
   */
  void consumerClear(void) { tail.store(head.load(std::memory_order_relaxed), std::memory_order_relaxed); }

  /**
   * \brief Check if buffer is empty
   * \return True if buffer is empty
   */
  bool isEmpty(void) const { return readAvailable() == 0; }

  /**
   * \brief Check if buffer is full
   * \return True if buffer is full
   */
  bool isFull(void) const { return writeAvailable() == 0; }

  /**
   * \brief Check how many elements can be read from the buffer
   * \return Number of elements that can be read
   */
  index_t readAvailable(void) const { return head.load(index_acquire_barrier) - tail.load(std::memory_order_relaxed); }

  /**
   * \brief Check how many elements can be written into the buffer
   * \return Number of free slots that can be be written
   */
  index_t writeAvailable(void) const {
    return buffer_size - (head.load(std::memory_order_relaxed) - tail.load(index_acquire_barrier));
  }

  /**
   * \brief Inserts data into internal buffer, without blocking
   * \param data element to be inserted into internal buffer
   * \return True if data was inserted
   */
  bool insert(T data) {
    index_t tmp_head = head.load(std::memory_order_relaxed);

    if ((tmp_head - tail.load(index_acquire_barrier)) == buffer_size)
      return false;
    else {
      data_buff[tmp_head++ & buffer_mask] = data;
      std::atomic_signal_fence(std::memory_order_release);
      head.store(tmp_head, index_release_barrier);
    }
    return true;
  }

  /**
   * \brief Inserts data into internal buffer, without blocking
   * \param[in] data Pointer to memory location where element, to be inserted into internal buffer, is located
   * \return True if data was inserted
   */
  bool insert(const T* data) {
    index_t tmp_head = head.load(std::memory_order_relaxed);

    if ((tmp_head - tail.load(index_acquire_barrier)) == buffer_size)
      return false;
    else {
      data_buff[tmp_head++ & buffer_mask] = *data;
      std::atomic_signal_fence(std::memory_order_release);
      head.store(tmp_head, index_release_barrier);
    }
    return true;
  }

  /**
   * \brief Inserts data returned by callback function, into internal buffer, without blocking
   *
   * This is a special purpose function that can be used to avoid redundant availability checks in case when
   * acquiring data have a side effects (like clearing status flags by reading a peripheral data register)
   *
   * \param get_data_callback Pointer to callback function that returns element to be inserted into buffer
   * \return True if data was inserted and callback called
   */
  bool insertFromCallbackWhenAvailable(T (*get_data_callback)(void)) {
    index_t tmp_head = head.load(std::memory_order_relaxed);

    if ((tmp_head - tail.load(index_acquire_barrier)) == buffer_size)
      return false;
    else {
      //execute callback only when there is space in buffer
      data_buff[tmp_head++ & buffer_mask] = get_data_callback();
      std::atomic_signal_fence(std::memory_order_release);
      head.store(tmp_head, index_release_barrier);
    }
    return true;
  }

  /**
   * \brief Removes single element without reading
   * \return True if one element was removed
   */
  bool remove() {
    index_t tmp_tail = tail.load(std::memory_order_relaxed);

    if (tmp_tail == head.load(std::memory_order_relaxed))
      return false;
    else
      tail.store(++tmp_tail, index_release_barrier);   // release in case data was loaded/used before

    return true;
  }

  /**
   * \brief Removes multiple elements without reading and storing it elsewhere
   * \param cnt Maximum number of elements to remove
   * \return Number of removed elements
   */
  size_t remove(size_t cnt) {
    index_t tmp_tail = tail.load(std::memory_order_relaxed);
    index_t avail = head.load(std::memory_order_relaxed) - tmp_tail;

    cnt = (cnt > avail) ? avail : cnt;

    tail.store(tmp_tail + cnt, index_release_barrier);
    return cnt;
  }

  /**
   * \brief Reads one element from internal buffer without blocking
   * \param[out] data Reference to memory location where removed element will be stored
   * \return True if data was fetched from the internal buffer
   */
  bool remove(T& data) {
    return remove(&data);   // references are anyway implemented as pointers
  }

  /**
   * \brief Reads one element from internal buffer without blocking
   * \param[out] data Pointer to memory location where removed element will be stored
   * \return True if data was fetched from the internal buffer
   */
  bool remove(T* data) {
    index_t tmp_tail = tail.load(std::memory_order_relaxed);

    if (tmp_tail == head.load(index_acquire_barrier))
      return false;
    else {
      *data = data_buff[tmp_tail++ & buffer_mask];
      std::atomic_signal_fence(std::memory_order_release);
      tail.store(tmp_tail, index_release_barrier);
    }
    return true;
  }

  /**
   * \brief Gets the first element in the buffer on consumed side
   *
   * It is safe to use and modify item contents only on consumer side
   *
   * \return Pointer to first element, nullptr if buffer was empty
   */
  T* peek() {
    index_t tmp_tail = tail.load(std::memory_order_relaxed);

    if (tmp_tail == head.load(index_acquire_barrier))
      return nullptr;
    else
      return &data_buff[tmp_tail & buffer_mask];
  }

  /**
   * \brief Gets the n'th element on consumed side
   *
   * It is safe to use and modify item contents only on consumer side
   *
   * \param index Item offset starting on the consumed side
   * \return Pointer to requested element, nullptr if index exceeds storage count
   */
  T* at(size_t index) {
    index_t tmp_tail = tail.load(std::memory_order_relaxed);

    if ((head.load(index_acquire_barrier) - tmp_tail) <= index)
      return nullptr;
    else
      return &data_buff[(tmp_tail + index) & buffer_mask];
  }

  /**
   * \brief Gets the n'th element on consumed side
   *
   * Unchecked operation, assumes that software already knows if the element can be used, if
   * requested index is out of bounds then reference will point to somewhere inside the buffer
   * The isEmpty() and readAvailable() will place appropriate memory barriers if used as loop limiter
   * It is safe to use and modify T contents only on consumer side
   *
   * \param index Item offset starting on the consumed side
   * \return Reference to requested element, undefined if index exceeds storage count
   */
  T& operator[](size_t index) { return data_buff[(tail.load(std::memory_order_relaxed) + index) & buffer_mask]; }

  /**
   * \brief Insert multiple elements into internal buffer without blocking
   *
   * This function will insert as much data as possible from given buffer.
   *
   * \param[in] buff Pointer to buffer with data to be inserted from
   * \param count Number of elements to write from the given buffer
   * \return Number of elements written into internal buffer
   */
  size_t writeBuff(const T* buff, size_t count);

  /**
   * \brief Insert multiple elements into internal buffer without blocking
   *
   * This function will continue writing new entries until all data is written or there is no more space.
   * The callback function can be used to indicate to consumer that it can start fetching data.
   *
   * \warning This function is not deterministic
   *
   * \param[in] buff Pointer to buffer with data to be inserted from
   * \param count Number of elements to write from the given buffer
   * \param count_to_callback Number of elements to write before calling a callback function in first loop
   * \param execute_data_callback Pointer to callback function executed after every loop iteration
   * \return Number of elements written into internal  buffer
   */
  size_t writeBuff(const T* buff, size_t count, size_t count_to_callback, void (*execute_data_callback)(void));

  /**
   * \brief Load multiple elements from internal buffer without blocking
   *
   * This function will read up to specified amount of data.
   *
   * \param[out] buff Pointer to buffer where data will be loaded into
   * \param count Number of elements to load into the given buffer
   * \return Number of elements that were read from internal buffer
   */
  size_t readBuff(T* buff, size_t count);

  /**
   * \brief Load multiple elements from internal buffer without blocking
   *
   * This function will continue reading new entries until all requested data is read or there is nothing
   * more to read.
   * The callback function can be used to indicate to producer that it can start writing new data.
   *
   * \warning This function is not deterministic
   *
   * \param[out] buff Pointer to buffer where data will be loaded into
   * \param count Number of elements to load into the given buffer
   * \param count_to_callback Number of elements to load before calling a callback function in first iteration
   * \param execute_data_callback Pointer to callback function executed after every loop iteration
   * \return Number of elements that were read from internal buffer
   */
  size_t readBuff(T* buff, size_t count, size_t count_to_callback, void (*execute_data_callback)(void));

 private:
  constexpr static index_t buffer_mask = buffer_size - 1;   //!< bitwise mask for a given buffer size
  constexpr static std::memory_order index_acquire_barrier =
     use_relaxed_mem_order
        ? std::memory_order_relaxed
        : std::memory_order_acquire;   // do not load from, or store to buffer before confirmed by the opposite side
  constexpr static std::memory_order index_release_barrier =
     use_relaxed_mem_order
        ? std::memory_order_relaxed
              : std::memory_order_release;   // do not update own side before all operations on data_buff committed

  alignas(align_size) std::atomic<index_t> head;   //!< head index
  alignas(align_size) std::atomic<index_t> tail;   //!< tail index

  // put buffer after variables so everything can be reached with short offsets
  alignas(align_size) T data_buff[buffer_size];   //!< actual buffer

  // let's assert that no UB will be compiled in
  static_assert((buffer_size != 0), "buffer cannot be of zero size");
  static_assert((buffer_size & buffer_mask) == 0, "buffer size is not a power of 2");
  static_assert(sizeof(index_t) <= sizeof(size_t),
                "indexing type size is larger than size_t, operation is not lock free and doesn't make sense");

  static_assert(std::numeric_limits<index_t>::is_integer, "indexing type is not integral type");
  static_assert(!(std::numeric_limits<index_t>::is_signed), "indexing type shall not be signed");
  static_assert(buffer_mask <= ((std::numeric_limits<index_t>::max)() >> 1),
                "buffer size is too large for a given indexing type (maximum size for n-bit type is 2^(n-1))");
};

/**
 * @brief Write data to the cyclic buffer.
 *
 * This function writes the provided data to the cyclic buffer.
 *
 * @tparam T The type of data stored in the buffer.
 * @tparam buffer_size The size of the buffer.
 * @tparam use_relaxed_mem_order Boolean value indicating whether use_relaxed_mem_order is enabled or not.
 * @tparam align_size The size of the alignment.
 * @tparam index_t The type of index used to access the buffer.
 *
 * @param buff A pointer to the data to be written.
 * @param count The number of elements to be written.
 *
 * @return The number of elements written to the buffer.
 */
template <typename T, size_t buffer_size, bool use_relaxed_mem_order, size_t align_size, typename index_t>
size_t Cycle_Buffer<T, buffer_size, use_relaxed_mem_order, align_size, index_t>::writeBuff(const T* buff, size_t count) {
  index_t available = 0;
  index_t tmp_head = head.load(std::memory_order_relaxed);
  size_t to_write = count;

  available = buffer_size - (tmp_head - tail.load(index_acquire_barrier));

  if (available < count)   // do not write more than we can
    to_write = available;

  // maybe divide it into 2 separate writes
  for (size_t i = 0; i < to_write; i++)
    data_buff[tmp_head++ & buffer_mask] = buff[i];

  std::atomic_signal_fence(std::memory_order_release);
  head.store(tmp_head, index_release_barrier);

  return to_write;
}

template <typename T, size_t buffer_size, bool use_relaxed_mem_order, size_t align_size, typename index_t>
size_t Cycle_Buffer<T, buffer_size, use_relaxed_mem_order, align_size, index_t>::writeBuff(const T* buff, size_t count,
                                                                                 size_t count_to_callback,
                                                                                 void (*execute_data_callback)()) {
  size_t written = 0;
  index_t available = 0;
  index_t tmp_head = head.load(std::memory_order_relaxed);
  size_t to_write = count;

  if (count_to_callback != 0 && count_to_callback < count)
    to_write = count_to_callback;

  while (written < count) {
    available = buffer_size - (tmp_head - tail.load(index_acquire_barrier));

    if (available == 0)   // less than ??
      break;

    if (to_write > available)   // do not write more than we can
      to_write = available;

    while (to_write--)
      data_buff[tmp_head++ & buffer_mask] = buff[written++];

    std::atomic_signal_fence(std::memory_order_release);
    head.store(tmp_head, index_release_barrier);

    if (execute_data_callback != nullptr)
      execute_data_callback();

    to_write = count - written;
  }

  return written;
}

template <typename T, size_t buffer_size, bool use_relaxed_mem_order, size_t align_size, typename index_t>
size_t Cycle_Buffer<T, buffer_size, use_relaxed_mem_order, align_size, index_t>::readBuff(T* buff, size_t count) {
  index_t available = 0;
  index_t tmp_tail = tail.load(std::memory_order_relaxed);
  size_t to_read = count;

  available = head.load(index_acquire_barrier) - tmp_tail;

  if (available < count)   // do not read more than we can
    to_read = available;

  // maybe divide it into 2 separate reads
  for (size_t i = 0; i < to_read; i++)
    buff[i] = data_buff[tmp_tail++ & buffer_mask];

  std::atomic_signal_fence(std::memory_order_release);
  tail.store(tmp_tail, index_release_barrier);

  return to_read;
}

template <typename T, size_t buffer_size, bool use_relaxed_mem_order, size_t align_size, typename index_t>
size_t Cycle_Buffer<T, buffer_size, use_relaxed_mem_order, align_size, index_t>::readBuff(T* buff, size_t count,
                                                                                size_t count_to_callback,
                                                                                void (*execute_data_callback)()) {
  size_t read = 0;
  index_t available = 0;
  index_t tmp_tail = tail.load(std::memory_order_relaxed);
  size_t to_read = count;

  if (count_to_callback != 0 && count_to_callback < count)
    to_read = count_to_callback;

  while (read < count) {
    available = head.load(index_acquire_barrier) - tmp_tail;

    if (available == 0)   // less than ??
      break;

    if (to_read > available)   // do not write more than we can
      to_read = available;

    while (to_read--)
      buff[read++] = data_buff[tmp_tail++ & buffer_mask];

    std::atomic_signal_fence(std::memory_order_release);
    tail.store(tmp_tail, index_release_barrier);

    if (execute_data_callback != nullptr)
      execute_data_callback();

    to_read = count - read;
  }

  return read;
}

}   // namespace cppsl::buffer

#endif /* INCLUDE_CPPSL_BUFFER_RINGBUFFER_HPP */
