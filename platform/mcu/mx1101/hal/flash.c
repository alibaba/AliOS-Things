#include <string.h>


extern const hal_logic_partition_t hal_partitions[];

int32_t hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ in_partition ];
    memcpy(partition, logic_partition, sizeof(hal_logic_partition_t));

    return 0;
}

int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get( in_partition, p_partition_info );

    platform_flash_erase(p_partition_info->partition_start_addr + off_set, p_partition_info->partition_start_addr + off_set + size);

    return 0;
}
                        
int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set, const void *in_buf , uint32_t in_buf_len)
{
    uint32_t addr;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get( in_partition, p_partition_info );
    addr = p_partition_info->partition_start_addr + *off_set;

    platform_flash_write(&addr, in_buf, in_buf_len);

    *off_set += in_buf_len;

    return 0;
}

int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set, void *out_buf, uint32_t out_buf_len)
{
    uint32_t addr;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get( in_partition, p_partition_info );
    addr = p_partition_info->partition_start_addr + *off_set;

    platform_flash_read(&addr, out_buf, out_buf_len);

    *off_set += out_buf_len;

    return 0;
}

int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

//================================================================
#include <stdbool.h>

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
	volatile uint8_t      Mid;             /**<Manufacturer's ID*/
	volatile uint16_t      Did;             /**<Device ID*/
	volatile uint8_t      SqiModeFlag;     /**<bit0:0:spi mode, 1:sqi mode. r/w*/
	volatile uint8_t      SqiWriteFlag;    /**<bit1:1 support 4bit page programm, 0 not support. read only*/
	volatile uint32_t     Capacity;        /**<flash capacity*/
	volatile uint8_t      NoneAreaProtect;
	volatile uint8_t      HalfAreaProtect;
	volatile uint8_t      ThreeQuarterAreaProtect;
	volatile uint8_t      SevenEighthsAreaProtect;
	volatile uint8_t      AllAreaProtect;

} SPI_FLASH_INFO;

#pragma pack()

//-------------------------------------------------------------------------------------------------------------------
// For code flash read and write ONLY usage
// Flash API���������ǡ�ԭ�ӡ�����

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
 * @brief  ��ʼ������ȡ������?
 * @param  SpiFlashInfo ��Ч����Ľṹ��?
 * @return None��
 *��ʼ������ȡ������?
 **/
void SpiFlashInfoInit(void);

/**
 * @brief  ��ȡFlash��Ϣ
 * @param  ������Ϣbuffer
 * @return �ɹ�����0�����򷵻ظ���
 *��ȡFlash��Ϣ
 *�����ڳ�ʼ��֮��ſ��Ե���?
 **/
int32_t SpiFlashGetInfo(SPI_FLASH_INFO * FlashInfo);

/**
*������
*����1�����涨��Ľṹ��?�Ҿ����˳�ʼ����
*����2��flashƬ����Ч��һƫ�Ƶ�ַ
*����3����Ҫ����flash�����ݴ���ڴ��׵��?
*����4����Ҫ����flash�����ݵĳ���
*�ɹ�������0��ʧ�ܷ��ظ���
**/
int32_t SpiFlashRead(uint32_t Offset, uint8_t* Buf, uint32_t BufLen);

/**
*д����
*����1�����涨��Ľṹ��?�Ҿ����˳�ʼ����
*����2��flashƬ����Ч��һƫ�Ƶ�ַ
*����3����Ҫд��flash�����ݵ��ڴ��׵�ַ
*����4����Ҫд��flash�����ݵĳ���
*�ɹ�������0��ʧ�ܷ��ظ���
**/
int32_t SpiFlashWrite(uint32_t Offset, uint8_t* Buf, uint32_t BufLen);


/**
*��������
*����1�����涨��Ľṹ��?�Ҿ����˳�ʼ����
*����2��flashƬ��ƫ�Ƶ�ַ��4KB����
*����3��flash�����ֽ�����4KB����
*�ɹ�������0��ʧ�ܷ��ظ���
*!!! Warning ::: Offset and Size must 4KBytes aligned
**/
int32_t SpiFlashErase(uint32_t Offset, uint32_t Size);

/**
*IO����
*����1�����涨��Ľṹ��?�Ҿ����˳�ʼ����
*����2��I/O control����
*����3���ɱ����?,arg1,arg2... ����cmd������
*					cmd = 1, flash protect. 											arg1:uint8_t ProtectRange
*					cmd = 2, flash unprotect. 										arg1:uint8_t* Buf, arg2: int32_t BufLen
*					cmd = 3, protection status regster. 					no va_arg
*					cmd = 4, set Flash clk&Hpm(if any)						arg1:uint32_t FshClk, arg2:HpmEn*
*					cmd = 5, flash deep power dwon								no va_arg
*					cmd = 6, flash release from deep power down		no va_arg
*�ɹ�������0��ʧ�ܷ��ظ���
*/
int32_t SpiFlashIOCtl(uint32_t Cmd, ...);

#define DEBUG_FLASH

#ifdef DEBUG_FLASH
#define APP_DBG printf
#endif

#ifdef DEBUG_FLASH
SPI_FLASH_INFO  FlashInfo;

void GetFlashGD(int32_t protect)
{    
	uint8_t  str[20];
	int32_t FlashCapacity = 0;
		
	switch(FlashInfo.Did)
	{
		case 0x1340:	
			strcpy((char *)str,"GD25Q40(GD25Q40B)");
			FlashCapacity = 0x00080000;
			break;

		case 0x1240:
        	strcpy((char *)str,"GD25Q20(GD25Q20B)");
			FlashCapacity = 0x00040000;
			break;       

		case 0x1540:
			strcpy((char *)str,"GD25Q16(GD25Q16B)");
			FlashCapacity = 0x00200000;			
			break; 

		case 0x1640:
        	strcpy((char *)str,"GD25Q32(GD25Q32B)");
			FlashCapacity = 0x00400000;        
			break;

		case 0x1740:
        	strcpy((char *)str,"GD25Q64B");
			FlashCapacity = 0x00800000;          
			break;

		case 0x1440:
        	strcpy((char *)str,"GD25Q80(GD25Q80B)");
			FlashCapacity = 0x00100000;         
			break;

		case 0x1840:
            strcpy((char *)str,"GD25Q128B");
            FlashCapacity = 0x01000000;         
            break;

		default:
			break;
	}
        
    if(FlashCapacity > 0)
    {
        APP_DBG("Module:                ");
        APP_DBG("%s\r\n",str);
        APP_DBG("Capacity:                     ");
        APP_DBG("0x%08X\r\n", FlashCapacity);
    }  
    else
    {
        APP_DBG("Found failed\r\n");
    }
}

void GetFlashWinBound(int32_t protect)
{	
    uint8_t  str[20];
	int32_t FlashCapacity = 0;
    
	switch(FlashInfo.Did)
    {
        case 0x1440:
            strcpy((char *)str,"W25Q80BV");
            FlashCapacity = 0x00100000;             
            break;
        
        case 0x1760:
            strcpy((char *)str,"W25Q64DW");
            FlashCapacity = 0x00800000;             
            break;
        				
        case 0x1740:
            strcpy((char *)str,"W25Q64CV");
            FlashCapacity = 0x00800000; 
            break;
        
        default:
            break;
    }
    
    if(FlashCapacity > 0)
    {
        APP_DBG("Module:                ");
        APP_DBG("%s\r\n",str);
        APP_DBG("Capacity:                     ");
        APP_DBG("0x%08X\r\n", FlashCapacity);
    }  
    else
    {
        APP_DBG("Found failed\r\n");
    }
}

void GetFlashPct(void)
{	
    uint8_t  str[20];
	int32_t FlashCapacity = 0;
    
	switch(FlashInfo.Did)
    {
        case 0x0126:
            strcpy((char *)str,"PCT26VF016");
            FlashCapacity = 0x00200000;        
			break;

        case 0x0226:       
            strcpy((char *)str,"PCT26VF032");
            FlashCapacity = 0x00400000;
            break;

        default:            
			break;
    }
      
    if(FlashCapacity > 0)
    {
        APP_DBG("Module:                ");
        APP_DBG("%s\r\n",str);
        APP_DBG("Capacity:                     ");
        APP_DBG("0x%08X\r\n", FlashCapacity);
    }  
    else
    {
        APP_DBG("Found failed\r\n");
    }
}

void GetFlashEon(int32_t protect)
{
    uint8_t  str[20];
	int32_t FlashCapacity = 0;
    
	switch(FlashInfo.Did)
    {
        case 0x1430:
            strcpy((char *)str,"EN25Q80A");
            FlashCapacity = 0x00100000; 
            break;

        case 0x1530:
            strcpy((char *)str,"EN25Q16A");
            FlashCapacity = 0x00200000; 
            break;

        case 0x1830:
            strcpy((char *)str,"EN25Q128");
            FlashCapacity = 0x01000000; 
            break;

        case 0x1630:
            strcpy((char *)str,"EN25Q32A");
            FlashCapacity = 0x00400000; 
            break;

        case 0x1330:
            strcpy((char *)str,"EN25Q40");
            FlashCapacity = 0x00080000; 
            break;

        case 0x1730:
            strcpy((char *)str,"EN25Q64");
            FlashCapacity = 0x00800000; 
            break;

        case 0x1570:
            strcpy((char *)str,"EN25QH16");
            FlashCapacity = 0x00200000; 
            break;

        case 0x1670: 
            strcpy((char *)str,"EN25QH32");
            FlashCapacity = 0x00400000; 
            break;

        default:
            break;
    }
    
    if(FlashCapacity > 0)
    {
        APP_DBG("Module:                ");
        APP_DBG("%s\r\n",str);
        APP_DBG("Capacity:                     ");
        APP_DBG("0x%08X\r\n", FlashCapacity);
    }  
    else
    {
        APP_DBG("Found failed\r\n");
    }
}

void GetFlashBg(int32_t protect)
{	
    uint8_t  str[20];
	int32_t FlashCapacity = 0;
    
    switch(FlashInfo.Did)
    {
        case 0x1540:
            strcpy((char *)str,"BG25Q16A");
            FlashCapacity = 0x00200000; 
            break;

        case 0x1340:
            strcpy((char *)str,"BG25Q40A");
            FlashCapacity = 0x00080000; 
            break;
        
        case 0x1440:
            strcpy((char *)str,"BG25Q80A");
            FlashCapacity = 0x00100000; 
            break;
        
        default:
			break;         
    }
    
    if(FlashCapacity > 0)
    {
        APP_DBG("Module:                ");
        APP_DBG("%s\r\n",str);
        APP_DBG("Capacity:                     ");
        APP_DBG("0x%08X\r\n", FlashCapacity);
    }  
    else
    {
        APP_DBG("Found failed\r\n");
    }
}

void GetFlashEsmt(int32_t protect)
{
    uint8_t  str[20];
	int32_t FlashCapacity = 0;
       
	switch(FlashInfo.Did)
    {
        case 0x1440:
            strcpy((char *)str,"F25L08QA");
            FlashCapacity = 0x00100000; 
            break;

        case 0x1540:
            strcpy((char *)str,"F25L16QA");
            FlashCapacity = 0x00200000; 
            break;

        case 0x1641:
            strcpy((char *)str,"F25L32QA");
            FlashCapacity = 0x00400000; 
            break;

        case 0x1741:
            strcpy((char *)str,"F25L64QA");
            FlashCapacity = 0x00800000; 
            break;
              
        default:
            break;
    }    
    
    if(FlashCapacity > 0)
    {
        APP_DBG("Module:                ");
        APP_DBG("%s\r\n",str);
        APP_DBG("Capacity:                     ");
        APP_DBG("0x%08X\r\n", FlashCapacity);
    }  
    else
    {
        APP_DBG("Found failed\r\n");
    }
}

void GetDidInfo(int32_t protect)
{
	APP_DBG("%-30s","Did:");
	APP_DBG("0x%08X\r\n",FlashInfo.Did);
	APP_DBG("%-30s","Lock Area(BP4~BP0:Bit4~Bit0):");
	APP_DBG("0x%08X\r\n",protect);
}

void GetFlashInfo(void)
{
	int32_t protect = 0;
   
	APP_DBG("\r\n\r\n****************************************************************\n");
        APP_DBG("%-30s\r\n","Flash information");
	
	if(FlashInfo.Mid != FLASH_PCT)
	{
		protect = SpiFlashIOCtl(3,0);
		protect = (protect >> 2) & 0x1F;
	}
	
    switch(FlashInfo.Mid)
    {
        case FLASH_GD:
            APP_DBG("Manufacture:                         GD\r\n");
			GetDidInfo(protect);
            GetFlashGD(protect);
            break;
        
        case FLASH_WINBOUND:
            APP_DBG("Manufacture:                         WINBOUND\r\n");
			GetDidInfo(protect);
            GetFlashWinBound(protect);
            break;
        
        case FLASH_PCT:
            APP_DBG("Manufacture:                         PCT\r\n");
            GetFlashPct();
            break;
        
        case FLASH_EON:            
            APP_DBG("Manufacture:                         EN\r\n");
			GetDidInfo(protect);
            GetFlashEon(protect);
            break;
        
        case FLASH_BG:
            APP_DBG("Manufacture:                         BG\r\n");
			GetDidInfo(protect);
            GetFlashBg(protect);
            break;
        
        case FLASH_ESMT:
            APP_DBG("Manufacture:                         ESMT\r\n");
			GetDidInfo(protect);
            GetFlashEsmt(protect);
            break;
        
        default:            
            APP_DBG("Manufacture:                         not found\r\n");
            break;
    }
	APP_DBG("\r\n");
	APP_DBG("****************************************************************\n");
}

#endif /* DEBUG_FLASH */

void platform_flash_init( void )
{
    SpiFlashInfoInit();
#ifdef  DEBUG_FLASH 
    SpiFlashGetInfo(&FlashInfo);
    GetFlashInfo();
#endif
}

#define MAX_OPERATE_LEN (16*1024)

int platform_flash_erase( uint32_t StartAddress, uint32_t EndAddress  )
{
    int erase_len = MAX_OPERATE_LEN, total_len = EndAddress - StartAddress +1;

    while(total_len > 0) {
        if (total_len > MAX_OPERATE_LEN) {
            erase_len = MAX_OPERATE_LEN;
            total_len -= MAX_OPERATE_LEN;
        } else {
            erase_len = total_len;
            total_len = 0;
        }
        SpiFlashErase( StartAddress, erase_len );

        StartAddress += erase_len;
    }

    return 0;
}

int platform_flash_write( volatile uint32_t* FlashAddress, uint8_t* Data ,uint32_t DataLength  )
{
    int write_len = MAX_OPERATE_LEN, total_len = DataLength;

    while(total_len > 0) {
        if (total_len > MAX_OPERATE_LEN) {
            write_len = MAX_OPERATE_LEN;
            total_len -= MAX_OPERATE_LEN;
        } else {
            write_len = total_len;
            total_len = 0;
        }
        SpiFlashWrite( *FlashAddress, Data, write_len );

        *FlashAddress += write_len;
        Data += write_len;
    }

    return 0;
}

int platform_flash_read( volatile uint32_t* FlashAddress, uint8_t* Data ,uint32_t DataLength  )
{
    int read_len = MAX_OPERATE_LEN, total_len = DataLength;

    while(total_len > 0) {
        if (total_len > MAX_OPERATE_LEN) {
            read_len = MAX_OPERATE_LEN;
            total_len -= MAX_OPERATE_LEN;
        } else {
            read_len = total_len;
            total_len = 0;
        }
        SpiFlashRead( *FlashAddress, Data, read_len );

        *FlashAddress += read_len;
        Data += read_len;
    }

    return 0;
}
