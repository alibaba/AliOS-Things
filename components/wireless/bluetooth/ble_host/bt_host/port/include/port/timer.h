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

#ifndef TIMER_H
#define TIMER_H

#if defined(__cplusplus)
extern "C"
{
#endif

typedef void (* timeout_handler)(void *arg);

struct mtimer {
    struct mtimer *next;
    uint32_t time;
    timeout_handler h;
    void *arg;
};

struct mtimer *mtimer_start(uint32_t msecs, timeout_handler handler, void *arg);

void mtimer_stop(struct mtimer *t);

void mtimer_mbox_fetch(k_mbox_t *mbox, void **msg);

#ifdef __cplusplus
}
#endif

#endif /* TIMER_H */

