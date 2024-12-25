#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <cppsl/byteSwap.hpp>

TEST_CASE("ByteSwapper SwapType::NX", "[ByteSwapper]") {
  uint16_t val = 0x1234;
  REQUIRE(cppsl::ByteSwapper::Swap(val, cppsl::ByteSwapper::SwapType::NX) == val);
}

TEST_CASE("ByteSwapper SwapType::AX", "[ByteSwapper]") {
  uint16_t val = 0x1234;
  uint16_t expected = 0x3412;
  REQUIRE(cppsl::ByteSwapper::Swap(val, cppsl::ByteSwapper::SwapType::AX) == expected);
}

TEST_CASE("ByteSwapper SwapType::BE", "[ByteSwapper]") {
  uint16_t val = 0x1234;
  uint16_t expected = 0x3412;
  if (std::endian::native == std::endian::big) {
    REQUIRE(cppsl::ByteSwapper::Swap(val, cppsl::ByteSwapper::SwapType::BE) == expected);
  } else {
    REQUIRE(cppsl::ByteSwapper::Swap(val, cppsl::ByteSwapper::SwapType::BE) == val);
  }
}

TEST_CASE("ByteSwapper SwapType::LE", "[ByteSwapper]") {
  uint16_t val = 0x1234;
  uint16_t expected = 0x3412;
  if (std::endian::native == std::endian::little) {
    REQUIRE(cppsl::ByteSwapper::Swap(val, cppsl::ByteSwapper::SwapType::LE) == expected);
  } else {
    REQUIRE(cppsl::ByteSwapper::Swap(val, cppsl::ByteSwapper::SwapType::LE) == val);
  }
}

TEST_CASE("ByteSwapper with different types", "[ByteSwapper]") {
  SECTION("uint32_t") {
    uint32_t val = 0x12345678;
    uint32_t expected = 0x78563412;
    REQUIRE(cppsl::ByteSwapper::Swap(val, cppsl::ByteSwapper::SwapType::AX) == expected);
  }

  SECTION("uint64_t") {
    uint64_t val = 0x123456789ABCDEF0;
    uint64_t expected = 0xF0DEBC9A78563412;
    REQUIRE(cppsl::ByteSwapper::Swap(val, cppsl::ByteSwapper::SwapType::AX) == expected);
  }

  SECTION("float") {
    float val = 1.23456789f;
    float swapped = cppsl::ByteSwapper::Swap(val, cppsl::ByteSwapper::SwapType::AX);
    REQUIRE(cppsl::ByteSwapper::Swap(swapped, cppsl::ByteSwapper::SwapType::AX) == val);
  }

  SECTION("double") {
    double val = 1.234567890123456789;
    double swapped = cppsl::ByteSwapper::Swap(val, cppsl::ByteSwapper::SwapType::AX);
    REQUIRE(cppsl::ByteSwapper::Swap(swapped, cppsl::ByteSwapper::SwapType::AX) == val);
  }
}