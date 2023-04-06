#include <iostream>
#include "gtest/gtest.h"
#include "gmock/gmock.h"


auto main(int argc, char **argv) -> int {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
