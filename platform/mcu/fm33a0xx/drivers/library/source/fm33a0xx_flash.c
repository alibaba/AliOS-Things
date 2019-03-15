/**
  ******************************************************************************
  * @file    fm33a0xx_flash.c
  * @author  FM33a0xx Application Team
  * @version V2.0.2
  * @date    3-1-2018
  * @brief   This file provides firmware functions to manage the following 
  *
*/

/* Includes ------------------------------------------------------------------*/
#include "fm33A0xx_flash.h" 

/** @addtogroup fm33a0xx_StdPeriph_Driver
  * @{
  */

/** @defgroup FLASH 
  * @brief FLASH driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup FLASH_Private_Functions
  * @{
  */ 
	
//Code_Start
//2018-04-02-13-21-07
//FM33A0XX_Driver_Gen_V1.4

/********************************
FLASH读等待周期配置函数
功能:FLASH读等待周期配置，当CPU时钟超过24M后需要开wait1 cycle
输入：配置下电复位电压（bit1:0）
      00/11：0 wait cycle
      01：1 wait cycle
      10：2 wait cycles
输出:无
********************************/
void FLASH_FLSRDCON_WAIT_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = FLASH->FLSRDCON;
	tmpreg &= ~(FLASH_FLSRDCON_WAIT_Msk);
	tmpreg |= (SetValue & FLASH_FLSRDCON_WAIT_Msk);
	FLASH->FLSRDCON = tmpreg;
}

/********************************
FLASH读等待周期状态函数
功能: 获取FLASH读等待周期
输入：无
输出: 00/11：0 wait cycle
      01：1 wait cycle
      10：2 wait cycles
********************************/
uint32_t FLASH_FLSRDCON_WAIT_Get(void)
{
	return (FLASH->FLSRDCON & FLASH_FLSRDCON_WAIT_Msk);
}

/********************************
FLASH DBG控制寄存器状态函数
功能: 获取FLASH DBG控制寄存器状态
输入：无
输出: SET   屏蔽MCUDBGCR寄存器功能
      RESET  MCUDBGCR寄存器使能
********************************/
FlagStatus FLASH_OPTBR_DBGCFGEN_Chk(void)
{
	if (FLASH->OPTBR & FLASH_OPTBR_DBGCFGEN_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}
/********************************
FLASH RAM引导使能状态函数
功能: 获取FLASH DBG控制寄存器状态
输入：无
输出: SET   允许芯片启动时从RAM引导
      RESET  禁止芯片从RAM引导
********************************/
FlagStatus FLASH_OPTBR_RAMBOOT_Chk(void)
{
	if (FLASH->OPTBR & FLASH_OPTBR_RAMBOOT_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}
/********************************
FLASH 应用代码权限锁定使能状态函数
功能: 获取FLASH 应用代码权限锁定使能状态
输入：无
输出: 00/01/11：ACLOCK不使能
      10：ACLOCK使能
********************************/
FlagStatus FLASH_OPTBR_ACLOCKEN_Chk(void)
{
	if (FLASH->OPTBR & FLASH_OPTBR_ACLOCKEN_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}
/********************************
FLASH Debug Port读取保护使能状态函数
功能: 获取FLASH Debug Port读取保护使能状态
输入：无
输出: 00/01/11：DBRDP不使能
      10：DBRDP使能
********************************/
FlagStatus FLASH_OPTBR_DBRDPEN_Chk(void)
{
	if (FLASH->OPTBR & FLASH_OPTBR_DBRDPEN_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/********************************
FLASH ACLOCK1配置函数
功能: 分别用于控制Block31~Block0的应用代码读写锁定
输入：1：读取和擦写权限锁定
      0：读取和擦写权限放开
输出: 无
********************************/
void FLASH_ACLOCK1_Write(uint32_t SetValue)
{
	FLASH->ACLOCK1 = (SetValue);
}

uint32_t FLASH_ACLOCK1_Read(void)
{
	return (FLASH->ACLOCK1);
}

/********************************
FLASH ACLOCK2配置函数
功能: 分别用于控制Block63~Block32的应用代码读写锁定
输入：1：读取和擦写权限锁定
      0：读取和擦写权限放开
输出: 无
********************************/
void FLASH_ACLOCK2_Write(uint32_t SetValue)
{
	FLASH->ACLOCK2 = (SetValue);
}
/********************************
FLASH ACLOCK2配置状态获取函数
功能:获取 ACLOCK1配置状态
输入：无
输出: 1：读取和擦写权限锁定
      0：读取和擦写权限放开
********************************/
uint32_t FLASH_ACLOCK2_Read(void)
{
	return (FLASH->ACLOCK2);
}

/********************************
FLASH 扇区擦除方式配置函数
功能:FLASH 扇区擦除方式
输入：00/11：Sector Erase 扇擦除
      01：Block Erase 块擦除
      10：禁止
输出: 无
********************************/
void FLASH_EPCON_ERTYPE_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = FLASH->EPCON;
	tmpreg &= ~(FLASH_EPCON_ERTYPE_Msk);
	tmpreg |= (SetValue & FLASH_EPCON_ERTYPE_Msk);
	FLASH->EPCON = tmpreg;
}
/********************************
FLASH 扇区擦除方式状态获取函数
功能:获取 擦除方式状态
输入：无
输出: 00/11：Sector Erase 扇擦除
      01：Block Erase 块擦除
      10：禁止
********************************/
uint32_t FLASH_EPCON_ERTYPE_Get(void)
{
	return (FLASH->EPCON & FLASH_EPCON_ERTYPE_Msk);
}
/********************************
编程请求函数
功能:编程请求
输入：软件置位，硬件完成编程后自动清零
输出: 无
********************************/
void FLASH_EPCON_PREQ_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = FLASH->EPCON;
	tmpreg &= ~(FLASH_EPCON_PREQ_Msk);
	tmpreg |= (SetValue & FLASH_EPCON_PREQ_Msk);
	FLASH->EPCON = tmpreg;
}
/********************************
编程请求状态获取函数
功能:获取 编程请求状态
输入：无
输出: 编程完成后清0
********************************/
uint32_t FLASH_EPCON_PREQ_Get(void)
{
	return (FLASH->EPCON & FLASH_EPCON_PREQ_Msk);
}
/********************************
擦除请求函数
功能:擦除请求
输入：软件置位，硬件完成编程后自动清零
输出: 无
********************************/
void FLASH_EPCON_EREQ_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = FLASH->EPCON;
	tmpreg &= ~(FLASH_EPCON_EREQ_Msk);
	tmpreg |= (SetValue & FLASH_EPCON_EREQ_Msk);
	FLASH->EPCON = tmpreg;
}
/********************************
擦除请求状态获取函数
功能:获取擦除请求状态
输入：无
输出: 编程完成后清0
********************************/
uint32_t FLASH_EPCON_EREQ_Get(void)
{
	return (FLASH->EPCON & FLASH_EPCON_EREQ_Msk);
}

/* Flash擦写Key输入寄存器，软件或者SWD在启动擦写前必须正确地向此地址写入合法KEY序列。空地址，物理上无寄存器实现。 相关函数 */
void FLASH_FLSKEY_Write(uint32_t SetValue)
{
	FLASH->FLSKEY = (SetValue);
}

/********************************
Flash擦写Key输入寄存器配置函数
功能:Flash擦写Key输入使能
输入：ENABLE   开启Flash擦写Key
      DISABLE  关闭Flash擦写Key
输出: 无
********************************/
void FLASH_FLSIE_AUTHIE_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		FLASH->FLSIE |= (FLASH_FLSIE_AUTHIE_Msk);
	}
	else
	{
		FLASH->FLSIE &= ~(FLASH_FLSIE_AUTHIE_Msk);
	}
}
/********************************
Flash擦写Key输入寄存器状态获取函数
功能:获取Flash擦写Key输入寄存器状态
输入：无
输出: ENABLE   开启Flash擦写Key
      DISABLE  关闭Flash擦写Key
********************************/
FunState FLASH_FLSIE_AUTHIE_Getable(void)
{
	if (FLASH->FLSIE & (FLASH_FLSIE_AUTHIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}
/********************************
Flash KEY错误中断使能控制函数
功能:Flash KEY错误中断使能
输入：ENABLE   开启Flash KEY错误中断
      DISABLE  关闭Flash KEY错误中断
输出: 无
********************************/
void FLASH_FLSIE_KEYIE_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		FLASH->FLSIE |= (FLASH_FLSIE_KEYIE_Msk);
	}
	else
	{
		FLASH->FLSIE &= ~(FLASH_FLSIE_KEYIE_Msk);
	}
}
/********************************
Flash KEY错误中断获取函数
功能:获取Flash KEY错误中断状态
输入：无
输出: ENABLE   Flash擦写Key中断产生
      DISABLE  Flash擦写Key中断未产生
********************************/
FunState FLASH_FLSIE_KEYIE_Getable(void)
{
	if (FLASH->FLSIE & (FLASH_FLSIE_KEYIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}
/********************************
擦写定时时钟错误中断使能控制函数
功能:擦写定时时钟错误中断使能
输入：ENABLE   开启擦写定时时钟错误中断使能
      DISABLE  关闭擦写定时时钟错误中断使能
输出: 无
********************************/
void FLASH_FLSIE_CKIE_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		FLASH->FLSIE |= (FLASH_FLSIE_CKIE_Msk);
	}
	else
	{
		FLASH->FLSIE &= ~(FLASH_FLSIE_CKIE_Msk);
	}
}
/********************************
擦写定时时钟错误中断状态获取函数
功能:擦写定时时钟错误中断使能状态
输入：无
输出: ENABLE   擦写定时时钟错误中断产生
      DISABLE  擦写定时时钟错误中断未产生
********************************/
FunState FLASH_FLSIE_CKIE_Getable(void)
{
	if (FLASH->FLSIE & (FLASH_FLSIE_CKIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}
/********************************
编程完成标志中断使能控制函数
功能:编程完成标志中断使能
输入：ENABLE   开启编程完成标志中断使能
      DISABLE  关闭编程完成标志中断使能
输出: 无
********************************/
void FLASH_FLSIE_PRDIE_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		FLASH->FLSIE |= (FLASH_FLSIE_PRDIE_Msk);
	}
	else
	{
		FLASH->FLSIE &= ~(FLASH_FLSIE_PRDIE_Msk);
	}
}
/********************************
编程完成标志中断使能状态获取函数
功能:编程完成标志中断使能
输入：无
输出: ENABLE   编程完成标志中断使能中断产生
      DISABLE  编程完成标志中断使能中断未产生
********************************/
FunState FLASH_FLSIE_PRDIE_Getable(void)
{
	if (FLASH->FLSIE & (FLASH_FLSIE_PRDIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}
/********************************
擦写完成标志中断使能控制函数
功能:擦写完成标志中断使能
输入：ENABLE   开启擦写完成标志中断使能
      DISABLE  关闭擦写完成标志中断使能
输出: 无
********************************/
void FLASH_FLSIE_ERDIE_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		FLASH->FLSIE |= (FLASH_FLSIE_ERDIE_Msk);
	}
	else
	{
		FLASH->FLSIE &= ~(FLASH_FLSIE_ERDIE_Msk);
	}
}
/********************************
擦写完成标志中断使能状态获取函数
功能:擦写完成标志中断使能
输入：无
输出: ENABLE   擦写完成标志中断使能中断产生
      DISABLE  擦写完成标志中断使能中断未产生
********************************/
FunState FLASH_FLSIE_ERDIE_Getable(void)
{
	if (FLASH->FLSIE & (FLASH_FLSIE_ERDIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/********************************
Flash读写权限错误函数
功能:读取LOCK块数据或对LOCK块擦写时置位，软件写1清零。
输入：软件写1清零。
输出: 无
********************************/
void FLASH_FLSIF_AUTHIF_Clr(void)
{
	FLASH->FLSIF = FLASH_FLSIF_AUTHIF_Msk;
}
/********************************
Flash读写权限错误状态获取函数
功能:获取Flash读写权限错误
输入：无
输出: SET    Flash读写权限错误中断产生
      RESET  Flash读写权限错误中断未产生
********************************/
FlagStatus FLASH_FLSIF_AUTHIF_Chk(void)
{
	if (FLASH->FLSIF & FLASH_FLSIF_AUTHIF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}
/********************************
Flash KEY错误函数
功能: 清Flash KEY错误。
输入：硬件置位，软件写1清零。
输出: 无
********************************/
void FLASH_FLSIF_KEYIF_Clr(void)
{
	FLASH->FLSIF = FLASH_FLSIF_KEYIF_Msk;
}
/********************************
Flash KEY错误状态获取函数
功能:获取Flash KEY错误
输入：无
输出: SET    Flash KEY错误中断产生
      RESET  Flash KEY错误中断未产生
********************************/
FlagStatus FLASH_FLSIF_KEYIF_Chk(void)
{
	if (FLASH->FLSIF & FLASH_FLSIF_KEYIF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}
/********************************
擦写定时时钟错误函数
功能: NVMIF擦写Flash时AHBCLK只能使用RCHF 8M或16M，使用其他时钟擦写都会触发CKERR，软件写1清零。
输入：清Flash CK错误。
输出: 无
********************************/
void FLASH_FLSIF_CKIF_Clr(void)
{
	FLASH->FLSIF = FLASH_FLSIF_CKIF_Msk;
}
/********************************
擦写定时时钟错误状态获取函数
功能:获取Flash 擦写定时时钟错状态
输入：无
输出: SET    Flash 擦写定时时钟错误中断产生
      RESET  Flash 擦写定时时钟错误中断未产生
********************************/
FlagStatus FLASH_FLSIF_CKIF_Chk(void)
{
	if (FLASH->FLSIF & FLASH_FLSIF_CKIF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}
/********************************
编程完成标志函数
功能: Program Done，编程完成标志，硬件置位，软件写1清零。
输入：清编程完成标志。
输出: 无
********************************/
void FLASH_FLSIF_PRDIF_Clr(void)
{
	FLASH->FLSIF = FLASH_FLSIF_PRDIF_Msk;
}
/********************************
编程完成标志函数状态获取函数
功能:获取编程完成标志状态
输入：无
输出: SET    编程完成中断产生
      RESET 编程完成中断未产生
********************************/
FlagStatus FLASH_FLSIF_PRDIF_Chk(void)
{
	if (FLASH->FLSIF & FLASH_FLSIF_PRDIF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}
/********************************
擦写完成标志函数
功能: Erase Done，擦写完成标志，硬件置位，软件写1清零
输入：清擦写完成标志。
输出: 无
********************************/
void FLASH_FLSIF_ERDIF_Clr(void)
{
	FLASH->FLSIF = FLASH_FLSIF_ERDIF_Msk;
}
/********************************
擦写完成标志函数状态获取函数
功能:获取擦写完成标志状态
输入：无
输出: SET    擦写完成中断产生
      RESET 擦写完成中断未产生
********************************/
FlagStatus FLASH_FLSIF_ERDIF_Chk(void)
{
	if (FLASH->FLSIF & FLASH_FLSIF_ERDIF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

void FLASH_Deinit(void)
{
	FLASH->FLSRDCON = 0x00000000;
	FLASH->PRFTCON = 0x00000000;
	//FLASH->OPTBR = 0x00000005;
	//FLASH->ACLOCK1 = ;
	//FLASH->ACLOCK2 = ;
	FLASH->EPCON = 0x00000000;
	//FLASH->FLSKEY = 0x00000000;//key密码  
	FLASH->FLSIE = 0x00000000;
	//FLASH->FLSIF = 0x00000000;
}
//Code_End


/********************************
块擦配置函数
功能:块擦配置
输入：擦除地址
输出: 无
********************************/
void FLASH_Erase_Sector(uint32_t erase_addr)
{
    //set to sector erase
    FLASH_EPCON_ERTYPE_Set(FLASH_EPCON_ERTYPE_SECTOR);
	
    //SET EREQ
    FLASH_EPCON_EREQ_Set(FLASH_EPCON_EREQ_ERASE);
	
    //write flash key
    FLASH_FLSKEY_Write(flash_sector_erase_key0);
    FLASH_FLSKEY_Write(flash_sector_erase_key1);
	
    //向需要擦除的扇区内任意地址写擦除请求0x1234_ABCD
    *(uint32_t*)erase_addr = flash_erase_data;
	
	//wait for finish
	while(SET != FLASH_FLSIF_ERDIF_Chk()){__NOP();}
	FLASH_FLSIF_ERDIF_Clr();
	
    FLASH_FLSKEY_Write(0x00000000);
}

/********************************
单字写flash函数
功能:单字写flash
输入：写入地址和写入数据
输出: 无
********************************/
void FLASH_Prog_SingleByte(uint32_t prog_addr,uint8_t prog_data)
{
	//set PREQ
    FLASH_EPCON_PREQ_Set(FLASH_EPCON_PREQ_PROG);
	
	//write flash key
    FLASH_FLSKEY_Write(flash_PROG_key0);
    FLASH_FLSKEY_Write(flash_PROG_key1);
	
    *(uint8_t*)prog_addr = prog_data;	

    while(RESET == FLASH_FLSIF_PRDIF_Chk()){__NOP();}
	FLASH_FLSIF_PRDIF_Clr();
	
    FLASH_FLSKEY_Write(0x00000000);
}

/********************************
连续写flash函数
功能:连续写flash
输入：写入地址、写入数据和长度
输出: 无
********************************/
void FLASH_Prog_ByteString(uint32_t prog_addr,uint8_t* prog_data, uint16_t Len)
{
	uint16_t i;
	//set PREQ
	FLASH_EPCON_PREQ_Set(FLASH_EPCON_PREQ_PROG);
	
	//write flash key
	FLASH_FLSKEY_Write(flash_PROG_key0);
	FLASH_FLSKEY_Write(flash_PROG_key1);
	
	for(i=0; i<Len; i++)
	{
		FLASH_EPCON_PREQ_Set(FLASH_EPCON_PREQ_PROG);	//set PREQ
		*(uint8_t*)prog_addr = prog_data[i];						//写一个字节
		prog_addr++;
		while(RESET == FLASH_FLSIF_PRDIF_Chk()){__NOP();}//等待写操作完成
		FLASH_FLSIF_PRDIF_Clr();						//清除写完成中断标志
	//	delay_50(1);
	}
	
    FLASH_FLSKEY_Write(0x00000000);
}

/******END OF FILE****/






