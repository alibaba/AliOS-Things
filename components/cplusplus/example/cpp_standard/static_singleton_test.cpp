/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include <iostream>

using namespace std;

class singleton {
  public:
    static singleton *get_instance()
    {
        static singleton single;
        return &single;
    }
    int  get_value() { return _value; };
    void set_value(int value) { _value = value; }

  private:
    int _value;
    singleton() : _value(0) {}
    ~singleton() {}
};

void static_singleton_test(void)
{
    cout << "static singleton test start" << endl;
    singleton *single0 = singleton::get_instance();
    singleton *single1 = singleton::get_instance();
    single0->set_value(10);
    cout << "single0 set value 10, single1 get vale is " << single1->get_value()
         << endl;

    if (single0 != single1) {
        cout << "static singleton test error" << endl;
    } else {
        cout << "static singleton test ok" << endl;
    }
}
