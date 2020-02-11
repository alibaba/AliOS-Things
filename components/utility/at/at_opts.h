#ifndef _ATPARSER_OPTS_H_
#define _ATPARSER_OPTS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define ATPSR_OPT_ENABLED                   1
#define ATPSR_OPT_DISABLED                  0

#ifndef ATPSR_SINGLE_TASK
#define ATPSR_SINGLE_TASK                   ATPSR_OPT_DISABLED
#endif

#ifndef ATPSR_DEV_MAX_NUM
#define ATPSR_DEV_MAX_NUM                   1
#endif

#ifndef AT_TASK_DEFAULT_WAIT_TIME_MS
#define AT_TASK_DEFAULT_WAIT_TIME_MS        5000
#endif

#ifndef AT_SEND_RECV_DEFAULT_TIMEOUT_MS
#define AT_SEND_RECV_DEFAULT_TIMEOUT_MS     1000
#endif

#ifndef AT_SEND_DEFAULT_PROMPT_TIMEOUT_MS
#define AT_SEND_DEFAULT_PROMPT_TIMEOUT_MS   200
#endif

#ifndef AT_RECV_DEFAULT_PREFIX
#define AT_RECV_DEFAULT_PREFIX              "\r\n"
#endif

#ifndef AT_RECV_DEFAULT_SUCCESS_POSTFIX
#define AT_RECV_DEFAULT_SUCCESS_POSTFIX     "OK\r\n"
#endif

#ifndef AT_RECV_DEFAULT_FAIL_POSTFIX
#define AT_RECV_DEFAULT_FAIL_POSTFIX        "ERROR\r\n"
#endif

#ifndef AT_SEND_DEFAULT_DELIMITER
#define AT_SEND_DEFAULT_DELIMITER           "\r"
#endif

#ifndef AT_SEND_DEFAULT_DATA_PROMPT
#define AT_SEND_DEFAULT_DATA_PROMPT         ">"
#endif

#ifndef AT_WORKER_DEFAULT_STACK_SIZE
#define AT_WORKER_DEFAULT_STACK_SIZE        1024
#endif

#ifndef AT_WORKER_DEFAULT_PRIORITY
#define AT_WORKER_DEFAULT_PRIORITY          32  /* Default AOS_DEFAULT_APP_PR */
#endif

#ifdef __cplusplus
}
#endif
#endif


