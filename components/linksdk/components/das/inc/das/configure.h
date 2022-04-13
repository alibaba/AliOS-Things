/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __DAS_CONFIGURE_H__
#define __DAS_CONFIGURE_H__

#define DAS_PLATFORM_LINUX 1

/* DAS heartbeat period in ms */
#define DAS_HEARTBEAT_PERIOD                    (1000*60*10) // 10min

/* servcie features */
#define DAS_SERVICE_ROM_ENABLED                 1

#ifdef DAS_PLATFORM_LINUX
#define DAS_SERVICE_FSCAN_ENABLED               1
#define DAS_SERVICE_NETFILER_ENABLED            1

#define DAS_AUTO_MEASURE                        0
#else
#define DAS_SERVICE_LWIP_NFI_ENABLED            1
#define DAS_SERVICE_STACK_PROTECTION_ENABLED    0

#define DAS_AUTO_MEASURE                        0
#endif

/* Maximum topic length */
#define DAS_TOPIC_LENGTH                        64

/* Message buffer length */
#define DAS_BUFFER_LENGTH                       1024

/* servcie configurations */
#define DAS_MAX_SERVICE_NUMBER                  8

/* servcie rom configurations */
#if (DAS_SERVICE_ROM_ENABLED)
#define DAS_SERVICE_ROM_STEP_SIZE               8192
#endif

/* servcie lwip nfi configurations */
#if (DAS_SERVICE_LWIP_NFI_ENABLED)
#define DAS_SERVICE_NFI_MAX_INFO_CNT            20
#define DAS_SERVICE_NFI_MAX_LOCAL_SVC_CNT       10
#endif

/* servcie stack protection configurations */
#if (DAS_SERVICE_STACK_PROTECTION_ENABLED)
#define DAS_SERVICE_SP_TAG_MAX_LEN 		        20

#define DAS_SERVICE_SP_BACKTRACE_MAX_DEEP   	5
#define DAS_SERVICE_SP_CALL_STACK_MAX_HASH_CNT 	20

#define DAS_SERVICE_SP_TRACE_MAX_FUNCTION_CNT 	20
#define DAS_SERVICE_SP_TRACE_MAX_REPORT_BUF_CNT 20
#endif

/* servcie netfilter configurations */
#if (DAS_SERVICE_NETFILER_ENABLED)
#define DAS_MAX_PREPROCESSOR_NUMBER             8
#define DAS_SERVICE_NETFILER_USE_NFC            0
#define DAS_SERVICE_NETFILER_USE_AFPACKET       1
#endif 

#endif /* __DAS_CONFIGURE_H__ */