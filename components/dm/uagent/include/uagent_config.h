/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#ifndef UAGENT_CONFIG_H
#define UAGENT_CONFIG_H

#define PK_STR_SIZE 16 /* PK LEN currently is 11 */
#define DN_STR_SIZE 36 /* DN LEN currently is 32 */

/* 11Bytes PK + MAX 32bytes DN + payload */
#define UAGENT_TOPIC_MAX_LEN 64
/* uagent support main service in max */
#define UAGENT_SERVICE_SIZE  32

/* uagent support mod in max, uLog uND uOTA CLI */
#define UAGENT_MOD_SIZE                 8
#define UAGENT_PAYLOAD_CMD_SIZE         520
#define UAGENT_PAYLOAD_CMD_STR_SIZE     512
#define UAGENT_QUEUE_COUNT              4
#define UAGENT_INFO_PAYLOAD_SIZE        1200
#define UAGENT_INFO_NEAR_FULL           800
#define UAGENT_ROUTINE_TASK_STACK_DEPTH 4096

#define UAGENT_INITED     (char)1
#define UAGENT_REGISTER   (char)1
#define UAGENT_UNREGISTER (char)0

#define UAGENT_CHECK_COMM_CYCLE 200
#define UAGENT_WAIT_BUFFER      500

#define UAGENT_MSG_IDX_INVALID 0xFFFFFFFF

#define TOPIC_AGENT_MAN  "/%s/%s/user/device/man"

#define TOPIC_AGENT_INFO "/%s/%s/user/device/info"

#define UAGENT_LOG_TAG   "AGENT"

#define UAGENT_INFO_STR \
"{\"id\":%lu,\"version\":\"1.0\",\"params\":\
{\"srcdev\":\"%s\",\"show\":%d,\"mod\":%d,\"func\":%d,\"param\":\"%s\"}}"

#define UAGENT_FORMAT_PRE \
"{\"id\":%lu,\"version\":\"1.0\",\"params\":\
{\"srcdev\":\"%s\",\"show\":%d,\"mod\":%d,\"func\":%d,\"param\":"

#define UAGENT_FORMAT_STR_SUFFIX "\"}}"

#define UAGENT_FORMAT_OBJ_SUFFIX "}}"

#define UAGENT_INFO_ACK_OK \
"{\"id\":%lu,\"version\":\"1.0\",\"params\":\
{\"srcdev\":\"%s\",\"show\":%d,\"mod\":%d,\"func\":%d,\"ack\":%d}}"

#define UAGENT_INFO_ACK_FAIL \
"{\"id\":%lu,\"version\":\"1.0\",\"params\":\
{\"srcdev\":\"%s\",\"show\":%d,\"mod\":%d,\"func\":%d,\"ack\":%d,\"param\":\"%s\"}}"

#endif /* !UAGENT_CONFIG_H */
