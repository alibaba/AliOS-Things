/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#if defined(__GNUC__)
typedef void (*cpp_func) ();
extern cpp_func __ctors_start__[];
extern cpp_func __ctors_end__[];

__attribute__((weak)) void * __dso_handle = 0;

void cpp_init(void)
{
    cpp_func *f;

    for (f = __ctors_start__; f < __ctors_end__; f++) {
        (*f)();
    }
}
#endif

