#ifndef _WRAPPERS_H_
#define _WRAPPERS_H_

#include "infra_types.h"
#include "infra_defs.h"

#include "wrappers_defs.h"
#include "wrappers_os.h"

#include "wrappers_tcp.h"

#if defined(COAP_CLIENT) || defined(COAP_SERVER)
    #include "wrappers_udp.h"
#endif

#if defined(WIFI_PROVISION_ENABLED) || defined(DEV_BIND_ENABLED)
    #include "wrappers_awss.h"
#endif

#if defined(HAL_KV)
    #include "wrappers_kv.h"
#endif

#if defined(FS_ENABLED)
    #include "wrappers_fs.h"
#endif

#if defined(AT_PARSER_ENABLED)
    #include "wrappers_uart.h"
#endif

#if defined(SUPPORT_TLS)
    #include "wrappers_tls.h"
#endif

#if defined(COAP_DTLS_SUPPORT)
    #include "wrappers_dtls.h"
#endif

#if defined(AT_TCP_ENABLED) || defined(AT_PARSER_ENABLED)
    #include "wrappers_atm.h"
#endif
#endif

