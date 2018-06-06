/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <aos/aos.h>
#ifdef WITH_LWIP
#include <aos/network.h>
#endif
#include "aos/uData.h"

#include <syscall_fnum.h>


/* ---------------------Macro--------------------- */

extern const int *syscall_ftbl;
#define SYSCALL_TBL syscall_ftbl

#define SYS_CALL0(nr, t) ((t (*)(void))(SYSCALL_TBL[nr]))()
#define SYS_CALL1(nr, t, t1, p1) ((t (*)(t1))(SYSCALL_TBL[nr]))(p1)
#define SYS_CALL2(nr, t, t1, p1, t2, p2) ((t (*)(t1, t2))(SYSCALL_TBL[nr]))(p1, p2)
#define SYS_CALL3(nr, t, t1, p1, t2, p2, t3, p3) ((t (*)(t1, t2, t3))(SYSCALL_TBL[nr]))(p1, p2, p3)
#define SYS_CALL4(nr, t, t1, p1, t2, p2, t3, p3, t4, p4) ((t (*)(t1, t2, t3, t4))(SYSCALL_TBL[nr]))(p1, p2, p3, p4)
#define SYS_CALL5(nr, t, t1, p1, t2, p2, t3, p3, t4, p4, t5, p5) ((t (*)(t1, t2, t3, t4, t5))(SYSCALL_TBL[nr]))(p1, p2, p3, p4, p5)
#define SYS_CALL6(nr, t, t1, p1, t2, p2, t3, p3, t4, p4, t5, p5, t6, p6) ((t (*)(t1, t2, t3, t4, t5, t6))(SYSCALL_TBL[nr]))(p1, p2, p3, p4, p5, p6)
#define SYS_CALL7(nr, t, t1, p1, t2, p2, t3, p3, t4, p4, t5, p5, t6, p6, t7, p7) ((t (*)(t1, t2, t3, t4, t5, t6, t7))(SYSCALL_TBL[nr]))(p1, p2, p3, p4, p5, p6, p7)

/* ---------------------Function--------------------- */

/* ---------------------syscall function--------------------- */
