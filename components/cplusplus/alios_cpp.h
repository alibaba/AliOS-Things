/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef ALIOS_CPP_H
#define ALIOS_CPP_H

#include "include/cpp_mem.h"
#include "include/cpp_thread.h"
#include "include/cpp_mutex.h"
#include "include/cpp_timer.h"
#include "include/cpp_semaphore.h"
#include "include/cpp_queue.h"
#include "include/cpp_workQueue.h"

/** @defgroup cpp_aos_init
 * @{
 */

/**
 * cplusplus component init。
 *
 */
extern "C" void cpp_init(void);

/**
 * @}
 */

#endif
