/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __DAS_SERVICE_H__
#define __DAS_SERVICE_H__

#include <das/platform.h>
#include <das/sha1.h>
#include <das.h>

#define das_sum_context_t das_sha1_context_t
#define das_mac_context_t das_sha1_context_t

#define das_sum_start  das_sha1_start
#define das_mac_start  das_sha1_start

#define das_sum_update das_sha1_update
#define das_mac_update das_sha1_update

#define das_sum_finish das_sha1_finish
#define das_mac_finish das_sha1_finish

typedef enum _das_service_status {
    DAS_SRV_STATUS_START = 0,
    DAS_SRV_STATUS_NEXT,
    DAS_SRV_STATUS_PUBLISH,
    DAS_SRV_STATUS_FINISH,
} das_service_status_t;

typedef struct _das_service_state {
    das_service_status_t status;
} das_service_state_t;

typedef struct _das_service {
    const char *name;
    int (*init)(void);
    das_result_t (*info)(char *buffer, 
                        size_t size, 
                        das_service_state_t *state);
    das_result_t (*attest)(char *path, 
                        size_t path_size, 
                        char* attribute, 
                        size_t attribute_size, 
                        das_sum_context_t *sum_context, 
                        das_service_state_t *state);
    das_result_t (*measure)(das_sum_context_t *sum_context, 
                            das_mac_context_t *mac_context, 
                            das_service_state_t *state);
} das_service_t;

extern das_service_t * das_service_table[];

typedef struct _das_attest_handler {
	const char *tag;
	int (*handler)(va_list params);
} das_attest_handler_t;

extern das_attest_handler_t* das_attest_handler_table[];

#define FIELDS_SEPARATE_TAG		'|'
#define INFOS_SEPARATE_TAG		','

int stack_protection(const char *tag);

#if (DAS_SERVICE_NETFILER_ENABLED)
#include <arpa/inet.h>

#if (DAS_SERVICE_NETFILER_USE_NFC)
#include <internal/internal.h>
#endif /* DAS_SERVICE_NETFILER_USE_NFC */

union _das_address {
    struct in_addr v4;
    struct in6_addr v6;
};

typedef struct _netflow_info {
    union _das_address src_ip;  // ip & port are network bytes order
    union _das_address dest_ip;
    uint16_t src_port;
    uint16_t dest_port;
    uint16_t domain;            // AF_INET/AF_INET6...
    uint16_t protocol;          // tcp/udp/icmp... 
    uint16_t direction;         // IN = 1; OUT = 2
    uint32_t reserved;
} netflow_info_t;

typedef enum _pre_process_status {
    PRE_PROCESS_REPORT_Q_FULL = -3, // the report queue is full. max = MAX_INFO_COUNT
    PRE_PROCESS_BUF_TOO_SMALL = -2, // report buf is too small.
    PRE_PROCESS_GENERAL_ERROR = -1, // general error, such as parameters error.
    PRE_PROCESS_REPORT_IGNORE = 0,  // do not report the netflow info.
    PRE_PROCESS_REPORT_CUSTOM = 1,  // report customer infos.
    PRE_PROCESS_CALL_NEXT = 2,      // call next pre processor.
} pre_process_status_t;

typedef struct _netfilter_pre_processor {
    const char *name;
    int (*init)(void);
    pre_process_status_t (*pre_process)(netflow_info_t *info, 
                                        void *payload, 
                                        size_t payload_size, 
                                        char *buf, 
                                        size_t buf_size);
} netfilter_pre_processor_t;

extern netfilter_pre_processor_t *netfilter_pre_processor_table[];
#endif // DAS_SERVICE_NETFILER_ENABLED

#endif /* __DAS_SRV_H__ */
