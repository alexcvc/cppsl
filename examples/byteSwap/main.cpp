//
// Author: Alexander Sacharov <a.sacharov@gmx.de>
// Year:   2022
//
// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>

#include <bit>
#include <iostream>
#include <iomanip>
#include <cppsl/byteSwap.hpp>

using namespace cppsl;

/*************************************************************************//**
 * dump
 * @tparam T
 * @param v
 * @param term
 */
template<typename T>
void dump(T v, char term = '\n') {
  // ensure we're only swapping integral types
  static_assert(std::is_arithmetic<T>::value, "only arithmetic types may be dumped");

  auto *bytePtr = (uint8_t *)&v;

  std::cout << std::hex << std::uppercase << std::setfill('0') << std::setw(sizeof(T) * 2) << v << " : ";

  for (std::size_t i{}; i != sizeof(T); ++i, ++bytePtr) {
    std::cout << std::setw(2) << static_cast<unsigned>(*bytePtr) << ' ';
  }

  std::cout << std::dec << term;
}

/*************************************************************************//**
 * main
 * @return
 */
int main() {
  std::cout << "ByteSwapper for U8:\n";
  uint8_t i = 0xAB;
  dump(i);
  dump(cppsl::ByteSwapper::Swap<std::endian::little, std::endian::big>(i));
  dump(cppsl::ByteSwapper(i));

  std::cout << "\nByteSwapper for U16:\n";
  auto x = std::uint16_t{0x0123};
  dump(x);
  dump(cppsl::ByteSwapper<std::endian::little, std::endian::big>(x));
  dump(cppsl::ByteSwapper(x));

  std::cout << "\nByteSwapper for U32:\n";
  auto j = std::uint32_t{0x01234567u};
  dump(j);
  dump(cppsl::ByteSwapper<std::endian::little, std::endian::big>(j));
  dump(cppsl::ByteSwapper(j));

  std::cout << "\nByteSwapper for U64:\n";
  constexpr auto z = std::uint64_t{0x0123456789ABCDEFull};
  dump(z);
  dump(cppsl::ByteSwapper<std::endian::little, std::endian::big>(z));
  dump(cppsl::ByteSwapper(z));

  std::cout << "\nByteSwapper for float:\n";
  constexpr auto f = float{0.1234567890};
  dump(f);
  dump(cppsl::ByteSwapper<std::endian::little, std::endian::big>(f));
  dump(cppsl::ByteSwapper(f));

  std::cout << "\nByteSwapper for double:\n";
  constexpr auto d = double{0.1234567890123456789};
  dump(d);
  dump(cppsl::ByteSwapper<std::endian::little, std::endian::big>(d));

  std::cout << "\nByteSwapper to host byte ordering:\n";
  dump(cppsl::ByteSwapper<std::endian::little, std::endian::native>(d));

  std::cout << "\nByteSwapper always:\n";
  dump(cppsl::ByteSwapper(d));

  return (0);
}
