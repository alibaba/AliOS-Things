/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <aos/aos.h>
#include <hal/hal.h>
#include <stdarg.h>
#ifdef CONFIG_AOS_MESH
#include <umesh.h>
#endif
#ifdef MBEDTLS_IN_KERNEL
#include <aos/mbedtls_ssl.h>
#endif
#ifdef CONFIG_ALICRYPTO
#include <ali_crypto.h>
#endif
#ifdef WITH_LWIP
#include <aos/network.h>
#endif

#include <syscall_knum.h>


/* ---------------------Macro--------------------- */

extern const int *syscall_ktbl;
#define SYSCALL_TBL syscall_ktbl

#define SYS_CALL0(nr, t) ((t (*)(void))(SYSCALL_TBL[nr]))()
#define SYS_CALL1(nr, t, t1, p1) ((t (*)(t1))(SYSCALL_TBL[nr]))(p1)
#define SYS_CALL2(nr, t, t1, p1, t2, p2) ((t (*)(t1, t2))(SYSCALL_TBL[nr]))(p1, p2)
#define SYS_CALL3(nr, t, t1, p1, t2, p2, t3, p3) ((t (*)(t1, t2, t3))(SYSCALL_TBL[nr]))(p1, p2, p3)
#define SYS_CALL4(nr, t, t1, p1, t2, p2, t3, p3, t4, p4) ((t (*)(t1, t2, t3, t4))(SYSCALL_TBL[nr]))(p1, p2, p3, p4)
#define SYS_CALL5(nr, t, t1, p1, t2, p2, t3, p3, t4, p4, t5, p5) ((t (*)(t1, t2, t3, t4, t5))(SYSCALL_TBL[nr]))(p1, p2, p3, p4, p5)
#define SYS_CALL6(nr, t, t1, p1, t2, p2, t3, p3, t4, p4, t5, p5, t6, p6) ((t (*)(t1, t2, t3, t4, t5, t6))(SYSCALL_TBL[nr]))(p1, p2, p3, p4, p5, p6)
#define SYS_CALL7(nr, t, t1, p1, t2, p2, t3, p3, t4, p4, t5, p5, t6, p6, t7, p7) ((t (*)(t1, t2, t3, t4, t5, t6, t7))(SYSCALL_TBL[nr]))(p1, p2, p3, p4, p5, p6, p7)

/* ---------------------function--------------------- */
typedef void (*aos_event_cb)(input_event_t *event, void *private_data);
typedef void (*aos_call_t)(void *arg);
typedef void (*aos_poll_call_t)(int fd, void *arg);
typedef void *aos_loop_t;


#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
extern void *sys_aos_malloc(unsigned int size, size_t allocator);
extern void *sys_aos_realloc(void *mem, unsigned int size, size_t allocator);
extern void *sys_aos_zalloc(unsigned int size, size_t allocator);
void *aos_malloc(unsigned int size)
{
    if ((size & AOS_UNSIGNED_INT_MSB) == 0) {
        return sys_aos_malloc(size, (size_t)__builtin_return_address(0));
    } else {
        return sys_aos_malloc(size, 0);
    }
}

void *aos_realloc(void *mem, unsigned int size)
{
    if ((size & AOS_UNSIGNED_INT_MSB) == 0) {
        return sys_aos_realloc(mem, size, (size_t)__builtin_return_address(0));
    } else {
        return sys_aos_realloc(mem, size, 0);
    }
}

void *aos_zalloc(unsigned int size)
{
    if ((size & AOS_UNSIGNED_INT_MSB) == 0) {
        return sys_aos_zalloc(size, (size_t)__builtin_return_address(0));
    } else {
        return sys_aos_zalloc(size, 0);
    }
}
#endif

int __wrap_vprintf(char *format, va_list param)
{
    return aos_vprintf(format, param);
}

int __wrap_fflush(FILE *stream)
{
    return aos_fflush(stream);
}

/* ---------------------syscall function--------------------- */
