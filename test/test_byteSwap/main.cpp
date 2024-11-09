#define CATCH_CONFIG_MAIN

#include <iostream>
#include <thread>

#include "catch2/catch.hpp"
#include "cppsl/byteSwap.hpp"

using namespace std;
using namespace cppsl;

TEST_CASE("BytesSwapTest, SwapNever") {
  uint16_t val = 0x1234;
  uint16_t swapped = ByteSwapper::Swap(val, ByteSwapper::SwapType::NX);
  CHECKED_IF(swapped == val);
}

TEST_CASE("BytesSwapTest, SwapAlways") {
  uint16_t val = 0x1234;
  uint16_t expectedSwapped = 0x3412;
  uint16_t swapped = ByteSwapper::Swap(val, ByteSwapper::SwapType::AX);
  CHECKED_IF(swapped == expectedSwapped);
}

TEST_CASE("BytesSwapTest, SwapBigEndian") {
  uint32_t val = 0x12345678;
  uint32_t expectedSwapped = 0x78563412;
  uint32_t swapped = ByteSwapper::Swap(val, ByteSwapper::SwapType::BE);

  if (std::endian::native == std::endian::big) {
    CHECKED_IF(swapped == expectedSwapped);
  } else {
    CHECKED_IF(swapped == val);
  }
}

TEST_CASE("BytesSwapTest, SwapLittleEndian") {
  uint32_t val = 0x12345678;
  uint32_t expectedSwapped = 0x78563412;
  uint32_t swapped = ByteSwapper::Swap(val, ByteSwapper::SwapType::LE);

  if (std::endian::native == std::endian::little) {
    CHECKED_IF(swapped == expectedSwapped);
  } else {
    CHECKED_IF(swapped == val);
  }
}