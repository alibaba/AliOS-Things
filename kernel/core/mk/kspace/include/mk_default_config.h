/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef U_DEFAULT_CONFIG_H
#define U_DEFAULT_CONFIG_H

#ifndef RHINO_CONFIG_PROTECTED
    #define RHINO_CONFIG_PROTECTED 1
#endif

#ifndef RHINO_CONFIG_DYN_SYSCALL_TBL
    #define RHINO_CONFIG_DYN_SYSCALL_TBL 0
#endif

/* Support user space task delete, kernel can
 * malloc, free buffer to user space heap */
#ifndef RHINO_CONFIG_URES
    #define RHINO_CONFIG_URES 1
#endif

/* Support user space CLI */
// #ifndef RHINO_CONFIG_UCLI
// #define RHINO_CONFIG_UCLI 0
// #endif

// #ifndef RHINO_CONFIG_UIPC
// #define RHINO_CONFIG_UIPC 0
// #endif

/**
 * the max num onwer of IPC
 */
#ifndef RHINO_CONFIG_PROC_MSG_MAX_OWNER_NUM
    #define RHINO_CONFIG_PROC_MSG_MAX_OWNER_NUM 8
#endif

/* Support user space file system */
#ifndef RHINO_CONFIG_UFS
    #define RHINO_CONFIG_UFS 0
#endif

/* Support user space TCPIP utility */
#ifndef RHINO_CONFIG_UTCPIP
    #define RHINO_CONFIG_UTCPIP 0
#endif

/*----------------- LWIP ----------------*/
#if (RHINO_CONFIG_UTCPIP > 0)
    #define RHINO_CONFIG_LWIP_SYSCALL 1
#endif

/**
 * Support user space callback, kernel sends
 * callback msg to user space agent(callback task),
 * the callback is executed in user space
 */
#ifndef RHINO_CONFIG_UCALLBACK
    #define RHINO_CONFIG_UCALLBACK 0
#endif

#endif /* U_DEFAULT_CONFIG_H */

