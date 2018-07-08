/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef CPP_MEM_H
void *operator new[](size_t size);
void *operator new(size_t size);
void operator delete[](void *ptr);
void operator delete(void *ptr);
#endif

