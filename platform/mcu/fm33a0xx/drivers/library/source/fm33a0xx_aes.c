

/**
    ******************************************************************************
    * @file    fm33a0xx_aes.c
    * @author  FM33a0xx Application Team
    * @version V2.0.2
    * @date    23-3-2018
    * @brief   This file provides firmware functions to manage the following 
    *          functionalities of....:
    *
  */

/* Includes ------------------------------------------------------------------*/
#include "fm33a0xx_aes.h" 


/** @addtogroup fm33a0xx_StdPeriph_Driver
    * @{
    */

/** @defgroup AES 
    * @brief AES driver modules
    * @{
    */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
//Code_Start
//2018-04-02-13-19-52
//FM33A0XX_Driver_Gen_V1.4

/********************************
  AES 功能控制函数
  功能: 配置AES加密密钥长度(仅AESEN=0时有效)
  输入：AES_AESCR_KEYLEN_128BIT			00：128bit
        AES_AESCR_KEYLEN_192BIT			01：192bit
        AES_AESCR_KEYLEN_256BIT			10：256bit
  输出: 无
  ********************************/
void AES_AESCR_KEYLEN_Set(uint32_t SetValue)
{
  uint32_t          tmpreg;

  tmpreg = AES->AESCR;
  tmpreg &= ~(AES_AESCR_KEYLEN_Msk);
  tmpreg |= (SetValue & AES_AESCR_KEYLEN_Msk);
  AES->AESCR = tmpreg;
}


/********************************
  AES 功能控制函数
  功能: 获取AES加密密钥长度配置(仅AESEN=0时有效)
  输入：无
  输出: AES_AESCR_KEYLEN_128BIT			00：128bit
        AES_AESCR_KEYLEN_192BIT			01：192bit
        AES_AESCR_KEYLEN_256BIT			10：256bit
  ********************************/
uint32_t AES_AESCR_KEYLEN_Get(void)
{
  return (AES->AESCR & AES_AESCR_KEYLEN_Msk);
}


/********************************
  AES 功能控制函数
  功能: 配置DMA自动读数据使能(工作模式1,3,4有效)
  输入：ENABLE/DISABLE		使能/禁止
  输出: 无
  ********************************/
void AES_AESCR_DMAOEN_Setable(FunState NewState)
{
  if (NewState == ENABLE)
  {
    AES->AESCR |= (AES_AESCR_DMAOEN_Msk);
  }
  else 
  {
    AES->AESCR &= ~(AES_AESCR_DMAOEN_Msk);
  }
}


/********************************
  AES 功能控制函数
  功能: 获取DMA自动读数据使能配置(工作模式1,3,4有效)
  输入：无
  输出: ENABLE/DISABLE		使能/禁止
  ********************************/
FunState AES_AESCR_DMAOEN_Getable(void)
{
  if (AES->AESCR & (AES_AESCR_DMAOEN_Msk))
  {
    return ENABLE;
  }
  else 
  {
    return DISABLE;
  }
}


/********************************
  AES 功能控制函数
  功能: 配置DMA自动写数据使能(工作模式1,3,4有效)
  输入：ENABLE/DISABLE		使能/禁止
  输出: 无
  ********************************/
void AES_AESCR_DMAIEN_Setable(FunState NewState)
{
  if (NewState == ENABLE)
  {
    AES->AESCR |= (AES_AESCR_DMAIEN_Msk);
  }
  else 
  {
    AES->AESCR &= ~(AES_AESCR_DMAIEN_Msk);
  }
}


/********************************
  AES 功能控制函数
  功能: 获取DMA自动写数据使能配置(工作模式1,3,4有效)
  输入：无
  输出: ENABLE/DISABLE		使能/禁止
  ********************************/
FunState AES_AESCR_DMAIEN_Getable(void)
{
  if (AES->AESCR & (AES_AESCR_DMAIEN_Msk))
  {
    return ENABLE;
  }
  else 
  {
    return DISABLE;
  }
}


/********************************
  AES 功能控制函数
  功能: 配置错误标志（RDERR和WRERR）中断使能
  输入：ENABLE/DISABLE		使能/禁止
  输出: 无
  ********************************/
void AES_AESCR_ERRIE_Setable(FunState NewState)
{
  if (NewState == ENABLE)
  {
    AES->AESCR |= (AES_AESCR_ERRIE_Msk);
  }
  else 
  {
    AES->AESCR &= ~(AES_AESCR_ERRIE_Msk);
  }
}


/********************************
  AES 功能控制函数
  功能: 获取错误标志（RDERR和WRERR）中断使能配置
  输入：无
  输出: ENABLE/DISABLE		使能/禁止
  ********************************/
FunState AES_AESCR_ERRIE_Getable(void)
{
  if (AES->AESCR & (AES_AESCR_ERRIE_Msk))
  {
    return ENABLE;
  }
  else 
  {
    return DISABLE;
  }
}


/********************************
  AES 功能控制函数
  功能: 配置CCF标志中断使能
  输入：ENABLE/DISABLE		使能/禁止
  输出: 无
  ********************************/
void AES_AESCR_CCFIE_Setable(FunState NewState)
{
  if (NewState == ENABLE)
  {
    AES->AESCR |= (AES_AESCR_CCFIE_Msk);
  }
  else 
  {
    AES->AESCR &= ~(AES_AESCR_CCFIE_Msk);
  }
}


/********************************
  AES 功能控制函数
  功能: 获取CCF标志中断使能配置
  输入：无
  输出: ENABLE/DISABLE		使能/禁止
  ********************************/
FunState AES_AESCR_CCFIE_Getable(void)
{
  if (AES->AESCR & (AES_AESCR_CCFIE_Msk))
  {
    return ENABLE;
  }
  else 
  {
    return DISABLE;
  }
}


/********************************
  AES 功能控制函数
  功能: 清除错误标志
  输入：无
  输出: 无
  ********************************/
void AES_AESCR_ERRC_Clr(void)
{
  AES->AESCR |= AES_AESCR_ERRC_Msk;
}


/********************************
  AES 功能控制函数
  功能: 清除CCF标志
  输入：无
  输出: 无
  ********************************/
void AES_AESCR_CCFC_Clr(void)
{
  AES->AESCR |= AES_AESCR_CCFC_Msk;
}


/********************************
  AES 功能控制函数
  功能: 配置AES数据流处理模式(AESEN=0时有效)
  输入：AES_AESCR_CHMOD_ECB		ECB
        AES_AESCR_CHMOD_CBC		CBC
        AES_AESCR_CHMOD_CTR		CTR
        AES_AESCR_CHMOD_MULTH	使用MultH模块
  输出: 无
  ********************************/
void AES_AESCR_CHMOD_Set(uint32_t SetValue)
{
  uint32_t          tmpreg;

  tmpreg = AES->AESCR;
  tmpreg &= ~(AES_AESCR_CHMOD_Msk);
  tmpreg |= (SetValue & AES_AESCR_CHMOD_Msk);
  AES->AESCR = tmpreg;
}


/********************************
  AES 功能控制函数
  功能: 获取AES数据流处理模式
  输入：无
  输出: AES_AESCR_CHMOD_ECB		ECB
        AES_AESCR_CHMOD_CBC		CBC
        AES_AESCR_CHMOD_CTR		CTR
        AES_AESCR_CHMOD_MULTH	使用MultH模块
  ********************************/
uint32_t AES_AESCR_CHMOD_Get(void)
{
  return (AES->AESCR & AES_AESCR_CHMOD_Msk);
}


/********************************
  AES 功能控制函数
  功能: 配置AES工作模式(AESEN=0时有效)
  输入：AES_AESCR_MODE_ENCRYPT		00：模式1：加密
        AES_AESCR_MODE_KEYEXP			01：模式2：密钥扩展
        AES_AESCR_MODE_DECRYPT		10：模式3：解密
        AES_AESCR_MODE_KEYEXPDECRYPT	11：模式4：密钥扩展+解密
  输出: 无
  ********************************/
void AES_AESCR_MODE_Set(uint32_t SetValue)
{
  uint32_t          tmpreg;

  tmpreg = AES->AESCR;
  tmpreg &= ~(AES_AESCR_MODE_Msk);
  tmpreg |= (SetValue & AES_AESCR_MODE_Msk);
  AES->AESCR = tmpreg;
}


/********************************
  AES 功能控制函数
  功能: 获取AES工作模式
  输入：无
  输出: AES_AESCR_MODE_ENCRYPT		00：模式1：加密
        AES_AESCR_MODE_KEYEXP			01：模式2：密钥扩展
        AES_AESCR_MODE_DECRYPT		10：模式3：解密
        AES_AESCR_MODE_KEYEXPDECRYPT	11：模式4：密钥扩展+解密
  ********************************/
uint32_t AES_AESCR_MODE_Get(void)
{
  return (AES->AESCR & AES_AESCR_MODE_Msk);
}


/********************************
  AES 功能控制函数
  功能: 配置AES数据类型(AESEN=0时有效)
  输入：AES_AESCR_DATATYP_32BITNOEX			00：32bit数据不交换
        AES_AESCR_DATATYP_16BITEX				01：16bit数据半字交换
        AES_AESCR_DATATYP_8BITEX				10：8bit数据字节交换
        AES_AESCR_DATATYP_1BITEX				11：1bit数据比特交换
  输出: 无
  ********************************/
void AES_AESCR_DATATYP_Set(uint32_t SetValue)
{
  uint32_t          tmpreg;

  tmpreg = AES->AESCR;
  tmpreg &= ~(AES_AESCR_DATATYP_Msk);
  tmpreg |= (SetValue & AES_AESCR_DATATYP_Msk);
  AES->AESCR = tmpreg;
}


/********************************
  AES 功能控制函数
  功能: 获取AES数据类型配置
  输入：无
  输出: AES_AESCR_DATATYP_32BITNOEX			00：32bit数据不交换
        AES_AESCR_DATATYP_16BITEX				01：16bit数据半字交换
        AES_AESCR_DATATYP_8BITEX				10：8bit数据字节交换
        AES_AESCR_DATATYP_1BITEX				11：1bit数据比特交换
  ********************************/
uint32_t AES_AESCR_DATATYP_Get(void)
{
  return (AES->AESCR & AES_AESCR_DATATYP_Msk);
}


/********************************
  AES 功能控制函数
  功能: 配置AES功能使能(清除AESEN位都能够复位AES模块)
  输入：ENABLE/DISABLE		使能/禁止
  输出: 无
  ********************************/
void AES_AESCR_AESEN_Setable(FunState NewState)
{
  if (NewState == ENABLE)
  {
    AES->AESCR |= (AES_AESCR_AESEN_Msk);
  }
  else 
  {
    AES->AESCR &= ~(AES_AESCR_AESEN_Msk);
  }
}


/********************************
  AES 功能控制函数
  功能: 获取AES功能使能配置
  输入：无
  输出: ENABLE/DISABLE		使能/禁止
  ********************************/
FunState AES_AESCR_AESEN_Getable(void)
{
  if (AES->AESCR & (AES_AESCR_AESEN_Msk))
  {
    return ENABLE;
  }
  else 
  {
    return DISABLE;
  }
}


/********************************
  AES 功能控制函数
  功能: 获取AES写错误标志
  输入：无
  输出: SET/RESET		计算或输出阶段发生写操作/计算和输出阶段未发生写操作
  ********************************/
FlagStatus AES_AESIF_WRERR_Chk(void)
{
  if (AES->AESIF & AES_AESIF_WRERR_Msk)
  {
    return SET;
  }
  else 
  {
    return RESET;
  }
}


/********************************
  AES 功能控制函数
  功能: 获取AES读错误标志
  输入：无
  输出: SET/RESET		计算或输出阶段发生读操作/计算和输出阶段未发生读操作
  ********************************/
FlagStatus AES_AESIF_RDERR_Chk(void)
{
  if (AES->AESIF & AES_AESIF_RDERR_Msk)
  {
    return SET;
  }
  else 
  {
    return RESET;
  }
}


/********************************
  AES 功能控制函数
  功能: 获取AES计算完成标志
  输入：无
  输出: SET/RESET		计算完成/计算没有完成
  ********************************/
FlagStatus AES_AESIF_CCF_Chk(void)
{
  if (AES->AESIF & AES_AESIF_CCF_Msk)
  {
    return SET;
  }
  else 
  {
    return RESET;
  }
}


/********************************
  AES 功能控制函数
  功能: 配置AES数据输入寄存器，当AES需要输入加解密数据时，应该往该寄?
    ??器连续写4次 
  输入：模式1（加密）：把明文从MSB到LSB分4次写入。
        模式2（密钥扩展）：无需使用数据输入寄存器
        模式3和模式4（解密）：把密文从MSB到LSB分4次写入。
        MultH模式：把乘数A或C从MSB到LSB分4次写入
  输出: 无
  ********************************/
void AES_AESDIN_Write(uint32_t SetValue)
{
  AES->AESDIN = (SetValue);
}


/********************************
  AES 功能控制函数
  功能: 获取AES数据输入寄存器内容
  输入：无
  输出: 数据寄存器值
  ********************************/
uint32_t AES_AESDIN_Read(void)
{
  return (AES->AESDIN);
}


/********************************
  AES 功能控制函数
  功能: 配置AES数据输出寄存器
  输入：
  输出: 无
  ********************************/
void AES_AESDOUT_Write(uint32_t SetValue)
{
  AES->AESDOUT = (SetValue);
}


/********************************
  AES 功能控制函数
  功能: 获取AES数据输出寄存器(当AES计算完成后，可以分四次读出加解密的
    结果)
  输入：无
  输出: 模式1（加密）：把密文从MSB到LSB分4次读出。
        模式2（密钥扩展）：无需使用数据输输出寄存器
        模式3和模式4（解密）：把明文从MSB到LSB分4次输出。
        MultH模式：运算结果存储在IVR寄存器中，无需读取AES_DOUTR寄存器
  ********************************/
uint32_t AES_AESDOUT_Read(void)
{
  return (AES->AESDOUT);
}


/********************************
  AES 功能控制函数
  功能: 配置AES为缺省值
  输入：无
  输出: 无
  ********************************/
void AES_Deinit(void)
{
  AES->AESCR = 0x00000000;

  //AES->AESIF = 0x00000000;
  AES->AESDIN = 0x00000000;

  //AES->AESDOUT = 0x00000000;
  AES->AESKEY0 = 0x00000000;
  AES->AESKEY1 = 0x00000000;
  AES->AESKEY2 = 0x00000000;
  AES->AESKEY3 = 0x00000000;
  AES->AESKEY4 = 0x00000000;
  AES->AESKEY5 = 0x00000000;
  AES->AESKEY6 = 0x00000000;
  AES->AESKEY7 = 0x00000000;
  AES->AESIVR0 = 0x00000000;
  AES->AESIVR1 = 0x00000000;
  AES->AESIVR2 = 0x00000000;
  AES->AESIVR3 = 0x00000000;
}


//Code_End

/********************************
  AES 功能控制函数
  功能: 加解密密钥输入(key0存放密钥最低32bit)
  输入：密钥	16/24/32字节长度的密钥
        长度	16/24/32
  输出: 无
  ********************************/
void AES_AESKEY_WriteEx(uint8_t * KeyIn, uint8_t Len)
{
  uint8_t           i;
  __IO uint32_t *   PointKeyReg;
  __IO uint32_t     Temp32;

  if ((Len != 16) && (Len != 24) && (Len != 32))
  {
    return;

  }

  PointKeyReg = (&AES->AESKEY0);
  for (i = 0; i < Len; i = i + 4)
  {
    Temp32 =
       (KeyIn[0 + i] << 24) | (KeyIn[1 + i] << 16) | (KeyIn[2 + i] << 8) | (KeyIn[3 + i] << 0);
    PointKeyReg[(Len - i) / 4 - 1] = Temp32;
  }
}


/*加解密密钥输出，第一个字存放密钥最低32bit  */
void AES_AESKEY_ReadEx(uint8_t * KeyOut, uint8_t Len)
{
  uint8_t           i;
  __IO uint32_t *   PointKeyReg;
  __IO uint32_t     Temp32;

  if ((Len != 16) && (Len != 24) && (Len != 32))
  {
    return;

  }

  PointKeyReg = (&AES->AESKEY0);
  for (i = 0; i < Len; i = i + 4)
  {
    Temp32 = PointKeyReg[(Len - i) / 4 - 1];
    KeyOut[0 + i] = Temp32 >> 24;
    KeyOut[1 + i] = Temp32 >> 16;
    KeyOut[2 + i] = Temp32 >> 8;
    KeyOut[3 + i] = Temp32 >> 0;
  }
}


/*初始向量输入 */
void AES_AESIVR_WriteEx(uint8_t * IVRIn)
{
  uint8_t           i;
  __IO uint32_t *   PointIvrReg;
  __IO uint32_t     Temp32;

  PointIvrReg = (&AES->AESIVR0);
  for (i = 0; i < 4 * 4; i = i + 4)
  {
    Temp32 =
       (IVRIn[0 + i] << 24) | (IVRIn[1 + i] << 16) | (IVRIn[2 + i] << 8) | (IVRIn[3 + i] << 0);
    PointIvrReg[(16 - i) / 4 - 1] = Temp32;
  }
}


/*初始向量读取 */
void AES_AESIVR_ReadEx(uint8_t * IVROut)
{
  uint8_t           i;
  __IO uint32_t *   PointIvrReg;
  __IO uint32_t     Temp32;

  PointIvrReg = (&AES->AESIVR0);
  for (i = 0; i < 4 * 4; i = i + 4)
  {
    Temp32 = PointIvrReg[(4 * 4 - i) / 4 - 1];
    IVROut[0 + i] = Temp32 >> 24;
    IVROut[1 + i] = Temp32 >> 16;
    IVROut[2 + i] = Temp32 >> 8;
    IVROut[3 + i] = Temp32 >> 0;
  }
}


/*加解密数据输入，16字节（128bit）分组数据输入  */
void AES_AESDIN_GroupWrite_128BIT(uint8_t * DataIn)
{
  uint8_t           i;
  __IO uint32_t     Temp32;

  for (i = 0; i < 16; i = i + 4)
  {
    Temp32 =
       (DataIn[0 + i] << 24) | (DataIn[1 + i] << 16) | (DataIn[2 + i] << 8) | (DataIn[3 + i] << 0);
    AES->AESDIN = Temp32;
  }
}


/*加解密数据输出，16字节（128bit）分组数据输出  */
void AES_AESDOUT_GroupRead_128BIT(uint8_t * DataOut)
{
  uint8_t           i;
  __IO uint32_t     Temp32;
  uint8_t           Buf[16];

  for (i = 0; i < 16; i = i + 4)
  {
    Temp32 = AES->AESDOUT;
    Buf[0 + i] = Temp32 >> 24;
    Buf[1 + i] = Temp32 >> 16;
    Buf[2 + i] = Temp32 >> 8;
    Buf[3 + i] = Temp32 >> 0;
  }
  if (RCC->SYSCLKSEL & (0x07UL << 16)) //APB时钟有分频
  {
    for (i = 0; i < 12; i++)
    {
      DataOut[4 + i] = Buf[i];
    }
    for (i = 0; i < 4; i++)
    {
      DataOut[i] = Buf[12 + i];
    }
  }
  else 
  {
    for (i = 0; i < 16; i++)
    {
      DataOut[i] = Buf[i];
    }
  }
}


uint8_t AES_GroupWriteAndRead_128BIT(uint8_t * DataIn, uint8_t * DataOut)
{
  uint16_t          i;

  //输入加解密数据
  AES_AESDIN_GroupWrite_128BIT(DataIn);

  //等待CCF标志置起,完成计算大约需要100个时钟周期
  for (i = 0; i < 500; i++)
  {
    if (SET == AES_AESIF_CCF_Chk())
    {
      break;
    }
  }

  //清CCF标志
  //读取前必须清除
  AES_AESCR_CCFC_Clr();

  //读取结果
  AES_AESDOUT_GroupRead_128BIT(DataOut);

  if (i == 500)
  {
    return 1;

  }
  else 
  {
    return 0;
  }
}


/* AES模块初始化函数 */
void AES_Init(AES_InitTypeDef * para)
{
  AES_AESCR_KEYLEN_Set(para->KEYLEN); //AES加密密钥长度，AESEN=1时不可修改
  AES_AESCR_CHMOD_Set(para->CHMOD); //AES数据流处理模式，AESEN=1时不可修改
  AES_AESCR_MODE_Set(para->MODE); //AES工作模式，AESEN=1时不可修改
  AES_AESCR_DATATYP_Set(para->DATATYP); //选择数据类型，AESEN=1时不可修改。具体交换规则可参考AES数据类型章节	
  AES_AESCR_DMAOEN_Setable(para->DMAOEN); //DMA数据自动读出使能
  AES_AESCR_DMAIEN_Setable(para->DMAIEN); //DMA数据自动写入使能
  AES_AESCR_ERRIE_Setable(para->ERRIE); //错误标志（RDERR和WRERR）中断使能
  AES_AESCR_CCFIE_Setable(para->CCFIE); //CCF标志中断使能
  AES_AESCR_AESEN_Setable(para->AESEN); //AES使能 
}


/******END OF FILE****/
