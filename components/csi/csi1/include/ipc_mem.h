/*
 * Copyright (C) 2019-2020 AlibabaGroup Holding Limited
 */

/******************************************************************************
 * @file     ipc_mem.h
 * @brief    header file for the ipc mem
 * @version  V1.0
 * @date     07. Aug 2019
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/
#ifndef _IPC_MEM_H_
#define _IPC_MEM_H_

#ifdef __cplusplus
extern "C" {
#endif

void drv_ipc_mem_init(void);
void *drv_ipc_mem_alloc(int *len);
void drv_ipc_mem_free(void *p);
int drv_ipc_mem_use_cache(void);

#ifdef __cplusplus
}
#endif

#endif /* _IPC_MEM_H_ */
