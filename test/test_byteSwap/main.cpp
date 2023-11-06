#include <iomanip>
#include "cppsl/byteSwap.hpp"
#include "gtest/gtest.h"

/*************************************************************************/ /**
 * dump
 * @tparam T
 * @param v
 * @param term
 */
template <typename T>
void dump(T v, char term = '\n') {
  // ensure we're only swapping integral types
  static_assert(std::is_arithmetic<T>::value, "only arithmetic types may be dumped");

  auto* bytePtr = (uint8_t*)&v;

  std::cout << std::hex << std::uppercase << std::setfill('0') << std::setw(sizeof(T) * 2) << v << " : ";

  for (std::size_t i{}; i != sizeof(T); ++i, ++bytePtr) {
    std::cout << std::setw(2) << static_cast<unsigned>(*bytePtr) << ' ';
  }

  std::cout << std::dec << term;
}

TEST(ByteSwap, TestSwapU8) {
  std::cout << "ByteSwap for U8:\n";
  uint8_t inp1 = 0xAB;
  uint8_t res1{};

  dump(inp1);
  res1 = cppsl::ByteSwap<std::endian::little, std::endian::big>(inp1);
  dump(res1);
  EXPECT_EQ(inp1, res1);

  res1 = cppsl::ByteSwap<std::endian::big, std::endian::little>(inp1);
  dump(res1);
  EXPECT_EQ(inp1, res1);

  res1 = cppsl::ByteSwap(inp1);
  dump(cppsl::ByteSwap(inp1));
  EXPECT_EQ(inp1, res1);
}

TEST(ByteSwap, TestSwapU16) {
  std::cout << "\nByteSwap for U16:\n";
  auto inp2 = std::uint16_t{0x0123};
  uint16_t res2;

  dump(inp2);

  res2 = cppsl::ByteSwap<std::endian::little, std::endian::big>(inp2);
  dump(res2);
  EXPECT_NE(inp2, res2);
  EXPECT_EQ(res2, 0x2301);

  res2 = cppsl::ByteSwap<std::endian::big, std::endian::little>(inp2);
  dump(res2);
  EXPECT_NE(inp2, res2);
  EXPECT_EQ(res2, 0x2301);

  res2 = cppsl::ByteSwap(inp2);
  dump(res2);
  EXPECT_NE(inp2, res2);
  EXPECT_EQ(res2, 0x2301);
}

TEST(ByteSwap, TestSwapU32) {
  std::cout << "\nByteSwap for U32:\n";
  auto inp4 = std::uint32_t{0x01234567};
  uint32_t res4;

  dump(inp4);

  res4 = cppsl::ByteSwap<std::endian::little, std::endian::big>(inp4);
  dump(res4);
  EXPECT_NE(inp4, res4);
  EXPECT_EQ(res4, 0x67452301);

  res4 = cppsl::ByteSwap<std::endian::big, std::endian::little>(inp4);
  dump(res4);
  EXPECT_NE(inp4, res4);
  EXPECT_EQ(res4, 0x67452301);

  res4 = cppsl::ByteSwap(inp4);
  dump(res4);
  EXPECT_NE(inp4, res4);
  EXPECT_EQ(res4, 0x67452301);
}

TEST(ByteSwap, TestSwapU64) {
  std::cout << "\nByteSwap for U64:\n";
  auto inp8 = std::uint64_t{0x0123456789ABCDEFull};
  uint64_t res8;

  dump(inp8);

  res8 = cppsl::ByteSwap<std::endian::little, std::endian::big>(inp8);
  dump(res8);
  EXPECT_NE(inp8, res8);
  EXPECT_EQ(res8, 0xEFCDAB8967452301);

  res8 = cppsl::ByteSwap<std::endian::little, std::endian::native>(inp8);
  dump(res8);
  EXPECT_EQ(inp8, res8);

  res8 = cppsl::ByteSwap(inp8);
  dump(res8);
  EXPECT_NE(inp8, res8);
  EXPECT_EQ(res8, 0xEFCDAB8967452301);
}

TEST(ByteSwap, TestSwappingFloatDouble) {

  std::cout << "\nByteSwap for float:\n";
  constexpr auto f = float{0.1234567890};
  float fr;

  dump(f);

  fr = cppsl::ByteSwap<std::endian::little, std::endian::big>(f);
  dump(fr);
  EXPECT_NE(f, fr);

  std::cout << "\nByteSwap for double:\n";
  constexpr auto d = double{0.1234567890123456789};
  double dr;

  dump(d);

  dr = cppsl::ByteSwap<std::endian::little, std::endian::big>(d);
  dump(dr);
  EXPECT_NE(d, dr);

  std::cout << "\nByteSwap to host byte ordering:\n";
  dr = cppsl::ByteSwap<std::endian::little, std::endian::native>(d);
  dump(dr);
  EXPECT_EQ(d, dr);

  std::cout << "\nByteSwap always:\n";
  dr = cppsl::ByteSwap(d);
  dump(dr);
  EXPECT_NE(d, dr);
}
