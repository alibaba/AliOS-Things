/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_TIMER_H
#define HAL_TIMER_H

typedef void (*hal_timer_cb_t)(void *arg);

typedef struct {
    int8_t         ch;
    void          *priv;
    hal_timer_cb_t cb;
    void          *arg;
} hal_timer_t;

/**
 * init a hardware timer
 *
 * @param[in]  tmr         timer struct
 * @param[in]  period      micro seconds for repeat timer trigger
 * @param[in]  auto_reoad  set to 0, if you just need oneshot timer
 * @param[in]  cb          callback to be triggered after useconds
 * @param[in]  ch          timer channel
 * @param[in]  arg         passed to cb
 */
void hal_timer_init(hal_timer_t *tmr, unsigned int period,
                    unsigned char auto_reload, unsigned char ch,
                    hal_timer_cb_t cb, void *arg);

/**
 * init a hardware timer
 *
 * @return  0 == success, EIO == failure
 */
int32_t hal_timer_start(hal_timer_t *tmr);

/**
 * stop a hardware timer
 *
 * @param[in]  tmr  timer struct
 * @param[in]  cb   callback to be triggered after useconds
 * @param[in]  arg  passed to cb
 */
void hal_timer_stop(hal_timer_t *tmr);

#endif /* HAL_TIMER_H*/

