/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#define _GNU_SOURCE

#include <sched.h>
#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include "aos/kernel.h"
#include "aos/list.h"

#include <cpu_event.h>
#include <k_api.h>
#include <time.h>
#include <ucontext.h>
#include <sys/syscall.h>

#ifdef HAVE_VALGRIND_H
#include <valgrind.h>
#elif defined(HAVE_VALGRIND_VALGRIND_H)
#include <valgrind/valgrind.h>
#else
#define VALGRIND_STACK_REGISTER(...)
#endif

#undef LOG
#define LOG(format, ...) //printf(format, __VA_ARGS__)

#if defined(GCOV_ENABLE) || defined(HAVE_VALGRIND_H) || defined(HAVE_VALGRIND_VALGRIND_H)
#define MIN_STACK_SIZE    8192
#else
#define MIN_STACK_SIZE    4096
#endif

/*warning sizeof(task_ext_t) should not be larger than minmal stack size which
  will satisfied R0~R12/LR/PC/SP storaged in arm m series*/

typedef struct {
    ktask_t     *tcb;
    void        *arg;
    task_entry_t entry;
    ucontext_t  *uctx;
    void        *real_stack;
    void        *real_stack_end;
    void        *orig_stack;
    int          orig_size;
    int          in_signals;
    int          int_lvl;
    int          saved_errno;
#if defined(HAVE_VALGRIND_H)||defined(HAVE_VALGRIND_VALGRIND_H)
    int          vid;
#endif
} task_ext_t;

extern int swapcontext_safe(ucontext_t *oucp, const ucontext_t *ucp);

static sigset_t cpu_sig_set;
static void cpu_sig_handler(int signo, siginfo_t *si, void *ucontext);
static void _cpu_task_switch(void);

static void task_proc(void);

/* data structures for cpu intr handling */
static ktask_t g_intr_task;
static ksem_t  g_intr_sem;
static klist_t g_event_list = { &g_event_list, &g_event_list };
static klist_t g_recycle_list = { &g_recycle_list, &g_recycle_list };
static dlist_t g_io_list = AOS_DLIST_INIT(g_io_list);
static int cpu_event_inited;
extern volatile uint64_t g_cpu_flag;

typedef struct {
    dlist_t node;
    void (*cb)(int, void *);
    void *arg;
} cpu_io_cb_t;

/*
 * this is tricky
 * as we need to compete with Rhino and non-Rhino tasks
 */
static pthread_mutex_t g_event_mutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_t rhino_cpu_thread[RHINO_CONFIG_CPU_NUM];
static pthread_mutex_t spin_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutexattr_t spin_lock_attr;

typedef struct {
    klist_t node;
    cpu_event_t event;
} cpu_event_internal_t;

static pid_t gettid(void)
{
    return syscall(SYS_gettid);
}

void syscall_error_label(void)
{
    assert(0);
}

void os_unlock_sys_spin(void)
{
    int ret;
    ret = pthread_mutex_unlock(&spin_lock);
    assert(ret == 0);
}

#if (RHINO_CONFIG_CPU_NUM > 1)
uint8_t cpu_cur_get(void)
{
    int i;
    pthread_t cur_thread = pthread_self();

    for (i = 0; i < RHINO_CONFIG_CPU_NUM; i++) {
        if (cur_thread == rhino_cpu_thread[i]) {
            return i;
        }
    }

    assert(0);
}

void cpu_signal(uint8_t cpu_num)
{
    pthread_kill(rhino_cpu_thread[cpu_num], SIGRTMIN);
}

void *cpu_entry(void *arg)
{
    cpu_set_t mask;

    CPU_ZERO(&mask);
    CPU_SET((int)arg, &mask);

    if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) != 0) {
        printf("Not enough cpu nums!!!\n");
    }

    printf("cpu num is %d\n", (int)arg);

    ktask_t    *tcb     = g_preferred_ready_task[(int)arg];
    task_ext_t *tcb_ext = (task_ext_t *)tcb->task_stack;

    setcontext(tcb_ext->uctx);

    assert(0);
}

void cpu_spin_lock(kspinlock_t *lock)
{
    int ret;

    ret = pthread_mutex_lock(&spin_lock);
    assert(ret == 0);
}

void cpu_spin_unlock(kspinlock_t *lock)
{
    int ret;

    ret = pthread_mutex_unlock(&spin_lock);
    assert(ret == 0);
}
#endif

void enter_signal(int signo)
{
    task_ext_t *tcb_ext = (task_ext_t *)g_active_task[cpu_cur_get()]->task_stack;
    tcb_ext->in_signals ++;
    assert(tcb_ext->in_signals == 1);
}

void leave_signal(int signo)
{
    task_ext_t *tcb_ext = (task_ext_t *)g_active_task[cpu_cur_get()]->task_stack;
    tcb_ext->in_signals --;
    assert(tcb_ext->in_signals >= 0);
}

static inline int in_signal(void)
{
    if (!g_active_task[cpu_cur_get()]) {
        return 0;
    }

    task_ext_t *tcb_ext = (task_ext_t *)g_active_task[cpu_cur_get()]->task_stack;
    return tcb_ext->in_signals;
}

sigset_t cpu_intrpt_save(void)
{
    sigset_t    oldset = {0};

    sigprocmask(SIG_BLOCK, &cpu_sig_set, &oldset);

    if (!in_signal()) {
        if (g_active_task[cpu_cur_get()]) {
            task_ext_t *tcb_ext = (task_ext_t *)g_active_task[cpu_cur_get()]->task_stack;
            tcb_ext->int_lvl++;
            if (tcb_ext->int_lvl >= 2) {
                return oldset;
            }
        }
        return oldset;
    }

    return oldset;
}

void cpu_intrpt_restore(sigset_t cpsr)
{
    if (!in_signal()) {
        if (g_active_task[cpu_cur_get()]) {
            task_ext_t *tcb_ext = (task_ext_t *)g_active_task[cpu_cur_get()]->task_stack;
            tcb_ext->int_lvl --;
            if (tcb_ext->int_lvl) {
                return;
            }
        }
        sigprocmask(SIG_UNBLOCK, &cpu_sig_set, NULL);
    }
}


void cpu_task_switch(void)
{
    task_ext_t *tcb_ext = (task_ext_t *)g_active_task[cpu_cur_get()]->task_stack;
    _cpu_task_switch();
    assert(!in_signal());
    assert((void *)&tcb_ext >= tcb_ext->real_stack && (void *)&tcb_ext < tcb_ext->real_stack_end);
}

void cpu_intrpt_switch(void)
{
    _cpu_task_switch();
#if (RHINO_CONFIG_CPU_NUM > 1)
    krhino_spin_lock(&g_sys_lock);
#endif
    assert(in_signal());
}


void cpu_tmr_sync(void)
{
    struct sigevent sevp;
    timer_t timerid;
    struct itimerspec ts;
    cpu_set_t mask;

    int     i    = 1;
    uint8_t loop = 1;
    int     ret  = 0;

    (void)i;
    (void)loop;

    CPU_ZERO(&mask);
    CPU_SET(0, &mask);

    if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) != 0) {
         assert(0);
    }

#if (RHINO_CONFIG_CPU_NUM > 1)
    for (i = 1; i < RHINO_CONFIG_CPU_NUM; i++) {
        if (pthread_create(&rhino_cpu_thread[i], NULL, (void *)cpu_entry, (void *)i) != 0) {
            assert(0);
        }
    }

    while (loop) {
        switch (RHINO_CONFIG_CPU_NUM) {
            case 2:
                if (g_cpu_flag == 2u) {
                    loop = 0;
                }
                break;
            case 3:
                if (g_cpu_flag == 6u) {
                    loop = 0;
                }
                break;
            case 4:
                if (g_cpu_flag == 14u) {
                    loop = 0;
                }
                break;
            default:
                printf("too many cpus!!!\n");
                break;
        }
    }
#endif

    memset(&sevp, 0, sizeof(sevp));
    sevp.sigev_notify = SIGEV_SIGNAL | SIGEV_THREAD_ID;
    sevp.sigev_signo = SIGUSR1;
    sevp._sigev_un._tid = gettid();
    ret = timer_create(CLOCK_REALTIME, &sevp, &timerid);
    assert(ret == 0);

    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 1000000000u / RHINO_CONFIG_TICKS_PER_SECOND;
    ts.it_value.tv_sec = 1;
    ts.it_value.tv_nsec = 0;

    ret = timer_settime(timerid, CLOCK_REALTIME, &ts, NULL);
    assert(ret == 0);
}

void cpu_first_task_start(void)
{
    ktask_t    *tcb     = g_preferred_ready_task[cpu_cur_get()];
    task_ext_t *tcb_ext = (task_ext_t *)tcb->task_stack;

    setcontext(tcb_ext->uctx);
    assert(0);
}

void *cpu_task_stack_init(cpu_stack_t *base, size_t size, void *arg, task_entry_t entry)
{
    CPSR_ALLOC();

    size_t real_size = size > MIN_STACK_SIZE ? size : MIN_STACK_SIZE;
    real_size *= sizeof(cpu_stack_t);

    void *real_stack = aos_malloc(real_size);
    assert(real_stack != NULL);

    /*warning sizeof(task_ext_t) should not be larger than minmal stack size which
      will satisfied R0~R12 storaged in arm m series*/
    task_ext_t *tcb_ext = (task_ext_t *)base;
    tcb_ext->uctx = (ucontext_t *)aos_malloc(sizeof(ucontext_t));
    assert(tcb_ext->uctx != NULL);
    cpu_stack_t *tmp;

    bzero(real_stack, real_size);

    tcb_ext->tcb   = NULL;
    tcb_ext->arg   = arg;
    tcb_ext->entry = entry;
    /* todo+ replace malloc with mmap */
    tcb_ext->real_stack = real_stack;
    tcb_ext->real_stack_end = real_stack + real_size;

#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
#if (RHINO_CONFIG_CPU_STACK_DOWN > 0)
    tmp  = tcb_ext->real_stack;
    *tmp = RHINO_TASK_STACK_OVF_MAGIC;
#else
    tmp  = (cpu_stack_t *)(tcb_ext->real_stack) + (real_size / sizeof(cpu_stack_t)) - 1u;
    *tmp = RHINO_TASK_STACK_OVF_MAGIC;
#endif
#endif

#if defined(HAVE_VALGRIND_H)||defined(HAVE_VALGRIND_VALGRIND_H)
    tcb_ext->vid = VALGRIND_STACK_REGISTER(tcb_ext->real_stack, (char *)(tcb_ext->real_stack) + real_size);
#endif

    RHINO_CPU_INTRPT_DISABLE();

    int ret = getcontext(tcb_ext->uctx);
    if (ret < 0) {
        RHINO_CPU_INTRPT_ENABLE();
        aos_free(real_stack);
        return NULL;
    }

    tcb_ext->uctx->uc_stack.ss_sp = tcb_ext->real_stack;
    tcb_ext->uctx->uc_stack.ss_size = real_size;
    makecontext(tcb_ext->uctx, task_proc, 0);

    RHINO_CPU_INTRPT_ENABLE();

    return tcb_ext;
}

void cpu_task_create_hook(ktask_t *tcb)
{
    task_ext_t *tcb_ext = (task_ext_t *)tcb->task_stack;

    LOG("+++ Task '%-20s' [%3.1d] is created\n", tcb->task_name, tcb->prio);

    tcb_ext->tcb = tcb;
    tcb_ext->orig_stack = tcb->task_stack_base;
    tcb_ext->orig_size = tcb->stack_size;
    /* hack: replace task_stack_base for stack checking */
    tcb->task_stack_base = tcb_ext->real_stack;
    tcb->stack_size = tcb_ext->real_stack_end - tcb_ext->real_stack;
    tcb->stack_size /= sizeof(cpu_stack_t);
}

void cpu_task_del_hook(ktask_t *tcb, res_free_t *arg)
{
    kstat_t ret;
    res_free_t *res = arg;

    task_ext_t *tcb_ext = (task_ext_t *)tcb->task_stack;

    LOG("--- Task '%-20s' is deleted\n", tcb->task_name);
#if defined(HAVE_VALGRIND_H)||defined(HAVE_VALGRIND_VALGRIND_H)
    VALGRIND_STACK_DEREGISTER(tcb_ext->vid);
#endif
    g_sched_lock[cpu_cur_get()]++;

    if (res->cnt == 0) {
        klist_insert(&g_res_list, &res->res_list);
    }

    res->res[res->cnt] = tcb_ext->uctx;
    res->cnt++;

    /*
     * ---- hack -----
     * for DYN_ALLOC case,
     * tcb->task_stack_base is replaced with real_stack in create_hook,
     * and tcb->task_stack_base is freed in krhino_task_dyn_del,
     * so we just need to free orig_stack
     * for STATIC_ALLOC case, need to free real_stack by ourself
     */
    if (tcb->mm_alloc_flag == K_OBJ_DYN_ALLOC) {
        res->res[res->cnt] = tcb_ext->orig_stack;
        res->cnt++;
        ret = krhino_sem_give(&g_res_sem);
        assert(ret == 0);
    } else {
        res->res[res->cnt] = tcb_ext->real_stack;
        res->cnt++;
        ret = krhino_sem_give(&g_res_sem);
        assert(ret == 0);
    }

    g_sched_lock[cpu_cur_get()]--;
}

void task_proc(void)
{
    ktask_t *task_tcb;

    sigprocmask(SIG_BLOCK, &cpu_sig_set, NULL);

    task_ext_t *tcb_ext = (task_ext_t *)g_active_task[cpu_cur_get()]->task_stack;

    LOG("Task '%-20s' running\n", tcb_ext->tcb->task_name);

    /* signals blocked before makecontext, unblock here */
    sigprocmask(SIG_UNBLOCK, &cpu_sig_set, NULL);

    (tcb_ext->entry)(tcb_ext->arg);

    LOG("Task %-20s end\n", tcb_ext->tcb->task_name);

    task_tcb = tcb_ext->tcb;
    if (task_tcb->mm_alloc_flag == K_OBJ_STATIC_ALLOC) {
        krhino_task_del(tcb_ext->tcb);
    } else if (task_tcb->mm_alloc_flag == K_OBJ_DYN_ALLOC) {
        krhino_task_dyn_del(tcb_ext->tcb);
    } else {
        LOG("System crash, the mm_alloc_flag of task is %d\n", task_tcb->mm_alloc_flag);
        assert(0);
    }
}

static void _cpu_task_switch(void)
{
    ktask_t     *from_tcb;
    ktask_t     *to_tcb;
    task_ext_t  *from_tcb_ext;
    task_ext_t  *to_tcb_ext;
    uint8_t      cur_cpu_num;

    krhino_task_sched_stats_get();
    cur_cpu_num = cpu_cur_get();

    from_tcb = g_active_task[cur_cpu_num];
    from_tcb_ext = (task_ext_t *)from_tcb->task_stack;

    to_tcb = g_preferred_ready_task[cur_cpu_num];
    to_tcb_ext = (task_ext_t *)to_tcb->task_stack;
    LOG("TASK SWITCH, '%-20s' (%d) -> '%-20s' (%d)\n",
        from_tcb->task_name, from_tcb->task_state,
        to_tcb->task_name, to_tcb->task_state);

    /* save errno */
    from_tcb_ext->saved_errno = errno;

#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
    assert(*(g_active_task[cur_cpu_num]->task_stack_base) == RHINO_TASK_STACK_OVF_MAGIC);
#endif

    g_active_task[cur_cpu_num] = g_preferred_ready_task[cur_cpu_num];

#if (RHINO_CONFIG_CPU_NUM > 1)
    swapcontext_safe(from_tcb_ext->uctx, to_tcb_ext->uctx);
#else
    swapcontext(from_tcb_ext->uctx, to_tcb_ext->uctx);
#endif
}

void cpu_idle_hook(void)
{
    usleep(10);
}

#if (RHINO_CONFIG_CPU_NUM > 1)
static void cpu_assert(int signo, siginfo_t *si, void *ucontext)
{
    enter_signal(signo);
    krhino_intrpt_enter();
    krhino_intrpt_exit();
    leave_signal(signo);
}
#endif

static void tick_interpt(int signo, siginfo_t *si, void *ucontext)
{
    enter_signal(signo);

    krhino_intrpt_enter();

    krhino_tick_proc();

    krhino_intrpt_exit();

    leave_signal(signo);
}

void *cpu_event_malloc(int size)
{
    return malloc(size);
}

void cpu_event_free(void *p)
{
    pthread_mutex_lock(&g_event_mutex);
    klist_insert(&g_recycle_list, (klist_t *)p);
    pthread_mutex_unlock(&g_event_mutex);
}

int cpu_notify_event(cpu_event_t *event)
{
    int          ret;
    cpu_event_internal_t *kevent = cpu_event_malloc(sizeof(*kevent));

    memcpy(&kevent->event, event, sizeof(*event));
    pthread_mutex_lock(&g_event_mutex);

    klist_insert(&g_event_list, &kevent->node);
    while (!is_klist_empty(&g_recycle_list)) {
        klist_t *node = g_recycle_list.next;
        klist_rm(node);
        free(node);
    }

    pthread_mutex_unlock(&g_event_mutex);

    if (!cpu_event_inited) {
        return 0;
    }

    ret = pthread_kill(rhino_cpu_thread[0], SIGUSR2);

    return ret;
}

static void cpu_intr_entry(void *arg)
{
    while (1) {
        while (!is_klist_empty(&g_event_list)) {
            cpu_event_internal_t *kevent;

            pthread_mutex_lock(&g_event_mutex);

            kevent = krhino_list_entry(g_event_list.next, cpu_event_internal_t, node);
            klist_rm(&kevent->node);

            pthread_mutex_unlock(&g_event_mutex);

            kevent->event.handler(kevent->event.arg);
            cpu_event_free(kevent);
        }

        krhino_sem_take(&g_intr_sem, RHINO_WAIT_FOREVER);
    }
}

static void create_intr_task(void)
{
    static uint32_t stack[1024];

    krhino_sem_create(&g_intr_sem, "intr count", 0);
    krhino_task_create(&g_intr_task, "cpu_intr", 0, 0, 0, stack, 1024, cpu_intr_entry, 1);

    cpu_event_inited = 1;
}

void cpu_init_hook(void)
{
    int              ret;

    struct sigaction event_sig_action = {
        .sa_flags = SA_SIGINFO | SA_RESTART,
        .sa_sigaction = cpu_sig_handler,
    };
    struct sigaction event_io_action = {
        .sa_flags = SA_SIGINFO | SA_RESTART,
        .sa_sigaction = cpu_sig_handler,
    };
    struct sigaction tick_interpt_action = {
        .sa_flags = SA_SIGINFO | SA_RESTART,
        .sa_sigaction = tick_interpt,
    };

#if (RHINO_CONFIG_CPU_NUM > 1)
    struct sigaction cpu_assert_action = {
        .sa_flags = SA_SIGINFO | SA_RESTART,
        .sa_sigaction = cpu_assert,
    };
#endif

    rhino_cpu_thread[0] = pthread_self();

    sigemptyset(&cpu_sig_set);
    sigaddset(&cpu_sig_set, SIGUSR1);
    sigaddset(&cpu_sig_set, SIGIO);
    sigaddset(&cpu_sig_set, SIGUSR2);
    sigaddset(&cpu_sig_set, SIGALRM);

#if (RHINO_CONFIG_CPU_NUM > 1)
    sigaddset(&cpu_sig_set, SIGRTMIN);
    cpu_assert_action.sa_mask   = cpu_sig_set;
#endif

    event_sig_action.sa_mask    = cpu_sig_set;
    event_io_action.sa_mask     = cpu_sig_set;
    tick_interpt_action.sa_mask = cpu_sig_set;

    ret  = sigaction(SIGUSR1, &tick_interpt_action, NULL);
    ret |= sigaction(SIGUSR2, &event_sig_action, NULL);
    ret |= sigaction(SIGIO, &event_io_action, NULL);
#if (RHINO_CONFIG_CPU_NUM > 1)
    ret |= sigaction(SIGRTMIN, &cpu_assert_action, NULL);
#endif

    assert(ret == 0);

    pthread_mutexattr_init(&spin_lock_attr);
    pthread_mutexattr_settype(&spin_lock_attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&spin_lock, &spin_lock_attr);
}

void cpu_start_hook(void)
{
    create_intr_task();
}

void cpu_io_register(void (*f)(int, void *), void *arg)
{
    sigset_t cpsr;
    cpu_io_cb_t *pcb = aos_malloc(sizeof(*pcb));
    pcb->cb = f;
    pcb->arg = arg;

    cpsr = cpu_intrpt_save();
    dlist_add_tail(&pcb->node, &g_io_list);
    cpu_intrpt_restore(cpsr);
}

void cpu_io_unregister(void (*f)(int, void *), void *arg)
{
    sigset_t cpsr;
    cpu_io_cb_t *pcb;
    cpsr = cpu_intrpt_save();
    dlist_for_each_entry(&g_io_list, pcb, cpu_io_cb_t, node) {
        if (pcb->cb != f) {
            continue;
        }
        if (pcb->arg != arg) {
            continue;
        }
        dlist_del(&pcb->node);
        cpu_intrpt_restore(cpsr);
        aos_free(pcb);
        return;
    }
    cpu_intrpt_restore(cpsr);
}

static void trigger_io_cb(int fd)
{
    cpu_io_cb_t *pcb;
    sigset_t cpsr;

    cpsr = cpu_intrpt_save();
    dlist_for_each_entry(&g_io_list, pcb, cpu_io_cb_t, node) {
        pcb->cb(fd, pcb->arg);
    }
    cpu_intrpt_restore(cpsr);
}

void cpu_sig_handler(int signo, siginfo_t *si, void *ucontext)
{
    int cpu_suite = 0;

    pthread_t cur_thread = pthread_self();

    for (int i = 0; i < RHINO_CONFIG_CPU_NUM; i++) {
        if (cur_thread == rhino_cpu_thread[i]) {
            cpu_suite = 1;
            break;
        }
    }

    if (cpu_suite == 0) {
        return;
    }

    enter_signal(signo);
    krhino_intrpt_enter();

    if (signo == SIGUSR2) {
        krhino_sem_give(&g_intr_sem);
    } else if (signo == SIGIO) {
        trigger_io_cb(si->si_fd);
    }

    krhino_intrpt_exit();
    leave_signal(signo);
}

void cpu_call_signal(uint8_t cpu_num,int sig)
{
    pthread_kill(rhino_cpu_thread[cpu_num], sig);
}


