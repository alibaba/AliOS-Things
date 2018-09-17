/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _TESTCMD_H_
#define _TESTCMD_H_

extern struct cli_command gw_test_cmd;

int testcmd_delete_device(const char *productKey, const char *deviceName);

#endif /* _TESTCMD_H_ */
