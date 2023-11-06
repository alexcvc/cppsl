#include "cppsl/enumBitmask.hpp"
#include "gtest/gtest.h"

enum class Permissions { Readable = 0x4, Writable = 0x2, Executable = 0x1 };
ENABLE_BITMASK_OPERATORS(Permissions)

TEST(TestEnumBitMask, TestPermissions) {

  Permissions p = Permissions::Readable | Permissions::Writable;   // Set default permissions
  p |= Permissions::Executable;                                    // Add permission
  p &= ~Permissions::Writable;                                     // Remove permission
  bool hasReadPermission = static_cast<bool>(p & Permissions::Readable);
  bool hasWritePermission = static_cast<bool>(p & Permissions::Writable);
  bool hasExecutePermission = static_cast<bool>(p & Permissions::Executable);

  EXPECT_TRUE(hasReadPermission);
  EXPECT_FALSE(hasWritePermission);
  EXPECT_TRUE(hasExecutePermission);
}
