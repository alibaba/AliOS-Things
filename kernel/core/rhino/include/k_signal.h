/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#ifndef K_SIGNAL_H
#define K_SIGNAL_H

#include <stdint.h>

#define KSIGHUP    1
#define KSIGINT    2
#define KSIGQUIT   3
#define KSIGILL    4
#define KSIGTRAP   5
#define KSIGABRT   6
#define KSIGIOT    KSIGABRT
#define KSIGBUS    7
#define KSIGFPE    8
#define KSIGKILL   9
#define KSIGUSR1   10
#define KSIGSEGV   11
#define KSIGUSR2   12
#define KSIGPIPE   13
#define KSIGALRM   14
#define KSIGTERM   15
#define KSIGSTKFLT 16
#define KSIGCHLD   17
#define KSIGCONT   18
#define KSIGSTOP   19
#define KSIGTSTP   20
#define KSIGTTIN   21
#define KSIGTTOU   22
#define KSIGURG    23
#define KSIGXCPU   24
#define KSIGXFSZ   25
#define KSIGVTALRM 26
#define KSIGPROF   27
#define KSIGWINCH  28
#define KSIGIO     29
#define KSIGPOLL   29
#define KSIGPWR    30
#define KSIGSYS    31
#define KSIGUNUSED KSIGSYS
/* Support only POSIX standard signal. No POSIX real-time signal. */
#define _KNSIG 32

#define KSIG_BLOCK     0
#define KSIG_UNBLOCK   1
#define KSIG_SETMASK   2

#define sigmask(sig)    (1UL << ((sig) - 1))
#define siginmask(sig, mask) \
    ((sig) > 0 && (sig) <= _KNSIG && (sigmask(sig) & (mask)))

#define SIGNAL_IGNORE_MASK (sigmask(KSIGCHLD) | sigmask(KSIGWINCH) | sigmask(KSIGURG))
#define SIGNAL_STOP_MASK \
    (sigmask(KSIGSTOP) | sigmask(KSIGTSTP) | sigmask(KSIGTTIN) | sigmask(KSIGTTOU))
#define SIGNAL_CONT_MASK sigmask(KSIGCONT)
#define SIGNAL_COREDUMP_MASK (\
    sigmask(KSIGQUIT) | sigmask(KSIGILL)  | sigmask(KSIGTRAP) | sigmask(KSIGABRT) | \
    sigmask(KSIGBUS)  | sigmask(KSIGFPE)  | sigmask(KSIGSEGV) | sigmask(KSIGXCPU) | \
    sigmask(KSIGXFSZ) | sigmask(KSIGSYS))

#define signal_is_ignore(sig)       siginmask(sig, SIGNAL_IGNORE_MASK)
#define signal_is_stop(sig)         siginmask(sig, SIGNAL_STOP_MASK)
#define signal_is_cont(sig)         siginmask(sig, SIGNAL_CONT_MASK)
#define signal_is_coredump(sig)     siginmask(sig, SIGNAL_COREDUMP_MASK)

/* Signal set */
typedef uint32_t ksigset_t;
/* Signal handler */
typedef void (*ksighandler_t)(int);

#define KSIG_ERR  ((ksighandler_t)-1)
#define KSIG_DFL  ((ksighandler_t) 0)
#define KSIG_IGN  ((ksighandler_t) 1)

/* User process's signal control block */
typedef struct sigpcb {
    ksigset_t pending;
    ksighandler_t sighand[_KNSIG];
} sigpcb_t;

/* User task's signal control block */
typedef struct sigptcb {
    ksigset_t pending;
    ksigset_t mask;
    ksigset_t mask_bk;
    uint8_t   processing;
} sigptcb_t;

/* Kernel task's signal control block */
typedef struct sigtcb {
    ksigset_t mask;
    ksigset_t mask_bk;
    ksigset_t pending;
    ksighandler_t sighand[_KNSIG];
} sigtcb_t;

typedef struct ksignal {
    int signo;
    ksighandler_t sighand;
} ksignal_t;

typedef struct ksig_regs {
    unsigned long r0;
    unsigned long lr;
    unsigned long pc;
    unsigned long sp;
} ksig_regs_t;

typedef struct {
    ksig_regs_t sigregs;
    unsigned long sigretcode[2];
} ksigcontext_t;

#endif /*K_SIGNAL_H*/

