/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "objects.h"
#include "PinNames.h"
#include "pinmap.h"
#include <drv/iic.h>

static const PinMap CSI_PinMap_I2C_SCL[] = {
    {PA_25,     (int32_t)I2C0_DEV,        PIN_DATA(PullUp, PINMUX_FUNCTION_I2C)},
    {NC,        NC,            0}
};

static const PinMap CSI_PinMap_I2C_SDA[] = {
    {PA_26,     (int32_t)I2C0_DEV,        PIN_DATA(PullUp, PINMUX_FUNCTION_I2C)},
    {NC,        NC,            0}
};

csi_error_t csi_iic_init(csi_iic_t *iic, uint32_t idx)
{
    if(idx != 0)
        return CSI_ERROR;

    if(!iic)
        return CSI_ERROR;

    iic->priv = (I2C_InitTypeDef *)malloc(sizeof(I2C_InitTypeDef));

    uint32_t i2c_idx = idx;
    I2C_TypeDef * I2Cx;
    I2C_InitTypeDef *I2C_InitStruct = (I2C_InitTypeDef *)iic->priv;
    
    iic->dev.idx = i2c_idx;
    iic->mode = I2C_MASTER_MODE;
    I2Cx = I2C_DEV_TABLE[i2c_idx].I2Cx;

    /* Set I2C Device Number */
    I2C_InitStruct->I2CIdx = i2c_idx;

    /* Load I2C default value */
    I2C_StructInit(I2C_InitStruct);

    /* Assign I2C Pin Mux */
    I2C_InitStruct->I2CMaster     = I2C_MASTER_MODE;
    I2C_InitStruct->I2CSpdMod     = I2C_SS_MODE;
    I2C_InitStruct->I2CClk        = 100;
    I2C_InitStruct->I2CAckAddr    = 0;    

    /* I2C Pin Mux Initialization */
    PinName scl = CSI_PinMap_I2C_SCL[i2c_idx].pin;
    PinName sda = CSI_PinMap_I2C_SDA[i2c_idx].pin;

    printf("scl:%d,sda:%d\n",scl,sda);

    Pinmux_Config(scl, PINMUX_FUNCTION_I2C);
    Pinmux_Config(sda, PINMUX_FUNCTION_I2C);

    PAD_PullCtrl(scl, GPIO_PuPd_UP);
    PAD_PullCtrl(sda, GPIO_PuPd_UP);
    
    /* I2C HAL Initialization */
    I2C_Init(I2Cx, I2C_InitStruct);

    /* Enable i2c master RESTART function */
    I2Cx->IC_CON |= BIT_CTRL_IC_CON_IC_RESTART_EN;
    /* I2C Enable Module */
    I2C_Cmd(I2Cx, ENABLE);
    return CSI_OK;
}

void csi_iic_uninit(csi_iic_t *iic)
{
    uint32_t i2c_idx = iic->dev.idx;

    if(i2c_idx != 0)
        return;

    I2C_TypeDef * I2Cx = I2C_DEV_TABLE[i2c_idx].I2Cx;

    /* I2C Disable Module */
    I2C_Cmd(I2Cx, DISABLE);

    if(iic && iic->priv) {
        free(iic->priv);
        iic->priv = NULL;
    }
}

csi_error_t csi_iic_mode(csi_iic_t *iic, csi_iic_mode_t mode)
{
    uint32_t i2c_idx = iic->dev.idx;

    if(i2c_idx != 0)
        return CSI_ERROR;
    
    I2C_TypeDef *I2Cx = I2C_DEV_TABLE[i2c_idx].I2Cx;
    I2C_InitTypeDef *I2C_InitStruct = (I2C_InitTypeDef *)iic->priv;

    I2C_Cmd(I2Cx, DISABLE);

    iic->mode = mode;

    if(mode == IIC_MODE_MASTER)
        I2C_InitStruct->I2CMaster = I2C_MASTER_MODE;
    else if(mode == IIC_MODE_SLAVE)
        I2C_InitStruct->I2CMaster = I2C_SLAVE_MODE;
    
    I2C_Init(I2Cx, I2C_InitStruct);
    I2C_Cmd(I2Cx, ENABLE);
    return CSI_OK;
}

csi_error_t csi_iic_addr_mode(csi_iic_t *iic, csi_iic_addr_mode_t addr_mode)
{
    uint32_t i2c_idx = iic->dev.idx;

    if(i2c_idx != 0)
        return CSI_ERROR;

    I2C_TypeDef * I2Cx = I2C_DEV_TABLE[i2c_idx].I2Cx;
    I2C_InitTypeDef *I2C_InitStruct = (I2C_InitTypeDef *)iic->priv;

    I2C_Cmd(I2Cx, DISABLE);

    I2C_InitStruct->I2CAddrMod = addr_mode;
    I2C_Init(I2Cx, I2C_InitStruct);

    I2C_Cmd(I2Cx, ENABLE);
    return CSI_OK;
}

csi_error_t csi_iic_speed(csi_iic_t *iic, csi_iic_speed_t speed)
{
    uint32_t i2c_idx = iic->dev.idx;

    if(i2c_idx != 0)
        return CSI_ERROR;

    I2C_TypeDef * I2Cx = I2C_DEV_TABLE[i2c_idx].I2Cx;
    I2C_InitTypeDef *I2C_InitStruct = (I2C_InitTypeDef *)iic->priv;

    I2C_Cmd(I2Cx, DISABLE);

    switch(speed) {
        case IIC_BUS_SPEED_STANDARD:
            I2C_InitStruct->I2CSpdMod = I2C_SS_MODE;
            I2C_InitStruct->I2CClk = 100;
        break;
        case IIC_BUS_SPEED_FAST:
            I2C_InitStruct->I2CSpdMod = I2C_FS_MODE;
            I2C_InitStruct->I2CClk = 400;
        break;
        case IIC_BUS_SPEED_FAST_PLUS:
        case IIC_BUS_SPEED_HIGH:
            I2C_InitStruct->I2CSpdMod = I2C_HS_MODE;
            I2C_InitStruct->I2CClk = 1000;
        break;
        default:
            I2C_InitStruct->I2CSpdMod = I2C_SS_MODE;
            I2C_InitStruct->I2CClk = 100;
    }
    I2C_Init(I2Cx, I2C_InitStruct);
    I2C_Cmd(I2Cx, ENABLE);
    return CSI_OK;
}

/**
  * @brief  Read data with special length in master mode through the I2Cx peripheral under in-house IP.
  * @param  I2Cx: where I2Cx can be I2C0_DEV .
  * @param  pBuf: point to the buffer to hold the received data.
  * @param  len: the length of data that to be received.
  * @param  timeout_ms: specifies timeout time, unit is ms.  
  * @retval the length of data read. 
  */
int32_t I2C_MasterRead_TimeOut(I2C_TypeDef *I2Cx, uint8_t* pBuf, uint8_t len, uint32_t timeout_ms)
{
    int32_t cnt = 0;
    uint32_t InTimeoutCount = 0;

    /* read in the DR register the data to be received */
    for(cnt = 0; cnt < len; cnt++) {
        InTimeoutCount = timeout_ms*500;

        if(cnt >= len - 1) {
            /* generate stop singal */
            I2Cx->IC_DATA_CMD = 0x0003 << 8;
        } else {
            I2Cx->IC_DATA_CMD = 0x0001 << 8;
        }

        /* wait for I2C_FLAG_RFNE flag */
        while((I2C_CheckFlagState(I2Cx, BIT_IC_STATUS_RFNE)) == 0) {
            if(I2C_GetRawINT(I2Cx) & BIT_IC_RAW_INTR_STAT_TX_ABRT) {
                I2C_ClearAllINT(I2Cx);
                return cnt;
            }
            
            DelayUs(2);

            if (InTimeoutCount == 0) {
                printf("MasterRead_TimeOut\n");
                return cnt;
            }
            InTimeoutCount--;
        }

        *pBuf++ = (uint8_t)I2Cx->IC_DATA_CMD;
    }

    return cnt;
}

/**
  * @brief  Write data with special length in master mode through the I2Cx peripheral under in-house IP.
  * @param  I2Cx: where I2Cx can be I2C0_DEV.
  * @param  pBuf: point to the data to be transmitted.
  * @param  len: the length of data that to be received.
  * @param  timeout_ms: specifies timeout time, unit is ms.  
  * @retval the length of data send. 
  */
int32_t I2C_MasterWrite_TimeOut(I2C_TypeDef *I2Cx, uint8_t* pBuf, uint8_t len, uint32_t timeout_ms, int32_t stop)
{
    int32_t cnt = 0;
    uint32_t InTimeoutCount = 0;

    /* Write in the DR register the data to be sent */
    for(cnt = 0; cnt < len; cnt++)
    {
        InTimeoutCount = timeout_ms*500;

        while((I2C_CheckFlagState(I2Cx, BIT_IC_STATUS_TFNF)) == 0);

        if(cnt >= len - 1)
        {
            if(stop == 1) {
                /*generate stop signal*/
                I2Cx->IC_DATA_CMD = (*pBuf++) | (1 << 9);
            } else {
                /*generate restart signal*/
                I2Cx->IC_DATA_CMD = (*pBuf++) | (1 << 10);
            }
        }
        else
        {
            I2Cx->IC_DATA_CMD = (*pBuf++);
        }

        while((I2C_CheckFlagState(I2Cx, BIT_IC_STATUS_TFE)) == 0) {
            if(I2C_GetRawINT(I2Cx) & BIT_IC_RAW_INTR_STAT_TX_ABRT) {
                printf(" TX_ABRT = %x\n", I2Cx->IC_TX_ABRT_SOURCE);
                I2C_ClearAllINT(I2Cx);
                return cnt;
            }
            
            DelayUs(2);

            if (InTimeoutCount == 0) {
                printf("MasterWrite_TimeOut\n");
                return cnt;
            }
            InTimeoutCount--;
        }
    }

    return cnt;
}

/**
  * @brief  Master sends single byte through the I2Cx peripheral to detect slave device.
  * @param  obj: i2c object defined in application software.
  * @param  address: the address of slave that to be detected.
  * @param  timeout_ms: specifies timeout time, unit is ms.
  * @retval Slave ack condition:
  *          - 0: Slave available
  *          - -1: Slave not available 
  */
int32_t I2C_MasterSendNullData_TimeOut(I2C_TypeDef *I2Cx, int32_t address, uint32_t timeout_ms) 
{
    uint8_t I2CTemp = (uint8_t)(address<<1);
    I2C_MasterSendNullData(I2Cx, &I2CTemp, 0, 1, 0);

    DelayMs(timeout_ms);

    if(I2C_GetRawINT(I2Cx) & BIT_IC_RAW_INTR_STAT_TX_ABRT) {
        I2C_ClearAllINT(I2Cx);
        
        /* Wait for i2c enter trap state from trap_stop state*/
        DelayUs(100);
        I2C_Cmd(I2Cx, DISABLE);
        I2C_Cmd(I2Cx, ENABLE);

        return CSI_ERROR;
    }
    return CSI_OK;
}

int32_t csi_iic_master_send(csi_iic_t *iic,uint32_t devaddr,const void *data,uint32_t size,uint32_t timeout)
{
    uint32_t i2c_idx = iic->dev.idx;

    if(i2c_idx != 0)
        return CSI_ERROR;

    I2C_TypeDef * I2Cx = I2C_DEV_TABLE[i2c_idx].I2Cx;
    I2C_InitTypeDef *I2C_InitStruct = (I2C_InitTypeDef *)iic->priv;

    /* Check the parameters */
    if(!IS_I2C_ALL_PERIPH(I2Cx))
        return CSI_ERROR;

    if (I2C_InitStruct->I2CAckAddr != devaddr) {
        /* Deinit I2C first */        
        I2C_Cmd(I2Cx, DISABLE);

        /* Load the user defined I2C target slave address */
        I2C_InitStruct->I2CAckAddr = devaddr;

        /* Init I2C now */
        I2C_Init(I2Cx, I2C_InitStruct);
        I2C_Cmd(I2Cx, ENABLE);
    }

    if(!size) {
        return (I2C_MasterSendNullData_TimeOut(I2Cx, devaddr, timeout));
    }

    return (I2C_MasterWrite_TimeOut(I2Cx, (uint8_t*)data, size,timeout, 1));
}

int32_t csi_iic_master_receive(csi_iic_t *iic, uint32_t devaddr, void *data, uint32_t size, uint32_t timeout)
{
    uint32_t i2c_idx = iic->dev.idx;

    if(i2c_idx != 0)
        return CSI_ERROR;

    I2C_TypeDef * I2Cx = I2C_DEV_TABLE[i2c_idx].I2Cx;
    I2C_InitTypeDef *I2C_InitStruct = (I2C_InitTypeDef *)iic->priv;

    /* Check the parameters */
    if(!IS_I2C_ALL_PERIPH(I2Cx))
        return CSI_ERROR;

    if (I2C_InitStruct->I2CAckAddr != devaddr) {
        /* Deinit I2C first */        
        I2C_Cmd(I2Cx, DISABLE);

        /* Load the user defined I2C target slave address */
        I2C_InitStruct->I2CAckAddr = devaddr;

        /* Init I2C now */
        I2C_Init(I2Cx, I2C_InitStruct);
        I2C_Cmd(I2Cx, ENABLE);
    }
    return (I2C_MasterRead_TimeOut(I2Cx, (uint8_t*)data, size,timeout));
}

int32_t csi_iic_mem_send(csi_iic_t *iic, uint32_t devaddr,uint16_t memaddr, csi_iic_mem_addr_size_t memaddr_size,const void *data,uint32_t size,uint32_t timeout)
{
    uint32_t i = 0;
    int cnt = 0;
    uint32_t i2c_idx = iic->dev.idx;
    uint32_t mem_size;
    char *buf = NULL;

    if(i2c_idx != 0)
        return CSI_ERROR;

    buf = malloc(memaddr_size + size);
    if(!buf) {
        printf("malloc for i2c mem send fail\r\n");
        return CSI_ERROR;
    }

    I2C_TypeDef * I2Cx = I2C_DEV_TABLE[i2c_idx].I2Cx;
    I2C_InitTypeDef *I2C_InitStruct = (I2C_InitTypeDef *)iic->priv;

    switch(memaddr_size) {
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

    /* Check the parameters */
    if(!IS_I2C_ALL_PERIPH(I2Cx)) {
        free(buf);
        buf = NULL;
        return CSI_ERROR;
    }

    if (I2C_InitStruct->I2CAckAddr != devaddr) {
        /* Deinit I2C first */        
        I2C_Cmd(I2Cx, DISABLE);

        /* Load the user defined I2C target slave address */
        I2C_InitStruct->I2CAckAddr = devaddr;

        /* Init I2C now */
        I2C_Init(I2Cx, I2C_InitStruct);
        I2C_Cmd(I2Cx, ENABLE);
    }

    if (mem_size == 1) {
	    buf[i++] = (memaddr) & 0xFF;
    } else
        buf[i++] = (memaddr >> 8) & 0xFF;

    memcpy(&buf[i], data, size);

    cnt = I2C_MasterWrite_TimeOut(I2Cx, (uint8_t*)buf, mem_size + size, timeout, 1);
    free(buf);
    buf = NULL;

    if (cnt == (mem_size + size))
        return size;
    else
        return cnt;
}

int32_t csi_iic_mem_receive(csi_iic_t *iic,uint32_t devaddr, uint16_t memaddr,csi_iic_mem_addr_size_t memaddr_size,void *data,uint32_t size,uint32_t timeout)
{
    int cnt = 0;
    uint32_t i2c_idx = iic->dev.idx;
    uint32_t mem_size;

    if(i2c_idx != 0)
        return CSI_ERROR;

    I2C_TypeDef * I2Cx = I2C_DEV_TABLE[i2c_idx].I2Cx;
    I2C_InitTypeDef *I2C_InitStruct = (I2C_InitTypeDef *)iic->priv;
    
    uint8_t memaddr_t[2];
    memaddr_t[0] = (memaddr) & 0xFF;
    memaddr_t[1] = (memaddr >> 8) & 0xFF;

    switch(memaddr_size) {
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

    /* Check the parameters */
    if(!IS_I2C_ALL_PERIPH(I2Cx))
        return CSI_ERROR;

    if (I2C_InitStruct->I2CAckAddr != devaddr) {
        /* Deinit I2C first */        
        I2C_Cmd(I2Cx, DISABLE);

        /* Load the user defined I2C target slave address */
        I2C_InitStruct->I2CAckAddr = devaddr;

        /* Init I2C now */
        I2C_Init(I2Cx, I2C_InitStruct);
        I2C_Cmd(I2Cx, ENABLE);
    }

    cnt = I2C_MasterWrite_TimeOut(I2Cx, memaddr_t, mem_size, timeout, 0);
    cnt = I2C_MasterRead_TimeOut(I2Cx, (uint8_t *)data, size, timeout);
    return cnt;
}
