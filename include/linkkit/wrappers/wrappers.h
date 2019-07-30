#ifndef _WRAPPERS_H_
#define _WRAPPERS_H_

#include "linkkit/infra/infra_types.h"
#include "linkkit/infra/infra_defs.h"

#include "linkkit/wrappers/wrappers_defs.h"
#include "linkkit/wrappers/wrappers_os.h"

#include "linkkit/wrappers/wrappers_tcp.h"

#if defined(COAP_CLIENT) || defined(COAP_SERVER)
    #include "linkkit/wrappers/wrappers_udp.h"
#endif

#if defined(WIFI_PROVISION_ENABLED) || defined(DEV_BIND_ENABLED)
    #include "linkkit/wrappers/wrappers_awss.h"
#endif

#if defined(HAL_KV)
    #include "linkkit/wrappers/wrappers_kv.h"
#endif

#if defined(FS_ENABLED)
    #include "linkkit/wrappers/wrappers_fs.h"
#endif

#if defined(AT_PARSER_ENABLED)
    #include "linkkit/wrappers/wrappers_uart.h"
#endif

#if defined(SUPPORT_TLS)
    #include "linkkit/wrappers/wrappers_tls.h"
#endif

#if defined(COAP_DTLS_SUPPORT)
    #include "linkkit/wrappers/wrappers_dtls.h"
#endif

#if defined(AT_TCP_ENABLED) || defined(AT_PARSER_ENABLED)
    #include "linkkit/wrappers/wrappers_atm.h"
#endif
#endif

