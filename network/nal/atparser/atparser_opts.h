#ifndef _ATPARSER_OPTS_H_
#define _ATPARSER_OPTS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define ATPSR_OPT_ENABLED                   1
#define ATPSR_OPT_DISABLED                  0

#define ATPSR_SINGLE_TASK                   ATPSR_OPT_DISABLED

#define ATPSR_DEV_MAX_NUM                   1

#define AT_SEND_DATA_WAIT_PROMPT_DEFAULT    ATPSR_OPT_DISABLED

#define AT_TASK_DEFAULT_WAIT_TIME_MS        5000
#define AT_SEND_RECV_DEFAULT_TIMEOUT_MS     1000

#define AT_RECV_DEFAULT_PREFIX              "\r\n"
#define AT_RECV_DEFAULT_SUCCESS_POSTFIX     "OK\r\n"
#define AT_RECV_DEFAULT_FAIL_POSTFIX        "ERROR\r\n"
#define AT_SEND_DEFAULT_DELIMITER           "\r"
#define AT_SEND_DEFAULT_DATA_PROMPT         ">"

#define AT_WORKER_DEFAULT_STACK_SIZE        1024
#define AT_WORKER_DEFAULT_PRIORITY          32  /* Default AOS_DEFAULT_APP_PR */

#ifdef __cplusplus
}
#endif
#endif


