#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"
#include "alibabacloud/core/Runnable.h"
#include "../src/Executor.h"

using namespace std;
using namespace AlibabaCloud;

namespace {

static unsigned int nbr = 0;

static void f0() {
  usleep(3000);
  nbr |= 0x01;
}

static void f1() {
  usleep(3000);
  nbr |= 0x02;
}

static void f2() {
   usleep(3000);
   nbr |= 0x04;
}

// there should be no exception
  TEST(Executor, basic) {
    std::function<void()> func0(f0);
    std::function<void()> func1(f1);
    std::function<void()> func2(f2);
    Runnable* rf0 = new Runnable(func0);
    Runnable* rf1 = new Runnable(func1);
    Runnable* rf2 = new Runnable(func2);

    Executor* e = new Executor;
    EXPECT_TRUE(e->isShutdown() == true);

    // thread created to execute tasks
    e->start();

    // before
    EXPECT_TRUE(nbr == 0);

    // push tasks to queue
    e->instance()->execute(rf0);
    e->instance()->execute(rf1);
    e->instance()->execute(rf2);

    // shutdown removes tasks in queue
    e->shutdown();

    // there is no time for task executing
    // maybe some tasks executed as the async reason
    // EXPECT_EQ(nbr, 0x07);

    rf0 = new Runnable(func0);
    rf1 = new Runnable(func1);
    rf2 = new Runnable(func2);
    nbr = 0;
    e->execute(rf0);
    usleep(5000);
    // executor not started, nbr no changed
    EXPECT_TRUE(nbr == 0);

    e->start();
    // there should be no more thread created
    e->start();
    e->instance()->execute(rf0);
    e->execute(rf1);
    e->execute(rf2);
    usleep(20000);
    // EXPECT_EQ(nbr, 0x07);
    e->shutdown();
  }
}
