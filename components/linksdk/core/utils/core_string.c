#include "core_string.h"

int32_t core_str2uint(char *input, uint8_t input_len, uint32_t *output)
{
    uint8_t index = 0;
    uint32_t temp = 0;

    for (index = 0; index < input_len; index++) {
        if (input[index] < '0' || input[index] > '9') {
            return STATE_USER_INPUT_OUT_RANGE;
        }
        temp = temp * 10 + input[index] - '0';
    }
    *output = temp;

    return STATE_SUCCESS;
}

int32_t core_str2uint64(char *input, uint8_t input_len, uint64_t *output)
{
    uint8_t index = 0;
    uint64_t temp = 0;

    for (index = 0; index < input_len; index++) {
        if (input[index] < '0' || input[index] > '9') {
            return STATE_USER_INPUT_OUT_RANGE;
        }
        temp = temp * 10 + input[index] - '0';
    }
    *output = temp;

    return STATE_SUCCESS;
}

int32_t core_uint2str(uint32_t input, char *output, uint8_t *output_len)
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

    return STATE_SUCCESS;
}

int32_t core_uint642str(uint64_t input, char *output, uint8_t *output_len)
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

    return STATE_SUCCESS;
}

int32_t core_int2str(int32_t input, char *output, uint8_t *output_len)
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

    return STATE_SUCCESS;
}

int32_t core_hex2str(uint8_t *input, uint32_t input_len, char *output, uint8_t lowercase)
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

    return STATE_SUCCESS;
}

int32_t core_str2hex(char *input, uint32_t input_len, uint8_t *output)
{
    uint32_t idx = 0;

    if (input_len % 2 != 0) {
        return STATE_USER_INPUT_OUT_RANGE;
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

    return STATE_SUCCESS;
}

int32_t core_strdup(aiot_sysdep_portfile_t *sysdep, char **dest, char *src, char *module_name)
{
    if (*dest != NULL) {
        sysdep->core_sysdep_free(*dest);
        *dest = NULL;
    }

    *dest = sysdep->core_sysdep_malloc((uint32_t)(strlen(src) + 1), module_name);
    if (*dest == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }
    memset(*dest, 0, strlen(src) + 1);
    memcpy(*dest, src, strlen(src));

    return STATE_SUCCESS;
}

int32_t core_sprintf(aiot_sysdep_portfile_t *sysdep, char **dest, char *fmt, char *src[], uint8_t count,
                     char *module_name)
{
    char *buffer = NULL, *value = NULL;
    uint8_t idx = 0, percent_idx = 0;
    uint32_t buffer_len = 0;

    buffer_len += strlen(fmt) - 2 * count;
    for (percent_idx = 0; percent_idx < count; percent_idx++) {
        value = (*(src + percent_idx) == NULL) ? ("") : (*(src + percent_idx));
        buffer_len += strlen(value);
    }

    buffer = sysdep->core_sysdep_malloc(buffer_len + 1, module_name);
    if (buffer == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }
    memset(buffer, 0, buffer_len + 1);

    for (idx = 0, percent_idx = 0; idx < strlen(fmt);) {
        if (fmt[idx] == '%' && fmt[idx + 1] == 's') {
            value = (*(src + percent_idx) == NULL) ? ("") : (*(src + percent_idx));
            memcpy(buffer + strlen(buffer), value, strlen(value));
            percent_idx++;
            idx += 2;
        } else {
            buffer[strlen(buffer)] = fmt[idx++];
        }
    }
    *dest = buffer;
    return STATE_SUCCESS;
}

int32_t core_json_value(const char *input, uint32_t input_len, const char *key, uint32_t key_len, char **value,
                        uint32_t *value_len)
{
    uint32_t idx = 0;

    for (idx = 0; idx < input_len; idx++) {
        if (idx + key_len >= input_len) {
            return STATE_USER_INPUT_JSON_PARSE_FAILED;
        }
        if ((memcmp(&input[idx], key, key_len) == 0) &&
            ((idx > 0) && (input[idx - 1] == '"')) &&
            ((idx + key_len < input_len) && (input[idx + key_len] == '"'))) {
            idx += key_len;
            /* shortest ":x, or ":x} or ":x] */
            if ((idx + 2 >= input_len) ||
                (input[idx + 1] != ':')) {
                return STATE_USER_INPUT_JSON_PARSE_FAILED;
            }
            idx += 2;
            if (input[idx] == '"') {
                *value = (char *)&input[++idx];
                for (; idx < input_len; idx++) {
                    if ((input[idx] == '"')) {
                        *value_len = (uint32_t)(idx - (*value - input));
                        return STATE_SUCCESS;
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
                            return STATE_SUCCESS;
                        }
                    }
                }
            } else {
                *value = (char *)&input[idx];
                for (; idx < input_len; idx++) {
                    if ((input[idx] == ',' || input[idx] == ']' || input[idx] == '}')) {
                        *value_len = (uint32_t)(idx - (*value - input));
                        return STATE_SUCCESS;
                    }
                }
            }
        }
    }

    return STATE_USER_INPUT_JSON_PARSE_FAILED;
}

int32_t core_utc2date(uint64_t utc, int8_t zone, core_date_t *date)
{
    uint32_t day_sec = 0, day_num = 0;
    uint64_t utc_zone_s = 0;

#define DAYS_IN_YEAR(year)        (IS_LEAP_YEAR(year) ? 366 : 365)
#define IS_LEAP_YEAR(year)        (!((year) % 400) || (((year) % 100) && !((year) % 4)))
#define DAYS_IN_MONTH(lpyr, mon)  ((mon == 1)?(28 + lpyr):((mon > 6)?(((mon - 1) & 1)?(30):(31)):((mon & 1)?(30):(31))))

    date->msec = utc % 1000;
    utc_zone_s = (utc / 1000) + (zone * 60 * 60);

    day_sec = utc_zone_s % (24 * 60 * 60);
    day_num = utc_zone_s / (24 * 60 * 60);

    date->sec = day_sec % 60;
    date->min = (day_sec % 3600) / 60;
    date->hour = day_sec / 3600;

    date->year = 1970;
    while (day_num >= DAYS_IN_YEAR(date->year)) {
        day_num -= DAYS_IN_YEAR(date->year);
        date->year++;
    }

    date->mon = 0;
    while (day_num >= DAYS_IN_MONTH(IS_LEAP_YEAR(date->year), date->mon)) {
        day_num -= DAYS_IN_MONTH(IS_LEAP_YEAR(date->year), date->mon);
        date->mon++;
    }
    date->mon++;
    date->day = day_num + 1;

    return 0;
}

#if 0
int32_t core_strcat(aiot_sysdep_portfile_t *sysdep, char **dest, char *src1, char *src2, char *module_name)
{
    uint32_t dest_len = 0;
    char *tmp_dest = NULL;

    dest_len = strlen(src1) + strlen(src2) + 1;
    tmp_dest = sysdep->core_sysdep_malloc(dest_len, module_name);
    if (tmp_dest == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }
    memset(tmp_dest, 0, dest_len);
    memcpy(tmp_dest + strlen(tmp_dest), src1, strlen(src1));
    memcpy(tmp_dest + strlen(tmp_dest), src2, strlen(src2));

    if (*dest != NULL) {
        sysdep->core_sysdep_free(*dest);
        *dest = NULL;
    }

    *dest = tmp_dest;

    return STATE_SUCCESS;
}
#endif

