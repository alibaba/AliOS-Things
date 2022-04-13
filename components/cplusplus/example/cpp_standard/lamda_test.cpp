/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <algorithm>
#include <iostream>

using namespace std;

void lamda_test(void)
{
    int total = 0;
    int a[5]  = {1, 2, 3, 4, 5};

    cout << "lamda test start" << endl;

    for_each(a, &a[5], [&total](int &x) { total += x; });
    if (total != 15) {
        cout << "lamda test error, total is " << total << endl;
    } else {
        cout << "lamda test ok" << endl;
    }
}
