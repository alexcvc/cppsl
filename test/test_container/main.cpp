#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "cppsl/container/dequeSafe.hpp"
#include "cppsl/container/listSafe.hpp"
#include "cppsl/container/queueLockFree.hpp"
#include "cppsl/container/queueSafe.hpp"

TEST_CASE("DequeSafe with int", "[DequeSafe]") {
  cppsl::container::DequeSafe<int> deque;

  REQUIRE(deque.empty());

  deque.push_back(1);
  deque.push_back(2);
  deque.push_back(3);

  REQUIRE(deque.size() == 3);
  REQUIRE(!deque.empty());

  int item;
  deque.wait_and_pop_front(item);
  REQUIRE(item == 1);
  deque.wait_and_pop_front(item);
  REQUIRE(item == 2);
  deque.wait_and_pop_front(item);
  REQUIRE(item == 3);
  REQUIRE(deque.empty());
}

TEST_CASE("DequeSafe with std::string", "[DequeSafe]") {
  cppsl::container::DequeSafe<std::string> deque;

  REQUIRE(deque.empty());

  deque.push_back("one");
  deque.push_back("two");
  deque.push_back("three");

  REQUIRE(deque.size() == 3);
  REQUIRE(!deque.empty());

  std::string item;
  deque.wait_and_pop_front(item);
  REQUIRE(item == "one");
  deque.wait_and_pop_front(item);
  REQUIRE(item == "two");
  deque.wait_and_pop_front(item);
  REQUIRE(item == "three");
  REQUIRE(deque.empty());
}

TEST_CASE("DequeSafe with custom struct", "[DequeSafe]") {
  struct CustomStruct {
    int id;
    std::string name;
  };

  cppsl::container::DequeSafe<CustomStruct> deque;

  REQUIRE(deque.empty());

  deque.push_back({1, "Alice"});
  deque.push_back({2, "Bob"});
  deque.push_back({3, "Charlie"});

  REQUIRE(deque.size() == 3);
  REQUIRE(!deque.empty());

  CustomStruct item;
  deque.wait_and_pop_front(item);
  REQUIRE(item.id == 1);
  REQUIRE(item.name == "Alice");
  deque.wait_and_pop_front(item);
  REQUIRE(item.id == 2);
  REQUIRE(item.name == "Bob");
  deque.wait_and_pop_front(item);
  REQUIRE(item.id == 3);
  REQUIRE(item.name == "Charlie");
  REQUIRE(deque.empty());
}

TEST_CASE("ListSafe with int", "[ListSafe]") {
  cppsl::container::ListSafe<int> list;

  REQUIRE(list.find_first_if([](int) { return true; }) == nullptr);

  list.push_front(1);
  list.push_front(2);
  list.push_front(3);

  REQUIRE(list.find_first_if([](int value) { return value == 1; }) != nullptr);
  REQUIRE(*list.find_first_if([](int value) { return value == 1; }) == 1);
  REQUIRE(*list.find_first_if([](int value) { return value == 2; }) == 2);
  REQUIRE(*list.find_first_if([](int value) { return value == 3; }) == 3);

  list.remove_if([](int value) { return value == 2; });
  REQUIRE(list.find_first_if([](int value) { return value == 2; }) == nullptr);
}

TEST_CASE("ListSafe with std::string", "[ListSafe]") {
  cppsl::container::ListSafe<std::string> list;

  REQUIRE(list.find_first_if([](const std::string&) { return true; }) == nullptr);

  list.push_front("one");
  list.push_front("two");
  list.push_front("three");

  REQUIRE(list.find_first_if([](const std::string& value) { return value == "one"; }) != nullptr);
  REQUIRE(*list.find_first_if([](const std::string& value) { return value == "one"; }) == "one");
  REQUIRE(*list.find_first_if([](const std::string& value) { return value == "two"; }) == "two");
  REQUIRE(*list.find_first_if([](const std::string& value) { return value == "three"; }) == "three");

  list.remove_if([](const std::string& value) { return value == "two"; });
  REQUIRE(list.find_first_if([](const std::string& value) { return value == "two"; }) == nullptr);
}

TEST_CASE("ListSafe with custom struct", "[ListSafe]") {
  struct CustomStruct {
    int id;
    std::string name;
  };

  cppsl::container::ListSafe<CustomStruct> list;

  REQUIRE(list.find_first_if([](const CustomStruct&) { return true; }) == nullptr);

  list.push_front({1, "Alice"});
  list.push_front({2, "Bob"});
  list.push_front({3, "Charlie"});

  REQUIRE(list.find_first_if([](const CustomStruct& value) { return value.id == 1; }) != nullptr);
  REQUIRE(list.find_first_if([](const CustomStruct& value) { return value.id == 2; }) != nullptr);
  REQUIRE(list.find_first_if([](const CustomStruct& value) { return value.id == 3; }) != nullptr);

  list.remove_if([](const CustomStruct& value) { return value.id == 2; });
  REQUIRE(list.find_first_if([](const CustomStruct& value) { return value.id == 2; }) == nullptr);
}

TEST_CASE("QueueLockFree with int", "[QueueLockFree]") {
  cppsl::container::QueueLockFree<int> queue;

  REQUIRE(queue.try_pop() == nullptr);

  queue.push(1);
  queue.push(2);
  queue.push(3);

  REQUIRE(*queue.pop() == 1);
  REQUIRE(*queue.pop() == 2);
  REQUIRE(*queue.pop() == 3);
  REQUIRE(queue.pop() == nullptr);
}

TEST_CASE("QueueLockFree with std::string", "[QueueLockFree]") {
  cppsl::container::QueueLockFree<std::string> queue;

  REQUIRE(queue.try_pop() == nullptr);

  queue.push("one");
  queue.push("two");
  queue.push("three");

  REQUIRE(*queue.pop() == "one");
  REQUIRE(*queue.pop() == "two");
  REQUIRE(*queue.pop() == "three");
  REQUIRE(queue.pop() == nullptr);
}

TEST_CASE("QueueLockFree with custom struct", "[QueueLockFree]") {
  struct CustomStruct {
    int id;
    std::string name;
  };

  cppsl::container::QueueLockFree<CustomStruct> queue;

  REQUIRE(queue.try_pop() == nullptr);

  queue.push({1, "Alice"});
  queue.push({2, "Bob"});
  queue.push({3, "Charlie"});

  auto ret = queue.pop();
  REQUIRE(ret != nullptr);
  REQUIRE(ret->id == 1);
  REQUIRE(ret->name == "Alice");

  ret = queue.pop();
  REQUIRE(ret != nullptr);
  REQUIRE(ret->id == 2);
  REQUIRE(ret->name == "Bob");

  ret = queue.pop();
  REQUIRE(ret != nullptr);
  REQUIRE(ret->id == 3);
  REQUIRE(ret->name == "Charlie");
}
