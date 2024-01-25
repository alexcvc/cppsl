
#ifndef INCLUDE_CPPSL_BUFFER_CYCLIC_BUFFER_LOCK_FREE_HPP
#define INCLUDE_CPPSL_BUFFER_CYCLIC_BUFFER_LOCK_FREE_HPP

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace cppsl::buffer {

/**
 * \class CyclicBufferLockFree
 *
 * \brief A lock-free cyclic buffer implementation.
 *
 * This class provides a lock-free cyclic buffer, which allows non-blocking insertion and removal of elements.
 *
 * \tparam T The type of elements stored in the buffer.
 * \tparam BufferSize The maximum number of elements that can be stored in the buffer.
 * \tparam UseRelaxedMemOrder Flag indicating whether to use relaxed memory ordering for atomic operations.
 * \tparam IndexType The type used for buffer indexes.
 */
template <typename T, size_t BufferSize = 16, bool UseRelaxedMemOrder = false, typename IndexType = size_t>
class CyclicBufferLockFree {
 public:

  void clear() { mTailInd.store(mHeadInd.load(std::memory_order_relaxed), std::memory_order_relaxed); }

  bool check_empty() const { return available_to_read() == 0; }

  bool check_full() const { return available_to_write() == 0; }

  IndexType available_to_read() const {
    return ((mHeadInd.load(mMemOrderBarrierToEntry) + BufferSize) - mTailInd.load(std::memory_order_relaxed)) & mBufferSizeMask;
  }

  IndexType available_to_write() const {
    return BufferSize - available_to_read();
  }

  bool try_to_insert(T data) {
    if( available_to_write() == 0) {
      return false;
    } else {
      IndexType tmp_head = mHeadInd.load(std::memory_order_relaxed);

        mDataBuffer[tmp_head++ & mBufferSizeMask] = data;
        std::atomic_signal_fence(std::memory_order_release);
        mHeadInd.store(tmp_head, mMemOrderBarrierToRelease);

      return true;
    }
  }

  bool try_to_insert(const T* data) {
    IndexType tmp_head = mHeadInd.load(std::memory_order_relaxed);

    if ((tmp_head - mTailInd.load(mMemOrderBarrierToEntry)) == BufferSize) {
      return false;
    } else {
      mDataBuffer[tmp_head++ & mBufferSizeMask] = *data;
      std::atomic_signal_fence(std::memory_order_release);
      mHeadInd.store(tmp_head, mMemOrderBarrierToRelease);
    }
    return true;
  }

  bool try_to_upgrade_or_insert(const T* data, bool (*compare_func)(const T& a)) {
    IndexType tmp_head = mHeadInd.load(std::memory_order_relaxed);
    // first try to find in available to read

    // second try to insert
    return try_to_insert(data);
  }

  bool try_to_insert(T (*get_data_callback)() ) {
    IndexType tmp_head = mHeadInd.load(std::memory_order_relaxed);

    if ((tmp_head - mTailInd.load(mMemOrderBarrierToEntry)) == BufferSize) {
      return false;
    } else {
      //execute callback only when there is space in buffer
      mDataBuffer[tmp_head++ & mBufferSizeMask] = get_data_callback();
      std::atomic_signal_fence(std::memory_order_release);
      mHeadInd.store(tmp_head, mMemOrderBarrierToRelease);
    }
    return true;
  }

  /**
   * \brief Removes single element without reading
   * \return True if one element was removed
   */
  bool remove() {
    IndexType tmp_tail = mTailInd.load(std::memory_order_relaxed);

    if (tmp_tail == mHeadInd.load(std::memory_order_relaxed))
      return false;
    else
      mTailInd.store(++tmp_tail, mMemOrderBarrierToRelease);   // release in case data was loaded/used before

    return true;
  }

  /**
   * \brief Removes multiple elements without reading and storing it elsewhere
   * \param cnt Maximum number of elements to remove
   * \return Number of removed elements
   */
  size_t remove(size_t cnt) {
    IndexType tmp_tail = mTailInd.load(std::memory_order_relaxed);
    IndexType avail = mHeadInd.load(std::memory_order_relaxed) - tmp_tail;

    cnt = (cnt > avail) ? avail : cnt;

    mTailInd.store(tmp_tail + cnt, mMemOrderBarrierToRelease);
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
    IndexType tmp_tail = mTailInd.load(std::memory_order_relaxed);

    if (tmp_tail == mHeadInd.load(mMemOrderBarrierToEntry))
      return false;
    else {
      *data = mDataBuffer[tmp_tail++ & mBufferSizeMask];
      std::atomic_signal_fence(std::memory_order_release);
      mTailInd.store(tmp_tail, mMemOrderBarrierToRelease);
    }
    return true;
  }

  T* peek() {
    IndexType tmp_tail = mTailInd.load(std::memory_order_relaxed);

    if (tmp_tail == mHeadInd.load(mMemOrderBarrierToEntry))
      return nullptr;
    else
      return &mDataBuffer[tmp_tail & mBufferSizeMask];
  }

  T* at(size_t index) {
    IndexType tmp_tail = mTailInd.load(std::memory_order_relaxed);

    if ((mHeadInd.load(mMemOrderBarrierToEntry) - tmp_tail) <= index)
      return nullptr;
    else
      return &mDataBuffer[(tmp_tail + index) & mBufferSizeMask];
  }

  T& operator[](size_t index) {
    return mDataBuffer[(mTailInd.load(std::memory_order_relaxed) + index) & mBufferSizeMask];
  }

  size_t WriteToBuffer(const T* elements, size_t num_elements);

  size_t WriteToBuffer(const T* buff, size_t count, size_t count_to_callback, void (*execute_data_callback)(void));

  size_t readBuff(T* buff, size_t count);

  size_t readBuff(T* buff, size_t count, size_t count_to_callback, void (*execute_data_callback)(void));

 private:
  constexpr static IndexType mBufferSizeMask = BufferSize - 1;   ///< bitwise mask for a given buffer size
  constexpr static std::memory_order mMemOrderBarrierToEntry =
     UseRelaxedMemOrder
        ? std::memory_order_relaxed
        : std::memory_order_acquire;   ///< do not load from, or store to buffer before confirmed by the opposite side
  constexpr static std::memory_order mMemOrderBarrierToRelease =
     UseRelaxedMemOrder
        ? std::memory_order_relaxed
        : std::memory_order_release;   ///< do not update own side before all operations on mDataBuffer committed

  // Buffer indexes and self-buffering
  std::atomic<IndexType> mHeadInd{0};        ///< head index
  std::atomic<IndexType> mTailInd{0};        ///< mTailInd index
  std::array<T, BufferSize> mDataBuffer{};   ///< cyclic buffer

  // static asserts to make compiler happy
  static_assert((BufferSize != 0), "buffer cannot be of zero size");
  static_assert((BufferSize & mBufferSizeMask) == 0, "buffer size is not a power of 2");
  static_assert(sizeof(IndexType) <= sizeof(size_t),
                "indexing type size is larger than size_t, operation is not lock free and doesn't make sense");
  static_assert(std::numeric_limits<IndexType>::is_integer, "indexing type is not integral type");
  static_assert(!(std::numeric_limits<IndexType>::is_signed), "indexing type shall not be signed");
  static_assert(mBufferSizeMask <= ((std::numeric_limits<IndexType>::max)() >> 1),
                "buffer size is too large for a given indexing type (maximum size for n-bit type is 2^(n-1))");
};

template <typename T, size_t BufferSize, bool UseRelaxedMemOrder, typename IndexType>
size_t CyclicBufferLockFree<T, BufferSize, UseRelaxedMemOrder, IndexType>::WriteToBuffer(const T* elements,
                                                                                         size_t num_elements) {
  IndexType isAvailable = 0;
  IndexType tmp_head = mHeadInd.load(std::memory_order_relaxed);
  size_t to_write = num_elements;

  isAvailable = BufferSize - (tmp_head - mTailInd.load(mMemOrderBarrierToEntry));

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
  mHeadInd.store(tmp_head, mMemOrderBarrierToRelease);

  return to_write;
}

template <typename T, size_t BufferSize, bool UseRelaxedMemOrder, size_t align_size, typename IndexType>
size_t CyclicBufferLockFree<T, BufferSize, UseRelaxedMemOrder, align_size, IndexType>::WriteToBuffer(
   const T* buff, size_t count, size_t count_to_callback, void (*execute_data_callback)()) {
  size_t written = 0;
  IndexType available = 0;
  IndexType tmp_head = mHeadInd.load(std::memory_order_relaxed);
  size_t to_write = count;

  if (count_to_callback != 0 && count_to_callback < count)
    to_write = count_to_callback;

  while (written < count) {
    available = BufferSize - (tmp_head - mTailInd.load(mMemOrderBarrierToEntry));

    if (available == 0)   // less than ??
      break;

    if (to_write > available)   // do not write more than we can
      to_write = available;

    while (to_write--)
      mDataBuffer[tmp_head++ & mBufferSizeMask] = buff[written++];

    std::atomic_signal_fence(std::memory_order_release);
    mHeadInd.store(tmp_head, mMemOrderBarrierToRelease);

    if (execute_data_callback != nullptr)
      execute_data_callback();

    to_write = count - written;
  }

  return written;
}

template <typename T, size_t BufferSize, bool UseRelaxedMemOrder, size_t align_size, typename IndexType>
size_t CyclicBufferLockFree<T, BufferSize, UseRelaxedMemOrder, align_size, IndexType>::readBuff(T* buff, size_t count) {
  IndexType available = 0;
  IndexType tmp_tail = mTailInd.load(std::memory_order_relaxed);
  size_t to_read = count;

  available = mHeadInd.load(mMemOrderBarrierToEntry) - tmp_tail;

  if (available < count)   // do not read more than we can
    to_read = available;

  // maybe divide it into 2 separate reads
  for (size_t i = 0; i < to_read; i++)
    buff[i] = mDataBuffer[tmp_tail++ & mBufferSizeMask];

  std::atomic_signal_fence(std::memory_order_release);
  mTailInd.store(tmp_tail, mMemOrderBarrierToRelease);

  return to_read;
}

template <typename T, size_t BufferSize, bool UseRelaxedMemOrder, typename IndexType>
size_t CyclicBufferLockFree<T, BufferSize, UseRelaxedMemOrder, IndexType>::readBuff(T* buff, size_t count,
                                                                                    size_t count_to_callback,
                                                                                    void (*execute_data_callback)()) {
  size_t read = 0;
  IndexType available = 0;
  IndexType tmp_tail = mTailInd.load(std::memory_order_relaxed);
  size_t to_read = count;

  if (count_to_callback != 0 && count_to_callback < count)
    to_read = count_to_callback;

  while (read < count) {
    available = mHeadInd.load(mMemOrderBarrierToEntry) - tmp_tail;

    if (available == 0)   // less than ??
      break;

    if (to_read > available)   // do not write more than we can
      to_read = available;

    while (to_read--)
      buff[read++] = mDataBuffer[tmp_tail++ & mBufferSizeMask];

    std::atomic_signal_fence(std::memory_order_release);
    mTailInd.store(tmp_tail, mMemOrderBarrierToRelease);

    if (execute_data_callback != nullptr)
      execute_data_callback();

    to_read = count - read;
  }

  return read;
}

}   // namespace cppsl::buffer

#endif /* INCLUDE_CPPSL_BUFFER_CYCLIC_BUFFER_LOCK_FREE_HPP */
