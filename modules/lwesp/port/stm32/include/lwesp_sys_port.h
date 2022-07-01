/**
 * \file            lwesp_sys_port.h
 * \brief           FreeRTOS native port
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwESP - Lightweight ESP-AT parser library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Author:          Adrian Carpenter (FreeRTOS port)
 * Version:         v1.1.2-dev
 */
#ifndef LWESP_HDR_SYSTEM_PORT_H
#define LWESP_HDR_SYSTEM_PORT_H

#include <stdint.h>
#include <stdlib.h>
#include "lwesp/lwesp_opt.h"
#include "pybthread.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/stackctrl.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if LWESP_CFG_OS && !__DOXYGEN__

typedef struct mp_ctask_struct {
    pyb_thread_t pyb_thread;
    lwesp_sys_thread_fn entry;
    void * stack;
    size_t stack_size;
    void *arg;
    mp_obj_dict_t *dict_locals;
    mp_obj_dict_t *dict_globals;
} mp_ctask_struct_t;



typedef pyb_mutex_t *lwesp_sys_mutex_t;
typedef pyb_sem_t   *lwesp_sys_sem_t;
typedef pyb_mbox_t   *lwesp_sys_mbox_t;
typedef mp_ctask_struct_t  *lwesp_sys_thread_t;
typedef int                 lwesp_sys_thread_prio_t;

#define LWESP_SYS_MUTEX_NULL          (0)
#define LWESP_SYS_SEM_NULL            (0)
#define LWESP_SYS_MBOX_NULL           (0)
#define LWESP_SYS_TIMEOUT             (0)
#define LWESP_SYS_THREAD_PRIO         (0)
#define LWESP_SYS_THREAD_SS           (4*1024)
#define LWESP_SYS_THREAD_PROCESS_SS          (2*1024)

#endif /* LWESP_CFG_OS && !__DOXYGEN__ */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWESP_HDR_SYSTEM_PORT_H */
