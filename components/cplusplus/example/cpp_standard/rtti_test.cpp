#include <iostream>
#include <typeinfo>

using namespace std;

class car {
  public:
    string name;
    virtual ~car(){};
};

class truck : public car {
    int colour;
};

void rtti_test(void)
{
    double f1 = 1.0;
    car *  pcar;
    truck *ptruck;

    car   tool0;
    truck tool1;

    pcar = &tool1;

    cout << "rtti test start" << endl;

    cout << "the double type name is :" << typeid(f1).name() << endl;
    cout << "the tool0 type name is :" << typeid(tool0).name() << endl;
    cout << "the tool1 type name is :" << typeid(tool1).name() << endl;
    cout << "the pcar type name is :" << typeid(pcar).name() << endl;

    pcar   = &tool1;
    ptruck = dynamic_cast<truck *>(pcar);
    if (ptruck == NULL) {
        cout << "rtti test error" << endl;
    }

    pcar   = &tool0;
    ptruck = dynamic_cast<truck *>(pcar);
    if (ptruck != NULL) {
        cout << "rtti test error" << endl;
    }

    cout << "rtti test ok" << endl;
}
