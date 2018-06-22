/*************************************************
  Copyright (C), 2011-2015 ZheJiang Dahua Technology Co., Ltd.
  文件名:   lcsfc.c
  作  者:   zheng_xingjian(11853)<zheng_xingjian@dahuatech.com>
  版  本:   V1.0.0
  日  期：  2016-02-22
  描  述:   LCSFC模块驱动，只要实现了初始化，以及对Flash的编程操作
  
            1、使用说明
            因LCSFC模块只有DH5010M有，所以直接在ARCH层实现API
            
            本文件中，外部函数使用DH_MDK_LCSFC_xxx, 内部函数使用LcsfcXxxx
           
            2、局限性
            仅适用于eCos3.0
  
  修订历史:
  1. 日    期: 2015-10-24c
     修订版本: v0.1
     作    者: zheng_xingjian
     修订备注: 创建
     
  2. 日    期:
     修订版本:2018-1-22
     作    者: zhang_jian
     修订备注:移植到dh5021a上
*************************************************/
#include <dh_type.h>
#include "lcsfc.h"


/* 保存最大的Flash大小 */
static DH_U32 gu32FlashMaxSize;

#define STATIC_CODE_SECTION    __attribute__((section (".staticcode")))
/* 测试代码  */
#if 0
/*************************************************
  函数名 :  LcsfcUartPutChar
  描  述 :  串口发送一个字节
  输  入 :  参数        描述
            c           待发送数据
  输  出 :  参数        描述
            无          无
  返  回 :  返回值      描述
            无          无
*************************************************/
static DH_VOID STATIC_CODE_SECTION LcsfcUartPutChar(const char c)
{
    volatile DH_U32 lu32Tmp;
    
    /*
    while(1)
    {
        lu32Tmp = *(volatile DH_U32 *)(0x14030014);
        
        if (0 == (lu32Tmp & (1 << 0x05)))
        {
            break;
        }
    }*/
    
    *(volatile DH_U8 *)(0x14030000) = c;
    
}


/*************************************************
  函数名 :  LcsfcUartPutString
  描  述 :  从串口读取一个字节的数据，阻塞方式
  输  入 :  参数        描述
            pszString   待发送的字符串指针
  输  出 :  参数        描述
            无          无
  返  回 :  返回值      描述
            无          无
*************************************************/
static DH_VOID STATIC_CODE_SECTION LcsfcUartPutString(char *pszString)
{
    char *p = pszString;
    
    while(1)
    {
        if ('\0' == *p)
        {
            break;
        }
        else
        {
            /* 因为控制台的换行需要在增加\r输出 */
            if ('\n' == *p)
            {
                LcsfcUartPutChar('\r');
            }
            
            LcsfcUartPutChar(*p);
        }
        
        p++;
    }
}


/*************************************************
函数名 :  SerialDrvNumToStr
描  述 :  数字转字符,因内部使用，简化代码,仅转换Hex
输  入 :  参数        描述
          u32Num     待转数字
输  出 :  参数        描述
          pszStr      转成字符缓冲
返  回 :  返回值      描述
          无          无
*************************************************/
static DH_VOID STATIC_CODE_SECTION LcsfcNumToStr(DH_CHAR * pszStr, DH_U32 u32Num)
{
    DH_U32 i, lu32Tmp, lu32Offset, lu32Zero;
    char *lpStr;
    char lcTmp;
    
    lpStr = pszStr;
    
    *lpStr ++ = '0';
    *lpStr ++ = 'x';
    lu32Offset = 28;
    lu32Zero = 0;
    
    for (i = 0; i < 8; i++)
    {
        lu32Tmp = (u32Num  >> lu32Offset);
        lu32Offset -= 4;
        
        lu32Tmp &= 0x0F;
        
        if (lu32Tmp < 10)
        {
            lcTmp = 0x30 + lu32Tmp;
        }
        else
        {
            lcTmp = 0x57 + lu32Tmp;
        }
        
        if ((lu32Tmp) && (0 == lu32Zero))
        {
            lu32Zero = 1;
        }
    
        if (lu32Zero)
        {
            *lpStr ++ = lcTmp;
        }
    }
    
    /* 处理全零问题 */
    if (0 == lu32Zero)
    {
        *lpStr ++ = '0';
    }
    
    /* 加入换行和结束符 */
    *lpStr ++ = '\n';
    *lpStr ++ = '\0';
    
}


/*************************************************
函数名 :  LcsfcPrintf
描  述 :  实现单个数据输入转换成字符，并与输入的字符合并输出
          全部字符不能超过32个字节，包括结束符
输  入 :  参数        描述
          pszSrcStr   待合并输出的字符，调试信息等
          u32Num      待转换成字符的数据
输  出 :  参数        描述
          无          无
返  回 :  返回值      描述
          无          无
*************************************************/
static DH_VOID STATIC_CODE_SECTION LcsfcPrintf(DH_U32 u32Num)
{
    static char szBuf[32];
    char *lpszDst;
    char lcTmp;
    
    lpszDst = szBuf;
    
    /* 复制源字符串 */
    *lpszDst++ = 'V';
    *lpszDst++ = 'a';
    *lpszDst++ = 'l';
    *lpszDst++ = '=';
    *lpszDst++ = ' ';
    
    /* 加入数字和结束符 */
    LcsfcNumToStr(lpszDst, u32Num);
    
    LcsfcUartPutString(szBuf);
}
#endif /* LCSFC_DEBUG */

/* 内联函数，实现单字节和4字节写命令 */
static inline void Lcsfc_WrCmdByte(DH_U8 u8Cmd)
{
    *(volatile DH_U8 *)(LCSFC_REG_BASE) = u8Cmd;
}

static inline void Lcsfc_WrCmdDw(DH_U32 u32Cmd)
{
    *(volatile DH_U32 *)(LCSFC_REG_BASE) = u32Cmd;
}

/*************************************************
  函数名 :  DH_MDK_LCSFC_Init
  描  述 :  LCSFC初始化
  输  入 :  参数            描述
            u32FMaxSize     最大支持Flash大小
            u32Baudrate     对Flash的分频配置，只能是偶数分频，如果配置了奇数，强制加1成偶数
  输  出 :  无
  返  回 :  返回值          描述
            DH_SUCCESS      成功
            DH_FAILURE      失败
*************************************************/
DH_S32 DH_MDK_LCSFC_Init(DH_U32 u32FMaxSize, DH_U32 u32Baudrate)
{
    DH_U32 lu32Tmp, lu32ChkSum;
    DH_S32 ls32Ret;
    
    ls32Ret = DH_FAILURE;
    
    /* 配置波特率 */
    if (0 == u32Baudrate)
    {
        u32Baudrate = 2;
    }
    else if (u32Baudrate & 0x01)
    {
        u32Baudrate++;
    }
    else
    {
        /* nothing */
    }
    lu32Tmp = *(volatile DH_U32 *)(LCSFC_REG_BAUDRATE);
    if(u32Baudrate > lu32Tmp)
    {
        *(volatile DH_U32 *)(LCSFC_REG_BAUDRATE) = u32Baudrate;  
        *(volatile DH_U32 *)(LCSFC_REG_SAMDLY) = u32Baudrate >> 1;         
    }
    else
    {
        *(volatile DH_U32 *)(LCSFC_REG_SAMDLY) = u32Baudrate >> 1;
        *(volatile DH_U32 *)(LCSFC_REG_BAUDRATE) = u32Baudrate;
    } 
    
    if (u32FMaxSize > LCSFC_FLASH_MAX_SIZE)
    {
        gu32FlashMaxSize = LCSFC_FLASH_MAX_SIZE;
    }
    else
    {
        gu32FlashMaxSize = u32FMaxSize;
    }
    
    /* 检查0地址是否是DH_FLAG，并且对头部空间做校验和，
    如果其中有一个错误，则返回错误 */
    lu32Tmp = *(volatile DH_U32 *)(LCSFC_REG_BASE);
    if ( DAHUA_FLAG == lu32Tmp )
    {
        /* 进行校验，如果校验失败，仍然需要进行默认编程 */
        lu32Tmp += *(volatile DH_U32 *)(LCSFC_REG_BASE + 0x04);
        lu32ChkSum = *(volatile DH_U32 *)(LCSFC_REG_BASE + 0x08);
        if (lu32Tmp == lu32ChkSum)
        {
            ls32Ret = DH_SUCCESS;
        }
    }
    
    return ls32Ret;
}


/*************************************************
  函数名 :  DH_MDK_LCSFC_DeInit
  描  述 :  LCSFC去初始化
  输  入 :  参数            描述
            无              无
  输  出 :  无
  返  回 :  返回值          描述
            无              无
*************************************************/
DH_VOID DH_MDK_LCSFC_DeInit(DH_VOID)
{
    /* nothing */
}


/*************************************************
  函数名 :  DH_MDK_LCSFC_Erase
  描  述 :  LCSFC擦除命令
  输  入 :  参数            描述
            u32TimeOutCnt   超时次数
  输  出 :  无
  返  回 :  返回值          描述
            DH_SUCCESS      Flash处于空闲状态
            DH_FAILURE      Flash忙，并超时
*************************************************/
static DH_S32 STATIC_CODE_SECTION  LcsfcCheckBusy(DH_U32 u32TimeOutCnt)
{
    DH_U32 lu32Tmp, lu32TmCnt;
    
    lu32TmCnt = 0;
    while (1)
    {
        lu32Tmp = *(volatile DH_U32 *)(LCSFC_CHK_FLUSHADDR);
        lu32Tmp = *(volatile DH_U32 *)(LCSFC_CHK_FLAGADDR);
        if (DAHUA_FLAG == lu32Tmp)
        {
            return DH_SUCCESS;
        }
        
        lu32TmCnt++;
        if (lu32TmCnt >= u32TimeOutCnt)
        {
            return DH_FAILURE;
        }
    }
}


/*************************************************
  函数名 :  DH_MDK_LCSFC_Erase
  描  述 :  LCSFC擦除命令
  输  入 :  参数            描述
            u32StartAddr    擦除起始地址，注意：函数内部会做4K对齐
            u32Length       擦除长度，字节为单位
  输  出 :  无
  返  回 :  返回值          描述
            DH_SUCCESS      成功
            DH_FAILURE      失败
*************************************************/
DH_S32 STATIC_CODE_SECTION DH_MDK_LCSFC_Erase(DH_U32 u32StartAddr, DH_U32 u32Length)
{
    DH_U32 lu32BlkAddr, lu32BlkCnt, lu32TmpLen;
    DH_U32 i;
    DH_S32 ls32Ret;
    
    lu32BlkAddr = (u32StartAddr & LCSFC_ADDR_MASK) & SPI_BLOCK_NOTMASK;
    
    lu32TmpLen = ((u32StartAddr & LCSFC_ADDR_MASK) & SPI_BLOCK_MASK);
    lu32TmpLen += u32Length;
    
    lu32BlkCnt = (lu32TmpLen / SPI_BLOCK_SIZE);
    if (lu32TmpLen & SPI_BLOCK_MASK)
    {
        lu32BlkCnt++;
    }
    
    ls32Ret = DH_SUCCESS;
    
    /* 锁中断，避免指令访问Flash, spinlock_t设置为0，因为实际不访问 */
    asm("psrclr ie");
    
    
    /* 预处理地址和擦除命令 */
    lu32BlkAddr = ((SPI_CMD_4KBLOCKE << 24) | (lu32BlkAddr & LCSFC_ADDR_MASK));
    for (i = 0; i < lu32BlkCnt; i++)
    {

        Lcsfc_WrCmdByte(SPI_CMD_WRITE_EN);
        Lcsfc_WrCmdDw(lu32BlkAddr);

        /* 检测是否完成擦除 */
        ls32Ret = LcsfcCheckBusy(0xfffff);
        if (DH_SUCCESS != ls32Ret)
        {
            ls32Ret = DH_FAILURE;
            break;
        }
        
        lu32BlkAddr += SPI_BLOCK_SIZE;
    }
    
    /* 解锁 */
    asm("psrset ie"); 

    return ls32Ret;
}


/*************************************************
  函数名 :  LcsfcWrFlashPage
  描  述 :  写一页Flash
  输  入 :  参数            描述
            u32FlashAddr    Flash页地址
            u32DatCnt       要写的数据数量
            pu8Buf          数据缓冲
  输  出 :  无
  返  回 :  返回值          描述
            DH_FAILURE      Flash写失败
            DH_SUCCESS      Flash写成功
*************************************************/
static DH_S32 STATIC_CODE_SECTION LcsfcWrFlashPage(DH_U32 u32FlashAddr, DH_U32 u32DatCnt, DH_U8 *pu8Buf)
{
    DH_U32 lu32Cmd, i;
    DH_U8 *lpu8WrBuf;
    
    lpu8WrBuf = pu8Buf;
    
    /* 支持23位地址 */
    lu32Cmd = (u32FlashAddr & LCSFC_ADDR_MASK);
    lu32Cmd |= (SPI_CMD_PAGE_PROG << 24);
    
    /* 发送写使能命令 */
    Lcsfc_WrCmdByte(SPI_CMD_WRITE_EN);
    
    *(volatile DH_U32 *)(LCSFC_WRTIE_HOLD_ADDLINE) = lu32Cmd;
    for (i = 0; i < (u32DatCnt - 1); i++)
    {
        *(volatile DH_U8 *)(LCSFC_WRTIE_HOLD_ADDLINE) = *lpu8WrBuf++;
    }
    
    *(volatile DH_U8 *)(LCSFC_REG_BASE) = *lpu8WrBuf;
    
    return DH_SUCCESS;         
}


/*************************************************
  函数名 :  DH_MDK_LCSFC_WriteFlash
  描  述 :  写flash，调用页写函数，主要判断是否存在跨页
  输  入 :  参数            描述
            u32Addr        Flash写地址
            u32Cnt          要写的个数
            pu8Buf         数据缓冲
  输  出 :  无
  返  回 :  返回值      描述
            DH_FAILURE  Flash写失败
            DH_SUCCESS  Flash写成功
*************************************************/
DH_S32 STATIC_CODE_SECTION DH_MDK_LCSFC_WriteFlash(DH_U32 u32Addr, DH_U32 u32Cnt, DH_U8 *pu8Buf)
{
	DH_U32 lu32BaseAddr;
    DH_U32 lu32PreWrCnt, lu32PageWrCnt, lu32WrCnt;
    DH_U8 *lpu8Ptr;
    DH_S32 ls32Ret;
    
    if((DH_NULL == pu8Buf) || ((u32Addr & LCSFC_ADDR_MASK) > gu32FlashMaxSize) || (0 == u32Cnt))
    {
       return DH_FAILURE;
    }
    
    lu32WrCnt = u32Cnt;
    lpu8Ptr = pu8Buf;
    lu32BaseAddr = u32Addr & LCSFC_ADDR_MASK;
    lu32PreWrCnt = 0;
    lu32PageWrCnt = 0;
    
    /* 锁中断，避免指令访问Flash, spinlock_t设置为0，因为实际不访问 */
    asm("psrclr ie");
    
    while (1)
    {
        ls32Ret = LcsfcCheckBusy(0xfffff);
        if ( (0 == lu32WrCnt) || (ls32Ret != DH_SUCCESS))
        {
            break;
        }

        /* 检查写地址页对齐，防止跨页写flash */
        lu32PageWrCnt = (LCSFC_FLASH_PAGE_SIZE - (lu32BaseAddr & (LCSFC_FLASH_PAGE_SIZE - 1)));
        
        if (lu32PageWrCnt > LCSFC_TX_FIFO_DEPTH)
        {
            lu32PreWrCnt = LCSFC_TX_FIFO_DEPTH;
        }
        else
        {
            lu32PreWrCnt = lu32PageWrCnt;
        }

        /* 判断剩余量，防止过量写数据 */
        if (lu32PreWrCnt > lu32WrCnt)
        {
            lu32PreWrCnt = lu32WrCnt;
        }

        ls32Ret = LcsfcWrFlashPage(lu32BaseAddr, lu32PreWrCnt, lpu8Ptr);
        if (ls32Ret != DH_SUCCESS)
        {
            break;
        }

        /* inc env */
        lu32BaseAddr += lu32PreWrCnt;
        lpu8Ptr += lu32PreWrCnt;
        lu32WrCnt -= lu32PreWrCnt;
    }
    
    /* 解锁 */
    asm("psrset ie"); 
    
    return ls32Ret;

}
