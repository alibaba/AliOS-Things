#ifndef _RECONNECT_STARTUP_H_
#define _RECONNECT_STARTUP_H_

#define RECONNECT_DEBUG

#ifdef RECONNECT_DEBUG
#define RECONNECT_PRT      os_printf
#define RECONNECT_WARN     warning_prf
#define RECONNECT_FATAL    fatal_prf
#else
#define RECONNECT_PRT      null_prf
#define RECONNECT_WARN     null_prf
#define RECONNECT_FATAL    null_prf
#endif

/*******************************************************************************
* Function Declarations
*******************************************************************************/

#endif //_RECONNECT_STARTUP_H_ 