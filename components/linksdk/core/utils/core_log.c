#include "core_log.h"

extern aiot_state_logcb_t g_logcb_handler;

static core_log_t g_core_log = { .time_start = 0, .time_interval = 0, .timestamp = 0 };

static void _core_log_append_code(int32_t code, char *buffer)
{
    uint8_t code_hex[4] = {0};
    char code_str[9] = {0};

    code_hex[0] = ((uint16_t)(-code) >> 8) & 0x000000FF;
    code_hex[1] = ((uint16_t)(-code)) & 0x000000FF;
    core_hex2str(code_hex, 2, code_str, 0);

    memcpy(buffer + strlen(buffer), "[LK-", strlen("[LK-"));
    memcpy(buffer + strlen(buffer), code_str, strlen(code_str));
    memcpy(buffer + strlen(buffer), "] ", strlen("] "));
}

static uint64_t _core_log_get_timestamp(aiot_sysdep_portfile_t *sysdep)
{
    uint64_t timenow = 0;

    timenow = sysdep->core_sysdep_time();
    if (timenow >= g_core_log.time_start) {
        g_core_log.time_interval += timenow - g_core_log.time_start;
    }else{
        /* loss (max_time - g_core_log.time_start) ms */
        g_core_log.time_interval += timenow;
    }
    g_core_log.time_start = timenow;

    return (g_core_log.timestamp + g_core_log.time_interval);
}

void _core_log_append_date(aiot_sysdep_portfile_t *sysdep, uint64_t timestamp, char *buffer)
{
    int32_t res = STATE_SUCCESS;
    char date_str[7][11] = {{0}};
    char *time = NULL;
    char *time_src[] = { date_str[0], date_str[1], date_str[2], date_str[3], date_str[4], date_str[5] };
    char *time_fmt = "%s/%s/%s %s:%s:%s";
    core_date_t date;

    memset(&date, 0, sizeof(core_date_t));

    core_utc2date(timestamp, 8, &date);
    core_uint2str(date.year, date_str[0], NULL);
    core_uint2str(date.mon, date_str[1], NULL);
    core_uint2str(date.day, date_str[2], NULL);
    core_uint2str(date.hour, date_str[3], NULL);
    core_uint2str(date.min, date_str[4], NULL);
    core_uint2str(date.sec, date_str[5], NULL);

    res = core_sprintf(sysdep, &time, time_fmt, time_src, sizeof(time_src)/sizeof(char *), CORE_LOG_MODULE_NAME);
    if (res >= STATE_SUCCESS) {
        memcpy(buffer + strlen(buffer), time, strlen(time));

        sysdep->core_sysdep_free(time);
    }
}


static void _core_log_append_prefix(aiot_sysdep_portfile_t *sysdep, int32_t code, char *buffer)
{
    if (sysdep == NULL) {
        return;
    }


    if (g_core_log.timestamp != 0) {
        memcpy(buffer + strlen(buffer), "[", strlen("["));
        _core_log_append_date(sysdep, _core_log_get_timestamp(sysdep), buffer);
        memcpy(buffer + strlen(buffer), "]", strlen("]"));
    } else
    {
        char timestamp_str[24] = {0};
        uint8_t timestamp_len = 0;

        core_uint642str(sysdep->core_sysdep_time(), timestamp_str, &timestamp_len);
        if (timestamp_len > 3) {
            memcpy(&timestamp_str[timestamp_len - 2], &timestamp_str[timestamp_len - 3], 3);
            timestamp_str[timestamp_len - 3] = '.';
        }

        memcpy(buffer + strlen(buffer), "[", strlen("["));
        memcpy(buffer + strlen(buffer), timestamp_str, strlen(timestamp_str));
        memcpy(buffer + strlen(buffer), "]", strlen("]"));
    }

    _core_log_append_code(code, buffer);
}

static void _core_log(aiot_sysdep_portfile_t *sysdep, int32_t code, char *buffer, char *fmt, void *datas[],
                      uint8_t count)
{
    uint32_t idx = 0, buffer_idx = 0, copy_len = 0, arg_flag = 0, arg_idx = 0;
    void *arg = datas[arg_idx];

    _core_log_append_prefix(sysdep, code, buffer);
    buffer_idx += strlen(buffer);

    for (idx = 0; idx < strlen(fmt);) {
        if (buffer_idx >= CORE_LOG_MAXLEN) {
            break;
        }

        if (arg_flag == 1) {
            if (arg_idx < count - 1) {
                arg = datas[++arg_idx];
            } else {
                arg = NULL;
            }
            arg_flag = 0;
        }

        if (fmt[idx] == '%' && fmt[idx + 1] == 's' && arg != NULL) {
            char *value = arg;
            copy_len = (strlen(buffer) + strlen(value) > CORE_LOG_MAXLEN) ? (CORE_LOG_MAXLEN - strlen(buffer)) : (strlen(value));
            memcpy(buffer + strlen(buffer), value, copy_len);
            buffer_idx += copy_len;
            idx += 2;
            arg_flag = 1;
        } else if (memcmp(&fmt[idx], "%.*s", strlen("%.*s")) == 0 && arg != NULL && (arg_idx + 1) < count) {
            char *value = (datas[arg_idx + 1] == NULL) ? ("") : (datas[arg_idx + 1]);
            uint32_t len = (datas[arg_idx + 1] == NULL) ? (0) : (*(uint32_t *)arg);
            copy_len = (strlen(buffer) + len > CORE_LOG_MAXLEN) ? (CORE_LOG_MAXLEN - strlen(buffer)) : (len);
            memcpy(buffer + strlen(buffer), value, copy_len);
            buffer_idx += copy_len;
            idx += strlen("%.*s");
            arg_flag = 1;
            arg_idx++;
        } else if (fmt[idx] == '%' && fmt[idx + 1] == 'd' && arg != NULL) {
            char uint32_str[11] = {0};
            core_uint2str(*(uint32_t *)arg, uint32_str, NULL);
            copy_len = (strlen(buffer) + strlen(uint32_str) > CORE_LOG_MAXLEN) ? (CORE_LOG_MAXLEN - strlen(buffer)) : (strlen(
                                   uint32_str));
            memcpy(buffer + strlen(buffer), uint32_str, copy_len);
            buffer_idx += copy_len;
            idx += 2;
            arg_flag = 1;
        } else {
            buffer[buffer_idx++] = fmt[idx++];
        }
    }
}

void core_log_set_timestamp(aiot_sysdep_portfile_t *sysdep, uint64_t timestamp)
{
    g_core_log.timestamp = timestamp;
    g_core_log.time_start = sysdep->core_sysdep_time();
    g_core_log.time_interval = 0;
}

uint64_t core_log_get_timestamp(aiot_sysdep_portfile_t *sysdep)
{
    return _core_log_get_timestamp(sysdep);
}

void core_log(aiot_sysdep_portfile_t *sysdep, int32_t code, char *data)
{
    char buffer[CORE_LOG_MAXLEN + 3] = {0};
    uint32_t len = 0;

    if (g_logcb_handler == NULL) {
        return;
    }

    buffer[CORE_LOG_MAXLEN] = '\r';
    buffer[CORE_LOG_MAXLEN + 1] = '\n';
    _core_log_append_prefix(sysdep, code, buffer);
    len = (strlen(buffer) + strlen(data) > CORE_LOG_MAXLEN) ? (CORE_LOG_MAXLEN - strlen(buffer)) : (strlen(data));
    memcpy(buffer + strlen(buffer), data, len);

    g_logcb_handler(code, buffer);
}

void core_log1(aiot_sysdep_portfile_t *sysdep, int32_t code, char *fmt, void *data)
{
    char buffer[CORE_LOG_MAXLEN + 3] = {0};
    void *datas[] = {data};

    if (g_logcb_handler == NULL) {
        return;
    }

    buffer[CORE_LOG_MAXLEN] = '\r';
    buffer[CORE_LOG_MAXLEN + 1] = '\n';
    _core_log(sysdep, code, buffer, fmt, datas, 1);

    g_logcb_handler(code, buffer);
}

void core_log2(aiot_sysdep_portfile_t *sysdep, int32_t code, char *fmt, void *data1, void *data2)
{
    char buffer[CORE_LOG_MAXLEN + 3] = {0};
    void *datas[] = {data1, data2};

    if (g_logcb_handler == NULL) {
        return;
    }

    buffer[CORE_LOG_MAXLEN] = '\r';
    buffer[CORE_LOG_MAXLEN + 1] = '\n';
    _core_log(sysdep, code, buffer, fmt, datas, 2);

    g_logcb_handler(code, buffer);
}

void core_log3(aiot_sysdep_portfile_t *sysdep, int32_t code, char *fmt, void *data1, void *data2, void *data3)
{
    char buffer[CORE_LOG_MAXLEN + 3] = {0};
    void *datas[] = {data1, data2, data3};

    if (g_logcb_handler == NULL) {
        return;
    }

    buffer[CORE_LOG_MAXLEN] = '\r';
    buffer[CORE_LOG_MAXLEN + 1] = '\n';
    _core_log(sysdep, code, buffer, fmt, datas, 3);

    g_logcb_handler(code, buffer);
}

#if 0
void core_log4(aiot_sysdep_portfile_t *sysdep, int32_t code, char *fmt, void *data1, void *data2, void *data3,
               void *data4)
{
    char buffer[CORE_LOG_MAXLEN + 3] = {0};
    void *datas[] = {data1, data2, data3, data4};

    if (g_logcb_handler == NULL) {
        return;
    }

    buffer[CORE_LOG_MAXLEN] = '\r';
    buffer[CORE_LOG_MAXLEN + 1] = '\n';
    _core_log(sysdep, code, buffer, fmt, datas, 4);

    g_logcb_handler(code, buffer);
}
#endif

void core_log_hexdump(int32_t code, char prefix, uint8_t *buffer, uint32_t len)
{
    uint32_t idx = 0, line_idx = 0, ch_idx = 0, code_len = 0;
    /* [LK-XXXX] + 1 + 1 + 16*3 + 1 + 1 + 1 + 16 + 2*/
    char hexdump[25 + 72] = {0};

    if (g_logcb_handler == NULL || len == 0) {
        return;
    }

    g_logcb_handler(code, "\r\n");
    _core_log_append_code(code, hexdump);
    code_len = strlen(hexdump);

    for (idx = 0; idx < len;) {
        memset(hexdump + code_len, ' ', 71);
        ch_idx = 2;
        hexdump[code_len + 0] = prefix;
        hexdump[code_len + 51] = '|';
        hexdump[code_len + 52] = ' ';
        for (line_idx = idx; ((line_idx - idx) < 16) && (line_idx < len); line_idx++) {
            if ((line_idx - idx) == 8) {
                ch_idx++;
            }
            core_hex2str((uint8_t *)&buffer[line_idx], 1, &hexdump[code_len + ch_idx], 0);
            hexdump[code_len + ch_idx + 2] = ' ';
            if (buffer[line_idx] >= 0x20 && buffer[line_idx] <= 0x7E) {
                hexdump[code_len + 53 + (line_idx - idx)] = buffer[line_idx];
            } else {
                hexdump[code_len + 53 + (line_idx - idx)] = '.';
            }
            ch_idx += 3;
        }
        hexdump[code_len + 69] = '\r';
        hexdump[code_len + 70] = '\n';
        idx += (line_idx - idx);
        g_logcb_handler(code, hexdump);
    }
    g_logcb_handler(code, "\r\n");
}

