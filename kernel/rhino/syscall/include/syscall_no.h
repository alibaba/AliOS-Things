#ifndef SYSCALL_NO_H
#define SYSCALL_NO_H

/* ------------------- task --------------------*/
#define K_TASK_BASE     (0)

#define SYS_KRHINO_CUR_TASK_GET (K_TASK_BASE + 0)

#define SYS_KRHINO_TASK_SLEEP (K_TASK_BASE + 1)

#define SYS_KRHINO_TASK_DEL (K_TASK_BASE + 2)

#define SYS_KRHINO_UPROCESS_CREATE (K_TASK_BASE + 3)

#define SYS_KRHINO_UTASK_CREATE (K_TASK_BASE + 4)

#define K_TASK_END      (SYS_KRHINO_UTASK_CREATE)


/* ------------------- time --------------------*/
#define K_TIME_BASE     (K_TASK_END + 1)

#define SYS_KRHINO_SYS_TIME_GET (K_TIME_BASE + 0)

#define SYS_KRHINO_SYS_TICK_GET (K_TIME_BASE + 1)

#define SYS_KRHINO_MS_TO_TICKS (K_TIME_BASE + 2)

#define SYS_KRHINO_TICKS_TO_MS (K_TIME_BASE + 3)

#define K_TIME_END      (SYS_KRHINO_TICKS_TO_MS)


/* ------------------- mutex --------------------*/
#define K_MUTEX_BASE    (K_TIME_END + 1)

#define SYS_KRHINO_MUTEX_CREATE (K_MUTEX_BASE + 0)

#define SYS_KRHINO_MUTEX_DEL (K_MUTEX_BASE + 1)

#define SYS_KRHINO_MUTEX_LOCK (K_MUTEX_BASE + 2)

#define SYS_KRHINO_MUTEX_UNLOCK (K_MUTEX_BASE + 3)

#define K_MUTEXT_END    (SYS_KRHINO_MUTEX_UNLOCK)


/* ------------------ semphore -------------------*/
#define K_SEM_BASE      (K_MUTEXT_END + 1)

#define SYS_KRHINO_SEM_CREATE (K_SEM_BASE + 0)

#define SYS_KRHINO_SEM_DEL (K_SEM_BASE + 1)

#define SYS_KRHINO_SEM_TAKE (K_SEM_BASE + 2)

#define SYS_KRHINO_SEM_GIVE (K_SEM_BASE + 3)

#define K_SEM_END   (SYS_KRHINO_SEM_GIVE)


/* ------------------ buf queue -------------------*/
#define K_BUF_QUEUE_BASE        (K_SEM_END + 1)

#define SYS_KRHINO_BUF_QUEUE_CREATE (K_BUF_QUEUE_BASE + 0)

#define SYS_KRHINO_FIX_BUF_QUEUE_CREATE (K_BUF_QUEUE_BASE + 1)

#define SYS_KRHINO_BUF_QUEUE_DEL (K_BUF_QUEUE_BASE + 2)

#define SYS_KRHINO_BUF_QUEUE_DYN_CREATE (K_BUF_QUEUE_BASE + 3)

#define SYS_KRHINO_BUF_QUEUE_DYN_DEL (K_BUF_QUEUE_BASE + 4)

#define SYS_KRHINO_BUF_QUEUE_SEND (K_BUF_QUEUE_BASE + 5)

#define SYS_KRHINO_BUF_QUEUE_RECV (K_BUF_QUEUE_BASE + 6)

#define SYS_KRHINO_BUF_QUEUE_FLUSH (K_BUF_QUEUE_BASE + 7)

#define SYS_KRHINO_BUF_QUEUE_INFO_GET (K_BUF_QUEUE_BASE + 8)

#define K_BUF_QUEUE_END     (SYS_KRHINO_BUF_QUEUE_INFO_GET)


/* ----------------- proc msg -----------------*/
#define U_PROC_MSG_BASE   (K_BUF_QUEUE_END + 1)

#define SYS_KRHINO_MSG_GET (U_PROC_MSG_BASE +0)

#define SYS_KRHINO_MSG_SND (U_PROC_MSG_BASE + 1)

#define SYS_KRHINO_MSG_RECV (U_PROC_MSG_BASE + 2)

#define U_PROC_MSG_END      (SYS_KRHINO_MSG_RECV)


/* ----------------- hal uart -----------------*/
#define HAL_UART_BASE   (U_PROC_MSG_END + 1)

#define SYS_HAL_UART_INIT (HAL_UART_BASE +0)

#define SYS_HAL_UART_SEND (HAL_UART_BASE + 1)

#define SYS_HAL_UART_RECV (HAL_UART_BASE + 2)

#define SYS_HAL_UART_RECV_II (HAL_UART_BASE + 3)

#define SYS_HAL_UART_FINALIZE (HAL_UART_BASE + 4)

#define HAL_UART_END    (SYS_HAL_UART_FINALIZE)


/* ------------------- vfs -------------------*/
#define VFS_BASE    (HAL_UART_END + 1)

#define SYS_AOS_LSEEK (VFS_BASE + 0)

#define SYS_AOS_CLOSE (VFS_BASE + 1)

#define SYS_AOS_READ (VFS_BASE + 2)

#define SYS_AOS_WRITE (VFS_BASE + 3)

#define VFS_END     (SYS_AOS_WRITE)


/* ------------------- end -------------------*/
#define MAX_SYSCALL_NO      VFS_END

#endif // SYSCALL_NO_H

