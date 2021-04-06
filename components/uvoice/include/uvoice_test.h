/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_TEST_H__
#define __UVOICE_TEST_H__

/** @defgroup uvoice_test_api uvoice_test
 *  @ingroup uvoice_aos_api
 * @{
 */

/**
 * 播放测试
 *
 */
void uvoice_play_test(int argc, char **argv);

/**
 * 录音测试
 *
 */
void uvoice_record_test(int argc, char **argv);

/**
 * TTS测试
 *
 */
void test_tts_handle(int argc, char **argv);

void test_swid_handle(int argc, char **argv);
void test_wetalk_handle(int argc, char **argv);

/**
 * @}
 */

#endif /* __UVOICE_TEST_H__ */

