/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdint.h>
#include <string.h>


// define string change error
typedef enum {
        AMP_STRING_STATE_SUCCESS            = 0,
        AMP_STRING_STATE_FAILED             = -1,
        AMP_STRING_INPUT_OUT_RANGE          = -2,
        AMP_STRING_INPUT_JSON_PARSE_FAILED  = -3,
} AMP_STRING_ERRNO_E;

int32_t amp_str2uint(char *input, uint8_t input_len, uint32_t *output);
int32_t amp_str2uint64(char *input, uint8_t input_len, uint64_t *output);
int32_t amp_uint2str(uint32_t input, char *output, uint8_t *output_len);
int32_t amp_uint642str(uint64_t input, char *output, uint8_t *output_len);
int32_t amp_int2str(int32_t input, char *output, uint8_t *output_len);
int32_t amp_hex2str(uint8_t *input, uint32_t input_len, char *output, uint8_t lowercase);
int32_t amp_str2hex(char *input, uint32_t input_len, uint8_t *output);
int32_t amp_json_value(const char *input, uint32_t input_len, const char *key, uint32_t key_len, char **value,
                        uint32_t *value_len);


