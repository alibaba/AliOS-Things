#ifndef RTL_SEC_H
#define RTL_SEC_H

#include <platform_stdlib.h>

#define SEC_PROCESS_OPT_ENC   1
#define SEC_PROCESS_OPT_DEC   2

#define sec_process_data ProcessSecData
uint32_t sec_process_data(uint8_t key_idx, uint32_t opt, uint8_t *iv, uint8_t *input_buf, uint32_t buf_len, uint8_t *output_buf);

#endif // RTL_SEC_H
