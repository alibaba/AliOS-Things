/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <pthread.h>

#include <drivers/bug.h>
#include <drivers/spinlock.h>
#include <drivers/ddkc_log.h>
#include <drivers/spinlock.h>

#define MUTEX_INIT_DONE_FLAG 0xACBDFDBC

/**
 * DEFINE_MUTEX(x) will define a mutex,
 * but the mutex cannot be initialized when the mutex is global variable
 * we use flag to record whether mutex is initialized or not
 * */
struct mutex {
	unsigned int flag;
	spinlock_t s;
	pthread_mutex_t m;
};

#define DEFINE_MUTEX(mutexname) struct mutex mutexname = {.flag = 0}

#define __mutex_init(lock, name, key) mutex_init(lock)

#define mutex_lock_nested(lock, depth) mutex_lock(lock)

// TODO: need to re-implement mutex_lock_interruptible
#define mutex_lock_interruptible(lock) ({\
	pr_warn("mutex_lock_interruptible is defined as mutex_lock\r\n");\
	mutex_lock((lock)); \
	0;})

static inline void mutex_init(struct mutex *lock) {
	int r = 0;

	if (lock->flag == MUTEX_INIT_DONE_FLAG) {
		ddkc_warn( "MUTEX:possible mutex reinit detected\r\n");
		BUG_ON_MSG(1, "mutex reinit detected");
        return;
	}

	spin_lock_init(&lock->s);
	r = pthread_mutex_init(&lock->m, NULL);
	if (!r) {
		lock->flag = MUTEX_INIT_DONE_FLAG;
		ddkc_loud("MUTEX:pthread_mutex_init %p success\r\n", lock);
		return;
	}

	ddkc_err("MUTEX:pthread_mutex_init %p fails, r:%d\r\n", lock, r);

	return;
}
static inline void mutex_lock(struct mutex *lock) {
	int r = 0;

	// TODO: how to guarantee mutex_init procedure is atomic operation?
	if (lock->flag != MUTEX_INIT_DONE_FLAG) {
		ddkc_loud("MUTEX:uninitialized mutex:%p detected, init it\r\n", lock);
		mutex_init(lock);
	}

	r = pthread_mutex_lock(&lock->m);

	if (!r) {
		ddkc_loud("MUTEX:pthread_mutex_lock %p success\r\n", lock);
		return;
	}
	ddkc_err("MUTEX:pthread_mutex_lock %p fails, r:%d\r\n", lock, r);

	return;
}

static inline int mutex_trylock(struct mutex *lock) {
	int r = 0;

	if (lock->flag != MUTEX_INIT_DONE_FLAG) {
		ddkc_loud("MUTEX:uninitialized mutex:%p detected, init it\r\n", lock);
		mutex_init(lock);
	}

	r = pthread_mutex_trylock(&lock->m);

	if (!r) {
		ddkc_loud("MUTEX:pthread_mutex_trylock %p success\r\n", lock);
		return 1;
	}
	ddkc_err("MUTEX:pthread_mutex_trylock fails, r:%d\r\n", r);

	return 0;
}

static inline void mutex_unlock(struct mutex *lock) {
	int r = pthread_mutex_unlock(&lock->m);

	if (!r) {
		ddkc_loud("MUTEX:pthread_mutex_unlock %p success\r\n", lock);
		return;
	}
	ddkc_err("MUTEX:pthread_mutex_unlock fails, r:%d\r\n", r);

	return;
}

static inline void mutex_destroy(struct mutex *lock) {
	pthread_mutex_destroy(&lock->m);
	lock->flag = ~MUTEX_INIT_DONE_FLAG;

}

#if 0
#define __MUTEX_INITIALIZER(lockname) \
        { \
          .blk_obj.blk_list.prev = &blk_obj.blk_list, \
          .blk_obj.blk_list.next = &blk_obj.blk_list, \
          .blk_obj.blk_policy = BLK_POLICY_PRI, \
          .blk_obj.name       = name, \
          #if (RHINO_CONFIG_TASK_DEL > 0) \
          .blk_obj.cancel     = 0u, \
          #endif \
          .mutex_task         = NULL, \
          .mutex_list         = NULL, \
          .mm_alloc_flag      = K_OBJ_STATIC_ALLOC, \
          .blk_obj.obj_type = RHINO_MUTEX_OBJ_TYPE, \
        }

#define DEFINE_MUTEX(mutexname) \
    struct mutex_s mutexname = __MUTEX_INITIALIZER(mutexname)
#endif

#endif //_MUTEX_H_
