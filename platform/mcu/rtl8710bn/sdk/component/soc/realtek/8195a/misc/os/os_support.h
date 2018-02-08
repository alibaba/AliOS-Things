 /******************************************************************************
  *
  * Name: sys-support.h - System type support for Linux
  *       $Revision: 1.1.1.1 $
  *
  *****************************************************************************/

#ifndef __OS_SUPPORT_H__
#define __OS_SUPPORT_H__


#include <FreeRTOS.h>
#include <basic_types.h>
#include "task.h"
#include "osdep_service.h"

#define RTL_HZ                          100

#define RtlKmalloc(size, flag)          pvPortMalloc(size)
#define RtlKfree(pv)                    vPortFreeAligned(pv)


#ifdef CONFIG_TIMER_MODULE
#define __Delay(t)                  HalDelayUs(t)
#else
static __inline__ u32 __Delay(u32 us)
{
    DBG_8195A("No Delay: please enable hardware Timer\n");
}
#endif


#define Mdelay(t)					__Delay(t*1000)
#define Udelay(t)					__Delay(t)

#endif /* __SYS_SUPPORT_H__ */
