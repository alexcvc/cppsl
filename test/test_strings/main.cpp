#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <iostream>
#include <cstring>
#include <compare>
#include <doctest/doctest.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using namespace std;

char* copyString(const char* str)
{
   //char* result{ new char[strlen(str)] };  // BUG! Off by one!
   char* result{ new char[strlen(str) + 1] };
   strcpy(result, str);
   return result;
}

char* appendStrings(const char* str1, const char* str2, const char* str3)
{
   char* result{ new char[strlen(str1) + strlen(str2) + strlen(str3) + 1] };
   strcpy(result, str1);
   strcat(result, str2);
   strcat(result, str3);
   return result;
}

// ///////////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("testing raw string literals") {
   // Console logger with color
   auto console = spdlog::stdout_color_mt("console");

   const char* str2{ "Hello \"World\"!" };
   console->info("{}",str2);

   const char* str3{ R"(Hello "World"!)" };
   console->info("{}",str3);

   const char* str4{ "Line 1\nLine 2" };
   console->info("{}",str4);

   const char* str5{ R"(Line 1
-----------------------
text\n\n\n\n\n\n\n\n\n
***********************
Line 2)" };
   console->info("{}",str5);

   const char* str6{ R"(Is the following a tab character? \t)" };
   console->info("{}",str6);

//   const char* str7{ R"(Embedded )" characters)" };    // Error!
   const char* str8{ R"-(Embedded )" characters)-" };
   console->info("{}",str8);

   // Release and close all loggers
   spdlog::drop_all();

}

// ///////////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("C++ string comparing") {
   {
      string a{ "12" };
      string b{ "34" };
      string c;
      c = a + b;    // c is "1234"
      cout << c << endl;

      a += b;    // a is "1234"
      cout << a << endl;

      CHECK( a == "1234" );
   }

   {
      string a{ "12" };
      string b{ "34" };

      auto result{ a.compare(b) };
      if (result < 0) { cout << "less" << endl; }
      if (result > 0) { cout << "greater" << endl; }
      if (result == 0) { cout << "equal" << endl; }
   }


   {
      // C++20 three-way comparison operator
      string a{ "12" };
      string b{ "34" };
      auto result{ a <=> b };
      if (is_lt(result)) { cout << "less" << endl; }
      if (is_gt(result)) { cout << "greater" << endl; }
      if (is_eq(result)) { cout << "equal" << endl; }
   }

   {
      string myString{ "hello" };

      myString += ", there";

      string myOtherString{ myString };

      if (myString == myOtherString) {
         myOtherString[0] = 'H';
      }

      cout << myString << endl;
      cout << myOtherString << endl;
   }

   {
      string strHello{ "Hello!!" };
      string strWorld{ "The World..." };
      auto position{ strHello.find("!!") };
      if (position != string::npos) {
         // Found the "!!" substring, now replace it.
         strHello.replace(position, 2, strWorld.substr(3, 6));
      }
      cout << strHello << endl;
   }

   {
      auto string1{ "Hello World" };    // string1 is a const char*.
      auto string2{ "Hello World"s };   // string2 is an std::string.
   }
}

// ///////////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Test stoi") {
   const string toParse{ "   123USD" };
   size_t index{ 0 };
   int value{ stoi(toParse, &index) };
   cout << fmt::format("Parsed value: {}", value) << endl;
   cout << fmt::format("First non-parsed character: '{}'", toParse[index]) << endl;

}
