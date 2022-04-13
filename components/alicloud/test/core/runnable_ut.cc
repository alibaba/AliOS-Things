#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"
#include "alibabacloud/core/Runnable.h"

using namespace std;
using namespace AlibabaCloud;


static int nbr = 0;
void foo() {
  nbr = 1;
}

// there should be no exception
TEST(Runnable, basic) {
  std::function<void()> func(foo);
  Runnable f(func);
  EXPECT_TRUE(nbr == 0);
  f.run();
  EXPECT_TRUE(nbr == 1);
}
