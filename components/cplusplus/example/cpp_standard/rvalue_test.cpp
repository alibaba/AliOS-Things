/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <algorithm>
#include <iostream>
#include <string.h>

using namespace std;

class custom_string {
  public:
    custom_string(const char *p)
    {
        len = strlen(p);
        copy_data(p);
    }

    custom_string(const custom_string &str)
    {
        len = str.len;
        copy_data(str.strptr);
    }

    custom_string(custom_string &str)
    {
        cout << "rvalue construcotr is called" << endl;
        if (this != &str) {
            len        = str.len;
            strptr     = str.strptr;
            str.strptr = NULL;
            str.len    = 0;
        }
    }

    custom_string &operator=(const custom_string &str)
    {
        if (this != &str) {
            len = str.len;
            copy_data(str.strptr);
        }
        return *this;
    }

    custom_string &operator=(custom_string &&str)
    {
        cout << "rvalue assignment is called" << endl;
        if (this != &str) {
            len        = str.len;
            strptr     = str.strptr;
            str.strptr = NULL;
            str.len    = 0;
        }
        return *this;
    }

    custom_string() : strptr(NULL), len(0) {}
    ~custom_string()
    {
        if (strptr != NULL) {
            delete (strptr);
        }
    }

    static void copycount_clear(void) { copy_count = 0; };
    static int  copycount_get(void) { return copy_count; };

  private:
    char *     strptr;
    size_t     len;
    static int copy_count;

    void copy_data(const char *s)
    {
        copy_count++;
        cout << "copy data, copy count is " << copy_count << endl;
        strptr = new char[len + 1];
        memcpy(strptr, s, len);
        strptr[len] = '\0';
    }
};

int custom_string::copy_count = 0;

void rvalue_test(void)
{
    int copy_count;

    custom_string::copycount_clear();

    custom_string a;
    a = custom_string("ATOS");

    custom_string b(a);

    copy_count = custom_string::copycount_get();

    if (copy_count > 2) {
        cout << "rvalue test error, copy_count is " << copy_count << endl;
    } else {
        cout << "rvalue test ok" << endl;
    }
}
