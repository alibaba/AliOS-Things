/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <iostream>
#include <stdio.h>
#include <aos/aos.h>
#include <alios_cpp.h>

using namespace std;

extern "C" int application_start(int argc, char *argv[]);

class A
{
public:
    int a;
    void foo()
    {
        cout << "A class foo." << endl;
    }
    virtual void fun()
    {
        cout << "A class fun." << endl;
    }

    A() {
        a = 5;
    }
};

class B : public A
{
public:
    int b;
    void foo()
    {
        cout << "B class foo." << endl;
    }
    void fun()
    {
        cout << "B class fun." << endl;
    }

    B() {
        b = 10;
    }
};

static B static_class_b;

static void static_class_test(void) 
{
    printf("B member b value is %d.\n", static_class_b.b);
    printf("B member a value is %d.\n", static_class_b.a);
}

static void class_ploy_test(void)
{
    A *pa = new A;
    B *pb = new B;
    A *pc = (A *)pb;

    pa->foo();
    pa->fun();

    pb->foo();
    pb->fun();

    pc->foo();
    pc->fun();

    delete pa;
    delete pb;
}

static double division(int a, int b) {
   if( b == 0 ) {
      throw "Division by zero condition!";
   }
   return (a/b);
}

static void exception_test(void)
{
   int x = 50;
   int y = 0;
   double z = 0;

   try {
      z = division(x, y);
      cout << z << endl;
   } catch (const char* msg) {
     cout << "catch exception" << endl;
     cerr << msg << endl;
   }

}
