/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "amp_string.h"

int32_t amp_str2uint(char *input, uint8_t input_len, uint32_t *output)
{
    uint8_t index = 0;
    uint32_t temp = 0;

    for (index = 0; index < input_len; index++) {
        if (input[index] < '0' || input[index] > '9') {
            return AMP_STRING_INPUT_OUT_RANGE;
        }
        temp = temp * 10 + input[index] - '0';
    }
    *output = temp;

    return AMP_STRING_STATE_SUCCESS;
}

int32_t amp_str2uint64(char *input, uint8_t input_len, uint64_t *output)
{
    uint8_t index = 0;
    uint64_t temp = 0;

    for (index = 0; index < input_len; index++) {
        if (input[index] < '0' || input[index] > '9') {
            return AMP_STRING_INPUT_OUT_RANGE;
        }
        temp = temp * 10 + input[index] - '0';
    }
    *output = temp;

    return AMP_STRING_STATE_SUCCESS;
}

int32_t amp_uint2str(uint32_t input, char *output, uint8_t *output_len)
{
    uint8_t i = 0, j = 0;
    char temp[10] = {0};

    do {
        temp[i++] = input % 10 + '0';
    } while ((input /= 10) > 0);

    do {
        output[--i] = temp[j++];
    } while (i > 0);

    if (output_len) {
        *output_len = j;
    }

    return AMP_STRING_STATE_SUCCESS;
}

int32_t amp_uint642str(uint64_t input, char *output, uint8_t *output_len)
{
    uint8_t i = 0, j = 0;
    char temp[20] = {0};

    do {
        temp[i++] = input % 10 + '0';
    } while ((input /= 10) > 0);

    do {
        output[--i] = temp[j++];
    } while (i > 0);

    if (output_len) {
        *output_len = j;
    }

    return AMP_STRING_STATE_SUCCESS;
}

int32_t amp_int2str(int32_t input, char *output, uint8_t *output_len)
{
    uint8_t i = 0, j = 0, minus = 0;
    char temp[11] = {0};
    int64_t in = input;

    if (in < 0) {
        minus = 1;
        in = -in;
    }

    do {
        temp[minus + (i++)] = in % 10 + '0';
    } while ((in /= 10) > 0);

    do {
        output[minus + (--i)] = temp[minus + (j++)];
    } while (i > 0);

    if (minus == 1) {
        output[0] = '-';
    }

    if (output_len) {
        *output_len = j + minus;
    }

    return AMP_STRING_STATE_SUCCESS;
}

int32_t amp_hex2str(uint8_t *input, uint32_t input_len, char *output, uint8_t lowercase)
{
    char *upper = "0123456789ABCDEF";
    char *lower = "0123456789abcdef";
    char *encode = upper;
    int i = 0, j = 0;

    if (lowercase) {
        encode = lower;
    }

    for (i = 0; i < input_len; i++) {
        output[j++] = encode[(input[i] >> 4) & 0xf];
        output[j++] = encode[(input[i]) & 0xf];
    }

    return AMP_STRING_STATE_SUCCESS;
}

int32_t amp_str2hex(char *input, uint32_t input_len, uint8_t *output)
{
    uint32_t idx = 0;

    if (input_len % 2 != 0) {
        return AMP_STRING_INPUT_OUT_RANGE;
    }

    for (idx = 0; idx < input_len; idx += 2) {
        if (input[idx] >= '0' && input[idx] <= '9') {
            output[idx / 2] = (input[idx] - '0') << 4;
        } else if (input[idx] >= 'A' && input[idx] <= 'F') {
            output[idx / 2] = (input[idx] - 'A' + 0x0A) << 4;
        } else if (input[idx] >= 'a' && input[idx] <= 'f') {
            output[idx / 2] = (input[idx] - 'a' + 0x0A) << 4;
        }
        if (input[idx + 1] >= '0' && input[idx + 1] <= '9') {
            output[idx / 2] |= (input[idx + 1] - '0');
        } else if (input[idx + 1] >= 'A' && input[idx + 1] <= 'F') {
            output[idx / 2] |= (input[idx + 1] - 'A' + 0x0A);
        } else if (input[idx + 1] >= 'a' && input[idx + 1] <= 'f') {
            output[idx / 2] |= (input[idx + 1] - 'a' + 0x0A);
        }
    }

    return AMP_STRING_STATE_SUCCESS;
}

int32_t amp_json_value(const char *input, uint32_t input_len, const char *key, uint32_t key_len, char **value,
                        uint32_t *value_len)
{
    uint32_t idx = 0;

    for (idx = 0; idx < input_len; idx++) {
        if (idx + key_len >= input_len) {
            return AMP_STRING_INPUT_JSON_PARSE_FAILED;
        }
        if ((memcmp(&input[idx], key, key_len) == 0) &&
            ((idx > 0) && (input[idx - 1] == '"')) &&
            ((idx + key_len < input_len) && (input[idx + key_len] == '"'))) {
            idx += key_len;
            /* shortest ":x, or ":x} or ":x] */
            if ((idx + 2 >= input_len) ||
                (input[idx + 1] != ':')) {
                return AMP_STRING_INPUT_JSON_PARSE_FAILED;
            }
            idx += 2;
            if (input[idx] == '"') {
                *value = (char *)&input[++idx];
                for (; idx < input_len; idx++) {
                    if ((input[idx] == '"')) {
                        *value_len = (uint32_t)(idx - (*value - input));
                        return AMP_STRING_STATE_SUCCESS;
                    }
                }
            } else if (input[idx] == '{' || input[idx] == '[') {
                char start = input[idx];
                char end = (start == '{') ? ('}') : (']');
                uint8_t count = 0;
                *value = (char *)&input[idx];
                for (; idx < input_len; idx++) {
                    if ((input[idx] == start)) {
                        count++;
                    } else if ((input[idx] == end)) {
                        if (--count == 0) {
                            *value_len = (uint32_t)(idx - (*value - input) + 1);
                            return AMP_STRING_STATE_SUCCESS;
                        }
                    }
                }
            } else {
                *value = (char *)&input[idx];
                for (; idx < input_len; idx++) {
                    if ((input[idx] == ',' || input[idx] == ']' || input[idx] == '}')) {
                        *value_len = (uint32_t)(idx - (*value - input));
                        return AMP_STRING_STATE_SUCCESS;
                    }
                }
            }
        }
    }

    return AMP_STRING_INPUT_JSON_PARSE_FAILED;
}

