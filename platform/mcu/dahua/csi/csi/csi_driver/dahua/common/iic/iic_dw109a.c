/*************************************************
  Copyright (C), 2011-2017 ZheJiang Dahua Technology Co., Ltd.
  文件名:   iic_drv.c
  作  者:   zhangjian(30803)<zhang_jian5@dahuatech.com>
  版  本:   V1.0.0
  日  期：  2017-10-12
  描  述:   IIC驱动drv层实现，为保证代码执行速度，有部分代码被
            放在了内部RAM区

            1、使用说明


            2、局限性


  修订历史:
  1. 日    期: 2017-10-12
     修订版本: V1.0.0
     作    者: zhangjian
     修订备注: 创建

  2. 日    期:
     修订版本:
     作    者:
     修订备注:
*************************************************/
#include <stdio.h>

#include "iic_dw109a.h"
#include "drv_iic.h"
#include "dh_macro.h"



#define DHC_I2C_TIMEOUT     (0x1FFFF)
#define DHC_I2C_FIFOSTATUS  (0x4)

/* 保存IIC速度配置 */
static DH_U32 gu32IicSpdMode;

#define STATIC_CODE_SECTION


#define IIC_REG_WRITE(u32Value, u32Addr)     (*(volatile DH_U32 *)(u32Addr) = (DH_U32)(u32Value))
#define IIC_REG_READ(u32Addr)                (*(volatile DH_U32*)(u32Addr)) 

/*************************************************
函数名 :   I2C_DRV_SetReg
描  述 :  配置IIC寄存器
输  入 :  参数        描述
          u32Value      配置的寄存器值
          pvBaseAddr    IIC设备基址
          u32RegOffset  具体寄存器地址
输  出 :  参数        描述
          无          无
返  回 :  返回值      描述
          无          无
*************************************************/
DH_VOID inline  I2C_DRV_SetReg(DH_U32 u32Value, DH_VOID *pvBaseAddr, DH_U32 u32RegOffset)
{
    DH_U32 lu32Addr;
    
    lu32Addr = (DH_U32)(pvBaseAddr) + u32RegOffset;
    
    IIC_REG_WRITE(u32Value, lu32Addr);
}

/*************************************************
函数名 :   I2C_DRV_GetReg
描  述 :  读IIC寄存器
输  入 :  参数        描述
          pvBaseAddr    IIC设备基址
          u32RegOffset  具体寄存器地址
输  出 :  参数        描述
          无          无
返  回 :  返回值      描述
          DH_U32       寄存器值
*************************************************/
DH_U32 inline  I2C_DRV_GetReg(DH_VOID *pvBaseAddr, DH_U32 u32RegOffset)
{
    DH_U32 lu32Addr;
    
    lu32Addr = (DH_U32)(pvBaseAddr) + u32RegOffset;
    
    return IIC_REG_READ(lu32Addr);
}

/*************************************************
函数名 :  I2cDrvEnableI2c
描  述 :  配置IIC使能
输  入 :  参数        描述
          pvBaseAddr   IIC模块寄存器首地址
输  出 :  参数        描述
          无          无
返  回 :  返回值      描述
          DH_SUCCESS  执行成功
*************************************************/
DH_S32 STATIC_CODE_SECTION I2cDrvEnableI2c(DH_VOID *pvBaseAddr, DH_U32 u32Enable)
{
     I2C_DRV_SetReg(u32Enable, pvBaseAddr, I2C_REG_ENABLE);

    return DH_SUCCESS;
}

/*******************************************************************************
 * 函数名  : I2C_DRV_SetStart
 * 描  述  : 配置i2c传输开关
 * 输  入  :  参数        描述
 *         :   pvBaseAddr         IIC模块寄存器首地址
 *         :   u32Enable          开关值
 * 输  出  : 无
 * 返回值  : DH_SUCCESS: 成功
 *           DH_FAILURE: 失败
 *******************************************************************************/
DH_S32 STATIC_CODE_SECTION I2C_DRV_SetStart(DH_VOID *pvBaseAddr,DH_U32 u32Enable)
{
     I2C_DRV_SetReg(u32Enable, pvBaseAddr, I2C_REG_START);

    return DH_SUCCESS;
}


/*************************************************
函数名 :  I2cDrvInitInterrupts
描  述 :  IIC中断初始化
输  入 :  参数        描述
          pvBaseAddr   IIC模块寄存器首地址
输  出 :  参数        描述
          无          无
返  回 :  返回值      描述
          DH_SUCCESS  执行成功
*************************************************/
static DH_S32 I2cDrvInitInterrupts(DH_VOID *pvBaseAddr)
{
    DH_U32 lu32IntReg = 0;

    /*清中断状态*/
     I2C_DRV_GetReg(pvBaseAddr, I2C_REG_CLR_INTR);

    //lu32IntReg = I2C_REG_BIT_INT_TX_ABRT;
    lu32IntReg = 0x8ff;
    
     I2C_DRV_SetReg(lu32IntReg, pvBaseAddr, I2C_REG_INTR_MASK);

    return DH_SUCCESS;
}


/*************************************************
函数名 :  I2C_DRV_ConfigClk
描  述 :  IIC配置时钟
输  入 :  参数        描述
          pvBaseAddr   IIC模块寄存器首地址
          u32Clk       IIC时钟
输  出 :  参数        描述
          无          无
返  回 :  返回值      描述
          DH_SUCCESS  配置成功
          DH_FAILURE  配置失败
*************************************************/
DH_S32 I2C_DRV_ConfigClk(DH_VOID *pvBaseAddr, DH_U32 u32Clk)
{
    DH_U32 lu32SpdMode, lu32Tmp, lu32RegOffset;


    if (u32Clk >= DHC_I2C_SPD_FAST)
    {
        lu32SpdMode = DHC_I2C_SPDMODE_HIGH;
        lu32RegOffset = I2C_REG_FS_SCL_HCNT;
    }
    else if (u32Clk > DHC_I2C_SPD_STANDARD)
    {
        lu32SpdMode = DHC_I2C_SPDMODE_FAST;
        lu32RegOffset = I2C_REG_FS_SCL_HCNT;
    }
    else
    {
        lu32SpdMode = DHC_I2C_SPDMODE_STD;
        lu32RegOffset = I2C_REG_FS_SCL_HCNT;
    }

    if (gu32IicSpdMode != lu32SpdMode)
    {
        lu32Tmp =  I2C_DRV_GetReg(pvBaseAddr, I2C_REG_CON);
        lu32Tmp &= I2C_REG_BITMASK_SPDMODE;
        lu32Tmp |= (lu32SpdMode << I2C_REG_BITSHF_SPDMODE);
         I2C_DRV_SetReg(lu32Tmp, pvBaseAddr, I2C_REG_CON);

        gu32IicSpdMode = lu32SpdMode;
    }

    lu32Tmp = (I2C_CLK / u32Clk) >> 1;
     I2C_DRV_SetReg(lu32Tmp, pvBaseAddr, lu32RegOffset);
     I2C_DRV_SetReg(lu32Tmp, pvBaseAddr, (lu32RegOffset + 0x04));

    return DH_SUCCESS;
}


/*************************************************
函数名 :  I2cDrvSetConfig
描  述 :  IIC配置
输  入 :  参数        描述
          pvBaseAddr   IIC模块寄存器首地址
          u32Clk       IIC时钟
输  出 :  参数        描述
          无          无
返  回 :  返回值      描述
          DH_SUCCESS  配置成功
*************************************************/
static DH_S32 I2cDrvSetConfig(DH_VOID *pvBaseAddr)
{
    DH_U32 lu32Cfg;
    
    lu32Cfg = I2C_REG_BIT_MASTER_MODE | I2C_REG_BIT_SPDMD_FAST;
    I2C_DRV_SetReg(lu32Cfg, pvBaseAddr, I2C_REG_CON);

    return DH_SUCCESS;
}


/*************************************************
函数名 :  I2cDrvSetSlaveAddr
描  述 :  IIC通讯时，配置从设备地址
输  入 :  参数        描述
          pvBaseAddr   IIC模块寄存器首地址
          u32DevAddr   IIC从设备地址
输  出 :  参数        描述
          无          无
返  回 :  返回值      描述
          DH_SUCCESS  配置成功
*************************************************/
static DH_S32 STATIC_CODE_SECTION I2cDrvSetSlaveAddr(DH_VOID *pvBaseAddr,DH_U32 u32DevAddr)
{
     I2C_DRV_SetReg(u32DevAddr, pvBaseAddr,I2C_REG_TAR);

    return DH_SUCCESS;
}


/*************************************************
函数名 :  I2cDrvWaitTimeOver
描  述 :  IIC通讯时，检查是否完成通讯
输  入 :  参数        描述
          pvBaseAddr   IIC模块寄存器首地址
          u32FifoFlag  发送或者接收fifo状态标志
输  出 :  参数        描述
          无          无
返  回 :  返回值      描述
          DH_SUCCESS  正常完成
          DH_FAILURE  超时退出
*************************************************/
static DH_S32 STATIC_CODE_SECTION I2cDrvWaitTimeOver(DH_VOID *pvBaseAddr, DH_U32 u32FifoFlag)
{
    DH_S32 i;
    volatile DH_U32 lu32Tmp;

    for (i = 0; i < DHC_I2C_TIMEOUT; i++)
    {
        lu32Tmp =  I2C_DRV_GetReg(pvBaseAddr, I2C_REG_STATUS);
        if ((I2C_REG_BIT_ACTIVITY != (lu32Tmp & I2C_REG_BITMASK_ACTIVITY))
            && (lu32Tmp & u32FifoFlag))
        {
            return DH_SUCCESS;
        }
    }

    return DH_FAILURE;
}


/*************************************************
函数名 :  I2cDrvGetIntStatus
描  述 :  获取IIC模块中断状态
输  入 :  参数        描述
          pvBaseAddr   IIC模块寄存器首地址
输  出 :  参数        描述
          无          无
返  回 :  返回值      描述
          DH_U32       中断状态
*************************************************/
static DH_U32 STATIC_CODE_SECTION I2cDrvGetIntStatus(DH_VOID *pvBaseAddr)
{
    return  I2C_DRV_GetReg(pvBaseAddr, I2C_REG_INTR_STAT);
}


/*************************************************
函数名 :  I2cDrvGetRecvFifoCnt
描  述 :  获取IIC模块接收FIFO中有效数据个数
输  入 :  参数        描述
          pvBaseAddr   IIC模块寄存器首地址
输  出 :  参数        描述
          无          无
返  回 :  返回值      描述
          DH_U32       有效个数
*************************************************/
static DH_U32 STATIC_CODE_SECTION I2cDrvGetRecvFifoCnt(DH_VOID *pvBaseAddr)
{
     I2C_DRV_GetReg(pvBaseAddr, I2C_REG_RXFLR);
    
    return  I2C_DRV_GetReg(pvBaseAddr, I2C_REG_RXFLR);
}


/*************************************************
函数名 :  I2C_DRV_IrqHandle
描  述 :  IIC模块中断服务程序
输  入 :  参数        描述
          pvBaseAddr   IIC模块寄存器首地址
输  出 :  参数        描述
          无          无
返  回 :  返回值      描述
          无          无
*************************************************/
DH_VOID STATIC_CODE_SECTION I2C_DRV_IrqHandle(DH_VOID *pvDevID)
{
    DH_U32 lu32Status;
    struct dhc_i2c_dev *pstDev = (struct dhc_i2c_dev *)pvDevID;

    lu32Status = I2cDrvGetIntStatus(pstDev->base);
    if ((lu32Status & I2C_REG_BIT_INT_TX_ABRT) > 0)
    {
        pstDev->abort = DH_TRUE;
    }

    if ((lu32Status & I2C_REG_BIT_INT_FREE) > 0)
    {
        pstDev->complete = DH_TRUE;
    }
}


/*************************************************
函数名 :  I2cDrvSendMsgs
描  述 :  IIC通讯发送部分,写iic时只有一个msgs
输  入 :  参数        描述
          pstDev        IIC设备
          pstMsgs       IIC消息
          s32Num        IIC消息个数
输  出 :  参数        描述
          无          无
返  回 :  返回值      描述
          DH_SUCCESS  传输完成
          DH_FAILURE  传输失败
*************************************************/
DH_S32 STATIC_CODE_SECTION I2cDrvSendMsgs(struct dhc_i2c_dev *pstDev, struct i2c_msg *pstMsgs, DH_S32 u32Num)
{
    DH_U8  i;
    DH_U16 *lpu16Cmd = pstDev->cmdBuf;
    DH_S32 ls32Ret;

    if ((u32Num != 1) || (pstMsgs->len > I2C_BUF_DEP))
    {
        printf("iic write err!\n");

        return DH_FAILURE;
    }

    for (i = 0; i < pstMsgs->len; i++)
    {
        lpu16Cmd[i] = pstMsgs->buf[i];
    }

    pstDev->complete = DH_FALSE;
    pstDev->abort = DH_FALSE;

    I2cDrvEnableI2c(pstDev->base, I2C_DISABLES);
    I2C_DRV_GetReg(pstDev->base, I2C_REG_CLR_INTR);
    I2cDrvSetSlaveAddr(pstDev->base, pstMsgs->addr);
    I2cDrvEnableI2c(pstDev->base, I2C_ENABLES);

    for (i = 0; i < pstMsgs->len; i++)
    {
         I2C_DRV_SetReg(lpu16Cmd[i], pstDev->base, I2C_REG_DATA_CMD);
    }

    I2C_DRV_SetStart(pstDev->base, I2C_START);


    ls32Ret = I2cDrvWaitTimeOver(pstDev->base, DHC_I2C_FIFOSTATUS);

    I2C_DRV_SetStart(pstDev->base, I2C_STOP);
    I2cDrvEnableI2c(pstDev->base, I2C_DISABLES);

    if ((DH_SUCCESS == ls32Ret))
    {
        return DH_SUCCESS;
    }
    else
    {
        if(DH_FAILURE == ls32Ret)
        {
            printf("IIC SendMsg timeout!\n");
        }

        return DH_FAILURE;
    }
    return 1;

}


/*************************************************
函数名 :  I2cDrvRecvMsgs
描  述 :  IIC通讯发送部分,读iic时只有两个msg,每次读取个数最多不超过6个字节
输  入 :  参数        描述
          pstDev        IIC设备
          pstMsgs       IIC消息
          s32Num        IIC消息个数
输  出 :  参数        描述
          无          无
返  回 :  返回值      描述
          DH_SUCCESS  传输完成
          DH_FAILURE  传输失败
*************************************************/
DH_S32 STATIC_CODE_SECTION I2cDrvRecvMsgs(struct dhc_i2c_dev *pstDev, struct i2c_msg *pstMsgs, DH_S32 s32Num)
{
    DH_U32 lu32RdCnt, i, j;
    DH_S32 ls32Ret;
    DH_U16 *lpu16CmdDatPtr;
    struct i2c_msg *lpstRdMsg,*lpstWrMsg;

    lpstWrMsg = pstMsgs;
    lpstRdMsg = pstMsgs + 1;
    lpu16CmdDatPtr = pstDev->cmdBuf;
    lu32RdCnt = lpstRdMsg->len + lpstWrMsg->len;

    if (lu32RdCnt > I2C_BUF_DEP)
    {
        return DH_FAILURE;
    }


    for (i = 0; i < lpstWrMsg->len; i++)
    {

        lpu16CmdDatPtr[i] = lpstWrMsg->buf[i];
        lpu16CmdDatPtr[i] &= ~(I2C_REG_BIT_CDM_READ);
    }


    for (j = 0; j < lpstRdMsg->len; j++, i++)
    {
        lpu16CmdDatPtr[i] = 0;
        lpu16CmdDatPtr[i] |= (0x1 << 8);
    }

    pstDev->complete = DH_FALSE;
    pstDev->abort = DH_FALSE;

    I2cDrvEnableI2c(pstDev->base, I2C_DISABLES);
    I2C_DRV_GetReg(pstDev->base, I2C_REG_CLR_INTR);
    I2cDrvSetSlaveAddr(pstDev->base, pstMsgs->addr);

    I2cDrvEnableI2c(pstDev->base, I2C_ENABLES);
    for (j = 0; j < i; j++)
    {
         I2C_DRV_SetReg(lpu16CmdDatPtr[j], pstDev->base, I2C_REG_DATA_CMD);
    }


    I2C_DRV_SetStart(pstDev->base, I2C_START);

    ls32Ret = I2cDrvWaitTimeOver(pstDev->base, DHC_I2C_FIFOSTATUS << 1);
    lu32RdCnt = I2cDrvGetRecvFifoCnt(pstDev->base);


    for (i = 0; i < lu32RdCnt; i++)
    {
        lpstRdMsg->buf[i] =  I2C_DRV_GetReg(pstDev->base, I2C_REG_DATA_CMD);
    }

 
    I2C_DRV_SetStart(pstDev->base, I2C_STOP);


    I2cDrvEnableI2c(pstDev->base, I2C_DISABLES);

    if ((DH_SUCCESS == ls32Ret) && (lu32RdCnt == lpstRdMsg->len))
    {
        return DH_SUCCESS;
    }
    else
    {
        if (DH_FAILURE == ls32Ret)
        {
            printf("IIC RecvMsg timeout!\n");
        }

        if (DH_TRUE == pstDev->abort)
        {
            printf("IIC RecvMsg abort!\n");
        }

        if (lu32RdCnt != lpstRdMsg->len)
        {
            printf("IIC set read cnt %d, but receive fifo cnt %d\n", lpstRdMsg->len, lu32RdCnt);
        }
    }

    return DH_FAILURE;
}


/*************************************************
函数名 :  I2C_DRV_Xfer
描  述 :  IIC通讯分发部分
输  入 :  参数        描述
          pstDev        IIC设备
          pstMsgs       IIC消息
          u32Num        IIC消息个数
输  出 :  参数        描述
          无          无
返  回 :  返回值      描述
          DH_SUCCESS  传输完成
          DH_FAILURE  传输失败
*************************************************/
DH_S32 STATIC_CODE_SECTION I2C_DRV_Xfer(struct dhc_i2c_dev *pstDev, struct i2c_msg *pstMsgs, int s32Num)
{
    //cyg_mutex_lock(&gstI2cDrvMutex);
    /*if ((s32Num > 1) && !(pstMsgs->flags & I2C_M_RD))
    {
        I2cDrvRecvMsgs(pstDev, pstMsgs, s32Num);
    }
    else
    {
        I2cDrvSendMsgs(pstDev, pstMsgs, s32Num);
    }*/
    //cyg_mutex_unlock(&gstI2cDrvMutex);
    return DH_SUCCESS;
}


/* I2C控制器ARCH层驱动去初始化 */
DH_S32 I2C_DRV_DeInit(DH_VOID *pvBaseAddr)
{
    return DH_SUCCESS;
}


/* I2C控制器ARCH层驱动初始化 */
DH_S32 I2C_DRV_Init(DH_VOID *pvBaseAddr, DH_U32 u32Clk)
{
    gu32IicSpdMode = DHC_I2C_SPDMODE_REV;


    IIC_REG_WRITE(0, 0x11002004);

    I2cDrvEnableI2c(pvBaseAddr, I2C_DISABLES);
    I2cDrvInitInterrupts(pvBaseAddr);
    I2cDrvSetConfig(pvBaseAddr);
    I2C_DRV_ConfigClk(pvBaseAddr, u32Clk);

    return DH_SUCCESS;
}

