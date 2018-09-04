/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#if defined(__GNUC__)
typedef void (*pfunc)();
extern pfunc __ctors_start__[];
extern pfunc __ctors_end__[];

__attribute__((weak)) void *__dso_handle = 0;

void cpp_init(void)
{
    pfunc *f;

    for (f = __ctors_start__; f < __ctors_end__; f++) {
        (*f)();
    }
}
#endif
