#include "cppsl/result.hpp"
#include "gtest/gtest.h"

TEST(ResultOptVal, BoolTest) {   // 12/2/2020 -> 737761
  cppsl::ResultOptVal<bool> rest{true};
  cppsl::ResultOptVal<bool> resf{false};
  cppsl::ResultOptVal<bool> res;

  EXPECT_EQ(rest.get(), true);
  EXPECT_EQ(rest.get_or(), true);
  EXPECT_NE(rest.invalid(), true);
  EXPECT_EQ(rest.valid(), true);

  EXPECT_EQ(resf.get(), false);
  EXPECT_EQ(resf.get_or(), false);
  EXPECT_NE(resf.invalid(), true);
  EXPECT_EQ(resf.valid(), true);

  try {
    [[maybe_unused]] auto n = res.get();
  } catch(const std::bad_optional_access& e) {
    EXPECT_EQ(res.valid(), false);
  }
  EXPECT_EQ(res.get_or(), false);
  EXPECT_EQ(res.invalid(), true);
  EXPECT_NE(res.valid(), true);
}

TEST(ResultOptVal, FloatTest) {   // 12/2/2020 -> 737761
  cppsl::ResultOptVal<float> res1{5.5};
  cppsl::ResultOptVal<float> res;

  EXPECT_EQ(res1.get(), 5.5);
  EXPECT_EQ(res1.get_or(), 5.5);
  EXPECT_NE(res1.invalid(), true);
  EXPECT_EQ(res1.valid(), true);

  try {
    [[maybe_unused]] auto n = res.get();
  } catch(const std::bad_optional_access& e) {
    EXPECT_EQ(res.valid(), false);
  }
  EXPECT_EQ(res.get_or(), 0);
  EXPECT_EQ(res.invalid(), true);
  EXPECT_NE(res.valid(), true);
}
