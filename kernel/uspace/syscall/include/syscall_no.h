/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef SYSCALL_NO_H
#define SYSCALL_NO_H

/* ------------------- task --------------------*/
#define K_TASK_BASE (0)

#define SYS_KRHINO_CUR_TASK_GET       (K_TASK_BASE + 0)
#define SYS_KRHINO_TASK_SLEEP         (K_TASK_BASE + 1)
#define SYS_KRHINO_UTASK_CREATE       (K_TASK_BASE + 2)
#define SYS_KRHINO_UTASK_DEL          (K_TASK_BASE + 3)

#define K_TASK_END (SYS_KRHINO_UTASK_DEL)

/* ------------------- process --------------------*/
#define K_UPROCESS_BASE (K_TASK_END + 1)

#define SYS_KRHINO_UPROCESS_CREATE    (K_UPROCESS_BASE + 0)
#define SYS_KRHINO_UPROCESS_EXIT      (K_UPROCESS_BASE + 1)
#define SYS_KRHINO_UPROCESS_RES_GET   (K_UPROCESS_BASE + 2)

#define K_UPROCESS_END (SYS_KRHINO_UPROCESS_RES_GET)

/* ------------------- time --------------------*/
#define K_TIME_BASE (K_UPROCESS_END + 1)

#define SYS_KRHINO_SYS_TIME_GET (K_TIME_BASE + 0)
#define SYS_KRHINO_SYS_TICK_GET (K_TIME_BASE + 1)
#define SYS_KRHINO_MS_TO_TICKS  (K_TIME_BASE + 2)
#define SYS_KRHINO_TICKS_TO_MS  (K_TIME_BASE + 3)

#define K_TIME_END (SYS_KRHINO_TICKS_TO_MS)

/* ------------------- mutex --------------------*/
#define K_MUTEX_BASE (K_TIME_END + 1)

#define SYS_KRHINO_MUTEX_CREATE     (K_MUTEX_BASE + 0)
#define SYS_KRHINO_MUTEX_DEL        (K_MUTEX_BASE + 1)
#define SYS_KRHINO_MUTEX_DYN_CREATE (K_MUTEX_BASE + 2)
#define SYS_KRHINO_MUTEX_DYN_DEL    (K_MUTEX_BASE + 3)
#define SYS_KRHINO_MUTEX_LOCK       (K_MUTEX_BASE + 4)
#define SYS_KRHINO_MUTEX_UNLOCK     (K_MUTEX_BASE + 5)

#define K_MUTEXT_END (SYS_KRHINO_MUTEX_UNLOCK)

/* ------------------ semphore -------------------*/
#define K_SEM_BASE (K_MUTEXT_END + 1)

#define SYS_KRHINO_SEM_CREATE     (K_SEM_BASE + 0)
#define SYS_KRHINO_SEM_DEL        (K_SEM_BASE + 1)
#define SYS_KRHINO_SEM_DYN_CREATE (K_SEM_BASE + 2)
#define SYS_KRHINO_SEM_DYN_DEL    (K_SEM_BASE + 3)
#define SYS_KRHINO_SEM_TAKE       (K_SEM_BASE + 4)
#define SYS_KRHINO_SEM_GIVE       (K_SEM_BASE + 5)

#define K_SEM_END (SYS_KRHINO_SEM_GIVE)

/* -------------------- queue --------------------*/
#define K_QUEUE_BASE (K_SEM_END + 1)

#define SYS_KRHINO_QUEUE_CREATE     (K_QUEUE_BASE + 0)
#define SYS_KRHINO_QUEUE_DEL        (K_QUEUE_BASE + 1)
#define SYS_KRHINO_QUEUE_DYN_CREATE (K_QUEUE_BASE + 2)
#define SYS_KRHINO_QUEUE_DYN_DEL    (K_QUEUE_BASE + 3)
#define SYS_KRHINO_QUEUE_BACK_SEND  (K_QUEUE_BASE + 4)
#define SYS_KRHINO_QUEUE_ALL_SEND   (K_QUEUE_BASE + 5)
#define SYS_KRHINO_QUEUE_RECV       (K_QUEUE_BASE + 6)
#define SYS_KRHINO_QUEUE_FLUSH      (K_QUEUE_BASE + 7)

#define K_QUEUE_END (SYS_KRHINO_QUEUE_FLUSH)

/* ------------------ buf queue -------------------*/
#define K_BUF_QUEUE_BASE (K_QUEUE_END + 1)

#define SYS_KRHINO_BUF_QUEUE_CREATE         (K_BUF_QUEUE_BASE + 0)
#define SYS_KRHINO_FIX_BUF_QUEUE_CREATE     (K_BUF_QUEUE_BASE + 1)
#define SYS_KRHINO_BUF_QUEUE_DEL            (K_BUF_QUEUE_BASE + 2)
#define SYS_KRHINO_BUF_QUEUE_DYN_CREATE     (K_BUF_QUEUE_BASE + 3)
#define SYS_KRHINO_FIX_BUF_QUEUE_DYN_CREATE (K_BUF_QUEUE_BASE + 4)
#define SYS_KRHINO_BUF_QUEUE_DYN_DEL        (K_BUF_QUEUE_BASE + 5)
#define SYS_KRHINO_BUF_QUEUE_SEND           (K_BUF_QUEUE_BASE + 6)
#define SYS_KRHINO_BUF_QUEUE_RECV           (K_BUF_QUEUE_BASE + 7)
#define SYS_KRHINO_BUF_QUEUE_FLUSH          (K_BUF_QUEUE_BASE + 8)

#define K_BUF_QUEUE_END (SYS_KRHINO_BUF_QUEUE_FLUSH)

/* ----------------- proc msg -----------------*/
#define U_PROC_MSG_BASE (K_BUF_QUEUE_END + 1)

#define SYS_KRHINO_MSG_GET  (U_PROC_MSG_BASE + 0)
#define SYS_KRHINO_MSG_SND  (U_PROC_MSG_BASE + 1)
#define SYS_KRHINO_MSG_RECV (U_PROC_MSG_BASE + 2)

#define U_PROC_MSG_END (SYS_KRHINO_MSG_RECV)

/* ----------------- hal uart -----------------*/
#define HAL_UART_BASE (U_PROC_MSG_END + 1)

#define SYS_HAL_UART_INIT     (HAL_UART_BASE + 0)
#define SYS_HAL_UART_SEND     (HAL_UART_BASE + 1)
#define SYS_HAL_UART_RECV     (HAL_UART_BASE + 2)
#define SYS_HAL_UART_RECV_II  (HAL_UART_BASE + 3)
#define SYS_HAL_UART_FINALIZE (HAL_UART_BASE + 4)

#define HAL_UART_END (SYS_HAL_UART_FINALIZE)

/* ------------------- lwip ------------------*/
#define LWIP_BASE (HAL_UART_END + 1)

#define SYS_LWIP_ACCEPT      (LWIP_BASE + 0)
#define SYS_LWIP_BIND        (LWIP_BASE + 1)
#define SYS_LWIP_SHUTDOWN    (LWIP_BASE + 2)
#define SYS_LWIP_GETPEERNAME (LWIP_BASE + 3)
#define SYS_LWIP_GETSOCKNAME (LWIP_BASE + 4)
#define SYS_LWIP_GETSOCKOPT  (LWIP_BASE + 5)
#define SYS_LWIP_SETSOCKET   (LWIP_BASE + 6)
#define SYS_LWIP_CLOSE       (LWIP_BASE + 7)
#define SYS_LWIP_CONNECT     (LWIP_BASE + 8)
#define SYS_LWIP_LISTEN      (LWIP_BASE + 9)
#define SYS_LWIP_RECV        (LWIP_BASE + 10)
#define SYS_LWIP_READ        (LWIP_BASE + 11)
#define SYS_LWIP_RECVFROM    (LWIP_BASE + 12)
#define SYS_LWIP_SEND        (LWIP_BASE + 13)
#define SYS_LWIP_SENDMSG     (LWIP_BASE + 14)
#define SYS_LWIP_SENDTO      (LWIP_BASE + 15)
#define SYS_LWIP_SOCKET      (LWIP_BASE + 16)
#define SYS_LWIP_WRITE       (LWIP_BASE + 17)
#define SYS_LWIP_WRITEV      (LWIP_BASE + 18)
#define SYS_LWIP_SELECT      (LWIP_BASE + 19)
#define SYS_LWIP_IOCTL       (LWIP_BASE + 20)
#define SYS_LWIP_FCNTL       (LWIP_BASE + 21)
#define SYS_LWIP_EVENTFD     (LWIP_BASE + 22)
#define SYS_LWIP_TRY_WAKEUP  (LWIP_BASE + 23)

#define LWIP_END (SYS_LWIP_TRY_WAKEUP)

/* ------------------- end -------------------*/
#define MAX_SYSCALL_NO (LWIP_END)

#endif /* SYSCALL_NO_H */

