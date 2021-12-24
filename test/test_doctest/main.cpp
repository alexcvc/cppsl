#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <iostream>
#include <memory>

int fact(int n) {
   return n <= 1 ? n : fact(n - 1) * n;
}

TEST_CASE("testing the factorial function") {
   CHECK_FALSE(fact(0) == 1); // should fail
   CHECK(fact(1) == 1);
   CHECK(fact(2) == 2);
   CHECK(fact(3) == 6);
   CHECK(fact(10) == 3628800);
}

TEST_CASE("spdlog function") {
   // Console logger with color
   auto console = spdlog::stdout_color_mt("console");
   console->info("Welcome to spdlog!");
   console->error("Some error message with arg{}..", 1);

   // Formatting examples
   console->warn("Easy padding in numbers like {:08d}", 12);
   console->critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
   console->info("Support for floats {:03.2f}", 1.23456);
   console->info("Positional args are {1} {0}..", "too", "supported");
   console->info("{:<30}", "left aligned");

   CHECK(1);

   // Release and close all loggers
   spdlog::drop_all();
}

