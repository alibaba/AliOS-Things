#include <atomic>
#include <iostream>
#include <thread>

using namespace std;

atomic<int> g_num(0);

static void task0_entry(void)
{
    int i;

    for (i = 0; i < 1000000; i++) {
        g_num++;
    }
}

static void task1_entry(void)
{
    int i;

    for (i = 0; i < 1000000; i++) {
        g_num--;
    }
}

void atomic_test(void)
{
    cout << "atomic test start" << endl;
    thread thread0(task0_entry);
    thread thread1(task1_entry);
    thread0.join();
    thread1.join();
    cout << "g_num is: " << g_num << endl;
    if (g_num != 0) {
        cout << "atomic test error" << endl;
    } else {
        cout << "atomic test ok" << endl;
    }
}
