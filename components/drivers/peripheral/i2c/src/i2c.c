/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <string.h>
#include <aos/i2c_core.h>

#define XFER_TIMEOUT            1000

#define EVENT_XFER_COMPLETE     ((uint32_t)1 << 0)
#define EVENT_XFER_ERROR        ((uint32_t)1 << 1)

aos_status_t aos_i2c_get(aos_i2c_ref_t *ref, uint32_t id)
{
    return aos_dev_get(ref, AOS_DEV_TYPE_I2C, id);
}

void aos_i2c_put(aos_i2c_ref_t *ref)
{
    aos_dev_put(ref);
}

static aos_status_t transfer_sequence(aos_i2c_t *i2c, const aos_i2c_msg_t *msgs, size_t num_msgs)
{
    const aos_i2c_msg_t *last_msg = NULL;
    size_t last_pos = 0;

    if (!(msgs[0].cfg & AOS_I2C_MCFG_RX))
        memcpy(i2c->buf[0], msgs[0].buf, AOS_I2C_BUF_SIZE < msgs[0].count ? AOS_I2C_BUF_SIZE : msgs[0].count);

    for (size_t i = 0, j = 0, pos = 0; i < num_msgs; j++) {
        const aos_i2c_msg_t *msg = &msgs[i];
        const aos_i2c_msg_t *next_msg;
        bool msg_tail;
        size_t next_pos;
        uint32_t mask;
        uint32_t val;
        aos_status_t ret;

        if (msg != last_msg) {
            i2c->x.cfg = msg->cfg;
            i2c->x.addr = msg->addr;
        }

        i2c->x.timeout = XFER_TIMEOUT;
        i2c->x.len = (pos + AOS_I2C_BUF_SIZE < msg->count) ? AOS_I2C_BUF_SIZE : msg->count - pos;
        i2c->x.pos[0] = 0;
        i2c->x.pos[1] = 0;
        msg_tail = (pos + i2c->x.len == msg->count);
        i2c->x.flags = (j & 0x1) ? AOS_I2C_XF_BUF_IDX : 0;
        i2c->x.flags |= (pos == 0) ? AOS_I2C_XF_MSG_HEAD : 0;
        i2c->x.flags |= msg_tail ? AOS_I2C_XF_MSG_TAIL : 0;
        i2c->x.flags |= (j == 0) ? AOS_I2C_XF_SEQ_HEAD : 0;
        i2c->x.flags |= (msg_tail && i + 1 == num_msgs) ? AOS_I2C_XF_SEQ_TAIL : 0;

        ret = i2c->ops->start_xfer(i2c);
        if (ret)
            return ret;

        if (last_msg && (last_msg->cfg & AOS_I2C_MCFG_RX)) {
            size_t last_len;

            if (last_pos + AOS_I2C_BUF_SIZE < last_msg->count)
                last_len = AOS_I2C_BUF_SIZE;
            else
                last_len = last_msg->count - last_pos;

            memcpy(&((uint8_t *)last_msg->buf)[last_pos], i2c->buf[(j - 1) & 0x1], last_len);
        }

        if (msg_tail) {
            next_msg = (i + 1 < num_msgs) ? &msgs[i + 1] : NULL;
            next_pos = 0;
        } else {
            next_msg = msg;
            next_pos = pos + i2c->x.len;
        }

        if (next_msg && !(next_msg->cfg & AOS_I2C_MCFG_RX)) {
            size_t next_len;

            if (next_pos + AOS_I2C_BUF_SIZE < next_msg->count)
                next_len = AOS_I2C_BUF_SIZE;
            else
                next_len = next_msg->count - next_pos;

            memcpy(i2c->buf[(j + 1) & 0x1], &((const uint8_t *)next_msg->buf)[next_pos], next_len);
        }

        mask = EVENT_XFER_COMPLETE | EVENT_XFER_ERROR;
        if (aos_event_get(&i2c->event, mask, AOS_EVENT_OR, &val, i2c->x.timeout) || (val & EVENT_XFER_ERROR)) {
            i2c->ops->abort_xfer(i2c);
            aos_event_set(&i2c->event, 0, AOS_EVENT_AND);
            return -EIO;
        } else {
            i2c->ops->finish_xfer(i2c);
            aos_event_set(&i2c->event, 0, AOS_EVENT_AND);
        }

        last_msg = msg;
        last_pos = pos;

        if (msg_tail) {
            pos = 0;
            i++;
        } else {
            pos += i2c->x.len;
        }
    }

    if (last_msg && (last_msg->cfg & AOS_I2C_MCFG_RX)) {
        int index = (i2c->x.flags & AOS_I2C_XF_BUF_IDX) ? 1 : 0;
        memcpy(&((uint8_t *)last_msg->buf)[last_pos], i2c->buf[index], i2c->x.len);
    }

    return 0;
}

aos_status_t aos_i2c_transfer(aos_i2c_ref_t *ref, const aos_i2c_msg_t *msgs, size_t num_msgs)
{
    aos_i2c_t *i2c;
    aos_status_t ret = 0;

    if (!ref || !aos_dev_ref_is_valid(ref) || !msgs || num_msgs == 0)
        return -EINVAL;

    i2c = aos_container_of(ref->dev, aos_i2c_t, dev);

    for (size_t i = 0; i < num_msgs; i++) {
        const aos_i2c_msg_t *msg = &msgs[i];

        if (msg->cfg & AOS_I2C_MCFG_ADDR_10) {
            if (!(i2c->flags & AOS_I2C_F_ADDR_10))
                return -EINVAL;

            if (msg->addr & ~(uint16_t)0x3FF)
                return -EINVAL;
        } else {
            if (msg->addr & ~(uint16_t)0x7F)
                return -EINVAL;

            if (msg->addr <= 0x07 || msg->addr >= 0x78)
                return -EINVAL;
        }

        if (msg->count == 0 || !msg->buf)
            return -EINVAL;
    }

    for (size_t i = 0, j = 0; i < num_msgs; i++) {
        if (i + 1 < num_msgs &&
            (msgs[i].cfg & AOS_I2C_MCFG_ADDR_10) == (msgs[i + 1].cfg & AOS_I2C_MCFG_ADDR_10) &&
            msgs[i].addr == msgs[i + 1].addr)
            continue;

        aos_dev_lock(ref->dev);
        ret = transfer_sequence(i2c, &msgs[j], i + 1 - j);
        aos_dev_unlock(ref->dev);
        if (ret)
            break;

        j = i + 1;
    }

    return ret;
}

static void dev_i2c_unregister(aos_dev_t *dev)
{
    aos_i2c_t *i2c = aos_container_of(dev, aos_i2c_t, dev);

    aos_event_free(&i2c->event);

    if (i2c->ops->unregister)
        i2c->ops->unregister(i2c);
}

static aos_status_t dev_i2c_get(aos_dev_ref_t *ref)
{
    aos_i2c_t *i2c = aos_container_of(ref->dev, aos_i2c_t, dev);
    aos_status_t ret;

    if (!aos_dev_ref_is_first(ref))
        return 0;

    ret = i2c->ops->startup(i2c);
    if (ret)
        return ret;

    return 0;
}

static void dev_i2c_put(aos_dev_ref_t *ref)
{
    aos_i2c_t *i2c = aos_container_of(ref->dev, aos_i2c_t, dev);

    if (!aos_dev_ref_is_last(ref))
        return;

    i2c->ops->shutdown(i2c);
}

static const aos_dev_ops_t dev_i2c_ops = {
    .unregister = dev_i2c_unregister,
    .get        = dev_i2c_get,
    .put        = dev_i2c_put,
};

aos_status_t aos_i2c_register(aos_i2c_t *i2c)
{
    aos_status_t ret;

    if (!i2c)
        return -EINVAL;

    if (!i2c->ops || !i2c->ops->startup || !i2c->ops->shutdown ||
        !i2c->ops->start_xfer || !i2c->ops->finish_xfer || !i2c->ops->abort_xfer)
        return -EINVAL;

    if (i2c->hz == 0)
        return -EINVAL;

    i2c->dev.type = AOS_DEV_TYPE_I2C;
    i2c->dev.ops = &dev_i2c_ops;
#ifdef AOS_COMP_VFS
    i2c->dev.vfs_helper.name[0] = '\0';
    i2c->dev.vfs_helper.ops = NULL;
#endif
    aos_spin_lock_init(&i2c->lock);

    ret = aos_event_new(&i2c->event, 0);
    if (ret)
        return ret;

    ret = aos_dev_register(&i2c->dev);
    if (ret) {
        aos_event_free(&i2c->event);
        return ret;
    }

    return 0;
}

aos_status_t aos_i2c_unregister(uint32_t id)
{
    return aos_dev_unregister(AOS_DEV_TYPE_I2C, id);
}

size_t aos_i2c_hard_push(aos_i2c_t *i2c, void *tx_buf, size_t count)
{
    if (i2c->x.pos[0] + count > i2c->x.len)
        count = i2c->x.len - i2c->x.pos[0];

    if (!(i2c->x.cfg & AOS_I2C_MCFG_RX) && tx_buf) {
        const uint8_t *buf = i2c->buf[(i2c->x.flags & AOS_I2C_XF_BUF_IDX) ? 1 : 0];

        for (size_t i = 0; i < count; i++)
            ((uint8_t *)tx_buf)[i] = buf[i2c->x.pos[0] + i];
    }

    i2c->x.pos[0] += count;

    return count;
}

bool aos_i2c_hard_pull(aos_i2c_t *i2c, const void *rx_buf, size_t count)
{
    if (i2c->x.pos[1] + count > i2c->x.pos[0])
        count = i2c->x.pos[0] - i2c->x.pos[1];

    if ((i2c->x.cfg & AOS_I2C_MCFG_RX) && rx_buf) {
        uint8_t *buf = i2c->buf[(i2c->x.flags & AOS_I2C_XF_BUF_IDX) ? 1 : 0];

        for (size_t i = 0; i < count; i++)
            buf[i2c->x.pos[1] + i] = ((const uint8_t *)rx_buf)[i];
    }

    i2c->x.pos[1] += count;
    if (i2c->x.pos[1] == i2c->x.len) {
        aos_event_set(&i2c->event, EVENT_XFER_COMPLETE, AOS_EVENT_OR);
        return true;
    }

    return false;
}

void aos_i2c_hard_fail(aos_i2c_t *i2c)
{
    aos_event_set(&i2c->event, EVENT_XFER_ERROR, AOS_EVENT_OR);
}
