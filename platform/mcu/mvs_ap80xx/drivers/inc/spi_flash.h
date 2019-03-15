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

/**
* @addtogroup 驱动
* @{
* @defgroup SPIFlash SPIFlash
* @{
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

#define     FLASH_HALF_PROTECT              (1)     /**<从Flash 0地址往上1半加锁*/
#define     FLASH_THREE_QUARTER_PROTECT     (2)     /**<从Flash 0地址往上3/4加锁*/
#define     FLASH_SEVEN_EIGHTH_PROTECT      (3)     /**<从Flash 0地址往上7/8加锁*/
#define     FLASH_ALL_PROTECT               (4)     /**<全加锁*/


/**
 * @brief Flash常数区域类型定义
 */
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


/**
 * @brief 用户数据区域类型定义
 */
typedef enum
{
    SPI_FLASH_USRDTA_BTKEY = 0x01,

} SPI_FLASH_USRDTA_TYPE;



/**
 * @brief Flash驱动错误码定义
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
* @brief SPI Flash IOCTL command
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
* @brief SPI Flash 解锁范围定义
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
 * @brief Flash 信息结构体定义
 * each byte definition like:    A B CMP SEC/BP4 TB/BP3 BP2 BP1 BP0 <A:area protection support flag,B:CMP bit exist flag>
 */
typedef struct _SPI_FLASH_INFO
{
	__IO uint8_t      Mid;             /**<厂商ID*/
	__IO uint16_t      Did;             /**<设备ID*/
	__IO uint8_t      SqiModeFlag;     /**<bit0:0:spi模式, 1:sqi模式. r/w*/
	__IO uint8_t      SqiWriteFlag;    /**<bit1:1 支持4bit编程, 0 不支持. 该成员只读*/
	__IO uint32_t     Capacity;        /**<flash容量*/
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

/**
* @brief SPI Flash 时钟频率设置
*/
#define FLASHCLK_60MHZ_SEL (0x0)		/**<Flash时钟使用DPLL 60MHz*/
#define FLASHCLK_SYSCLK_SEL (0x1)		/**<Flash时钟使用系统时钟*/
#define FLASHCLK_80MHZ_SEL (0x2)		/**<Flash时钟使用DPLL 80MHz*/
#define FLASHCLK_48MHZ_SEL (0x3)		/**<Flash时钟使用DPLL 48MHz*/


extern int32_t SpiFlashConstGetInfo(uint32_t IdxId, uint32_t* DatSz, uint32_t* Offset);
extern int32_t SpiFlashConstDataRead(uint8_t* Buf, uint32_t BufLen, uint32_t Offset);

/**
 * @brief  设置Flash的时钟频率
 * @param  FshcClkSel	可设置的Flash时钟频率为60MHz，系统时钟，80MHz, 48MHz，见上述宏定义
 * @param  EnHpm	 	1:开启Hpm; 0:关闭Hpm。如果是GD FLASH 可以开启Hpm，其他型号建议关闭Hpm。		 			
 * @return 0：成功, 非0：失败
 * @Note   该函数的调用需要放在ClkPorRcToDpll()之后。
 */
int32_t SpiFlashClkSet(uint8_t FshcClkSel, bool EnHpm);

/**
 * @brief  将Flash的时钟切换到系统时钟
 * @param  无.
 * @return 成功返回0, 错误返回非0值.
 */
int32_t SpiFlashClktoSysClk(void);

/**
 * @brief  恢复原来的Flash时钟配置
 * @param  无.
 * @return 成功返回0, 错误返回非0值.
 */
int32_t SpiFlashClkRestore(void);

/**
 * @brief  初始化并获取相关信息，如Flash ID，容量。
 * @param  无
 * @return 无
 * @note   必须先调用该函数之后才能调用Flash 读/写/IO操作函数
 **/
void SpiFlashInfoInit(void);

/**
 * @brief  获取Flash信息
 * @param  接收信息buffer
 * @return 成功返回0，否则返回负数
 * @note   该函数必须在SpiFlashInfoInit之后才可以调用
 **/
int32_t SpiFlashGetInfo(SPI_FLASH_INFO * FlashInfo);

/**
 * @brief	Flash读操作
 * @param	Offset	Flash片内有效任一偏移地址
 * @param	Buf		需要读出flash的数据存放内存首地址
 * @param	BufLen	需要读出flash的数据的长度
 * @return  成功返回0，否则返回负数
**/
int32_t SpiFlashRead(uint32_t Offset, uint8_t* Buf, uint32_t BufLen);

/**
 * @brief	Flash写操作
 * @param	Offset	Flash片内有效任一偏移地址
 * @param	Buf		需要写入flash的数据存放内存首地址
 * @param	BufLen	需要写入flash的数据的长度
 * @return  成功返回0，否则返回负数
**/
int32_t SpiFlashWrite(uint32_t Offset, uint8_t* Buf, uint32_t BufLen);


/**
 * @brief	Flash擦除操作
 * @param	Offset	flash片内偏移地址，4KB对齐
 * @param	Size	flash擦除字节量，如不足4KB将自动4KB对齐
 * @return  成功返回0，否则返回负数
**/
int32_t SpiFlashErase(uint32_t Offset, uint32_t Size);

/**
 * @brief	Flash IO 操作
 * @param	Cmd	 	IO control 命令
 * @param	可变参数,arg1,arg2... 根据Cmd决定：
 *			IOCTL_FLASH_PROTECT: 
 *				arg1: uint8_t ProtectRange
 *					1: Half code area protect;
 *					2: Three Quarter Area Protect;
 *					3: Seven Eighths Area Protect;
 *					4: All Area Protect
 *					
 *			IOCTL_FLASH_UNPROTECT:
 *				arg1: uint8_t* Buf, Buf指向的内容须为"\x35\xBA\x69"
 *				arg2: int32_t BufLen 固定为 3
 *				
 *			IOCTL_FLASH_RDID:	
 *				arg1: uint16_t * did
 *				arg2: uint8_t * mid
 *				
 *			IOCTL_FLASH_RDSTATUS:
 *				arg1: uint32_t *status
 *				
 *			IOCTL_FLASH_CRMRESET:
 *			
 *			IOCTL_FLASH_IO_MODE:
 *				arg1: SPIFLASH_IO_MODE io_mode
 *				
 *			IOCTL_FLASH_DEEPPOWERDOWN:
 *			
 *			IOCTL_FLASH_EXIT_DEEEPPOWERDOWN:
 *			
 *			IOCTL_FLASH_SETHPM:
 *				arg1: bool hpmEn
 *			
 * @return  成功返回0，否则返回负数
**/
int32_t SpiFlashIOCtl(uint32_t Cmd, ...);




#ifdef  __cplusplus
}
#endif//__cplusplus

#endif  //__SPI_FLASH_H__

/**
 * @}
 * @}
 */
