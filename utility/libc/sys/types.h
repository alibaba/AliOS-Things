/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/* unified sys/types.h: 
   start with sef's sysvi386 version.
   merge go32 version -- a few ifdefs.
   h8300hms, h8300xray, and sysvnecv70 disagree on the following types:

   typedef int gid_t;
   typedef int uid_t;
   typedef int dev_t;
   typedef int ino_t;
   typedef int mode_t;
   typedef int caddr_t;

   however, these aren't "reasonable" values, the sysvi386 ones make far 
   more sense, and should work sufficiently well (in particular, h8300 
   doesn't have a stat, and the necv70 doesn't matter.) -- eichin
 */

#ifndef _SYS_TYPES_H

#include <_ansi.h>
#include <sys/cdefs.h>
#include <machine/_types.h>

/* BSD types permitted by POSIX and always exposed as in Glibc.  Only provided
   for backward compatibility with BSD code.  The uintN_t standard types should
   be preferred in new code. */
#if ___int8_t_defined
typedef __uint8_t	u_int8_t;
#endif
#if ___int16_t_defined
typedef __uint16_t	u_int16_t;
#endif 
#if ___int32_t_defined
typedef __uint32_t	u_int32_t;
#endif
#if ___int64_t_defined
typedef __uint64_t	u_int64_t;
#endif
typedef int register_t;
#define __BIT_TYPES_DEFINED__ 1

#if defined(__rtems__) || defined(__XMK__)
/*
 *  The following section is RTEMS specific and is needed to more
 *  closely match the types defined in the BSD sys/types.h.
 *  This is needed to let the RTEMS/BSD TCP/IP stack compile.
 */

/* deprecated */
#if ___int64_t_defined
typedef	__uint64_t	u_quad_t;
typedef	__int64_t	quad_t;
typedef	quad_t *	qaddr_t;
#endif

#endif /* __rtems__ || __XMK__ */

#ifndef __need_inttypes

#define _SYS_TYPES_H
/* <stddef.h> must be before <sys/_types.h> for __size_t considerations */
#include <stddef.h>
#include <sys/_types.h>
#include <sys/_stdint.h>

#if __BSD_VISIBLE
#include <machine/endian.h>
#include <sys/select.h>
#  define	physadr		physadr_t
#  define	quad		quad_t

#ifndef _IN_ADDR_T_DECLARED
typedef	__uint32_t	in_addr_t;	/* base type for internet address */
#define	_IN_ADDR_T_DECLARED
#endif

#ifndef _IN_PORT_T_DECLARED
typedef	__uint16_t	in_port_t;
#define	_IN_PORT_T_DECLARED
#endif
#endif /* __BSD_VISIBLE */

#if __MISC_VISIBLE
#ifndef _BSDTYPES_DEFINED
/* also defined in mingw/gmon.h and in w32api/winsock[2].h */
#ifndef __u_char_defined
typedef	unsigned char	u_char;
#define __u_char_defined
#endif
#ifndef __u_short_defined
typedef	unsigned short	u_short;
#define __u_short_defined
#endif
#ifndef __u_int_defined
typedef	unsigned int	u_int;
#define __u_int_defined
#endif
#ifndef __u_long_defined
typedef	unsigned long	u_long;
#define __u_long_defined
#endif
#define _BSDTYPES_DEFINED
#endif
#endif	/*__BSD_VISIBLE || __CYGWIN__ */

#if __MISC_VISIBLE
typedef	unsigned short	ushort;		/* System V compatibility */
typedef	unsigned int	uint;		/* System V compatibility */
typedef	unsigned long	ulong;		/* System V compatibility */
#endif

#ifndef _BLKCNT_T_DECLARED
typedef	__blkcnt_t	blkcnt_t;
#define	_BLKCNT_T_DECLARED
#endif

#ifndef _BLKSIZE_T_DECLARED
typedef	__blksize_t	blksize_t;
#define	_BLKSIZE_T_DECLARED
#endif

#if !defined(__clock_t_defined) && !defined(_CLOCK_T_DECLARED)
typedef	_CLOCK_T_	clock_t;
#define	__clock_t_defined
#define	_CLOCK_T_DECLARED
#endif

#if !defined(__time_t_defined) && !defined(_TIME_T_DECLARED)
typedef	_TIME_T_	time_t;
#define	__time_t_defined
#define	_TIME_T_DECLARED
#endif

#ifndef __daddr_t_defined
typedef	long	daddr_t;
#define __daddr_t_defined
#endif
#ifndef __caddr_t_defined
typedef	char *	caddr_t;
#define __caddr_t_defined
#endif

#ifndef _FSBLKCNT_T_DECLARED		/* for statvfs() */
typedef	__fsblkcnt_t	fsblkcnt_t;
typedef	__fsfilcnt_t	fsfilcnt_t;
#define	_FSBLKCNT_T_DECLARED
#endif

#ifndef _ID_T_DECLARED
typedef	__id_t		id_t;		/* can hold a uid_t or pid_t */
#define	_ID_T_DECLARED
#endif

#ifndef _INO_T_DECLARED
typedef	__ino_t		ino_t;		/* inode number */
#define	_INO_T_DECLARED
#endif

#if defined(__i386__) && (defined(GO32) || defined(__MSDOS__))
typedef	char *		addr_t;
typedef unsigned long vm_offset_t;
typedef unsigned long vm_size_t;
#endif /* __i386__ && (GO32 || __MSDOS__) */

/*
 * All these should be machine specific - right now they are all broken.
 * However, for all of Cygnus' embedded targets, we want them to all be
 * the same.  Otherwise things like sizeof (struct stat) might depend on
 * how the file was compiled (e.g. -mint16 vs -mint32, etc.).
 */

#ifndef _OFF_T_DECLARED
typedef	__off_t		off_t;		/* file offset */
#define	_OFF_T_DECLARED
#endif
#ifndef _DEV_T_DECLARED
typedef	__dev_t		dev_t;		/* device number or struct cdev */
#define	_DEV_T_DECLARED
#endif
#ifndef _UID_T_DECLARED
typedef	__uid_t		uid_t;		/* user id */
#define	_UID_T_DECLARED
#endif
#ifndef _GID_T_DECLARED
typedef	__gid_t		gid_t;		/* group id */
#define	_GID_T_DECLARED
#endif

#ifndef _PID_T_DECLARED
typedef	__pid_t		pid_t;		/* process id */
#define	_PID_T_DECLARED
#endif

#ifndef _KEY_T_DECLARED
typedef	__key_t		key_t;		/* IPC key */
#define	_KEY_T_DECLARED
#endif

#ifndef _SSIZE_T_DECLARED
typedef _ssize_t ssize_t;
#define	_SSIZE_T_DECLARED
#endif

#ifndef _MODE_T_DECLARED
typedef	__mode_t	mode_t;		/* permissions */
#define	_MODE_T_DECLARED
#endif

#ifndef _NLINK_T_DECLARED
typedef	__nlink_t	nlink_t;	/* link count */
#define	_NLINK_T_DECLARED
#endif

#if !defined(__clockid_t_defined) && !defined(_CLOCKID_T_DECLARED)
typedef	__clockid_t	clockid_t;
#define	__clockid_t_defined
#define	_CLOCKID_T_DECLARED
#endif

#if !defined(__timer_t_defined) && !defined(_TIMER_T_DECLARED)
typedef	__timer_t	timer_t;
#define	__timer_t_defined
#define	_TIMER_T_DECLARED
#endif

#ifndef _USECONDS_T_DECLARED
typedef	__useconds_t	useconds_t;	/* microseconds (unsigned) */
#define	_USECONDS_T_DECLARED
#endif

#ifndef _SUSECONDS_T_DECLARED
typedef	__suseconds_t	suseconds_t;
#define	_SUSECONDS_T_DECLARED
#endif

typedef	__int64_t	sbintime_t;

#include <sys/features.h>


/*
 * Cygwin is using a complete distinct implementation of pthread objects and
 * pointers rather than structs.  This means we can't use the types defined
 * here, but rather in <machine/types.h>.
 */
#if !defined(__CYGWIN__)

#if defined(_POSIX_THREADS)

#include <sys/sched.h>

/*
 *  2.5 Primitive System Data Types,  P1003.1c/D10, p. 19.
 */

#if defined(__XMK__)
typedef unsigned int pthread_t;          /* identify a thread */
#else
typedef __uint32_t pthread_t;            /* identify a thread */
#endif

/* P1003.1c/D10, p. 118-119 */
#define PTHREAD_SCOPE_PROCESS 0
#define PTHREAD_SCOPE_SYSTEM  1

/* P1003.1c/D10, p. 111 */
#define PTHREAD_INHERIT_SCHED  1      /* scheduling policy and associated */
                                      /*   attributes are inherited from */
                                      /*   the calling thread. */
#define PTHREAD_EXPLICIT_SCHED 2      /* set from provided attribute object */

/* P1003.1c/D10, p. 141 */
#define PTHREAD_CREATE_DETACHED 0
#define PTHREAD_CREATE_JOINABLE  1

#if defined(__rtems__)
  #include <sys/cpuset.h>
#endif

#if defined(__XMK__)
typedef struct pthread_attr_s {
  int contentionscope;
  struct sched_param schedparam;
  int  detachstate;
  void *stackaddr;
  size_t stacksize;
} pthread_attr_t;

#define PTHREAD_STACK_MIN       200

#else /* !defined(__XMK__) */
typedef struct {
  int is_initialized;
  void *stackaddr;
  int stacksize;
  int contentionscope;
  int inheritsched;
  int schedpolicy;
  struct sched_param schedparam;
#if defined(__rtems__)
  size_t guardsize;
#endif

  /* P1003.4b/D8, p. 54 adds cputime_clock_allowed attribute.  */
#if defined(_POSIX_THREAD_CPUTIME)
  int  cputime_clock_allowed;  /* see time.h */
#endif
  int  detachstate;
#if defined(__rtems__)
  size_t affinitysetsize;
  cpu_set_t *affinityset;
  cpu_set_t affinitysetpreallocated;
#endif
} pthread_attr_t;

#endif /* !defined(__XMK__) */

#if defined(_POSIX_THREAD_PROCESS_SHARED)
/* NOTE: P1003.1c/D10, p. 81 defines following values for process_shared.  */

#define PTHREAD_PROCESS_PRIVATE 0 /* visible within only the creating process */
#define PTHREAD_PROCESS_SHARED  1 /* visible too all processes with access to */
                                  /*   the memory where the resource is */
                                  /*   located */
#endif

#if defined(_POSIX_THREAD_PRIO_PROTECT)
/* Mutexes */

/* Values for blocking protocol. */

#define PTHREAD_PRIO_NONE    0
#define PTHREAD_PRIO_INHERIT 1
#define PTHREAD_PRIO_PROTECT 2
#endif

#if defined(_UNIX98_THREAD_MUTEX_ATTRIBUTES)

/* Values for mutex type */

/* The following defines are part of the X/Open System Interface (XSI). */

/*
 * This type of mutex does not detect deadlock. A thread attempting to
 * relock this mutex without first unlocking it shall deadlock. Attempting
 * to unlock a mutex locked by a different thread results in undefined
 * behavior.  Attempting to unlock an unlocked mutex results in undefined
 * behavior.
 */
#define PTHREAD_MUTEX_NORMAL     0

/*
 * A thread attempting to relock this mutex without first unlocking
 * it shall succeed in locking the mutex.  The relocking deadlock which
 * can occur with mutexes of type PTHREAD_MUTEX_NORMAL cannot occur with
 * this type of mutex.  Multiple locks of this mutex shall require the
 * same number of unlocks to release the mutex before another thread can
 * acquire the mutex. A thread attempting to unlock a mutex which another
 * thread has locked shall return with an error.  A thread attempting to
 * unlock an unlocked mutex shall return with an error.
 */
#define PTHREAD_MUTEX_RECURSIVE  1

/* 
 * This type of mutex provides error checking. A thread attempting
 * to relock this mutex without first unlocking it shall return with an
 * error. A thread attempting to unlock a mutex which another thread has
 * locked shall return with an error. A thread attempting to unlock an
 * unlocked mutex shall return with an error.
 */
#define PTHREAD_MUTEX_ERRORCHECK 2

/*
 * Attempting to recursively lock a mutex of this type results
 * in undefined behavior. Attempting to unlock a mutex of this type
 * which was not locked by the calling thread results in undefined
 * behavior. Attempting to unlock a mutex of this type which is not locked
 * results in undefined behavior. An implementation may map this mutex to
 * one of the other mutex types.
 */
#define PTHREAD_MUTEX_DEFAULT    3

#endif /* !defined(_UNIX98_THREAD_MUTEX_ATTRIBUTES) */

#if defined(__XMK__)
typedef unsigned int pthread_mutex_t;    /* identify a mutex */

typedef struct {
  int type;
} pthread_mutexattr_t;

#else /* !defined(__XMK__) */
typedef __uint32_t pthread_mutex_t;      /* identify a mutex */

typedef struct {
  int   is_initialized;
#if defined(_POSIX_THREAD_PROCESS_SHARED)
  int   process_shared;  /* allow mutex to be shared amongst processes */
#endif
#if defined(_POSIX_THREAD_PRIO_PROTECT)
  int   prio_ceiling;
  int   protocol;
#endif
#if defined(_UNIX98_THREAD_MUTEX_ATTRIBUTES)
  int type;
#endif
  int   recursive;
} pthread_mutexattr_t;
#endif /* !defined(__XMK__) */

/* Condition Variables */

typedef __uint32_t pthread_cond_t;       /* identify a condition variable */

typedef struct {
  int      is_initialized;
  clock_t  clock;             /* specifiy clock for timeouts */
#if defined(_POSIX_THREAD_PROCESS_SHARED)
  int      process_shared;    /* allow this to be shared amongst processes */
#endif
} pthread_condattr_t;         /* a condition attribute object */

/* Keys */

typedef __uint32_t pthread_key_t;        /* thread-specific data keys */

typedef struct {
  int   is_initialized;  /* is this structure initialized? */
  int   init_executed;   /* has the initialization routine been run? */
} pthread_once_t;       /* dynamic package initialization */
#endif /* defined(_POSIX_THREADS) */

/* POSIX Barrier Types */

#if defined(_POSIX_BARRIERS)
typedef __uint32_t pthread_barrier_t;        /* POSIX Barrier Object */
typedef struct {
  int   is_initialized;  /* is this structure initialized? */
#if defined(_POSIX_THREAD_PROCESS_SHARED)
  int   process_shared;       /* allow this to be shared amongst processes */
#endif
} pthread_barrierattr_t;
#endif /* defined(_POSIX_BARRIERS) */

/* POSIX Spin Lock Types */

#if defined(_POSIX_SPIN_LOCKS)
typedef __uint32_t pthread_spinlock_t;        /* POSIX Spin Lock Object */
#endif /* defined(_POSIX_SPIN_LOCKS) */

/* POSIX Reader/Writer Lock Types */

#if defined(_POSIX_READER_WRITER_LOCKS)
typedef __uint32_t pthread_rwlock_t;         /* POSIX RWLock Object */
typedef struct {
  int   is_initialized;       /* is this structure initialized? */
#if defined(_POSIX_THREAD_PROCESS_SHARED)
  int   process_shared;       /* allow this to be shared amongst processes */
#endif
} pthread_rwlockattr_t;
#endif /* defined(_POSIX_READER_WRITER_LOCKS) */

#endif /* __CYGWIN__ */

#include <machine/types.h>

#endif  /* !__need_inttypes */

#undef __need_inttypes

#endif	/* _SYS_TYPES_H */
