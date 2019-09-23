/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _SAL_ARCH_INTERNAL_H_
#define _SAL_ARCH_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#define SAL_ARCH_TIMEOUT (~0)

/** sys_mbox_tryfetch() returns SAL_MBOX_EMPTY if appropriate.
 * For now we use the same magic value, but we allow this to change in future.
 */
#define SAL_MBOX_EMPTY SAL_ARCH_TIMEOUT

#define SAL_SEM_NULL  NULL

#define SAL_LIGHTWEIGHT_PROT  1

typedef struct
{
  void *hdl;
} sal_hdl_t;

typedef sal_hdl_t sal_sem_t;
typedef sal_hdl_t sal_mutex_t;
typedef sal_hdl_t sal_mbox_t;
typedef sal_hdl_t sal_task_t;

#define sal_sem_set_invalid(sem)     do { if(sem != NULL) { (sem)->hdl = NULL; }}while(0)

typedef uint32_t sal_prot_t;

#define sal_mutex_set_invalid(mutex) do { if(mutex != NULL) { (mutex)->hdl = NULL; }}while(0)

#define sal_mbox_set_invalid(mbox) do { if(mbox != NULL) { (mbox)->hdl = NULL; }}while(0)

typedef void *sal_thread_t;

/**
 * @ingroup sal_memory
 * allocate memory
 * @param memory size to be allocated
 * @return memory pointer if success, null if failed
 */
void *sal_malloc(uint32_t size);
/**
 * @ingroup sal_memory
 * free memory
 * @param memory pointer
 */
void sal_free(void *ptr);

/**
 * @ingroup sal_sleep
 * sleep for milliseconds
 * @param milliseconds to sleep
 */
void sal_msleep(uint32_t ms);

/**
 * @ingroup sal_mutex
 * Create a new mutex.
 * Note that mutexes are expected to not be taken recursively by the lwIP code,
 * so both implementation types (recursive or non-recursive) should work.
 * @param mutex pointer to the mutex to create
 * @return ERR_OK if successful, another err_t otherwise
 */
err_t sal_mutex_new(sal_mutex_t *mutex);
/**
 * @ingroup sal_mutex
 * Lock a mutex
 * @param mutex the mutex to lock
 */
void sal_mutex_lock(sal_mutex_t *mutex);
/**
 * @ingroup sal_mutex
 * Unlock a mutex
 * @param mutex the mutex to unlock
 */
void sal_mutex_unlock(sal_mutex_t *mutex);
/**
 * @ingroup sal_mutex
 * Delete a semaphore
 * @param mutex the mutex to delete
 */
void sal_mutex_free(sal_mutex_t *mutex);

/**
 * @ingroup sal_mutex
 * Check if a mutex is valid/allocated: return 1 for valid, 0 for invalid
 */
int sal_mutex_valid(sal_mutex_t *mutex);

#ifndef sal_mutex_set_invalid
/**
 * @ingroup sal_mutex
 * Set a mutex invalid so that sal_mutex_valid returns 0
 */
void sal_mutex_set_invalid(sal_mutex_t *mutex);
#endif
/* Semaphore functions: */

/**
 * @ingroup sal_sem
 * Create a new semaphore
 * @param sem pointer to the semaphore to create
 * @param count initial count of the semaphore
 * @return ERR_OK if successful, another err_t otherwise
 */
err_t sal_sem_new(sal_sem_t *sem, uint8_t count);
/**
 * @ingroup sal_sem
 * Signals a semaphore
 * @param sem the semaphore to signal
 */
void sal_sem_signal(sal_sem_t *sem);
/**
 * @ingroup sal_sem
 * Wait for a semaphore for the specified timeout
 * @param sem the semaphore to wait for
 * @param timeout timeout in milliseconds to wait (0 = wait forever)
 * @return time (in milliseconds) waited for the semaphore
 *         or SAL_ARCH_TIMEOUT on timeout
 */
uint32_t sal_arch_sem_wait(sal_sem_t *sem, uint32_t timeout);
/**
 * @ingroup sal_sem
 * Delete a semaphore
 * @param sem semaphore to delete
 */
void sal_sem_free(sal_sem_t *sem);
/** Wait for a semaphore - forever/no timeout */
#define sal_sem_wait(sem)                  sal_arch_sem_wait(sem, 0)

/**
 * @ingroup sal_sem
 * Check if a semaphore is valid/allocated: return 1 for valid, 0 for invalid
 */
int sal_sem_valid(sal_sem_t *sem);

#ifndef sal_sem_set_invalid
/**
 * @ingroup sal_sem
 * Set a semaphore invalid so that sal_sem_valid returns 0
 */
void sal_sem_set_invalid(sal_sem_t *sem);
#endif
#ifndef sal_sem_valid_val
/**
 * Same as sal_sem_valid() but taking a value, not a pointer
 */
#define sal_sem_valid_val(sem)       sal_sem_valid(&(sem))
#endif
#ifndef sal_sem_set_invalid_val
/**
 * Same as sal_sem_set_invalid() but taking a value, not a pointer
 */
#define sal_sem_set_invalid_val(sem) sal_sem_set_invalid(&(sem))
#endif

/* sal_mutex_arch_init() must be called before anything else. */
void sal_mutex_arch_init(void);

void sal_mutex_arch_free(void);

/* Mailbox functions. */

/**
 * @ingroup sys_mbox
 * Create a new mbox of specified size
 * @param mbox pointer to the mbox to create
 * @param size (minimum) number of messages in this mbox
 * @return ERR_OK if successful, another err_t otherwise
 */
err_t sal_mbox_new(sal_mbox_t *mbox, int size);
/**
 * @ingroup sys_mbox
 * Post a message to an mbox - may not fail
 * -> blocks if full, only used from tasks not from ISR
 * @param mbox mbox to posts the message
 * @param msg message to post (ATTENTION: can be NULL)
 */
void sal_mbox_post(sal_mbox_t *mbox, void *msg);
/**
 * @ingroup sys_mbox
 * Try to post a message to an mbox - may fail if full or ISR
 * @param mbox mbox to posts the message
 * @param msg message to post (ATTENTION: can be NULL)
 */
err_t sal_mbox_trypost(sal_mbox_t *mbox, void *msg);
/**
 * @ingroup sys_mbox
 * Wait for a new message to arrive in the mbox
 * @param mbox mbox to get a message from
 * @param msg pointer where the message is stored
 * @param timeout maximum time (in milliseconds) to wait for a message (0 = wait forever)
 * @return time (in milliseconds) waited for a message, may be 0 if not waited
           or SYS_ARCH_TIMEOUT on timeout
 *         The returned time has to be accurate to prevent timer jitter!
 */
u32_t sal_arch_mbox_fetch(sal_mbox_t *mbox, void **msg, u32_t timeout);
/* Allow port to override with a macro, e.g. special timeout for sys_arch_mbox_fetch() */
#ifndef sal_arch_mbox_tryfetch
/**
 * @ingroup sys_mbox
 * Wait for a new message to arrive in the mbox
 * @param mbox mbox to get a message from
 * @param msg pointer where the message is stored
 * @return 0 (milliseconds) if a message has been received
 *         or SAL_MBOX_EMPTY if the mailbox is empty
 */
u32_t sal_arch_mbox_tryfetch(sal_mbox_t *mbox, void **msg);
#endif
/**
 * For now, we map straight to sys_arch implementation.
 */
#define sal_mbox_tryfetch(mbox, msg) sal_arch_mbox_tryfetch(mbox, msg)
/**
 * @ingroup sys_mbox
 * Delete an mbox
 * @param mbox mbox to delete
 */
void sal_mbox_free(sal_mbox_t *mbox);

#define sal_mbox_fetch(mbox, msg) sal_arch_mbox_fetch(mbox, msg, 0)
/**
 * @ingroup sys_mbox
 * Check if an mbox is valid/allocated: return 1 for valid, 0 for invalid
 */
int sal_mbox_valid(sal_mbox_t *mbox);

#ifndef sal_mbox_set_invalid
/**
 * @ingroup sys_mbox
 * Set an mbox invalid so that sys_mbox_valid returns 0
 */
void sal_mbox_set_invalid(sal_mbox_t *mbox);
#endif

/**
 * @ingroup sal_time
 * Returns the current time in milliseconds,
 * may be the same as sal_jiffies or at least based on it.
 */
uint32_t sal_now(void);

#if SAL_PACKET_SEND_MODE_ASYNC
/**
 * @ingroup sal_task
 * @param task pointer to task
 * @param name task name
 * @param fn pointer to task function
 * @param arg pointer to task funciton arguments
 * @param stack_size stack size for task
 * @param prio task priority
 * Returns 0 success
 */
err_t sal_task_new_ext(sal_task_t *task, char *name, void *(*fn)(void *),
                       void *arg, int stack_size, int prio);
#endif

/* Critical Region Protection */
/* These functions must be implemented in the sal_arch.c file.
   In some implementations they can provide a more light-weight protection
   mechanism than using semaphores. Otherwise semaphores can be used for
   implementation */
#ifndef SAL_ARCH_PROTECT
/** SAL_LIGHTWEIGHT_PROT
 * define SAL_LIGHTWEIGHT_PROT in lwipopts.h if you want inter-task protection
 * for certain critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#if SAL_LIGHTWEIGHT_PROT

/**
 * @ingroup sal_prot
 * SAL_ARCH_DECL_PROTECT
 * declare a protection variable. This macro will default to defining a variable of
 * type sal_prot_t. If a particular port needs a different implementation, then
 * this macro may be defined in sal_arch.h.
 */
#define SAL_ARCH_DECL_PROTECT(lev) sal_prot_t lev
/**
 * @ingroup sal_prot
 * SAL_ARCH_PROTECT
 * Perform a "fast" protect. This could be implemented by
 * disabling interrupts for an embedded system or by using a semaphore or
 * mutex. The implementation should allow calling SAL_ARCH_PROTECT when
 * already protected. The old protection level is returned in the variable
 * "lev". This macro will default to calling the sal_arch_protect() function
 * which should be implemented in sal_arch.c. If a particular port needs a
 * different implementation, then this macro may be defined in sal_arch.h
 */
#define SAL_ARCH_PROTECT(lev) lev = sal_arch_protect()
/**
 * @ingroup sal_prot
 * SAL_ARCH_UNPROTECT
 * Perform a "fast" set of the protection level to "lev". This could be
 * implemented by setting the interrupt level to "lev" within the MACRO or by
 * using a semaphore or mutex.  This macro will default to calling the
 * sal_arch_unprotect() function which should be implemented in
 * sal_arch.c. If a particular port needs a different implementation, then
 * this macro may be defined in sal_arch.h
 */
#define SAL_ARCH_UNPROTECT(lev) sal_arch_unprotect(lev)
sal_prot_t sal_arch_protect(void);
void sal_arch_unprotect(sal_prot_t pval);

#else

#define SAL_ARCH_DECL_PROTECT(lev)
#define SAL_ARCH_PROTECT(lev)
#define SAL_ARCH_UNPROTECT(lev)

#endif /* SAL_LIGHTWEIGHT_PROT */

#endif /* SAL_ARCH_PROTECT */

/*
 * Macros to set/get and increase/decrease variables in a thread-safe way.
 * Use these for accessing variable that are used from more than one thread.
 */

#ifndef SAL_ARCH_INC
#define SAL_ARCH_INC(var, val) do { \
                                SAL_ARCH_DECL_PROTECT(old_level); \
                                SAL_ARCH_PROTECT(old_level); \
                                var += val; \
                                SAL_ARCH_UNPROTECT(old_level); \
                              } while(0)
#endif /* SAL_ARCH_INC */

#ifndef SAL_ARCH_DEC
#define SAL_ARCH_DEC(var, val) do { \
                                SAL_ARCH_DECL_PROTECT(old_level); \
                                SAL_ARCH_PROTECT(old_level); \
                                var -= val; \
                                SAL_ARCH_UNPROTECT(old_level); \
                              } while(0)
#endif /* SAL_ARCH_DEC */

#ifndef SAL_ARCH_GET
#define SAL_ARCH_GET(var, ret) do { \
                                SAL_ARCH_DECL_PROTECT(old_level); \
                                SAL_ARCH_PROTECT(old_level); \
                                ret = var; \
                                SAL_ARCH_UNPROTECT(old_level); \
                              } while(0)
#endif /* SAL_ARCH_GET */

#ifndef SAL_ARCH_SET
#define SAL_ARCH_SET(var, val) do { \
                                SAL_ARCH_DECL_PROTECT(old_level); \
                                SAL_ARCH_PROTECT(old_level); \
                                var = val; \
                                SAL_ARCH_UNPROTECT(old_level); \
                              } while(0)
#endif /* SAL_ARCH_SET */


#ifdef __cplusplus
}
#endif

#endif /*_SAL_ARCH_H_*/
