#include "lwip/opt.h"

#if LWIP_NETIF_API /* don't build if not configured for use in lwipopts.h */

#include "lwip/netifapi.h"
#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/* API for application */
/** @ingroup netifapi_netif */
#define netifapi_netif_set_link_up(n)   netifapi_netif_common(n, netif_set_link_up, NULL)
/** @ingroup netifapi_netif */
#define netifapi_netif_set_link_down(n) netifapi_netif_common(n, netif_set_link_down, NULL)

#ifdef __cplusplus
}
#endif

#endif /* LWIP_NETIF_API */

