/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_api.h"
#include <stdatomic.h>

/*
 * Hash buckets are shared by all the futex_keys that hash to the same location.  
 * Each key may have multiple futex_q structures, one for each task
 * waiting on a futex.
 */
typedef struct {
    kspinlock_t lock;
    volatile unsigned int waiters;
    blk_obj_t blk_obj;
} k_futex_hash_bucket_t;

#define RHINO_CONFIG_FUTEX_HASH_SIZE 256
static k_futex_hash_bucket_t g_futex_hash_bucket[RHINO_CONFIG_FUTEX_HASH_SIZE];

/**
 * k_rol32 - rotate a 32-bit value left
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline uint32_t k_rol32(uint32_t word, unsigned int shift)
{
    return (word << (shift & 31)) | (word >> ((-shift) & 31));
}

/* k_jhash_mix -- mix 3 32-bit values reversibly. */
#define k_jhash_mix(a, b, c)                \
{                                           \
    a -= c;  a ^= k_rol32(c, 4);  c += b;     \
    b -= a;  b ^= k_rol32(a, 6);  a += c;     \
    c -= b;  c ^= k_rol32(b, 8);  b += a;     \
    a -= c;  a ^= k_rol32(c, 16); c += b;     \
    b -= a;  b ^= k_rol32(a, 19); a += c;     \
    c -= b;  c ^= k_rol32(b, 4);  b += a;     \
}

/* k_jhash_final - final mixing of 3 32-bit values (a,b,c) into c */
#define k_jhash_final(a, b, c)          \
{                                       \
    c ^= b; c -= k_rol32(b, 14);          \
    a ^= c; a -= k_rol32(c, 11);          \
    b ^= a; b -= k_rol32(a, 25);          \
    c ^= b; c -= k_rol32(b, 16);          \
    a ^= c; a -= k_rol32(c, 4);           \
    b ^= a; b -= k_rol32(a, 14);          \
    c ^= b; c -= k_rol32(b, 24);          \
}

/* An arbitrary initial parameter */
#define K_JHASH_INITVAL       0xdeadbeef


/* k_jhash2 - hash an array of uint32_t's
 * @key: the key which must be an array of uint32_t's
 * @len: the number of uint32_t's
 * @initval: the previous hash, or an arbitray value
 * Returns the hash value of the key.
 */
static inline uint32_t k_jhash2(const uint32_t *key, uint32_t len, uint32_t initval)
{
    uint32_t a, b, c;

    /* Set up the internal state */
    a = b = c = K_JHASH_INITVAL + (len << 2) + initval;

    /* Handle most of the key */
    while (len > 3) {
        a += key[0];
        b += key[1];
        c += key[2];
        k_jhash_mix(a, b, c);
        len -= 3;
        key += 3;
    }

    /* Handle the last 3 uint32_t's */
    switch (len) {
        case 3:
            c += key[2];
        case 2:
            b += key[1];
        case 1:
            a += key[0];
            k_jhash_final(a, b, c);
        case 0:    /* Nothing left to add */
            break;
    }

    return c;
}


/**
 * hash_futex - Return the hash bucket in the global hash
 * @key:    Pointer to the futex key for which the hash is calculated
 *
 * We hash on the keys returned from get_futex_key (see below) and return the
 * corresponding hash bucket in the global hash.
 */
static k_futex_hash_bucket_t *hash_futex(futex_key_t *key)
{
    uint32_t hash = k_jhash2((uint32_t *)key, sizeof(futex_key_t) / 4, key->offset);

    return &g_futex_hash_bucket[hash & (RHINO_CONFIG_FUTEX_HASH_SIZE - 1)];
}

static int get_futex_key(uint32_t *uaddr, futex_key_t *key)
{
    unsigned long address = (unsigned long)uaddr;
    uint8_t  cur_cpu_num;
    /*
     * The futex address must be "naturally" aligned.
     */
    key->offset = address % 4096;
    if ((address % sizeof(uint32_t)) != 0) {
        return -RHINO_SYS_FATAL_ERR;
    }
    key->address = address - key->offset;
    cur_cpu_num = cpu_cur_get();
    key->pid = g_active_task[cur_cpu_num]->pid;

    return 0;
}

static void format_futex_node(k_futex_t *futex, futex_key_t *key)
{
    ktask_t *task;
    uint8_t  cur_cpu_num;
    futex->key = *key;
    cur_cpu_num = cpu_cur_get();
    task = g_active_task[cur_cpu_num];
    futex->task = task;
}

/**
 * match_futex - Check whether two futex keys are equal
 * @key1:    Pointer to key1
 * @key2:    Pointer to key2
 *
 * Return 1 if two futex_keys are equal, 0 otherwise.
 */
static inline int match_futex(futex_key_t *key1, futex_key_t *key2)
{
    return (key1 && key2
            && key1->pid == key2->pid
            && key1->address == key2->address
            && key1->offset == key2->offset);
}

static int unqueue_futex(k_futex_hash_bucket_t *hb, futex_key_t *key)
{
    int ret = 0;
    klist_t *tmp;
    klist_t *list_start = &hb->blk_obj.blk_list;
    klist_t *list_end   = &hb->blk_obj.blk_list;

    ///< search the match futex frome the list
    for (tmp = list_start->next; tmp != list_end; tmp = tmp->next) {
        ret = match_futex(&(krhino_list_entry(tmp, ktask_t, task_list)->futex->key), key);
        if (ret == 1) {
            break;
        }
    }

    if (ret == 1) {
        //< wakeup the task if matched.
        krhino_list_entry(tmp, ktask_t, task_list)->futex = NULL;
        pend_task_wakeup(krhino_list_entry(tmp, ktask_t, task_list));
    }

    return ret;
}

static kstat_t krhino_futex_wait(uint32_t *uaddr, uint32_t val, const tick_t timeout)
{
    int ret;
    kstat_t stat;
    uint32_t uval;
    k_futex_t futex;
    futex_key_t key;
    k_futex_hash_bucket_t *hb = NULL;

    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();

    ret = get_futex_key(uaddr, &key);
    if (ret != 0) {
        RHINO_CRITICAL_EXIT();
        return ret;
    }

    hb = hash_futex(&key);
    if (hb == NULL) {
        RHINO_CRITICAL_EXIT();
        return -RHINO_INV_PARAM;
    }

    //< spin lock
    atomic_fetch_add(&hb->waiters, 1);
    //krhino_spin_lock(&hb->lock); //< g_sys_lock will protect the whole operation.

    //< check val in uaddr
    uval = atomic_load(uaddr);
    if (uval != val) {
        //krhino_spin_unlock(&hb->lock);
        atomic_fetch_sub(&hb->waiters, 1);
        RHINO_CRITICAL_EXIT();
        return -RHINO_SYS_FATAL_ERR;
    }

    //< queue the task if needed
    format_futex_node(&futex, &key);
    futex.task->futex = &futex;
    /* any way block the current task */
    pend_to_blk_obj(&hb->blk_obj, futex.task, timeout);

    RHINO_CRITICAL_EXIT_SCHED();


    RHINO_CPU_INTRPT_DISABLE();

    stat = pend_state_end_proc(g_active_task[cpu_cur_get()], &hb->blk_obj);

    atomic_fetch_sub(&hb->waiters, 1);
    RHINO_CPU_INTRPT_ENABLE();

    return stat;
    //< unlock and reschedule
    //krhino_spin_unlock(&hb->lock); //< g_sys_lock will protect the whole operation.
    //< spin unlock
    //< unqueue the task
}

static kstat_t krhino_futex_wake(uint32_t *uaddr, uint32_t nr_wake)
{
    int ret;
    uint32_t uval;
    futex_key_t key;
    k_futex_hash_bucket_t *hb = NULL;

    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();

    ret = get_futex_key(uaddr, &key);
    if (ret != 0) {
        RHINO_CRITICAL_EXIT();
        return -RHINO_INV_PARAM;
    }

    hb = hash_futex(&key);
    if (hb == NULL) {
        RHINO_CRITICAL_EXIT();
        return -RHINO_INV_PARAM;
    }

    //< check val in uaddr
    uval = atomic_load(&hb->waiters);
    if (uval == 0) {
        //< no task pending on the list
        RHINO_CRITICAL_EXIT();
        return RHINO_SUCCESS;
    }
    //< wakeup spceifie num of pending task
    for (int i = 0; i < nr_wake; i++) {
        ret = unqueue_futex(hb, &key);
        if (ret == 0) {
            //< no matched futex in the pend list
            break;
        }
    }

    RHINO_CRITICAL_EXIT_SCHED();

    return RHINO_SUCCESS;
}



void krhino_futex_init(void)
{
    int i;
    for (i = 0; i < RHINO_CONFIG_FUTEX_HASH_SIZE; i++) {
        klist_init(&g_futex_hash_bucket[i].blk_obj.blk_list);
        g_futex_hash_bucket[i].blk_obj.blk_policy = BLK_POLICY_PRI;
        g_futex_hash_bucket[i].blk_obj.name       = NULL;
#if (RHINO_CONFIG_TASK_DEL > 0)
        g_futex_hash_bucket[i].blk_obj.cancel     = 0u;
#endif
        g_futex_hash_bucket[i].blk_obj.obj_type = RHINO_FUTEX_OBJ_TYPE;

        krhino_spin_lock_init(&g_futex_hash_bucket[i].lock);
        g_futex_hash_bucket[i].waiters = 0;
    }
}

kstat_t krhino_futex(uint32_t *uaddr, int futex_op, uint32_t val, const tick_t timeout)
{
    int cmd = futex_op & KFUTEX_CMD_MASK;
    int ret = 0;
    switch (cmd) {
        case KFUTEX_WAIT:
            ret = krhino_futex_wait(uaddr, val, timeout);
            break;
        case KFUTEX_WAKE:
            ret = krhino_futex_wake(uaddr, val);
            break;
        default:
            return -RHINO_SYS_FATAL_ERR;
    }
    return ret;
}

