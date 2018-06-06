/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_ADAPTER_TIMEOUT_H
#define AOS_ADAPTER_TIMEOUT_H


typedef struct 
{
    UINT8 count;
    UINT8 mbx;
}sxr_rhino_sema;


extern BOOL sxr_rhino_SemaTryTake(sxr_rhino_sema *sema, UINT32 ms);
extern VOID sxr_rhino_SemaRelease(sxr_rhino_sema *sema);
extern  VOID sxr_rhino_SemaInit(sxr_rhino_sema *sema, UINT32 init);

#endif /* AOS_ADAPTER_TIMEOUT_H */
