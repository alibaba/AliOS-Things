#ifndef _PTHREAD_H_
#define _PTHREAD_H_
//#include <stddef.h>
//#include <sys/timespec.h>
#include <sys/time.h>
#include <time.h>
//#include <emod.h>
#include "kernel/os/os.h"

#define ONCE_INITIALIZING           (1 << 0)
#define ONCE_COMPLETED              (1 << 1)
#define PTHREAD_ONCE_INIT 			0


#if defined(__LP64__)
  #define __RESERVED_INITIALIZER , {0}
#else
  #define __RESERVED_INITIALIZER
#endif

typedef long long int64_t;
typedef int pthread_key_t;
typedef void * pthread_t;
typedef volatile int pthread_once_t;
typedef int  pid_t;
typedef long time_t;

typedef enum MELIS_2_PTHREAD_OS_STATE
{
    PTHREAD_TASK_STATE_READY = 0,
    PTHREAD_TASK_STATE_RUNNING,
    PTHREAD_TASK_STATE_EXIT,
    PTHREAD_TASK_STATE_INVALID,
}pthread_osstate_t;


/*typedef struct {
	unsigned int 	flag;
	unsigned int 	entercnt;
	pthread_t       owner;
	__krnl_event_t* lock;
}pthread_mutex_t;*/

typedef struct {
//	unsigned int 	flag;
//	unsigned int 	entercnt;
//	pthread_t       owner;
	OS_Mutex_t lock;
} pthread_mutex_t;

typedef struct {
	int cnt;
	int wait;
	OS_Semaphore_t sem;
} sem_t;

/*struct timespec {
	long tv_sec;
	long tv_nsec;
};

struct timeval{
 long tv_sec;
 long tv_usec;
};

struct timezone {
 int tz_minuteswest;
 int tz_dsttime;
};*/

//#define CLOCK_REALTIME  						0
#define CLOCK_MONOTONIC 						(CLOCK_REALTIME + 1)
#define  SEM_MAX_VALUE             			   0xffff
#define  ETIMEOUT							   21
#define  EINVAL 							   22
#define  __PTHREAD_MUTEX_INIT_VALUE            0
#define  __PTHREAD_RECURSIVE_MUTEX_INIT_VALUE  0x4000
#define  __PTHREAD_ERRORCHECK_MUTEX_INIT_VALUE 0x8000

#define  PTHREAD_MUTEX_INITIALIZER             {__PTHREAD_MUTEX_INIT_VALUE __RESERVED_INITIALIZER}
#define  PTHREAD_RECURSIVE_MUTEX_INITIALIZER   {__PTHREAD_RECURSIVE_MUTEX_INIT_VALUE __RESERVED_INITIALIZER}
#define  PTHREAD_ERRORCHECK_MUTEX_INITIALIZER  {__PTHREAD_ERRORCHECK_MUTEX_INIT_VALUE __RESERVED_INITIALIZER}

enum {
    PTHREAD_MUTEX_NORMAL = 0,
    PTHREAD_MUTEX_RECURSIVE = 1,
    PTHREAD_MUTEX_ERRORCHECK = 2,

    PTHREAD_MUTEX_ERRORCHECK_NP = PTHREAD_MUTEX_ERRORCHECK,
    PTHREAD_MUTEX_RECURSIVE_NP  = PTHREAD_MUTEX_RECURSIVE,

    PTHREAD_MUTEX_DEFAULT = PTHREAD_MUTEX_NORMAL
};

enum
{
   PTHREAD_EVENT_SET_0            = 1<<0,
   PTHREAD_EVENT_SET_1            = 1<<1,
   PTHREAD_EVENT_SET_2            = 1<<2,
   PTHREAD_EVENT_SET_3    		  = 1<<3,
   PTHREAD_EVENT_SET_4            = 1<<4,
   PTHREAD_EVENT_SET_5            = 1<<5,
   PTHREAD_EVENT_SET_6            = 1<<6
};


typedef sem_t pthread_cond_t;

#define PTHREAD_COND_INITIALIZER  {0 __RESERVED_INITIALIZER}

typedef struct {
  unsigned int flags;
  void* stack_base;
  size_t stack_size;
  size_t guard_size;
  int sched_policy;
  int sched_priority;
#ifdef __LP64__
  char __reserved[16];
#endif
} pthread_attr_t;

typedef long pthread_mutexattr_t;
typedef long pthread_condattr_t;

typedef long pthread_rwlockattr_t;

typedef struct {
#if !defined(__LP64__)
  pthread_mutex_t __unused_lock;
  pthread_cond_t __unused_cond;
#endif
  volatile int state; // 0=unlock, -1=writer lock, +n=reader lock
  volatile int writer_thread_id;
  volatile int pending_readers;
  volatile int pending_writers;
  int attr;
#ifdef __LP64__
  char __reserved[36];
#else
  char __reserved[12];
#endif

} pthread_rwlock_t;

#ifdef __LP64__
  #define PTHREAD_RWLOCK_INITIALIZER  { 0, 0, 0, 0, 0, { 0 } }
#else
  #define PTHREAD_RWLOCK_INITIALIZER  { PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0, 0, 0, 0, 0, { 0 } }
#endif

#define PTHREAD_ONCE_INIT 0

#if defined(__LP64__)
#define PTHREAD_STACK_MIN (4 * PAGE_SIZE)
#else
#define PTHREAD_STACK_MIN (2 * PAGE_SIZE)
#endif

#define PTHREAD_CREATE_DETACHED  0x00000001
#define PTHREAD_CREATE_JOINABLE  0x00000000

#define PTHREAD_PROCESS_PRIVATE  0
#define PTHREAD_PROCESS_SHARED   1

#define PTHREAD_SCOPE_SYSTEM     0
#define PTHREAD_SCOPE_PROCESS    1

typedef void (*__pthread_cleanup_func_t)(void*);

typedef struct __pthread_cleanup_t {
  struct __pthread_cleanup_t*   __cleanup_prev;
  __pthread_cleanup_func_t      __cleanup_routine;
  void*                         __cleanup_arg;
} __pthread_cleanup_t;

#ifdef __cplusplus
extern "C"
{
#endif
	pid_t gettid(void);
	int pthread_atfork(void (*)(void), void (*)(void), void(*)(void));
	int pthread_attr_destroy(pthread_attr_t*);
	int pthread_attr_getdetachstate(const pthread_attr_t*, int*);
	int pthread_attr_getguardsize(const pthread_attr_t*, size_t*);
	int pthread_attr_getschedpolicy(const pthread_attr_t*, int*);
	int pthread_attr_getscope(const pthread_attr_t*, int*);
	int pthread_attr_getstack(const pthread_attr_t*, void**, size_t*);
	int pthread_attr_getstacksize(const pthread_attr_t*, size_t*) ;
	int pthread_attr_init(pthread_attr_t*);
	int pthread_attr_setdetachstate(pthread_attr_t*, int);
	int pthread_attr_setguardsize(pthread_attr_t*, size_t);
	int pthread_attr_setschedpolicy(pthread_attr_t*, int);
	int pthread_attr_setscope(pthread_attr_t*, int);
	int pthread_attr_setstack(pthread_attr_t*, void*, size_t);
	int pthread_attr_setstacksize(pthread_attr_t*, size_t stack_size);

	int pthread_condattr_destroy(pthread_condattr_t*);
	int pthread_condattr_getpshared(const pthread_condattr_t*, int*);
	int pthread_condattr_init(pthread_condattr_t*);
	int pthread_condattr_setpshared(pthread_condattr_t*, int);

	int pthread_cond_broadcast(pthread_cond_t*);
	int pthread_cond_destroy(pthread_cond_t*);
	int pthread_cond_init(pthread_cond_t*, const pthread_condattr_t*);
	int pthread_cond_signal(pthread_cond_t*);
	int pthread_cond_timedwait(pthread_cond_t*, pthread_mutex_t*, const struct timespec*);
	int pthread_cond_wait(pthread_cond_t*, pthread_mutex_t*);

	int pthread_create(pthread_t*, pthread_attr_t const*, void *(*)(void*), void*);
	int pthread_detach(pthread_t);
	void pthread_exit(void*);

	int pthread_equal(pthread_t, pthread_t);

	int pthread_getattr_np(pthread_t, pthread_attr_t*);
	void* pthread_getspecific(pthread_key_t);

	pid_t pthread_gettid_np(pthread_t);

	int pthread_join(pthread_t, void**);

	int pthread_key_create(pthread_key_t*, void (*)(void*));
	int pthread_key_delete(pthread_key_t);

	int pthread_kill(pthread_t, int);

	int pthread_mutexattr_destroy(pthread_mutexattr_t*);
	int pthread_mutexattr_getpshared(const pthread_mutexattr_t*, int*);
	int pthread_mutexattr_gettype(const pthread_mutexattr_t*, int*);
	int pthread_mutexattr_init(pthread_mutexattr_t*) ;
	int pthread_mutexattr_setpshared(pthread_mutexattr_t*, int);
	int pthread_mutexattr_settype(pthread_mutexattr_t*, int);

	int pthread_mutex_destroy(pthread_mutex_t*);
	int pthread_mutex_init(pthread_mutex_t*, const pthread_mutexattr_t*);
	int pthread_mutex_lock(pthread_mutex_t*) ;
	int pthread_mutex_timedlock(pthread_mutex_t*, const struct timespec*);
	int pthread_mutex_trylock(pthread_mutex_t*) ;
	int pthread_mutex_unlock(pthread_mutex_t*) ;

	int pthread_once(pthread_once_t*, void (*)(void));

	int pthread_rwlockattr_destroy(pthread_rwlockattr_t*);
	int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t*, int*);
	int pthread_rwlockattr_init(pthread_rwlockattr_t*);
	int pthread_rwlockattr_setpshared(pthread_rwlockattr_t*, int);

	int pthread_rwlock_destroy(pthread_rwlock_t*);
	int pthread_rwlock_init(pthread_rwlock_t*, const pthread_rwlockattr_t*) ;
	int pthread_rwlock_rdlock(pthread_rwlock_t*) ;
	int pthread_rwlock_timedrdlock(pthread_rwlock_t*, const struct timespec*);
	int pthread_rwlock_timedwrlock(pthread_rwlock_t*, const struct timespec*);
	int pthread_rwlock_tryrdlock(pthread_rwlock_t*);
	int pthread_rwlock_trywrlock(pthread_rwlock_t*);
	int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
	int pthread_rwlock_wrlock(pthread_rwlock_t*);
	time_t time(time_t *t);

	pthread_t pthread_self(void);

	int pthread_setname_np(pthread_t, const char*);
	int pthread_setspecific(pthread_key_t, const void*);



	extern void __pthread_cleanup_push(__pthread_cleanup_t* c, __pthread_cleanup_func_t, void*);
	extern void __pthread_cleanup_pop(__pthread_cleanup_t*, int);

	int sem_init(sem_t *sem, int pshared, unsigned int value);
	int sem_destroy(sem_t *sem);
	int sem_wait (sem_t *sem);
	int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
	int sem_post(sem_t *sem);
	int sem_trywait(sem_t *sem);
	int sem_getvalue(sem_t *sem, int *sval);
	int sem_unlink(const char* name);
	int sem_close(sem_t *sem);
	sem_t *sem_open(const char *name, int oflag, ...);
#define gettimeofday	__gettimeofday
	int __gettimeofday(struct timeval* tv, struct timezone* tz);
	void cdx_abort(void);
	int clock_gettime(int a,struct timespec *t);
	void exit(int status);
#ifdef __cplusplus
}
#endif

#endif /* _PTHREAD_H_ */
