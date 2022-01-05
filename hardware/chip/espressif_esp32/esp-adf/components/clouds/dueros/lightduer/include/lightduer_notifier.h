/**
 * Copyright (2017) Baidu Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
 * File: lightduer_notifier.h
 * Auth: Zhong Shuai (zhongshuai@baidu.com)
 * Desc: Notifier Head file
 */

#ifndef BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_NOTIFIER_H
#define BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_NOTIFIER_H

#include <stdint.h>
#include "lightduer_mutex.h"

#define DUER_NOTIFY_STOP_MASK	0x80        /* Don't call further */

#define DUER_NOTIFY_DONE        0x00        /* Don't care */
#define DUER_NOTIFY_OK          0x01        /* Suits me */
#define DUER_NOTIFY_BAD         (DUER_NOTIFY_STOP_MASK | 0x02) /* Bad/Veto action */

#ifdef __cplusplus
extern "C" {
#endif

struct duer_notifier_block;

typedef	int (*notifier_func)(struct duer_notifier_block *nb,
            uint32_t val, void *data);

typedef struct duer_notifier_block {
    notifier_func notifier_call;
    int priority;
    struct duer_notifier_block *next;
} duer_notifier_block_t;

typedef struct duer_notifier_block_head {
    duer_mutex_t lock;
    struct duer_notifier_block *head;
} duer_notifier_block_head_t;

/*
 * Create a notifier to receive the message
 *
 * @param void:
 *
 * @return pointer: Success: Pointer to duer_notifier_block_t
 *                  Failed:  NULL
 */
extern duer_notifier_block_t *duer_notifier_create_notifier(void);

/*
 * Destroy a notifier
 *
 * @param pointer to a notifier
 *
 * @return void:
 */
extern void duer_notifier_destroy_notifier(duer_notifier_block_t *notifier);

/*
 * Create a notifier chain to send the message
 *
 * @param void:
 *
 * @return pointer: Success: Pointer to duer_notifier_block_head_t
 *                  Failed:  NULL
 */
extern duer_notifier_block_head_t *duer_notifier_create_notifier_chain(void);

/*
 * Destroy a notifier chain
 *
 * @param pointer to duer_notifier_block_head_t
 *
 * @return int: Success: DUER_OK
 *              Failed:  DUER_ERR_FAILED
 */
extern int duer_notifier_destroy_notifier_chain(duer_notifier_block_head_t *head);

/*
 * Register a notifier to a notifier chain
 *
 * @param pointer to duer_notifier_block_head_t
 * @param pointer to duer_notifier_block_t
 *
 * @return int: Success: DUER_OK
 *              Failed:  DUER_ERR_FAILED
 */
extern int duer_notifier_register(
        duer_notifier_block_head_t *head,
        duer_notifier_block_t *notifier);

/*
 * Unregister a notifier from a notifier chain
 *
 * @param pointer to duer_notifier_block_head_t
 *        pointer to duer_notifier_block_t
 *
 * @return int: Success: DUER_OK
 *              Failed:  DUER_ERR_FAILED
 */
extern int duer_notifier_unregister(
        duer_notifier_block_head_t *head,
        duer_notifier_block_t *notifier);

/**
 * Send message to single notifier
 *
 * @param notifier: single notifier
 *        val: the input data
 *        data: the output data
 *
 * @return int: DUER_NOTIFY_ value
 *
 */
int duer_notifier_call(duer_notifier_block_t *notifier, uint32_t val, void *data);

/*
 * Send message to the notifier
 *
 * @param pointer to duer_notifier_block_head_t
 *        val: the message
 *        data: which you can receive the message from the notifier
 *
 * @return int: Success: DUER_NOTIFY_OK || DUER_NOTIFY_DONE
 *              Failed:  DUER_NOTIFY_BAD
 */
extern int duer_notifier_call_chain(
        duer_notifier_block_head_t *head,
        uint32_t val, void *data);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_NOTIFIER_H
