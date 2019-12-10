#ifndef _HTTP_DNS_OPT_H_
#define _HTTP_DNS_OPT_H_

#ifndef HTTP_DNS_DISABLED
#define HTTP_DNS_DISABLED 0
#endif

#ifndef HTTP_DNS_USER_ID
#define HTTP_DNS_USER_ID "189323"
#endif

#ifndef HTTP_DNS_TASK_PRIO
#define HTTP_DNS_TASK_PRIO  32
#endif

#ifndef HTTP_DNS_TASK_STACK
#define HTTP_DNS_TASK_STACK (8 * 1024)
#endif

#ifndef HTTP_DNS_QRY_INTV_MS
#define HTTP_DNS_QRY_INTV_MS 50
#endif

const char server_list[ ][16] = {
    "203.107.1.1",
    "203.107.1.33",
    "203.107.1.34",
    "203.107.1.35",
    "203.107.1.65",
    "203.107.1.66"
};

#endif
