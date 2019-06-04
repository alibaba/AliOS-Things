/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UASSERT_H
#define UASSERT_H

#ifdef NDEBUG
#define uassert(expr) ((void)(0))
#else
void uassert_fail(const char *expr, const char *file, int line);
#define uassert(expr) \
    do { \
        if (!(expr)) { \
            uassert_fail(#expr, __FILE__, __LINE__); \
        } \
    }while (0)
#endif /* NDEBUG */

#endif /* UASSERT_H */

