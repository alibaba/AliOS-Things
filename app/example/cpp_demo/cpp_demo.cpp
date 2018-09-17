/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <iostream>
#include <stdio.h>
#include <aos/aos.h>
#include <alios_cpp.h>

using namespace std;

extern "C" int application_start(int argc, char *argv[]);

extern void test_thread(void);
extern void test_timer(void);
extern void test_queue(void);
extern void test_semaphore(void);
extern void test_mutex(void);
extern void test_workqueue(void);


int application_start(int argc, char *argv[])
{

    printf("hello world C!\n");
    cout<< "hello world C++!" << endl;

    //test_thread();
    //test_timer();
    //test_queue();
    //test_semaphore();
    //test_mutex();
    //test_workqueue();

    return 0;
}

