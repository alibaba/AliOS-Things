/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef HAL_FLASH_H_
#define HAL_FLASH_H_

/**
 * Write data to OTP area on a chip from data buffer in RAM
 *
 * @param[in]  in_buf       Point to the data buffer that stores the data read from flash
 * @param[in]  in_buf_len  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_write_triples(const void *in_buf, uint32_t in_buf_len);

/**
 * Read data from OTP area on a chip to data buffer in RAM
 *
 * @param[in]  out_buf       Point to the data buffer that stores the data read from flash
 * @param[in]  out_buf_len  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_read_triples(void *out_buf, uint32_t out_buf_len);

/**
 * Write data to OTP area on a chip from data buffer in RAM
 *
 * @param[in]  in_buf       Point to the data buffer that stores the data read from flash
 * @param[in]  in_buf_len  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_write_group_addr(const void *in_buf, uint32_t in_buf_len);

/**
 * Read data from OTP area on a chip to data buffer in RAM
 *
 * @param[in]  out_buf       Point to the data buffer that stores the data read from flash
 * @param[in]  out_buf_len  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_read_group_addr(void *out_buf, uint32_t out_buf_len);

int32_t hal_flash_write_xtalcap_params(const void *in_buf, uint32_t in_buf_len);

int32_t hal_flash_read_xtalcap_params(void *out_buf, uint32_t out_buf_len);

int32_t hal_flash_read_mac_params(void *out_buf, uint32_t out_buf_len);

int32_t hal_flash_write_mac_params(const void *in_buf, uint32_t in_buf_len);

/**
 * Write data to OTP area on a chip from data buffer in RAM
 *
 * @param[in]  in_buf       Point to the data buffer that stores the data read from flash
 * @param[in]  in_buf_len  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_write_sn_params(const void *in_buf, uint32_t in_buf_len);

/**
 * Read data from OTP area on a chip to data buffer in RAM
 *
 * @param[in]  out_buf       Point to the data buffer that stores the data read from flash
 * @param[in]  out_buf_len  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_read_sn_params(void *out_buf, uint32_t out_buf_len);


#endif /* HAL_FLASH_H_ */

