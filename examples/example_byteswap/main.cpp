//
// Author: Alexander Sacharov <a.sacharov@gmx.de>
// Year:   2022
//
// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>

#include <bit>
#include <cstdint>
#include <iostream>
#include <iomanip>

#include "cppsl/sys/byteswap.hpp"

template<typename T>
void dump(T v, char term = '\n') {
  // ensure we're only swapping integral types
  static_assert(std::is_integral<T>::value, "only integral types may be ");

  std::cout << std::hex << std::uppercase << std::setfill('0') << std::setw(sizeof(T) * 2) << v << " : ";
  for (std::size_t i{}; i != sizeof(T); ++i, v >>= 8) {
    std::cout << std::setw(2) << static_cast<unsigned>(T(0xFF) & v) << ' ';
  }
  std::cout << std::dec << term;
}

int main() {
  int sum=0;
  for (int i=0;i<2000000000;i++)sum+= cppsl::sys::ByteSwap<cppsl::endiannes::little_endian,cppsl::endiannes::big_endian>(i);
  return sum;
//
//  std::cout << "byteswap for U16:\n";
//  constexpr auto x = std::uint16_t(0xCAFE);
//  dump(x);
//  dump(std::byteswap(x));
//
//  std::cout << "\nbyteswap for U32:\n";
//  constexpr auto y = std::uint32_t(0xDEADBEEFu);
//  dump(y);
//  dump(std::byteswap(y));
//
//  std::cout << "\nbyteswap for U64:\n";
//  constexpr auto z = std::uint64_t{0x0123456789ABCDEFull};
//  dump(z);
//  dump(std::byteswap(z));

  return 0;
}
