/**
 *****************************************************************************
 * @file     spi_flash.h
 * @author   lujiangang
 * @version  V1.0.3
 * @date     30-May-2013
 * @brief    spi code flash module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __SPI_FLASH_H__
#define __SPI_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#define     POSEDGE_SEL          (0)       /**<sampling at clk posedge */
#define     NEGEDGE_SEL          (1)       /**<sampling at clk negedge */
#define     DELAY_CHAIN_SEL      (4)       /**<sampling at the n clks */

#define     FLASH_GD             (0xC8)    /**<MID */
#define     FLASH_MX             (0xC2)    /**<MID */
#define     FLASH_WINBOUND       (0xEF)    /**<MID */
#define     FLASH_PCT            (0xBF)    /**<MID */
#define     FLASH_EON            (0x1C)    /**<MID */
#define     FLASH_BG             (0xE0)    /**<MID */
#define     FLASH_ESMT           (0x8C)    /**<MID */

#define     FLASH_HALF_PROTECT              (1)     /**<half protection area from bottom to top*/
#define     FLASH_THREE_QUARTER_PROTECT     (2)     /**<three quarter protection area from bottom to top*/
#define     FLASH_SEVEN_EIGHTH_PROTECT      (3)     /**<seven eighth protection area from bottom to top*/
#define     FLASH_ALL_PROTECT               (4)     /**<all flash area protect*/


typedef enum
{
    SPI_FLASH_CONSTDAT_BTAD = 0x00,
    SPI_FLASH_CONSTDAT_BTNM,
    SPI_FLASH_CONSTDAT_BTCN,
    SPI_FLASH_CONSTDAT_BTDC,
    SPI_FLASH_CONSTDAT_BTCM,

    SPI_FLASH_CONSTDAT_POMU = 0x10,
    SPI_FLASH_CONSTDAT_PFMU,

    SPI_FLASH_CONSTDAT_STSD = 0x20,
    SPI_FLASH_CONSTDAT_STPU,
    SPI_FLASH_CONSTDAT_STBT,
    SPI_FLASH_CONSTDAT_STLN,
    SPI_FLASH_CONSTDAT_STFM,
    SPI_FLASH_CONSTDAT_STUD,
} SPI_FLASH_CONSTDTA_TYPE;

typedef enum
{
    SPI_FLASH_USRDTA_BTKEY = 0x01,

} SPI_FLASH_USRDTA_TYPE;



/**
 * err code definition
 */
typedef enum _SPI_FLASH_ERR_CODE
{
    TIME_OUT_ERR = -255,             /**<function execute time out*/
    UNKNOWN_MID_ERR,                 /**<MID can't indentify*/
    UNKNOWN_DID_ERR,                 /**<identify MID but can't identify DID*/
    FIFO_ALWAYS_EMPTY_ERR,           /**<during reading, time out but fifo is still empty*/
    FIFO_ALWAYS_FULL_ERR,            /**<during writing, time out but fifo is still full*/
    DATALEN_LESS_THEN_ZERO_ERR,      /**<read data but data len is < 0*/
    CODE_AREA_PROTECT_NOT_SUPPORT,   /**<indicate this area which choosed before not support*/
    UNLOCK_FLASH_ERR,                /**<unlock flash err*/
    FLASH_NONE_ERR = 0,
} SPI_FLASH_ERR_CODE;

/**
* SPI Flash IOCTL command
*/
typedef enum _SPI_FLASH_IOCTL_CMD
{
	IOCTL_FLASH_PROTECT = 1,
	IOCTL_FLASH_UNPROTECT,
	IOCTL_STATUS_REGISTER,
	IOCTL_SET_CLK_HPM,
	IOCTL_DEEP_POWER_DOWN,
	IOCTL_RELEASE_DEEP_POWER_DOWN
}SPI_FLASH_IOCTL_CMD;

/**
* SPI Flash lock range
*/
typedef enum _SPI_FLASH_LOCK_RANGE
{
	FLASH_LOCK_RANGE_HALF = 1,
	FLASH_LOCK_RANGE_THREE_QUARTERS,
	FLASH_LOCK_RANGE_SEVENTH_EIGHT,
	FLASH_LOCK_RANGE_ALL
}SPI_FLASH_LOCK_RANGE;

#pragma pack(1)

/**
 * flash structure define
 * each byte definition like:    A B CMP SEC/BP4 TB/BP3 BP2 BP1 BP0 <A:area protection support flag,B:CMP bit exist flag>
 */
typedef struct _SPI_FLASH_INFO
{
	__IO uint8_t      Mid;             /**<Manufacturer's ID*/
	__IO uint16_t      Did;             /**<Device ID*/
	__IO uint8_t      SqiModeFlag;     /**<bit0:0:spi mode, 1:sqi mode. r/w*/
	__IO uint8_t      SqiWriteFlag;    /**<bit1:1 support 4bit page programm, 0 not support. read only*/
	__IO uint32_t     Capacity;        /**<flash capacity*/
	__IO uint8_t      NoneAreaProtect;
	__IO uint8_t      HalfAreaProtect;
	__IO uint8_t      ThreeQuarterAreaProtect;
	__IO uint8_t      SevenEighthsAreaProtect;
	__IO uint8_t      AllAreaProtect;

} SPI_FLASH_INFO;

#pragma pack()

//-------------------------------------------------------------------------------------------------------------------
// For code flash read and write ONLY usage
// Flash API函数，都是“原子”操作

#define FLASHCLK_60MHZ_SEL (0x0)		/*flash clock use dpll clock 60MHz*/
#define FLASHCLK_SYSCLK_SEL (0x1)		/*flash clock use system clock */
#define FLASHCLK_80MHZ_SEL (0x2)		/*flash clock use dpll clock 80MHz*/
#define FLASHCLK_48MHZ_SEL (0x3)		/*flash clock use dpll clock 48MHz*/


extern int32_t SpiFlashConstGetInfo(uint32_t IdxId, uint32_t* DatSz, uint32_t* Offset);
extern int32_t SpiFlashConstDataRead(uint8_t* Buf, uint32_t BufLen, uint32_t Offset);

/**
 * @brief  Set the spi flash clock.
 * @param  FshcClkSel, see header file for desc. 
 *		    EnHpm, 1:enable Hpm; 0:disable Hpm			 			
 * @return 0 if success, otherwise errcode.
 * @Note   this function should be called after ClkPorRcToDpll().
 */
int32_t SpiFlashClkSet(uint8_t FshcClkSel, bool EnHpm);

/*
* in function ModsClkReset(), SpiFlashClktoSysClk() & SpiFlashClkRestore() should be called in pairs
*/

/**
 * @brief  Change spi flash clock to system clock.
 * @param  None.
 * @return 0 if success, otherwise errcode.
 * @Note   this function should be called before notify_call in ModsClkReset().
 */
int32_t SpiFlashClktoSysClk(void);

/**
 * @brief  Restore flash clock to previous flash clock.
 * @param  None.
 * @return 0 if success, otherwise errcode.
 * @Note   This function should be called after notify_call in ModsClkReset().
 */
int32_t SpiFlashClkRestore(void);

/**
 * @brief  初始化并获取相关信息
 * @param  SpiFlashInfo 有效定义的结构体
 * @return None。
 *初始化并获取相关信息
 **/
void SpiFlashInfoInit(void);

/**
 * @brief  获取Flash信息
 * @param  接收信息buffer
 * @return 成功返回0，否则返回负数
 *获取Flash信息
 *必须在初始化之后才可以调用
 **/
int32_t SpiFlashGetInfo(SPI_FLASH_INFO * FlashInfo);

/**
*读操作
*参数1：上面定义的结构体，且经过了初始化的
*参数2：flash片内有效任一偏移地址
*参数3：需要读出flash的数据存放内存首地址
*参数4：需要读出flash的数据的长度
*成功，返回0，失败返回负数
**/
int32_t SpiFlashRead(uint32_t Offset, uint8_t* Buf, uint32_t BufLen);

/**
*写操作
*参数1：上面定义的结构体，且经过了初始化的
*参数2：flash片内有效任一偏移地址
*参数3：需要写入flash的数据的内存首地址
*参数4：需要写入flash的数据的长度
*成功，返回0，失败返回负数
**/
int32_t SpiFlashWrite(uint32_t Offset, uint8_t* Buf, uint32_t BufLen);


/**
*擦除操作
*参数1：上面定义的结构体，且经过了初始化的
*参数2：flash片内偏移地址，4KB对齐
*参数3：flash擦除字节量，4KB对齐
*成功，返回0，失败返回负数
*!!! Warning ::: Offset and Size must 4KBytes aligned
**/
int32_t SpiFlashErase(uint32_t Offset, uint32_t Size);

/**
*IO操作
*参数1：上面定义的结构体，且经过了初始化的
*参数2：I/O control命令
*参数3：可变参数,arg1,arg2... 根据cmd决定：
*					cmd = 1, flash protect. 											arg1:uint8_t ProtectRange
*					cmd = 2, flash unprotect. 										arg1:uint8_t* Buf, arg2: int32_t BufLen
*					cmd = 3, protection status regster. 					no va_arg
*					cmd = 4, set Flash clk&Hpm(if any)						arg1:uint32_t FshClk, arg2:HpmEn*
*					cmd = 5, flash deep power dwon								no va_arg
*					cmd = 6, flash release from deep power down		no va_arg
*成功，返回0，失败返回负数
*/
int32_t SpiFlashIOCtl(uint32_t Cmd, ...);




#ifdef  __cplusplus
}
#endif//__cplusplus

#endif  //__SPI_FLASH_H__

