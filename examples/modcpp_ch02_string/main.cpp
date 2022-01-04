#include <cstring>
#include <iostream>

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

void code01_strcpy() {
   const char* str1{ "Hello" };
   char* copy{ copyString(str1) };
   cout << copy << endl;
   delete [] copy;
   copy = nullptr;

   const char* str2{ " World" };
   const char* str3{ "!" };
   char* result{ appendStrings(str1, str2, str3) };
   cout << result << endl;
   delete [] result;
   result = nullptr;
}

void code01_strlen()
{
   char text1[]{ "abcdef" };
   size_t s1{ sizeof(text1) };  // is 7
   size_t s2{ strlen(text1) };  // is 6
   cout << s1 << endl << s2 << endl;

   const char* text2{ "abcdef" };
   size_t s3{ sizeof(text2) };  // is platform-dependent (e.g. 4 bytes for x86, 8 bytes for x64)
   size_t s4{ strlen(text2) };  // is 6
   cout << s3 << endl << s4 << endl;
}

void code02_rawstring()
{
   //const char* str1{ "Hello "World"!" };    // Error!
   const char* str2{ "Hello \"World\"!" };
   const char* str3{ R"(Hello "World"!)" };

   const char* str4{ "Line 1\nLine 2" };
   const char* str5{ R"(Line 1
Line 2)" };

   const char* str6{ R"(Is the following a tab character? \t)" };

   //const char* str7{ R"(Embedded )" characters)" };    // Error!
   const char* str8{ R"-(Embedded )" characters)-" };
}

int main()
{
   code01_strcpy();
   code01_strlen();
   code02_rawstring();
}
