/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _TESTCMD_LOCK_H_
#define _TESTCMD_LOCK_H_

void *testcmd_lock_alloc_property();

void testcmd_lock_destroy_property(void **p_prop);

int testcmd_lock_get_propery(char *in, char *out, int out_len, void *ctx);

int testcmd_lock_set_property(char *in, void *ctx);

int testcmd_lock_call_service(int devid, char *identifier, char *in, char *out, int out_len, void *ctx);

#endif /* _TESTCMD_LOCK_H_ */
