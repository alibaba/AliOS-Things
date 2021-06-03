/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <drv/iic.h>
#include "hal_i2c.h"
#include "app_hal.h"
#include "aos/kernel.h"
#include "hal_trace.h"

csi_error_t csi_iic_init(csi_iic_t *iic, uint32_t idx)
{
    uint32_t ret = 0;
    int32_t lock_ret = -1;
    uint8_t i2c_port;
    struct HAL_I2C_CONFIG_T *i2c_cfg;

    if (!iic)
        return CSI_ERROR;

    i2c_cfg = (struct HAL_I2C_CONFIG_T *)malloc(sizeof(struct HAL_I2C_CONFIG_T));
    if (!i2c_cfg) {
        TRACEME("malloc for iHAL_I2C_CONFIG_T2c fail\n");
        return CSI_ERROR;
    }
    iic->dev.idx = idx;
    i2c_port = idx;

    if (i2c_port == 0) {
        hal_iomux_set_i2c0();
    } else if (i2c_port == 1) {
        hal_iomux_set_i2c1();
    }

    i2c_cfg->mode = HAL_I2C_API_MODE_TASK;
    i2c_cfg->use_dma = 0;
    i2c_cfg->use_sync = 1;
    i2c_cfg->speed = 100000; // set to 100k by default
    i2c_cfg->as_master = 1;

    ret = hal_i2c_open(i2c_port, i2c_cfg);
    if (ret) {
        TRACEME("open i2c fail\n");
        free(i2c_cfg);
        return CSI_ERROR;
    } else {
        TRACEME("open i2c succ.\n");
        iic->priv = i2c_cfg;
        return CSI_OK;
    }
}

void csi_iic_uninit(csi_iic_t *iic)
{
    uint8_t i2c_port;
    uint32_t ret = -1;

    if (!iic)
        return;

    i2c_port = iic->dev.idx;

    ret = hal_i2c_close(i2c_port);
    if (ret) {
        TRACEME("close i2c fail\n");
    }
    free(iic->priv);
    iic->priv = NULL;

    return;
}

csi_error_t csi_iic_mode(csi_iic_t *iic, csi_iic_mode_t mode)
{
    if (!iic)
        return CSI_ERROR;

    if (mode != IIC_MODE_MASTER) {
        TRACEME("i2c only support master mode\n");
        return CSI_UNSUPPORTED;
    }

    return CSI_OK;
}

csi_error_t csi_iic_addr_mode(csi_iic_t *iic, csi_iic_addr_mode_t addr_mode)
{
    if (!iic)
        return CSI_ERROR;
    // does not support addr mode set, always return success
    return CSI_OK;
}

csi_error_t csi_iic_speed(csi_iic_t *iic, csi_iic_speed_t speed)
{
    int32_t ret;
    uint32_t i2c_port;
    struct HAL_I2C_CONFIG_T *i2c_cfg = NULL;

    if (!iic)
        return CSI_ERROR;
#if 1

    i2c_port = iic->dev.idx;
    i2c_cfg = (struct HAL_I2C_CONFIG_T *)iic->priv;

    switch(speed) {
        case IIC_BUS_SPEED_STANDARD:
            i2c_cfg->speed = 100000;
        break;
        case IIC_BUS_SPEED_FAST:
            i2c_cfg->speed = 400000;
        break;
        case IIC_BUS_SPEED_FAST_PLUS:
        case IIC_BUS_SPEED_HIGH:
            i2c_cfg->speed = 1000000;
        break;
        default:
            i2c_cfg->speed = 100000;
        break;
    }
    //hal_i2c_set_speed(i2c_port, i2c_cfg->speed);
#else
    ret = hal_i2c_reopen(i2c_port, i2c_cfg);
    if (ret) {
        printf("reopen i2c fail\n");
        return CSI_ERROR;
    } else {
        printf("reopen i2c succ.\n");
        return CSI_OK;
    }
#endif
    return CSI_OK;
}



int32_t csi_iic_master_send(csi_iic_t *iic,
                            uint32_t devaddr,
                            const void *data, uint32_t size,
                            uint32_t timeout)
{
    int32_t ret;
    uint8_t i2c_port;

    if (!iic)
        return CSI_ERROR;

    i2c_port = iic->dev.idx;

    ret = hal_i2c_task_send(i2c_port, devaddr, data, size, 0, NULL);
    if (ret) {
        TRACEME("%s:%d,i2c send fail, devaddr = 0x%x, data[0] = 0x%x, data[1]= 0x%x, ret = %d\n",
                __func__, __LINE__, devaddr, data[0], data[1], ret);
        return 0;
    } else
        return size;
}

int32_t csi_iic_master_receive(csi_iic_t *iic,
                               uint32_t devaddr,
                               void *data, uint32_t size,
                               uint32_t timeout)
{
    int32_t ret;
    uint8_t i2c_port;

    if (!iic)
        return CSI_ERROR;

    if (!data || !size)
    {
        TRACE("i2c input para err");
        return -1;
    }

    i2c_port = iic->dev.idx;

    ret = hal_i2c_recv(i2c_port, devaddr, data, 0, size, HAL_I2C_RESTART_AFTER_WRITE, 0, NULL);
    if(ret) {
        TRACEME("%s:%d,i2c send fail, devaddr = 0x%x, data[0] = 0x%x, data[1]= 0x%x, ret = %d\n",
                __func__, __LINE__, devaddr, data[0], data[1], ret);
        return 0;
    } else
        return size;
}

int32_t csi_iic_mem_send(csi_iic_t *iic,
                         uint32_t devaddr,
                         uint16_t memaddr, csi_iic_mem_addr_size_t memaddr_size,
                         const void *data, uint32_t size,
                         uint32_t timeout)
{
    int32_t ret;
    uint8_t i2c_port;
    uint8_t *txbuf;
    uint16_t txlen;
    uint32_t mem_size;

    if (!iic)
        return CSI_ERROR;

    if (!data || !size)
    {
        TRACE("i2c input para err");
        return -1;
    }

    i2c_port = iic->dev.idx;

    switch (memaddr_size) {
        case IIC_MEM_ADDR_SIZE_8BIT:
            mem_size = 1;
            break;
        case IIC_MEM_ADDR_SIZE_16BIT:
            mem_size = 2;
            break;
        default:
            mem_size = 1;
            break;
    }
    txlen = size + mem_size;
    txbuf = (uint8_t *)malloc(txlen);
    if (txbuf  == NULL)
    {
        TRACE("%s malloc size %d error\r", __FUNCTION__, txlen);
        return -1;
    }

    memset(txbuf, 0, txlen);
    txbuf[0] = (uint8_t)memaddr;
    memcpy(&txbuf[1], data, size);

    ret = hal_i2c_task_send(i2c_port, devaddr, txbuf, txlen, 0, NULL);
    free(txbuf);

    if (ret) {
        TRACEME("%s:%d,i2c send failed,devaddr = 0x%x,ret = %d\n", __func__, __LINE__, devaddr, ret);
        return 0;
    } else
        return size;
}

int32_t csi_iic_mem_receive(csi_iic_t *iic,
                            uint32_t devaddr,
                            uint16_t memaddr, csi_iic_mem_addr_size_t memaddr_size,
                            void *data, uint32_t size,
                            uint32_t timeout)
{
    int32_t ret;
    uint8_t i2c_port;
    uint8_t *txrxbuf;
    uint16_t txrxlen;
    uint32_t mem_size;

    if (!iic)
        return CSI_ERROR;

    if (!data || !size)
    {
        TRACE("i2c input para err");
        return -1;
    }

    i2c_port = iic->dev.idx;

    switch (memaddr_size) {
        case IIC_MEM_ADDR_SIZE_8BIT:
            mem_size = 1;
            break;
        case IIC_MEM_ADDR_SIZE_16BIT:
            mem_size = 2;
            break;
        default:
            mem_size = 1;
            break;
    }

    txrxlen = size + mem_size;
    txrxbuf = (uint8_t *)malloc(txrxlen);
    if (txrxbuf  == NULL)
    {
        TRACE("%s malloc size %d error\r", __FUNCTION__, txrxlen);
        return -1;
    }

    memset(txrxbuf, 0, txrxlen);
    txrxbuf[0] = (uint8_t)memaddr;

    ret = hal_i2c_recv(i2c_port, devaddr, txrxbuf, mem_size, size, HAL_I2C_RESTART_AFTER_WRITE, 0, 0);
    if (ret) {
        TRACEME("%s:i2c recv failed,devaddr = 0x%x,ret = %d\n", __func__, devaddr, ret);
        ret = 0;
    } else {
        memcpy(data, &txrxbuf[1], size);
        ret = size;
    }
    free(txrxbuf);

    return ret;
}
