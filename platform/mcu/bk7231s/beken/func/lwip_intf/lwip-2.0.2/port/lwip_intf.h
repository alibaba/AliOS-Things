#ifndef __LWIP_INTF_H__
#define __LWIP_INTF_H__

#define LWIP_INTF_DEBUG
#ifdef LWIP_INTF_DEBUG
#define LWIP_INTF_PRT      warning_prf
#define LWIP_INTF_WARN     warning_prf
#define LWIP_INTF_FATAL    fatal_prf
#else
#define LWIP_INTF_PRT      null_prf
#define LWIP_INTF_WARN     null_prf
#define LWIP_INTF_FATAL    null_prf
#endif

#endif
// eof

