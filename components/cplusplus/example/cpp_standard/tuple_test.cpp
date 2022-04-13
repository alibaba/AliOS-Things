/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include <iostream>
#include <string>
#include <tuple>

using namespace std;

void tuple_test(void)
{
    string name;
    char   x;
    double p;
    int    l;

    auto tup0 = std::make_tuple("Hello World!", 'a', 1.23, 6);
    auto tup1 = std::make_tuple("Hello ATOS!", 'b', 3.14, 7);

    std::tie(name, x, p, l) = tup0;
    cout << "data from tup0 is:" << name << " " << x << " " << p << " " << l
         << endl;

    auto tup2 = tuple_cat(tup0, tup1);
    int  size = tuple_size<decltype(tup2)>::value;

    if (size != 8) {
        cout << "tuple test error, size is:" << size << endl;
    } else {
        cout << "tuple test ok" << endl;
    }
}
