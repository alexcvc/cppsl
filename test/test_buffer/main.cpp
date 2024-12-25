#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <cppsl/container/circularBuffer.hpp>

TEST_CASE("CircularBufferLF with int", "[CircularBufferLF]") {
  cppsl::container::CircularBufferLF<int> buffer(8);

  REQUIRE(buffer.empty());
  REQUIRE(buffer.capacity() == 8);

  buffer.push(1);
  buffer.push(2);
  buffer.push(3);

  REQUIRE(buffer.size() == 3);
  REQUIRE(!buffer.empty());

  int item;
  REQUIRE(buffer.pop(item));
  REQUIRE(item == 1);
  REQUIRE(buffer.pop(item));
  REQUIRE(item == 2);
  REQUIRE(buffer.pop(item));
  REQUIRE(item == 3);
  REQUIRE(buffer.empty());
}

TEST_CASE("CircularBufferLF with std::string", "[CircularBufferLF]") {
  cppsl::container::CircularBufferLF<std::string> buffer(4);

  REQUIRE(buffer.empty());
  REQUIRE(buffer.capacity() == 4);

  buffer.push("one");
  buffer.push("two");
  buffer.push("three");

  REQUIRE(buffer.size() == 3);
  REQUIRE(!buffer.empty());

  std::string item;
  REQUIRE(buffer.pop(item));
  REQUIRE(item == "one");
  REQUIRE(buffer.pop(item));
  REQUIRE(item == "two");
  REQUIRE(buffer.pop(item));
  REQUIRE(item == "three");
  REQUIRE(buffer.empty());
}

TEST_CASE("CircularBufferLF with custom struct", "[CircularBufferLF]") {
  struct CustomStruct {
    int id;
    std::string name;
  };

  cppsl::container::CircularBufferLF<CustomStruct> buffer(4);

  REQUIRE(buffer.empty());
  REQUIRE(buffer.capacity() == 4);

  buffer.push({1, "Alice"});
  buffer.push({2, "Bob"});
  buffer.push({3, "Charlie"});
  buffer.push({4, "Sahra"});

  REQUIRE(buffer.size() == 3);
  REQUIRE(!buffer.empty());

  CustomStruct item;
  REQUIRE(buffer.pop(item));
  REQUIRE(item.id == 1);
  REQUIRE(item.name == "Alice");
  REQUIRE(buffer.pop(item));
  REQUIRE(item.id == 2);
  REQUIRE(item.name == "Bob");
  REQUIRE(buffer.pop(item));
  REQUIRE(item.id == 3);
  REQUIRE(item.name == "Charlie");
  REQUIRE(buffer.empty());
}

TEST_CASE("CircularBufferLF with custom struct preemptive", "[CircularBufferLFPreemptive]") {
  struct CustomStruct {
    int id;
    std::string name;
  };

  cppsl::container::CircularBufferLF<CustomStruct> buffer(4);

  REQUIRE(buffer.empty());
  REQUIRE(buffer.capacity() == 4);

  buffer.push({1, "Alice"});
  buffer.push({2, "Bob"});
  buffer.push({3, "Charlie"});
  if (!buffer.push({4, "Sahra"})) {
    CustomStruct ret;
    buffer.pop(ret);
    buffer.push({4, "Sahra"});
  }

  REQUIRE(buffer.size() == 3);
  REQUIRE(!buffer.empty());

  CustomStruct item;
  REQUIRE(buffer.pop(item));
  REQUIRE(item.id == 2);
  REQUIRE(item.name == "Bob");
  REQUIRE(buffer.pop(item));
  REQUIRE(item.id == 3);
  REQUIRE(item.name == "Charlie");
  REQUIRE(buffer.pop(item));
  REQUIRE(item.id == 4);
  REQUIRE(item.name == "Sahra");
  REQUIRE(buffer.empty());
}