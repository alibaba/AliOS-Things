/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _UTILS_EPOCH_TIME_H_
#define _UTILS_EPOCH_TIME_H_
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get epoch time from the remote server(iot-nsauth.alibaba.net/system/time) through http request.
 *        The type of the epoch time is millisecond.
 *
 * @param none
 *
 * @return 0, failed to get epoch time; OTHERS, the actual value of epoch time
 */
uint64_t utils_get_epoch_time(char copy[], int len);

#ifdef __cplusplus
extern "C" {
#endif
#endif /* _ALIOT_EPOCH_TIME_H_ */
