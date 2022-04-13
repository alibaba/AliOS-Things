#include <posix/timer.h>

#include <iostream>
#include <thread>

using namespace std;

#define TEST_COUNT 10
static void task0_entry(void)
{
    int i;

    for (i = 0; i < TEST_COUNT; i++) {
        sleep(1);
        cout << "I'm task0" << endl;
    }
}

static void task1_entry(void)
{
    int i;

    for (i = 0; i < TEST_COUNT; i++) {
        sleep(1);
        cout << "I'm task1" << endl;
    }
}

void thread_test(void)
{
    int cpu_num;

    cout << "thead test start" << endl;
    thread thread0(task0_entry);
    thread thread1(task1_entry);
    thread0.join();
    thread1.join();
#if 0
    cpu_num = thread0.hardware_concurrency();
    if (cpu_num <= 0) {
        cout << "thead test error, cpu_num:" << cpu_num << endl;
        return;
    }
#endif

    cout << "thead test ok" << endl;
}
