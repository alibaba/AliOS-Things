/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef _LOG_H_
#define _LOG_H_

extern int32_t printk(const int8_t *fmt, ...);
#define assert(expr) \
    do { \
        if (!(expr)) { \
            printk("%s:%d\n", __FILE__, __LINE__); \
            while(1); \
        } \
    } while (0)

#define COMPILE_TIME_ASSERT(x) \
    do { \
        switch (0) { case 0: case ((x) ? 1: 0): default : break; } \
    } while (0)

#endif /* _LOG_H_ */
