//
// Author: Alexander Sacharov <a.sacharov@gmx.de>
// Year:   2022
//
// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>

#include <bit>
#include <iostream>
#include <iomanip>
#include <cppsl/sys/endian.h>

using namespace cppsl::sys;
namespace bytes = cppsl::sys;

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
  std::cout << "byteswap for U8:\n";
  uint8_t i = 0xAB;
  dump(i);
  dump(cppsl::sys::byteswap<cppsl::endiannes::little_endian, cppsl::endiannes::big_endian>(i));
  dump(cppsl::sys::byteswap(i));

  std::cout << "\nbyteswap for U16:\n";
  auto x = std::uint16_t{0x0123};
  dump(x);
  dump(cppsl::sys::byteswap<cppsl::endiannes::little_endian, cppsl::endiannes::big_endian>(x));
  dump(cppsl::sys::byteswap(x));

  std::cout << "\nbyteswap for U32:\n";
  auto j = std::uint32_t{0x01234567u};
  dump(j);
  dump(cppsl::sys::byteswap<cppsl::endiannes::little_endian, cppsl::endiannes::big_endian>(j));
  dump(cppsl::sys::byteswap(j));

  std::cout << "\nbyteswap for U64:\n";
  constexpr auto z = std::uint64_t{0x0123456789ABCDEFull};
  dump(z);
  dump(cppsl::sys::byteswap<cppsl::endiannes::little_endian, cppsl::endiannes::big_endian>(z));
  dump(cppsl::sys::byteswap(z));

  std::cout << "\nbyteswap for float:\n";
  constexpr auto f = float{0.1234567890};
  dump(f);
  dump(cppsl::sys::byteswap<cppsl::endiannes::little_endian, cppsl::endiannes::big_endian>(f));
  dump(cppsl::sys::byteswap(f));

  std::cout << "\nbyteswap for double:\n";
  constexpr auto d = double{0.1234567890123456789};
  dump(d);
  dump(bytes::byteswap<bytes::ByteOrder::little_endian, bytes::ByteOrder::big_endian>(d));

  std::cout << "\nbyteswap to host byte ordering:\n";
  dump(bytes::byteswap<bytes::ByteOrder::little_endian, bytes::ByteOrder::host_endian>(d));

  std::cout << "\nbyteswap to network byte ordering:\n";
  dump(bytes::byteswap<bytes::ByteOrder::little_endian, bytes::ByteOrder::network_endian>(d));

  std::cout << "\nbyteswap always:\n";
  dump(bytes::byteswap(d));

  return (0);
}
