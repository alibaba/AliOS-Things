/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_TTY_H
#define AOS_TTY_H

#include <termios.h>
#include <aos/device.h>

typedef aos_dev_ref_t aos_tty_ref_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Get a TTY device.
 * @param   ref TTY ref to operate.
 * @param   id  TTY device ID.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_tty_get(aos_tty_ref_t *ref, uint32_t id);
/**
 * @brief   Release a TTY device.
 * @param   ref TTY ref to operate.
 * @return  None.
 */
void aos_tty_put(aos_tty_ref_t *ref);
/**
 * @brief   Get the parameters associated with a TTY device.
 * @param   ref     TTY ref to operate.
 * @param   termios POSIX-compliant termios structure.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_tty_get_attr(aos_tty_ref_t *ref, struct termios *termios);
/**
 * @brief   Set the parameters associated with a TTY device.
 * @param   ref                 TTY ref to operate.
 * @param   optional_actions    Specifie when the changes take effect.
 * @param   termios             POSIX-compliant termios structure.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_tty_set_attr(aos_tty_ref_t *ref, int optional_actions, const struct termios *termios);
/**
 * @brief   Read data from a TTY device.
 * @param   ref     TTY ref to operate.
 * @param   buf     Data buffer.
 * @param   count   Attempt to read up to count bytes.
 * @param   timeout Timeout in milliseconds.
 * @return  > 0: the number of bytes read; < 0: on failure.
 */
ssize_t aos_tty_read(aos_tty_ref_t *ref, void *buf, size_t count, uint32_t timeout);
/**
 * @brief   Write data to a TTY device.
 * @param   ref     TTY ref to operate.
 * @param   buf     Data buffer.
 * @param   count   Attempt to write up to count bytes.
 * @param   timeout Timeout in milliseconds.
 * @return  > 0: the number of bytes written; < 0: on failure.
 */
ssize_t aos_tty_write(aos_tty_ref_t *ref, const void *buf, size_t count, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* AOS_TTY_H */
