#ifndef _GADEGT_DEBUG_H_
#define _GADGET_DEBUG_H_

#include "diag.h"

#define GADGET_DEBUG       0

#if GADGET_DEBUG
#define GADGET_PRINT(fmt, args...)		            DBG_8195A("\n\r[%s]: " fmt, __FUNCTION__, ## args)
#define GADGET_ERROR(fmt, args...)		            DBG_8195A("\n\r[%s]: " fmt, __FUNCTION__, ## args)
#define GADGET_WARN(fmt, args...)		            DBG_8195A("\n\r[%s]: " fmt, __FUNCTION__, ## args)
#define FUN_ENTER                                   DBG_8195A("\n\r[%s ==>]\n", __func__)
#define FUN_EXIT                                    DBG_8195A("\n\r[%s <==]\n", __func__)
#define FUN_TRACE                                   DBG_8195A("\n\r[%s]:%d \n", __func__, __LINE__)
#else
#define GADGET_PRINT(fmt, args...)
#define GADGET_ERROR(fmt, args...)		            DBG_8195A("\n\r[%s]: " fmt, __FUNCTION__, ## args)
#define GADGET_WARN(fmt, args...)
#define FUN_ENTER
#define FUN_EXIT
#define FUN_TRACE
#endif

#endif