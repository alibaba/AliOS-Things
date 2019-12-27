/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __DPM_H__
#define __DPM_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
#endif

#define DPM_OTP_STEP_MIN_LEN        180
#define DPM_OTP_STEP_MAX_LEN        1800

#define DPM_OTP_DATA_MIN_LEN        6
#define DPM_OTP_DATA_MAX_LEN        16

/* Error Codes */
#define DPM_SUCCESS                 (0x00000000)
#define DPM_ERROR_GENERIC           (0xFFFF0000)
#define DPM_ERROR_BAD_PARAMETERS    (0xFFFF0001)
#define DPM_ERROR_SHORT_BUFFER      (0xFFFF0002)
#define DPM_ERROR_EXCESS_DATA       (0xFFFF0003)
#define DPM_ERROR_OUT_OF_MEMORY     (0xFFFF0004)
#define DPM_ERROR_COMMUNICATION     (0xFFFF0005)
#define DPM_ERROR_NOT_SUPPORTED     (0xFFFF0006)
#define DPM_ERROR_NOT_IMPLEMENTED   (0xFFFF0007)
#define DPM_ERROR_TIMEOUT           (0xFFFF0008)
#define DPM_ERROR_ITEM_NOT_FOUND    (0xFFFF0009)

typedef uint32_t dpm_stat_t;

/**
 * @brief Get Time-based One-Time Password.
 *
 * @param [in] timestamp: Current Unix Time.
 * @param [in] index: Password index.
 * @param [in] otp_len: Specify the length of password.
 * @param [in] otp_step: Specify the step size of password.
 * @param [out] otp_data: Specify the buffer to contain output password data.
 *
 * @return: @see Error Codes.
 */
dpm_stat_t dpm_get_totp(uint64_t timestamp, uint32_t index,
               uint32_t otp_step, uint32_t otp_len, uint8_t *otp_data);

/**
 * @brief Extract index from password.
 *
 * @param [in] otp_data: Password data.
 * @param [in] otp_len: The length of Password.
 * @param [out] index: Password index.
 *
 * @return: @see Error Codes.
 */
dpm_stat_t dpm_get_index(uint8_t *otp_data, uint32_t otp_len, uint32_t *index);

#ifdef __cplusplus
}
#endif

#endif /* __DPM_H__ */
