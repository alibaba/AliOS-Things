/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
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

#ifndef MBOX_H
#define MBOX_H

#if defined(__cplusplus)
extern "C"
{
#endif

#define K_MBOX_SIZE 128

#define SYS_ARCH_TIMEOUT 0xffffffffUL
#define SYS_MBOX_EMPTY SYS_ARCH_TIMEOUT

typedef struct k_mbox {
    int first, last;
    void *msgs[K_MBOX_SIZE];
    struct k_sem not_empty;
    struct k_sem not_full;
    struct k_sem mutex;
    int wait_send;
} k_mbox_t;

int k_mbox_new(k_mbox_t **mb, int size);
void k_mbox_free(k_mbox_t *mb);
void k_mbox_post(k_mbox_t *mb, void *msg);
int k_mbox_trypost(k_mbox_t *mb, void *msg);
int k_mbox_fetch(k_mbox_t *mb, void **msg, uint32_t timeout);
int k_mbox_tryfetch(k_mbox_t *mb, void **msg);

#ifdef __cplusplus
}
#endif

#endif /* MBOX_H */

