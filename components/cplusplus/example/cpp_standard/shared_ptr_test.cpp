/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <iostream>
#include <memory>

using namespace std;

class resource {
  public:
    resource() { cout << "resource is create" << endl; }
    ~resource() { cout << "resource is delete" << endl; }
};

void shared_ptr_test(void)
{
    cout << "shared ptr test start" << endl;
    shared_ptr<resource> ptest0(new resource);
    shared_ptr<resource> ptest1 = ptest0;
    cout << "shared count:" << ptest1.use_count() << endl;
    if (ptest1.use_count() != 2) {
        cout << "shared ptr test error" << endl;
        return;
    }

    ptest0.reset();
    cout << "shared count:" << ptest1.use_count() << endl;

    if (ptest1.use_count() == 1) {
        cout << "shared ptr test ok" << endl;
    } else {
        cout << "shared ptr test error" << endl;
    }
}
