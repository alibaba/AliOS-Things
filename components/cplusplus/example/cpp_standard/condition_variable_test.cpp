/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <posix/timer.h>

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

#define TEST_COUNT 10
static bool data_ready        = false;
static int  critical_resource = 0;

std::mutex              _mutex;
std::condition_variable cond_var;

static void task0_entry(void)
{
    int                          i;
    std::unique_lock<std::mutex> lck(_mutex, std::defer_lock);

    for (i = 0; i < TEST_COUNT; i++) {
        lck.lock();
        while (data_ready == false) {
            cond_var.wait(lck);
        }
        critical_resource--;
        cout << "access the resource through a condition variable" << endl;
        if (critical_resource != 0) {
            cout << "condition varialbe test error" << endl;
        }
        data_ready = false;
        lck.unlock();
    }
}

static void task1_entry(void)
{
    int                          i;
    std::unique_lock<std::mutex> lck(_mutex, std::defer_lock);

    for (i = 0; i < TEST_COUNT; i++) {
        lck.lock();
        data_ready = true;
        critical_resource++;
        cond_var.notify_one();
        lck.unlock();
        sleep(1);
    }
}

void conditon_varialbe_test(void)
{
    cout << "conditon varialbe test start" << endl;
    thread thread0(task0_entry);
    thread thread1(task1_entry);
    thread0.join();
    thread1.join();
    cout << "conditon varialbe test ok" << endl;
}
