/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef YTS_H
#define YTS_H

int yts_get_args(const char ***argv);

void yts_run(int argc, char **argv);

void osupdate_online_test_run(char *bin, int id2);

#define check_cond_wait(cond, seconds)                               \
        do {                                                         \
            unsigned int i;                                          \
            for (i = 0; i < (unsigned int)seconds && !(cond); i++) { \
                aos_msleep(1000);                                    \
            }                                                        \
            YUNIT_ASSERT(cond);                                      \
        } while(0);

#define run_times(func, times)                 \
        do {                                   \
            int i;                             \
            for (i = 0; i < times; i++, func); \
        } while(0);

#endif /* YTS_H */

