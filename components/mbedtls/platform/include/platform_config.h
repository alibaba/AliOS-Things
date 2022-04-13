#ifndef _PLAT_MBEDTLS_CONFIG_H_
#define _PLAT_MBEDTLS_CONFIG_H_

#if defined(CONFIG_CLOUDIO_ALICOAP) && defined(CONFIG_CLOUDIO_ALIMQTT)
    #include "config_yoc_aliiot.h"
#elif defined(CONFIG_CLOUDIO_ALICOAP)
    #include "config_yoc_alicoap.h"
#elif defined(CONFIG_CLOUDIO_ALIMQTT)
    #include "config_yoc_alimqtt.h"
#elif defined(CONFIG_USING_TLS)
    #include "config_yoc_tls.h"
#elif defined(CONFIG_USING_YOC_BASE)
    #include "config_yoc_base.h"
#else
    #include "config_aos_base.h"
#endif

#endif /* _PLAT_MBEDTLS_CONFIG_H_ */
