#ifndef _INCLUDES_H_
#define _INCLUDES_H_

#include "sys_config.h"
#include "typedef.h"
#include "generic.h"
#include "compiler.h"
#if (!CFG_SUPPORT_ALIOS)
#include "arch.h"
#if (!CFG_SUPPORT_RTT)
#include "demos_config.h"
#endif
#endif
#endif // _INCLUDES_H_ 
