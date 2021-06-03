/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <string.h>
#include <aos/tty.h>

#define STA_RX_EN               ((uint32_t)1 << 0)
#define STA_TX_EN               ((uint32_t)1 << 1)

#define EVENT_RX_READY          ((uint32_t)1 << 0)
#define EVENT_TX_READY          ((uint32_t)1 << 1)

#define rx_buf_count(tty) \
    ((AOS_TTY_RX_BUF_SIZE + (tty)->rx_buf_head - (tty)->rx_buf_tail) & \
     (AOS_TTY_RX_BUF_SIZE - 1))
#define rx_buf_space(tty) \
    (AOS_TTY_RX_BUF_SIZE - 1 - rx_buf_count(tty))
#define tx_buf_count(tty) \
    ((AOS_TTY_TX_BUF_SIZE + (tty)->tx_buf_head - (tty)->tx_buf_tail) & \
     (AOS_TTY_TX_BUF_SIZE - 1))
#define tx_buf_space(tty) \
    (AOS_TTY_TX_BUF_SIZE - 1 - tx_buf_count(tty))

static void reset_termios(struct termios *termios)
{
    memset(termios, 0, sizeof(*termios));
    cfmakeraw(termios);
    termios->c_cc[VMIN] = 0;
    termios->c_cc[VTIME] = 0;
    cfsetspeed(termios, B9600);
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
    aos_status_t ret;

    if (!aos_dev_ref_is_first(ref))
        return 0;

    ret = tty->ops->startup(tty);
    if (ret)
        return ret;

    tty->status |= STA_TX_EN;
    tty->ops->enable_tx(tty);
    aos_event_set(&tty->event, EVENT_TX_READY, AOS_EVENT_OR);

    return 0;
}

static void dev_tty_put(aos_dev_ref_t *ref)
{
    aos_tty_t *tty = aos_container_of(ref->dev, aos_tty_t, dev);

    if (!aos_dev_ref_is_last(ref))
        return;

    if (tty->status & STA_RX_EN) {
        tty->status &= ~STA_RX_EN;
        tty->ops->disable_rx(tty);
    }

    if (tty->status & STA_TX_EN) {
        tty->status &= ~STA_TX_EN;
        tty->ops->disable_tx(tty);
    }

    tty->ops->shutdown(tty);
    reset_termios(&tty->termios);
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

aos_status_t aos_tty_unregister(uint32_t id)
{
    return aos_dev_unregister(AOS_DEV_TYPE_TTY, id);
}

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

aos_status_t aos_tty_set_attr(aos_tty_ref_t *ref, int optional_actions,
                              const struct termios *termios)
{
    aos_tty_t *tty;
    tcflag_t hw_mask;

    if (!ref || !aos_dev_ref_is_valid(ref) || !termios)
        return -EINVAL;

    if (optional_actions != TCSANOW)
        return -EINVAL;

    if (termios->c_iflag & (IGNBRK | BRKINT | PARMRK | ISTRIP |
                            INLCR | IGNCR | ICRNL | IXON))
        return -EINVAL;

    if (termios->c_oflag & OPOST)
        return -EINVAL;

    if (termios->c_lflag & (ECHO | ECHONL | ICANON | ISIG | IEXTEN))
        return -EINVAL;

    tty = aos_container_of(ref->dev, aos_tty_t, dev);
    aos_dev_lock(ref->dev);

    hw_mask = CBAUD | CBAUDEX | CSIZE | CSTOPB | PARENB | PARODD |
              HUPCL | CLOCAL | CRTSCTS;
    if ((termios->c_cflag & hw_mask) != (tty->termios.c_cflag & hw_mask)) {
        struct termios old_termios;
        bool old_rx_en;
        bool old_tx_en;
        aos_status_t ret;

        if (tty->status & STA_RX_EN) {
            old_rx_en = true;
            tty->status &= ~STA_RX_EN;
            tty->ops->disable_rx(tty);
        } else {
            old_rx_en = false;
        }

        if (tty->status & STA_TX_EN) {
            old_tx_en = true;
            tty->status &= ~STA_TX_EN;
            tty->ops->disable_tx(tty);
        } else {
            old_tx_en = false;
        }

        old_termios = tty->termios;
        tty->termios = *termios;
        ret = tty->ops->set_attr(tty);
        if (ret) {
            tty->termios = old_termios;

            if (old_tx_en) {
                aos_irqsave_t flags;
                tty->status |= STA_TX_EN;
                tty->ops->enable_tx(tty);
                flags = aos_spin_lock_irqsave(&tty->lock);
                if (tx_buf_count(tty) > 0)
                    tty->ops->start_tx(tty);
                aos_spin_unlock_irqrestore(&tty->lock, flags);
            }

            if (old_rx_en) {
                tty->status |= STA_RX_EN;
                tty->ops->enable_rx(tty);
            }

            aos_dev_unlock(ref->dev);
            return ret;
        }

        if (old_tx_en) {
            aos_irqsave_t flags;
            tty->status |= STA_TX_EN;
            tty->ops->enable_tx(tty);
            flags = aos_spin_lock_irqsave(&tty->lock);
            if (tx_buf_count(tty) > 0)
                tty->ops->start_tx(tty);
            aos_spin_unlock_irqrestore(&tty->lock, flags);
        }

        if (tty->termios.c_cflag & CREAD) {
            tty->status |= STA_RX_EN;
            tty->ops->enable_rx(tty);
        }
    } else {
        tty->termios = *termios;

        if (tty->status & STA_RX_EN) {
            if (!(tty->termios.c_cflag & CREAD)) {
                tty->status &= ~STA_RX_EN;
                tty->ops->disable_rx(tty);
            }
        } else {
            if (tty->termios.c_cflag & CREAD) {
                tty->status |= STA_RX_EN;
                tty->ops->enable_rx(tty);
            }
        }
    }

    aos_dev_unlock(ref->dev);

    return 0;
}

static size_t rx_buffer_consume(aos_tty_t *tty, void *buf, size_t count)
{
    size_t c = 0;
    aos_irqsave_t flags;

    flags = aos_spin_lock_irqsave(&tty->lock);

    while (c < count && rx_buf_count(tty) > 0) {
        ((uint8_t *)buf)[c++] = tty->rx_buf[tty->rx_buf_tail++];
        tty->rx_buf_tail &= AOS_TTY_RX_BUF_SIZE - 1;
        if (!(c & 0xFF)) {
            aos_spin_unlock_irqrestore(&tty->lock, flags);
            flags = aos_spin_lock_irqsave(&tty->lock);
        }
    }

    if (rx_buf_count(tty) == 0)
        aos_event_set(&tty->event, ~EVENT_RX_READY, AOS_EVENT_AND);

    aos_spin_unlock_irqrestore(&tty->lock, flags);

    return c;
}

ssize_t aos_tty_read(aos_tty_ref_t *ref, void *buf,
                     size_t count, uint32_t timeout)
{
    aos_tty_t *tty;
    aos_event_t *event;
    size_t c = 0;
    ssize_t ret = 0;

    if (!ref || !aos_dev_ref_is_valid(ref) || !buf || count == 0)
        return -EINVAL;

    tty = aos_container_of(ref->dev, aos_tty_t, dev);
    event = &tty->event;

    while (1) {
        cc_t vmin;
        cc_t vtime;
        uint32_t v;
        aos_status_t r;

        aos_dev_lock(ref->dev);
        c += rx_buffer_consume(tty, &((uint8_t *)buf)[c], count - c);
        vmin = tty->termios.c_cc[VMIN];
        vtime = tty->termios.c_cc[VTIME];
        aos_dev_unlock(ref->dev);

        if (vmin == 0 && vtime == 0) {
            ret = (ssize_t)c;
            break;
        } else if (vmin > 0 && vtime == 0) {
            if (c >= (size_t)vmin || c == count) {
                ret = (ssize_t)c;
                break;
            }
            r = aos_event_get(event, EVENT_RX_READY, AOS_EVENT_OR,
                              &v, AOS_WAIT_FOREVER);
            (void)r;
        } else if (vmin == 0 && vtime > 0) {
            if (c > 0) {
                ret = (ssize_t)c;
                break;
            }
            r = aos_event_get(event, EVENT_RX_READY, AOS_EVENT_OR,
                              &v, (uint32_t)vtime * 100);
            if (r == -ETIMEDOUT) {
                ret = 0;
                break;
            }
        } else {
            if (c >= (size_t)vmin || c == count) {
                ret = (ssize_t)c;
                break;
            }
            r = aos_event_get(event, EVENT_RX_READY, AOS_EVENT_OR,
                              &v, (uint32_t)vtime * 100);
            if (r == -ETIMEDOUT) {
                ret = (ssize_t)c;
                break;
            }
        }
    }

    return ret;
}

static size_t tx_buffer_produce(aos_tty_t *tty, const void *buf, size_t count)
{
    size_t c = 0;
    size_t old_count;
    aos_irqsave_t flags;

    flags = aos_spin_lock_irqsave(&tty->lock);
    old_count = tx_buf_count(tty);

    while (c < count && tx_buf_space(tty) > 0) {
        tty->tx_buf[tty->tx_buf_head++] = ((const uint8_t *)buf)[c++];
        tty->tx_buf_head &= AOS_TTY_TX_BUF_SIZE - 1;
        if (!(c & 0xFF)) {
            aos_spin_unlock_irqrestore(&tty->lock, flags);
            flags = aos_spin_lock_irqsave(&tty->lock);
        }
    }

    if (tx_buf_space(tty) == 0)
        aos_event_set(&tty->event, ~EVENT_TX_READY, AOS_EVENT_AND);

    if (old_count == 0 && c > 0 && (tty->status & STA_TX_EN))
        tty->ops->start_tx(tty);

    aos_spin_unlock_irqrestore(&tty->lock, flags);

    return c;
}

ssize_t aos_tty_write(aos_tty_ref_t *ref, const void *buf,
                      size_t count, uint32_t timeout)
{
    aos_tty_t *tty;
    aos_event_t *event;
    ssize_t ret = 0;

    if (!ref || !aos_dev_ref_is_valid(ref) || !buf || count == 0)
        return -EINVAL;

    tty = aos_container_of(ref->dev, aos_tty_t, dev);
    event = &tty->event;

    while (1) {
        size_t c;
        uint32_t v;
        aos_status_t r;

        aos_dev_lock(ref->dev);
        c = tx_buffer_produce(tty, buf, count);
        aos_dev_unlock(ref->dev);

        if (c > 0) {
            ret = (ssize_t)c;
            break;
        }

        if (timeout == 0) {
            ret = -EAGAIN;
            break;
        }

        r = aos_event_get(event, EVENT_TX_READY, AOS_EVENT_OR, &v, timeout);
        if (r == -ETIMEDOUT) {
            ret = -ETIMEDOUT;
            break;
        }
    }

    return ret;
}

size_t aos_tty_rx_buffer_produce(aos_tty_t *tty, const void *buf, size_t count)
{
    size_t c = 0;
    size_t old_count;

    old_count = rx_buf_count(tty);

    while (c < count && rx_buf_space(tty) > 0) {
        tty->rx_buf[tty->rx_buf_head++] = ((const uint8_t *)buf)[c++];
        tty->rx_buf_head &= AOS_TTY_RX_BUF_SIZE - 1;
    }

    if (old_count == 0 && c > 0)
        aos_event_set(&tty->event, EVENT_RX_READY, AOS_EVENT_OR);

    return c;
}

size_t aos_tty_tx_buffer_consume(aos_tty_t *tty, void *buf, size_t count)
{
    size_t c = 0;

    while (c < count && tx_buf_count(tty) > 0) {
        ((uint8_t *)buf)[c++] = tty->tx_buf[tty->tx_buf_tail++];
        tty->tx_buf_tail &= AOS_TTY_TX_BUF_SIZE - 1;
    }

    if (tx_buf_count(tty) < AOS_TTY_TX_BUF_SIZE / 2)
        aos_event_set(&tty->event, EVENT_TX_READY, AOS_EVENT_OR);

    return c;
}
