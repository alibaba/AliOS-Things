/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <aos/spi_core.h>
#if AOS_SPI_BUF_SIZE > 0
#include <string.h>
#endif

#define XFER_TIMEOUT            1000

#define EVENT_XFER_COMPLETE     ((uint32_t)1 << 0)
#define EVENT_XFER_ERROR        ((uint32_t)1 << 1)

aos_status_t aos_spi_get(aos_spi_ref_t *ref, uint32_t id)
{
    return aos_dev_get(ref, AOS_DEV_TYPE_SPI, id);
}

void aos_spi_put(aos_spi_ref_t *ref)
{
    aos_dev_put(ref);
}

aos_status_t aos_spi_get_info(aos_spi_ref_t *ref, aos_spi_info_t *info)
{
    aos_spi_t *spi;

    if (!ref || !aos_dev_ref_is_valid(ref) || !info)
        return -EINVAL;

    spi = aos_container_of(ref->dev, aos_spi_t, dev);
    info->flags = spi->flags;
    info->num_cs = spi->num_cs;
    info->min_hz = spi->min_hz;
    info->max_hz = spi->max_hz;

    return 0;
}

static aos_status_t transfer_sequence(aos_spi_t *spi, const aos_spi_msg_t *msgs, size_t num_msgs)
{
#if AOS_SPI_BUF_SIZE > 0
    const aos_spi_msg_t *last_msg = NULL;
    size_t last_pos = 0;
#endif

    if (spi->ops->transfer_sequence)
        return spi->ops->transfer_sequence(spi, msgs, num_msgs);

#if AOS_SPI_BUF_SIZE > 0
    if (msgs[0].tx_buf)
        memcpy(spi->tx_buf[0], msgs[0].tx_buf, AOS_SPI_BUF_SIZE < msgs[0].count ? AOS_SPI_BUF_SIZE : msgs[0].count);

    for (size_t i = 0, j = 0, pos = 0; i < num_msgs; j++) {
        const aos_spi_msg_t *msg = &msgs[i];
        const aos_spi_msg_t *next_msg;
        bool msg_tail;
        size_t next_pos;
        uint32_t mask;
        uint32_t val;
        aos_status_t ret;

        if (msg != last_msg) {
            spi->x.cfg = msg->cfg;
            spi->x.cs = msg->cs;
            spi->x.hz = msg->hz;
            spi->x.pre_cs = msg->pre_cs;
            spi->x.post_cs = msg->post_cs;
            spi->x.pre_clk = msg->pre_clk;
            spi->x.post_clk = msg->post_clk;
        }

        spi->x.timeout = XFER_TIMEOUT;
        spi->x.len = (pos + AOS_SPI_BUF_SIZE < msg->count) ? AOS_SPI_BUF_SIZE : msg->count - pos;
        spi->x.head = 0;
        spi->x.tail = 0;
        msg_tail = (pos + spi->x.len == msg->count);
        spi->x.flags = 0;
        spi->x.flags |= (pos == 0) ? AOS_SPI_XF_MSG_HEAD : 0;
        spi->x.flags |= msg_tail ? AOS_SPI_XF_MSG_TAIL : 0;
        spi->x.flags |= (j == 0) ? AOS_SPI_XF_SEQ_HEAD : 0;
        spi->x.flags |= (msg_tail && i + 1 == num_msgs) ? AOS_SPI_XF_SEQ_TAIL : 0;
        spi->x.flags |= msg->rx_buf ? AOS_SPI_XF_RX : 0;
        spi->x.flags |= msg->tx_buf ? AOS_SPI_XF_TX : 0;
        spi->x.rx_buf = msg->rx_buf ? spi->rx_buf[j & 0x1] : NULL;
        spi->x.tx_buf = msg->tx_buf ? spi->tx_buf[j & 0x1] : NULL;

        ret = spi->ops->start_xfer(spi);
        if (ret)
            return ret;

        if (last_msg && last_msg->rx_buf) {
            size_t last_len;

            if (last_pos + AOS_SPI_BUF_SIZE < last_msg->count)
                last_len = AOS_SPI_BUF_SIZE;
            else
                last_len = last_msg->count - last_pos;

            memcpy(&((uint8_t *)last_msg->rx_buf)[last_pos], spi->rx_buf[(j - 1) & 0x1], last_len);
        }

        if (msg_tail) {
            next_msg = (i + 1 < num_msgs) ? &msgs[i + 1] : NULL;
            next_pos = 0;
        } else {
            next_msg = msg;
            next_pos = pos + spi->x.len;
        }

        if (next_msg && next_msg->tx_buf) {
            size_t next_len;

            if (next_pos + AOS_SPI_BUF_SIZE < next_msg->count)
                next_len = AOS_SPI_BUF_SIZE;
            else
                next_len = next_msg->count - next_pos;

            memcpy(spi->tx_buf[(j + 1) & 0x1], &((const uint8_t *)next_msg->tx_buf)[next_pos], next_len);
        }

        mask = EVENT_XFER_COMPLETE | EVENT_XFER_ERROR;
        if (aos_event_get(&spi->event, mask, AOS_EVENT_OR, &val, spi->x.timeout) || (val & EVENT_XFER_ERROR)) {
            spi->ops->abort_xfer(spi);
            aos_event_set(&spi->event, 0, AOS_EVENT_AND);
            return -EIO;
        } else {
            spi->ops->finish_xfer(spi);
            aos_event_set(&spi->event, 0, AOS_EVENT_AND);
        }

        last_msg = msg;
        last_pos = pos;

        if (msg_tail) {
            pos = 0;
            i++;
        } else {
            pos += spi->x.len;
        }
    }

    if (last_msg && last_msg->rx_buf)
        memcpy(&((uint8_t *)last_msg->rx_buf)[last_pos], spi->x.rx_buf, spi->x.len);
#else
    for (size_t i = 0; i < num_msgs; i++) {
        const aos_spi_msg_t *msg = &msgs[i];
        uint32_t mask;
        uint32_t val;
        aos_status_t ret;

        spi->x.timeout = XFER_TIMEOUT;
        spi->x.flags = AOS_SPI_XF_MSG_HEAD | AOS_SPI_XF_MSG_TAIL;
        spi->x.flags |= (i == 0) ? AOS_SPI_XF_SEQ_HEAD : 0;
        spi->x.flags |= (i + 1 == num_msgs) ? AOS_SPI_XF_SEQ_TAIL : 0;
        spi->x.flags |= msg->rx_buf ? AOS_SPI_XF_RX : 0;
        spi->x.flags |= msg->tx_buf ? AOS_SPI_XF_TX : 0;
        spi->x.cfg = msg->cfg;
        spi->x.cs = msg->cs;
        spi->x.hz = msg->hz;
        spi->x.pre_cs = msg->pre_cs;
        spi->x.post_cs = msg->post_cs;
        spi->x.pre_clk = msg->pre_clk;
        spi->x.post_clk = msg->post_clk;
        spi->x.len = msg->count;
        spi->x.head = 0;
        spi->x.tail = 0;
        spi->x.rx_buf = msg->rx_buf;
        spi->x.tx_buf = msg->tx_buf;

        ret = spi->ops->start_xfer(spi);
        if (ret)
            return ret;

        mask = EVENT_XFER_COMPLETE | EVENT_XFER_ERROR;
        if (aos_event_get(&spi->event, mask, AOS_EVENT_OR, &val, spi->x.timeout) || (val & EVENT_XFER_ERROR)) {
            spi->ops->abort_xfer(spi);
            aos_event_set(&spi->event, 0, AOS_EVENT_AND);
            return -EIO;
        } else {
            spi->ops->finish_xfer(spi);
            aos_event_set(&spi->event, 0, AOS_EVENT_AND);
        }
    }
#endif

    return 0;
}

aos_status_t aos_spi_transfer(aos_spi_ref_t *ref, const aos_spi_msg_t *msgs, size_t num_msgs)
{
    aos_spi_t *spi;
    aos_status_t ret = 0;

    if (!ref || !aos_dev_ref_is_valid(ref) || !msgs || num_msgs == 0)
        return -EINVAL;

    spi = aos_container_of(ref->dev, aos_spi_t, dev);

    for (size_t i = 0; i < num_msgs; i++) {
        const aos_spi_msg_t *msg = &msgs[i];

        if ((msg->cfg & AOS_SPI_MCFG_MODE_MASK) == AOS_SPI_MCFG_MODE_0 && !(spi->flags & AOS_SPI_F_MODE_0))
            return -EINVAL;

        if ((msg->cfg & AOS_SPI_MCFG_MODE_MASK) == AOS_SPI_MCFG_MODE_1 && !(spi->flags & AOS_SPI_F_MODE_1))
            return -EINVAL;

        if ((msg->cfg & AOS_SPI_MCFG_MODE_MASK) == AOS_SPI_MCFG_MODE_2 && !(spi->flags & AOS_SPI_F_MODE_2))
            return -EINVAL;

        if ((msg->cfg & AOS_SPI_MCFG_MODE_MASK) == AOS_SPI_MCFG_MODE_3 && !(spi->flags & AOS_SPI_F_MODE_3))
            return -EINVAL;

        if ((msg->cfg & AOS_SPI_MCFG_LSB_FIRST) && !(spi->flags & AOS_SPI_F_LSB_FIRST))
            return -EINVAL;

        if (!(msg->cfg & AOS_SPI_MCFG_LSB_FIRST) && !(spi->flags & AOS_SPI_F_MSB_FIRST))
            return -EINVAL;

        if (msg->count == 0 || (!msg->rx_buf && !msg->tx_buf))
            return -EINVAL;

        if ((msg->cfg & AOS_SPI_MCFG_WIDTH_MASK) == AOS_SPI_MCFG_WIDTH_1) {
            if ((msg->cfg & AOS_SPI_MCFG_3WIRE) && !(spi->flags & AOS_SPI_F_3WIRE))
                return -EINVAL;
            if (!(msg->cfg & AOS_SPI_MCFG_3WIRE) && !(spi->flags & AOS_SPI_F_4WIRE))
                return -EINVAL;
            if ((msg->cfg & AOS_SPI_MCFG_3WIRE) && msg->rx_buf && msg->tx_buf)
                return -EINVAL;
        } else {
            if (msg->rx_buf && msg->tx_buf)
                return -EINVAL;
        }

        if (msg->cs >= spi->num_cs)
            return -EINVAL;

        if (msg->hz < spi->min_hz || msg->hz > spi->max_hz)
            return -EINVAL;
    }

    for (size_t i = 0, j = 0; i < num_msgs; i++) {
        if (i + 1 < num_msgs && msgs[i].cs == msgs[i + 1].cs &&
            (msgs[i].cfg & AOS_SPI_MCFG_MODE_MASK) == (msgs[i + 1].cfg & AOS_SPI_MCFG_MODE_MASK))
            continue;

        aos_dev_lock(ref->dev);
        ret = transfer_sequence(spi, &msgs[j], i + 1 - j);
        aos_dev_unlock(ref->dev);
        if (ret)
            break;

        j = i + 1;
    }

    return ret;
}

static void dev_spi_unregister(aos_dev_t *dev)
{
    aos_spi_t *spi = aos_container_of(dev, aos_spi_t, dev);

    aos_event_free(&spi->event);

    if (spi->ops->unregister)
        spi->ops->unregister(spi);
}

static aos_status_t dev_spi_get(aos_dev_ref_t *ref)
{
    aos_spi_t *spi = aos_container_of(ref->dev, aos_spi_t, dev);

    if (!aos_dev_ref_is_first(ref))
        return 0;

    return spi->ops->startup(spi);
}

static void dev_spi_put(aos_dev_ref_t *ref)
{
    aos_spi_t *spi = aos_container_of(ref->dev, aos_spi_t, dev);

    if (!aos_dev_ref_is_last(ref))
        return;

    spi->ops->shutdown(spi);
}

static const aos_dev_ops_t dev_spi_ops = {
    .unregister = dev_spi_unregister,
    .get        = dev_spi_get,
    .put        = dev_spi_put,
};

aos_status_t aos_spi_register(aos_spi_t *spi)
{
    aos_status_t ret;

    if (!spi)
        return -EINVAL;

    if (!spi->ops || !spi->ops->startup || !spi->ops->shutdown)
        return -EINVAL;

    if ((!spi->ops->start_xfer || !spi->ops->finish_xfer || !spi->ops->abort_xfer) && !spi->ops->transfer_sequence)
        return -EINVAL;

    if (!(spi->flags & AOS_SPI_F_MODE_0) && !(spi->flags & AOS_SPI_F_MODE_1) &&
        !(spi->flags & AOS_SPI_F_MODE_2) && !(spi->flags & AOS_SPI_F_MODE_3))
        return -EINVAL;

    if (!(spi->flags & AOS_SPI_F_WIDTH_1) && !(spi->flags & AOS_SPI_F_WIDTH_2) &&
        !(spi->flags & AOS_SPI_F_WIDTH_4) && !(spi->flags & AOS_SPI_F_WIDTH_8))
        return -EINVAL;

    if (spi->flags & AOS_SPI_F_WIDTH_1) {
        if (!(spi->flags & AOS_SPI_F_4WIRE) && !(spi->flags & AOS_SPI_F_3WIRE))
            return -EINVAL;
    } else {
        spi->flags &= ~(AOS_SPI_F_4WIRE | AOS_SPI_F_3WIRE);
    }

    if (!(spi->flags & AOS_SPI_F_MSB_FIRST) && !(spi->flags & AOS_SPI_F_LSB_FIRST))
        return -EINVAL;

    if (spi->flags & AOS_SPI_F_NO_CS)
        spi->num_cs = 1;

    if (spi->num_cs == 0)
        return -EINVAL;

    if (spi->min_hz == 0 || spi->min_hz > spi->max_hz)
        return -EINVAL;

    spi->dev.type = AOS_DEV_TYPE_SPI;
    spi->dev.ops = &dev_spi_ops;
#ifdef AOS_COMP_VFS
    spi->dev.vfs_helper.name[0] = '\0';
    spi->dev.vfs_helper.ops = NULL;
#endif

    ret = aos_event_new(&spi->event, 0);
    if (ret)
        return ret;

    ret = aos_dev_register(&spi->dev);
    if (ret) {
        aos_event_free(&spi->event);
        return ret;
    }

    return 0;
}

aos_status_t aos_spi_register_argumented(aos_spi_t *spi, uint32_t id, const aos_spi_ops_t *ops, uint32_t flags,
                                         uint32_t num_cs, uint32_t min_hz, uint32_t max_hz)
{
    if (!spi)
        return -EINVAL;

    spi->dev.id = id;
    spi->ops = ops;
    spi->flags = flags;
    spi->num_cs = num_cs;
    spi->min_hz = min_hz;
    spi->max_hz = max_hz;

    return aos_spi_register(spi);
}

aos_status_t aos_spi_unregister(uint32_t id)
{
    return aos_dev_unregister(AOS_DEV_TYPE_SPI, id);
}

size_t aos_spi_hard_push(aos_spi_t *spi, void *tx_buf, size_t count)
{
    if (spi->x.head + count > spi->x.len)
        count = spi->x.len - spi->x.head;

    if ((spi->x.flags & AOS_SPI_XF_TX) && tx_buf) {
        for (size_t i = 0; i < count; i++)
            ((uint8_t *)tx_buf)[i] = ((const uint8_t *)spi->x.tx_buf)[spi->x.head + i];
    }

    spi->x.head += count;

    return count;
}

bool aos_spi_hard_pull(aos_spi_t *spi, const void *rx_buf, size_t count)
{
    if (spi->x.tail + count > spi->x.head)
        count = spi->x.head - spi->x.tail;

    if ((spi->x.flags & AOS_SPI_XF_RX) && rx_buf) {
        for (size_t i = 0; i < count; i++)
            ((uint8_t *)spi->x.rx_buf)[spi->x.tail + i] = ((const uint8_t *)rx_buf)[i];
    }

    spi->x.tail += count;
    if (spi->x.tail == spi->x.len) {
        aos_event_set(&spi->event, EVENT_XFER_COMPLETE, AOS_EVENT_OR);
        return true;
    }

    return false;
}

void aos_spi_hard_fail(aos_spi_t *spi)
{
    aos_event_set(&spi->event, EVENT_XFER_ERROR, AOS_EVENT_OR);
}
