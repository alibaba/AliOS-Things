/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef __SST_TEST_H__
#define __SST_TEST_H__

#define sst_test_malloc malloc
#define sst_test_free   free
#define sst_test_memset memset
#define sst_test_memcmp memcmp

int sst_test_create_file(char *file_name, uint32_t over_w_flag);
int sst_test_read_file(char *file_name);
int sec_sst_basic_test();
int sst_test_migration();
int sst_file_mig_whole_test();
int sst_test_performance(int task_count, int test_count);

#endif// __SST_TEST_H__

