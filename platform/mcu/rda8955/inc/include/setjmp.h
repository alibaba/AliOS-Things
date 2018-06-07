/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/












#ifndef SETJMP_H
#define SETJMP_H
#ifdef __cplusplus
extern "C" {
#endif


#ifdef FPU
typedef unsigned long jmp_buf[22];

#else
typedef unsigned long jmp_buf[12];
#endif

int  setjmp (jmp_buf env);

volatile void longjmp (jmp_buf env,  int value);

#ifdef __cplusplus
}
#endif
#endif //SETJMP_H

