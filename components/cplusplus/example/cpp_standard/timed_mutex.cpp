#include <posix/timer.h>

#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

static std::timed_mutex mutex_test;

static void task0_entry(void)
{
    bool                 islock;
    chrono::milliseconds waittime(2000);

    auto start  = chrono::steady_clock::now();
    islock = mutex_test.try_lock_for(waittime);
    auto end    = std::chrono::steady_clock::now();

    chrono::duration<double, ratio<1, 1000>> diff = end - start;

    cout << "thread0 trylock result:" << islock << " time to try lock "
         << diff.count() << " ms" << endl;
}

static void task1_entry(void)
{
    bool                 islock;
    chrono::milliseconds waittime(1000);
    auto                 start = chrono::steady_clock::now();
    islock                     = mutex_test.try_lock_for(waittime);
    auto                 end   = chrono::steady_clock::now();
    chrono::duration<float, ratio<1, 1000>> diff = end - start;
    cout << "thread1 trylock result:" << islock << " time to try lock "
         << diff.count() << " ms" << endl;
}

void timed_mutex_test(void)
{
    cout << "timed mutex test start" << endl;
    thread thread0(task0_entry);
    thread thread1(task1_entry);
    thread0.join();
    thread1.join();
    cout << "timed mutex test ok" << endl;
}
