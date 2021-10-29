/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <aos/tty_core.h>

#define STA_TX_EN               ((uint32_t)1 << 0)  /* reserved for XON/XOFF flow control */

#define RW_BUDGET               256
#define RX_COUNT_THRESHOLD      ((size_t)UINT8_MAX)
#define TX_SPACE_THRESHOLD      ((size_t)UINT8_MAX)

#define EVENT_RX_COUNT(x)       ((uint32_t)(x) << 0)
#define EVENT_RX_COUNT_MASK     EVENT_RX_COUNT(RX_COUNT_THRESHOLD)
#define EVENT_TX_SPACE(x)       ((uint32_t)(x) << 16)
#define EVENT_TX_SPACE_MASK     EVENT_TX_SPACE(TX_SPACE_THRESHOLD)
#define EVENT_TX_EMPTY          ((uint32_t)1 << 24)

#define rx_count_to_event(x)    ((x) < RX_COUNT_THRESHOLD ? EVENT_RX_COUNT(x) : EVENT_RX_COUNT_MASK)
#define tx_space_to_event(x)    ((x) < TX_SPACE_THRESHOLD ? EVENT_TX_SPACE(x) : EVENT_TX_SPACE_MASK)

#define rx_buf_count(tty) \
    ((AOS_TTY_RX_BUF_SIZE + (tty)->rx_buf_head - (tty)->rx_buf_tail) & (AOS_TTY_RX_BUF_SIZE - 1))
#define rx_buf_space(tty)       (AOS_TTY_RX_BUF_SIZE - 1 - rx_buf_count(tty))
#define tx_buf_count(tty) \
    ((AOS_TTY_TX_BUF_SIZE + (tty)->tx_buf_head - (tty)->tx_buf_tail) & (AOS_TTY_TX_BUF_SIZE - 1))
#define tx_buf_space(tty)       (AOS_TTY_TX_BUF_SIZE - 1 - tx_buf_count(tty))

aos_status_t aos_tty_get(aos_tty_ref_t *ref, uint32_t id)
{
    return aos_dev_get(ref, AOS_DEV_TYPE_TTY, id);
}

void aos_tty_put(aos_tty_ref_t *ref)
{
    aos_dev_put(ref);
}

aos_status_t aos_tty_get_attr(aos_tty_ref_t *ref, struct termios *termios)
{
    aos_tty_t *tty;

    if (!ref || !aos_dev_ref_is_valid(ref) || !termios)
        return -EINVAL;

    tty = aos_container_of(ref->dev, aos_tty_t, dev);
    aos_dev_lock(ref->dev);
    *termios = tty->termios;
    aos_dev_unlock(ref->dev);

    return 0;
}

static aos_status_t set_attr_nonblock(aos_tty_t *tty, int optional_actions, const struct termios *termios)
{
    struct termios old_termios;
    tcflag_t hw_mask;
    aos_status_t ret = 0;

    aos_dev_lock(&tty->dev);

    if (optional_actions == TCSADRAIN || optional_actions == TCSAFLUSH) {
        size_t count;
        aos_irqsave_t flags;

        flags = aos_spin_lock_irqsave(&tty->lock);
        count = tx_buf_count(tty);
        aos_spin_unlock_irqrestore(&tty->lock, flags);

        if (count > 0) {
            aos_dev_unlock(&tty->dev);
            return -EAGAIN;
        }
    }

    if (optional_actions == TCSAFLUSH) {
        aos_irqsave_t flags;

        flags = aos_spin_lock_irqsave(&tty->lock);
        tty->rx_buf_head = 0;
        tty->rx_buf_tail = 0;
        aos_event_set(&tty->event, ~EVENT_RX_COUNT_MASK, AOS_EVENT_AND);
        aos_spin_unlock_irqrestore(&tty->lock, flags);
    }

    old_termios = tty->termios;
    tty->termios = *termios;
    hw_mask = CBAUD | CBAUDEX | CSIZE | CSTOPB | PARENB | PARODD | HUPCL | CLOCAL | CRTSCTS;
    if ((tty->termios.c_cflag & hw_mask) != (old_termios.c_cflag & hw_mask)) {
        if (old_termios.c_cflag & CREAD)
            tty->ops->disable_rx(tty);

        if (tty->status & STA_TX_EN)
            tty->ops->stop_tx(tty);

        ret = tty->ops->set_attr(tty);
        if (ret)
            tty->termios = old_termios;

        if (tty->status & STA_TX_EN) {
            aos_irqsave_t flags;
            flags = aos_spin_lock_irqsave(&tty->lock);
            if (tx_buf_count(tty) > 0)
                tty->ops->start_tx(tty);
            aos_spin_unlock_irqrestore(&tty->lock, flags);
        }

        if (tty->termios.c_cflag & CREAD)
            tty->ops->enable_rx(tty);
    } else {
        bool old_rx_en = !!(old_termios.c_cflag & CREAD);
        bool new_rx_en = !!(tty->termios.c_cflag & CREAD);

        if (old_rx_en && !new_rx_en)
            tty->ops->disable_rx(tty);
        else if (!old_rx_en && new_rx_en)
            tty->ops->enable_rx(tty);
    }

    aos_dev_unlock(&tty->dev);

    return ret;
}

aos_status_t aos_tty_set_attr(aos_tty_ref_t *ref, int optional_actions, const struct termios *termios)
{
    aos_tty_t *tty;
    aos_status_t ret;

    if (!ref || !aos_dev_ref_is_valid(ref) || !termios)
        return -EINVAL;

    if (optional_actions != TCSANOW && optional_actions != TCSADRAIN && optional_actions != TCSAFLUSH)
        return -EINVAL;

    if (termios->c_iflag & (IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON))
        return -EINVAL;

    if (termios->c_oflag & OPOST)
        return -EINVAL;

    if (termios->c_lflag & (ECHO | ECHONL | ICANON | ISIG | IEXTEN))
        return -EINVAL;

    if (sizeof(cc_t) > 1 && termios->c_cc[VMIN] > (cc_t)RX_COUNT_THRESHOLD)
        return -EINVAL;

    tty = aos_container_of(ref->dev, aos_tty_t, dev);

    while (1) {
        uint32_t val;
        aos_status_t r;

        ret = set_attr_nonblock(tty, optional_actions, termios);
        if (ret != -EAGAIN)
            break;

        r = aos_event_get(&tty->event, EVENT_TX_EMPTY, AOS_EVENT_OR, &val, AOS_WAIT_FOREVER);
        if (r == -EINTR) {
            /* reserved for signal mechanism */
            ret = -EINTR;
            break;
        }
    }

    return ret;
}

static ssize_t rx_buffer_consume(aos_tty_t *tty, void *buf, size_t count)
{
    size_t c = 0;

    if (count > SIZE_MAX / 2 - 1)
        count = SIZE_MAX / 2 - 1;

    while (c < count) {
        size_t to_rx = count - c;
        size_t buf_count;
        uint32_t old_event;
        uint32_t new_event;
        aos_irqsave_t flags;

        if (to_rx > RW_BUDGET)
            to_rx = RW_BUDGET;

        flags = aos_spin_lock_irqsave(&tty->lock);

        buf_count = rx_buf_count(tty);
        if (buf_count == 0) {
            aos_spin_unlock_irqrestore(&tty->lock, flags);
            break;
        } else if (buf_count < to_rx) {
            to_rx = buf_count;
        }

        old_event = rx_count_to_event(buf_count);

        for (size_t i = 0; i < to_rx; i++) {
            ((uint8_t *)buf)[c++] = tty->rx_buf[tty->rx_buf_tail++];
            tty->rx_buf_tail &= AOS_TTY_RX_BUF_SIZE - 1;
        }

        new_event = rx_count_to_event(rx_buf_count(tty));
        if (new_event != old_event) {
            aos_event_set(&tty->event, ~EVENT_RX_COUNT_MASK, AOS_EVENT_AND);
            if (new_event)
                aos_event_set(&tty->event, new_event, AOS_EVENT_OR);
        }

        aos_spin_unlock_irqrestore(&tty->lock, flags);

#ifdef AOS_TTY_RESERVED_FOR_USERSPACE
        /* reserved for copying to user space */
        if (0)
            return -EFAULT;
#endif
    }

    return (ssize_t)c;
}

static ssize_t read_timeout(aos_tty_t *tty, void *buf, size_t count, uint32_t *timeout)
{
    ssize_t ret;

    while (1) {
        ssize_t c;
        uint32_t val;
        aos_status_t r;

        aos_dev_lock(&tty->dev);

        if (count == 0) {
            aos_irqsave_t flags;
            flags = aos_spin_lock_irqsave(&tty->lock);
            c = rx_buf_count(tty);
            aos_spin_unlock_irqrestore(&tty->lock, flags);
        } else {
            c = rx_buffer_consume(tty, buf, count);
        }

        aos_dev_unlock(&tty->dev);

        if (c > 0 || c < 0) {
            ret = c;
            break;
        }

        if (*timeout == 0) {
            ret = -EAGAIN;
            break;
        }

        r = aos_event_get(&tty->event, EVENT_RX_COUNT_MASK, AOS_EVENT_OR, &val, *timeout);
        if (r == -ETIMEDOUT) {
            *timeout = 0;
            ret = -ETIMEDOUT;
            break;
        } else if (r == -EINTR) {
            /* reserved for signal mechanism */
            ret = -EINTR;
            break;
        }
    }

    return ret;
}

static ssize_t read_retro(aos_tty_t *tty, void *buf, size_t count)
{
    ssize_t ret;

    while (1) {
        ssize_t c;
        cc_t vmin;
        cc_t vtime;

        aos_dev_lock(&tty->dev);
        vmin = tty->termios.c_cc[VMIN];
        vtime = tty->termios.c_cc[VTIME];

        if (vmin == 0 && vtime == 0) {
            if (count == 0) {
                aos_irqsave_t flags;
                flags = aos_spin_lock_irqsave(&tty->lock);
                c = rx_buf_count(tty);
                aos_spin_unlock_irqrestore(&tty->lock, flags);
            } else {
                c = rx_buffer_consume(tty, buf, count);
            }
        } else if (vmin > 0 && vtime == 0) {
            aos_irqsave_t flags;
            if (count > 0 && count < vmin)
                vmin = (cc_t)count;
            flags = aos_spin_lock_irqsave(&tty->lock);
            c = rx_buf_count(tty);
            aos_spin_unlock_irqrestore(&tty->lock, flags);
            if (count > 0 && c >= vmin)
                c = rx_buffer_consume(tty, buf, count);
        } else if (vmin == 0 && vtime > 0) {
            if (count == 0) {
                aos_irqsave_t flags;
                flags = aos_spin_lock_irqsave(&tty->lock);
                c = rx_buf_count(tty);
                aos_spin_unlock_irqrestore(&tty->lock, flags);
            } else {
                c = rx_buffer_consume(tty, buf, count);
            }
        } else {
            aos_irqsave_t flags;
            if (count > 0 && count < vmin)
                vmin = (cc_t)count;
            flags = aos_spin_lock_irqsave(&tty->lock);
            c = rx_buf_count(tty);
            aos_spin_unlock_irqrestore(&tty->lock, flags);
            if (count > 0 && c >= vmin)
                c = rx_buffer_consume(tty, buf, count);
        }

        aos_dev_unlock(&tty->dev);

        if (vmin == 0 && vtime == 0) {
            ret = c;
            break;
        } else if (vmin > 0 && vtime == 0) {
            uint32_t mask;
            uint32_t val;
            aos_status_t r;
            if (c >= vmin || c < 0) {
                ret = c;
                break;
            }
            /* wait for more than c bytes */
            mask = ~EVENT_RX_COUNT((uint32_t)c) & EVENT_RX_COUNT_MASK;
            r = aos_event_get(&tty->event, mask, AOS_EVENT_OR, &val, AOS_WAIT_FOREVER);
            if (r == -EINTR) {
                /* reserved for signal mechanism */
                ret = -EINTR;
                break;
            }
        } else if (vmin == 0 && vtime > 0) {
            uint32_t val;
            aos_status_t r;
            if (c > 0 || c < 0) {
                ret = c;
                break;
            }
            r = aos_event_get(&tty->event, EVENT_RX_COUNT_MASK, AOS_EVENT_OR, &val, (uint32_t)vtime * 100);
            if (r == -EINTR) {
                /* reserved for signal mechanism */
                ret = -EINTR;
                break;
            } else if (r == -ETIMEDOUT) {
                aos_dev_lock(&tty->dev);
                if (count == 0) {
                    aos_irqsave_t flags;
                    flags = aos_spin_lock_irqsave(&tty->lock);
                    c = rx_buf_count(tty);
                    aos_spin_unlock_irqrestore(&tty->lock, flags);
                } else {
                    c = rx_buffer_consume(tty, buf, count);
                }
                aos_dev_unlock(&tty->dev);
                ret = c;
                break;
            }
        } else {
            uint32_t mask;
            uint32_t val;
            uint32_t timeout;
            aos_status_t r;
            if (c >= vmin || c < 0) {
                ret = c;
                break;
            }
            /* wait for more than c bytes */
            mask = ~EVENT_RX_COUNT((uint32_t)c) & EVENT_RX_COUNT_MASK;
            timeout = (c == 0) ? AOS_WAIT_FOREVER : (uint32_t)vtime * 100;
            r = aos_event_get(&tty->event, mask, AOS_EVENT_OR, &val, timeout);
            if (r == -EINTR) {
                /* reserved for signal mechanism */
                ret = -EINTR;
                break;
            } else if (r == -ETIMEDOUT) {
                aos_dev_lock(&tty->dev);
                if (count == 0) {
                    aos_irqsave_t flags;
                    flags = aos_spin_lock_irqsave(&tty->lock);
                    c = rx_buf_count(tty);
                    aos_spin_unlock_irqrestore(&tty->lock, flags);
                } else {
                    c = rx_buffer_consume(tty, buf, count);
                }
                aos_dev_unlock(&tty->dev);
                if (c > 0 || c < 0) {
                    ret = c;
                    break;
                }
            }
        }
    }

    return ret;
}

/* reserved for POSIX */
static ssize_t aos_tty_read_fancy(aos_tty_ref_t *ref, void *buf, size_t count, uint32_t *timeout)
{
    aos_tty_t *tty;
    ssize_t ret;

    if (!ref || !aos_dev_ref_is_valid(ref) || (!buf && count > 0))
        return -EINVAL;

    tty = aos_container_of(ref->dev, aos_tty_t, dev);

    if (timeout)
        ret = read_timeout(tty, buf, count, timeout);
    else
        ret = read_retro(tty, buf, count);

    return ret;
}

ssize_t aos_tty_read(aos_tty_ref_t *ref, void *buf, size_t count, uint32_t timeout)
{
    return aos_tty_read_fancy(ref, buf, count, &timeout);
}

static size_t tx_buffer_produce(aos_tty_t *tty, const void *buf, size_t count)
{
    size_t c = 0;

    if (count > SIZE_MAX / 2 - 1)
        count = SIZE_MAX / 2 - 1;

    while (c < count) {
        size_t to_tx = count - c;
        size_t buf_space;
        size_t old_count;
        size_t new_count;
        uint32_t old_event;
        uint32_t new_event;
        aos_irqsave_t flags;

        if (to_tx > RW_BUDGET)
            to_tx = RW_BUDGET;

#ifdef AOS_TTY_RESERVED_FOR_USERSPACE
        /* reserved for copying from user space */
        if (0)
            return -EFAULT;
#endif

        flags = aos_spin_lock_irqsave(&tty->lock);

        buf_space = tx_buf_space(tty);
        if (buf_space == 0) {
            aos_spin_unlock_irqrestore(&tty->lock, flags);
            break;
        } else if (buf_space < to_tx) {
            to_tx = buf_space;
        }

        old_count = tx_buf_count(tty);
        old_event = tx_space_to_event(buf_space);

        for (size_t i = 0; i < to_tx; i++) {
            tty->tx_buf[tty->tx_buf_head++] = ((const uint8_t *)buf)[c++];
            tty->tx_buf_head &= AOS_TTY_TX_BUF_SIZE - 1;
        }

        new_count = tx_buf_count(tty);
        new_event = tx_space_to_event(tx_buf_space(tty));

        if (new_event != old_event) {
            aos_event_set(&tty->event, ~EVENT_TX_SPACE_MASK, AOS_EVENT_AND);
            if (new_event)
                aos_event_set(&tty->event, new_event, AOS_EVENT_OR);
        }

        if (old_count == 0 && new_count > 0) {
            aos_event_set(&tty->event, ~EVENT_TX_EMPTY, AOS_EVENT_AND);
            if (tty->status & STA_TX_EN)
                tty->ops->start_tx(tty);
        }

        aos_spin_unlock_irqrestore(&tty->lock, flags);
    }

    return c;
}

/* reserved for POSIX */
static ssize_t aos_tty_write_fancy(aos_tty_ref_t *ref, const void *buf, size_t count, uint32_t *timeout)
{
    aos_tty_t *tty;
    ssize_t ret;

    if (!ref || !aos_dev_ref_is_valid(ref) || (!buf && count > 0))
        return -EINVAL;

    tty = aos_container_of(ref->dev, aos_tty_t, dev);

    while (1) {
        ssize_t c;
        uint32_t val;
        aos_status_t r;

        aos_dev_lock(ref->dev);

        if (count == 0) {
            aos_irqsave_t flags;
            flags = aos_spin_lock_irqsave(&tty->lock);
            c = tx_buf_space(tty);
            aos_spin_unlock_irqrestore(&tty->lock, flags);
        } else {
            c = tx_buffer_produce(tty, buf, count);
        }

        aos_dev_unlock(ref->dev);

        if (c > 0 || c < 0) {
            ret = c;
            break;
        }

        if (timeout && *timeout == 0) {
            ret = -EAGAIN;
            break;
        }

        /* wait for at least TX_SPACE_THRESHOLD bytes of buffer space */
        r = aos_event_get(&tty->event, EVENT_TX_SPACE_MASK, AOS_EVENT_AND, &val, timeout ? *timeout : AOS_WAIT_FOREVER);
        if (r == -ETIMEDOUT) {
            if (timeout)
                *timeout = 0;
            ret = -ETIMEDOUT;
            break;
        } else if (r == -EINTR) {
            /* reserved for signal mechanism */
            ret = -EINTR;
            break;
        }
    }

    return ret;
}

ssize_t aos_tty_write(aos_tty_ref_t *ref, const void *buf, size_t count, uint32_t timeout)
{
    return aos_tty_write_fancy(ref, buf, count, &timeout);
}

static void reset_termios(struct termios *termios)
{
    termios->c_iflag = 0;
    termios->c_oflag = 0;
    termios->c_cflag = 0;
    termios->c_lflag = 0;
    termios->c_cc[VINTR] = '\3';
    termios->c_cc[VQUIT] = '\34';
    termios->c_cc[VERASE] = '\177';
    termios->c_cc[VKILL] = '\25';
    termios->c_cc[VEOF] = '\4';
    termios->c_cc[VEOL] = '\0';
    termios->c_cc[VMIN] = 0;
    termios->c_cc[VTIME] = 0;
    termios->c_cc[VEOL2] = '\0';
    termios->c_cc[VSTART] = '\21';
    termios->c_cc[VSTOP] = '\23';
    termios->c_cc[VSUSP] = '\32';
    cfmakeraw(termios);
    (void)cfsetspeed(termios, 9600);
}

static void dev_tty_unregister(aos_dev_t *dev)
{
    aos_tty_t *tty = aos_container_of(dev, aos_tty_t, dev);

    aos_event_free(&tty->event);

    if (tty->ops->unregister)
        tty->ops->unregister(tty);
}

static aos_status_t dev_tty_get(aos_dev_ref_t *ref)
{
    aos_tty_t *tty = aos_container_of(ref->dev, aos_tty_t, dev);
    uint32_t event_mask;
    aos_status_t ret;

    if (!aos_dev_ref_is_first(ref))
        return (tty->flags & AOS_TTY_F_UNIQUE_REF) ? -EBUSY : 0;

    ret = tty->ops->startup(tty);
    if (ret)
        return ret;

    tty->status |= STA_TX_EN;
    event_mask = tx_space_to_event(AOS_TTY_TX_BUF_SIZE - 1);
    event_mask |= EVENT_TX_EMPTY;
    aos_event_set(&tty->event, event_mask, AOS_EVENT_OR);

    return 0;
}

static void dev_tty_put(aos_dev_ref_t *ref)
{
    aos_tty_t *tty = aos_container_of(ref->dev, aos_tty_t, dev);

    if (!aos_dev_ref_is_last(ref))
        return;

    if (tty->termios.c_cflag & CREAD)
        tty->ops->disable_rx(tty);

    if (tty->status & STA_TX_EN)
        tty->ops->stop_tx(tty);

    tty->ops->shutdown(tty);
    reset_termios(&tty->termios);
    tty->status = 0;
    aos_event_set(&tty->event, 0, AOS_EVENT_AND);
    tty->rx_buf_head = 0;
    tty->rx_buf_tail = 0;
    tty->tx_buf_head = 0;
    tty->tx_buf_tail = 0;
}

static const aos_dev_ops_t dev_tty_ops = {
    .unregister = dev_tty_unregister,
    .get        = dev_tty_get,
    .put        = dev_tty_put,
};

aos_status_t aos_tty_register(aos_tty_t *tty)
{
    aos_status_t ret;

    if (!tty)
        return -EINVAL;

    if (!tty->ops || !tty->ops->startup || !tty->ops->shutdown || !tty->ops->set_attr ||
        !tty->ops->enable_rx || !tty->ops->disable_rx || !tty->ops->start_tx || !tty->ops->stop_tx)
        return -EINVAL;

    tty->dev.type = AOS_DEV_TYPE_TTY;
    tty->dev.ops = &dev_tty_ops;
#ifdef AOS_COMP_VFS
    tty->dev.vfs_helper.name[0] = '\0';
    tty->dev.vfs_helper.ops = NULL;
#endif
    reset_termios(&tty->termios);
    tty->status = 0;
    aos_spin_lock_init(&tty->lock);

    ret = aos_event_new(&tty->event, 0);
    if (ret)
        return ret;

    tty->rx_buf_head = 0;
    tty->rx_buf_tail = 0;
    tty->tx_buf_head = 0;
    tty->tx_buf_tail = 0;

    ret = aos_dev_register(&tty->dev);
    if (ret) {
        aos_event_free(&tty->event);
        return ret;
    }

    return 0;
}

aos_status_t aos_tty_register_argumented(aos_tty_t *tty, uint32_t id, const aos_tty_ops_t *ops, uint32_t flags)
{
    if (!tty)
        return -EINVAL;

    tty->dev.id = id;
    tty->ops = ops;
    tty->flags = flags;

    return aos_tty_register(tty);
}

aos_status_t aos_tty_unregister(uint32_t id)
{
    return aos_dev_unregister(AOS_DEV_TYPE_TTY, id);
}

size_t aos_tty_rx_buffer_produce(aos_tty_t *tty, const void *buf, size_t count)
{
    size_t c = 0;
    uint32_t old_event;
    uint32_t new_event;

    old_event = rx_count_to_event(rx_buf_count(tty));

    while (c < count && rx_buf_space(tty) > 0) {
        tty->rx_buf[tty->rx_buf_head++] = ((const uint8_t *)buf)[c++];
        tty->rx_buf_head &= AOS_TTY_RX_BUF_SIZE - 1;
    }

    new_event = rx_count_to_event(rx_buf_count(tty));
    if (new_event != old_event) {
        aos_event_set(&tty->event, ~EVENT_RX_COUNT_MASK, AOS_EVENT_AND);
        aos_event_set(&tty->event, new_event, AOS_EVENT_OR);
    }

    return c;
}

size_t aos_tty_tx_buffer_consume(aos_tty_t *tty, void *buf, size_t count)
{
    size_t c = 0;
    size_t old_count;
    size_t new_count;
    uint32_t old_event;
    uint32_t new_event;

    old_count = tx_buf_count(tty);
    old_event = tx_space_to_event(tx_buf_space(tty));

    while (c < count && tx_buf_count(tty) > 0) {
        ((uint8_t *)buf)[c++] = tty->tx_buf[tty->tx_buf_tail++];
        tty->tx_buf_tail &= AOS_TTY_TX_BUF_SIZE - 1;
    }

    new_count = tx_buf_count(tty);
    new_event = tx_space_to_event(tx_buf_space(tty));

    if (new_event != old_event) {
        aos_event_set(&tty->event, ~EVENT_TX_SPACE_MASK, AOS_EVENT_AND);
        aos_event_set(&tty->event, new_event, AOS_EVENT_OR);
    }

    if (old_count > 0 && new_count == 0)
        aos_event_set(&tty->event, EVENT_TX_EMPTY, AOS_EVENT_OR);

    return c;
}
