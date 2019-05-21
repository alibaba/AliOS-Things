/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "include.h"
#include "lite-log.h"
#include "lite-log_internal.h"
#include "uart_pub.h"
#include "str_pub.h"
#include "mem_pub.h"


#define LITE_HEXDUMP_DRAWLINE(start_mark, len, end_mark)    \
    do { \
        int                     i; \
        \
        LITE_printf("%s", start_mark); \
        for(i = 0; i < len; ++i) { LITE_printf("-"); } \
        LITE_printf("%s", end_mark); \
        LITE_printf("\r\n"); \
        \
    } while(0)

int LITE_hexdump(const char *title, const void *buff, const int len)
{
    int                     i, j, written;
    unsigned char           ascii[16 + 1] = {0};
    char                    header[64] = {0};
    unsigned char          *buf = (unsigned char *)buff;

    LITE_snprintf(header, sizeof(header), "| %s: (len=%d) |\r\n", title, (int)len);

    LITE_HEXDUMP_DRAWLINE("+", os_strlen(header) - 4, "+");
    LITE_printf("%s", header);
    LITE_printf("%s\r\n", HEXDUMP_SEP_LINE);

    written = 0;
    for (i = 0; i < len; ++ i) {
        if (i % 16 == 0) {
            LITE_printf("| %08X: ", (unsigned int)(i + (long)buff));
            written += 8;
        }

        LITE_printf("%02X", buf[i]);
        written += 2;

        if (i % 2 == 1) {
            LITE_printf(" ");
            written += 1;
        }
        LITE_sprintf((char *__restrict__)ascii + i % 16, "%c", ((buf[i] >= ' ' && buf[i] <= '~') ?  buf[i] : '.'));

        if (((i + 1) % 16 == 0) || (i == len - 1)) {
            for (j = 0; j < 48 - written; ++j) {
                LITE_printf(" ");
            }

            LITE_printf(" %s", ascii);
            LITE_printf("\r\n");

            written = 0;
            os_memset(ascii, 0, sizeof(ascii));
        }
    }
    LITE_printf("%s\r\n", HEXDUMP_SEP_LINE);

    return 0;
}

#if defined(LITE_LOG_ENABLED)
static log_client logcb;

static char *lvl_names[] = {
    "emg", "crt", "err", "wrn", "inf", "dbg",
};

void LITE_syslog(const char *f, const int l, const int level, const char *fmt, ...)
{
    char       *tmpbuf = logcb.text_buf;
    char       *o = tmpbuf;
    int         truncated = 0;
    va_list     ap;

    if (!os_strlen(LITE_get_logname()) || LITE_get_loglevel() < level || level < LOG_EMERG_LEVEL) {
        return;
    }

    LITE_printf(LOG_PREFIX_FMT, lvl_names[level], f, l);

    os_memset(tmpbuf, 0, sizeof(logcb.text_buf));
    va_start(ap, fmt);
    o += vsnprintf(o, LOG_MSG_MAXLEN + 1, fmt, ap);
    va_end(ap);

    if (o - tmpbuf > LOG_MSG_MAXLEN) {
        o = tmpbuf + os_strlen(tmpbuf);
        truncated = 1;
    }
    if (os_strlen(tmpbuf) == LOG_MSG_MAXLEN) {
        truncated = 1;
    }

    LITE_printf("%s", tmpbuf);
    if (truncated) {
        LITE_printf(" ...");
    }

    LITE_printf("\r\n");
    return;
}

int LITE_log_enabled(void)
{
    return 1;
}

void LITE_openlog(const char *ident)
{
    os_strncpy(logcb.name, ident, LOG_MOD_NAME_LEN);
    logcb.name[LOG_MOD_NAME_LEN] = 0;
    logcb.priority = 0;
}

void LITE_closelog(void)
{
    os_strncpy(logcb.name, "", LOG_MOD_NAME_LEN);
    logcb.name[LOG_MOD_NAME_LEN] = 0;
    logcb.priority = 0;
}

char *LITE_get_logname(void)
{
    return logcb.name;
}

int LITE_get_loglevel(void)
{
    return logcb.priority;
}

void LITE_set_loglevel(int pri)
{
    logcb.priority = pri;
}

void LITE_rich_hexdump(const char *f, const int l,
                       const int level,
                       const char *buf_str,
                       const void *buf_ptr,
                       const int buf_len)
{
    if (LITE_get_loglevel() < level) {
        return;
    }

    LITE_printf(LOG_PREFIX_FMT, lvl_names[LITE_get_loglevel()], f, l);
    LITE_printf("HEXDUMP %s @ %p[%d]\r\n", buf_str, buf_ptr, buf_len);
    LITE_hexdump(buf_str, buf_ptr, buf_len);

    return;
}

int log_multi_line_internal(const char *f, const int l,
                            const char *title, int level, char *payload, const char *mark)
{
    const char     *pos;
    const char     *endl;
    int             i;

    if (LITE_get_loglevel() < level) {
        return 1;
    }

    LITE_printf("[%s] %s(%d): %s (Length: %d Bytes)\r\n",
                lvl_names[LITE_get_loglevel()], f, l, title, (int)strlen(payload));

    pos = payload;
    while (pos && *pos) {
        LITE_printf("%s ", mark);

        if (*pos == '\r') {
            LITE_printf("\r\n");
            pos += 2;
            continue;
        }

        endl = os_strchr(pos + 1, '\r');
        if (endl == NULL) {
            endl = pos;
            do {
                ++endl;
            } while (*endl);
        }

        for (i = 0; i < endl - pos; ++i) {
            LITE_printf("%c", pos[i]);
        }
        LITE_printf("\r\n");

        pos = *endl ? endl + 2 : 0;
    }

    return 0;
}

#else   /* defined(LITE_LOG_ENABLED) */
void LITE_syslog(const char *f, const int l, const int level, const char *fmt, ...)
{

    return;
}

int log_multi_line_internal(const char *f, const int l,
                            const char *title,
                            int level,
                            char *payload,
                            const char *mark)
{

    return 0;
}


int LITE_log_enabled(void)
{
    return 0;
}
void LITE_openlog(const char *ident)
{
    return;
}
void LITE_closelog(void)
{
    return;
}
char *LITE_get_logname(void)
{
    return NULL;
}
int LITE_get_loglevel(void)
{
    return -1;
}
void LITE_set_loglevel(int lvl)
{
    return;
}
void LITE_rich_hexdump(const char *f, const int l,
                       const int level,
                       const char *buf_str,
                       const void *buf_ptr,
                       const int buf_len)
{
    return;
}

#endif  /* defined(LITE_LOG_ENABLED) */

