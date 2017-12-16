
/*******************************************************************************
 *
 * Filename:
 * mtk_fm_drv.c
 * maintainer: Halley
 *******************************************************************************/
#include "type.h"
#include "app_config.h"

#ifdef RADIO_MT6626_EN

#include "mtk_fm_drv.h"
#include "mtk_fm_drv_dsp.h"

/* defined in custom folder */

#define MT6626_VOL_MAX 0x2B //43 volume(0-15)

uint8 mtk_fm_on = 0;

//typedef BOOL bool;
/*check fm chip whether exist*/
static bool FMChip_Exist = FALSE;
static bool FM_Detect_Done = FALSE;

#define FM_write_I2C_address 0xE0
#define FM_read_I2C_address 0xE1

static void * FMI2cHandler = NULL;

#ifdef RADIO_MT6626_EN
#define FM_WRITE_I2C_ADDR 0xE0
#endif

const uint16 FMR_RSSI_THRESHOLD_LANT = 0xFF01;
const uint16 FMR_RSSI_THRESHOLD_SANT = 0xFEE0;
const bool FMR_DEEMPHASIS_50us = FALSE;
const uint16 FMR_CQI_TH = 0xFFE9;


#define BAND_UP 1080 //108MHz
#define BAND_DN 875 //87.5MHz


bool FM_MCUFA_OPEN = FALSE;

#define  LEVEL_HIGH           TRUE
#define  LEVEL_LOW            FALSE


static bool fm_state = LEVEL_LOW;


/// Global variables for current FM status
int16 _current_frequency = -1;
int16 _backup_frequency = -1;
static bool  _is_fm_on = FALSE;
static uint16 Chip_ID = 0;
static chip_ver Chip_Version = E1;
const uint16* bin_patch;
const uint16* bin_coeff;
uint8 HWSearch_flag = 0;
static uint8 Valid_flag = 0;
static uint16 FreqKHz = 0;
uint16 FreqSt = 987;
bool DebugSwith = TRUE;
#if defined(FM_PowerOn_with_ShortAntenna)
uint8 is_shortAntenna = 1;
#else
uint8 is_shortAntenna = 0;
#endif
//static uint16 I2Ssetting = 0;

static void MT6626_ParameterDownloadToDsp(const uint16* dsp_array, Download_type type);

static uint8 FMR_SleepHandle = 0xFF;
void FMDrv_Mute(uint8 mute);
void FMDrv_EvaluateRSSIThreshold(void);
void FMDrv_ChipReset(void);

// enginner mode struct

typedef enum group_idx 
{
	mono = 0,
	stereo,
	RSSI_threshold,
	HCC_Enable,
	PAMD_threshold,
	Softmute_Enable,
	De_emphasis,
	HL_Side,
	Demod_BW,
	Dynamic_Limiter,
	Softmute_Rate,
	AFC_Enable,
	Softmute_Level,
	Analog_Volume,
	GROUP_TOTAL_NUMS
} FACTORY_GROUP_ENUM;
	
typedef enum item_idx 
{
	Sblend_OFF = 0,
	Sblend_ON,  
	ITEM_TOTAL_NUMS
} FACTORY_ITEM_INDEX;
// enginner mode struct

typedef struct 
{
	uint16 addr;
	uint16 and;
	uint16 or;
} ctrl_word_operation;



#if defined(__FMRADIO_I2S_SUPPORT__)
#define POWER_ON_COMMAND_COUNT 21
#else  
#define POWER_ON_COMMAND_COUNT 19
#endif

static const ctrl_word_operation PowerOnSetting[POWER_ON_COMMAND_COUNT] = 
{
//RX DIGITAL Init
//FM Digital Init: fm_rgf_maincon
    {0x60, 0x0, 0x3000},
    {0x60, 0x0, 0x3001},
    {0xFFFB, 0x0, 0x0003},//FM_DelayUs 3ms
    {0x60, 0x0, 0x3003},
    {0x60, 0x0, 0x3007},
    {0xFFFD, 0x99, 0x0000},
#if defined(__FMRADIO_I2S_SUPPORT__)
    #if defined(FM_PowerOn_with_ShortAntenna)
    {0x61, 0xFF73, 0x0090},//no low power mode, I2S, short antenna
    #else
    {0x61, 0xFF73, 0x0080},//no low power mode, I2S, long antenna
    #endif
    {0x9B, 0xFFF7, 0x0008},//0000->master, 0008->slave 
    {0x5F, 0xE7FF, 0x0000},//0000->32K, 0800->44.1K, 1000->48K
    //{0x61, 0xFF73, 0x0080},//no low power mode, I2S, long antenna, 0xff63
    //{0x9B, 0xFFF7, 0x0008},//0000->master, 0008->slave 
    //{0x5F, 0xE7FF, 0x0000},//0000->32K, 0800->44.1K, 1000->48K
#else
    #if defined(FM_PowerOn_with_ShortAntenna)
    {0x61, 0xFF63, 0x0010},//no low power mode, analog line in, short antenna
    #else
    {0x61, 0xFF63, 0x0000},//no low power mode, analog line in, long antenna
    #endif
    //{0x5566, 0x0000, 0x0000}
#endif
    {0xFFFD, 0x0062, 0x0000},//read the HW version
    {0xFFF9, 0x0, 0x0002},
    {0xFFF9, 0x0, 0x0003},
    {0xFFF9, 0x0, 0x0004},
    {0x90, 0x0, 0x0040},
    {0x90, 0x0, 0x0000},
    {0x6A, 0x0, 0x0020},
    {0x6B, 0x0, 0x0020},
    {0x60, 0x0, 0x300F},
    {0x61, 0xFFFF, 0x0002},
    {0x61, 0xFFFE, 0x0000},
    //{0xFFFB, 0x0, 0x0064},//FM_DelayUs 100ms
    {0xFFFF, 0x64, 0x2}
    //,{0x65, 0x0, 0x02B6},
    //{0x63, 0x0, 0x0001},
    //{0xFFFF, 0x69, 0x0001},
    //{0x69, 0x0, 0x0001}
};

/*----------------------------------------------------------------------------*/
/* Loop count for time measurement                                            */
/*----------------------------------------------------------------------------*/
#if defined(MCU_13M)
/* 1T ~= 77 ns */
#define COUNTER_50US (325)
#define COUNTER_20US (130)
#define COUNTER_15US (97)
#define COUNTER_10US (65)
#define COUNTER_05US (32)
#define COUNTER_350NS (3)

#elif defined(MCU_15_36M)
/* 1T ~= 65 ns */
#define COUNTER_50US (384)
#define COUNTER_20US (153)
#define COUNTER_15US (174)
#define COUNTER_10US (76)
#define COUNTER_05US (38)
#define COUNTER_350NS (4)

#elif defined(MCU_26M)
/* 1T ~= 38 ns */
#define COUNTER_50US (658)
#define COUNTER_20US (263)
#define COUNTER_15US (197)
#define COUNTER_10US (132)
#define COUNTER_05US (66)
#define COUNTER_350NS (5)

#elif defined(MCU_30_72M)
/* 1T ~= 33 ns */
#define COUNTER_50US (757)
#define COUNTER_20US (303)
#define COUNTER_15US (227)
#define COUNTER_10US (151)
#define COUNTER_05US (75)
#define COUNTER_350NS (6)

#elif defined(MCU_39M)
/* 1T ~= 25 ns */
#define COUNTER_50US (1000)
#define COUNTER_20US (400)
#define COUNTER_15US (300)
#define COUNTER_10US (200)
#define COUNTER_05US (100)
#define COUNTER_350NS (7)

#elif defined(MCU_52M)
/* 1T ~= 19 ns */
#define COUNTER_50US (1316)
#define COUNTER_20US (526)
#define COUNTER_15US (395)
#define COUNTER_10US (263)
#define COUNTER_05US (132)
#define COUNTER_350NS (10)

#elif defined(MCU_61_44M)
/* 1T ~= 16 ns */
#define COUNTER_50US (1563)
#define COUNTER_20US (625)
#define COUNTER_15US (468)
#define COUNTER_10US (312)
#define COUNTER_05US (156)
#define COUNTER_350NS (11)

#elif defined(MCU_65M)
/* 1T ~= 15 ns */
#define COUNTER_50US (1666)
#define COUNTER_20US (666)
#define COUNTER_15US (500)
#define COUNTER_10US (333)
#define COUNTER_05US (166)
#define COUNTER_350NS (12)

#elif defined(MCU_78M)
/* 1T ~= 13 ns */
#define COUNTER_50US (1923)
#define COUNTER_20US (769)
#define COUNTER_15US (576)
#define COUNTER_10US (384)
#define COUNTER_05US (192)
#define COUNTER_350NS (13)

#elif defined(MCU_91M)
/* 1T ~= 11 ns */
#define COUNTER_50US (2272)
#define COUNTER_20US (909)
#define COUNTER_15US (681)
#define COUNTER_10US (454)
#define COUNTER_05US (227)
#define COUNTER_350NS (15)

#elif defined(MCU_104M)
/* 1T ~= 9.6 ns */
#define COUNTER_50US (2604)
#define COUNTER_20US (1042)
#define COUNTER_15US (781)
#define COUNTER_10US (521)
#define COUNTER_05US (260)
#define COUNTER_350NS (19)

#elif defined(MCU_122_88M)
/* 1T ~= 8.1 ns */
#define COUNTER_50US (3086)
#define COUNTER_20US (1235)
#define COUNTER_15US (925)
#define COUNTER_10US (617)
#define COUNTER_05US (308)
#define COUNTER_350NS (22)

#elif defined(MCU_208M)
/* 1T ~= 4.8 ns */
#define COUNTER_50US (5208)
#define COUNTER_20US (2084)
#define COUNTER_15US (1562)
#define COUNTER_10US (1042)
#define COUNTER_05US (520)
#define COUNTER_350NS (37)

#elif defined(MCU_245_76M)
/* 1T ~= 4.0 ns */
#define COUNTER_50US (6172)
#define COUNTER_20US (2470)
#define COUNTER_15US (1850)
#define COUNTER_10US (1234)
#define COUNTER_05US (616)
#define COUNTER_350NS (44)

#elif defined(MCU_312M)
/* 1T ~= 3.3 ns */
#define COUNTER_50US (10000)
#define COUNTER_20US (3000)
#define COUNTER_15US (2000)
#define COUNTER_10US (1500)
#define COUNTER_05US (1000)
#define COUNTER_350NS (60)

#else

#endif

static void Delayus(uint32 u4MicroSec)
{
	int i;
	for(i = 0; i < u4MicroSec * 10; i++);
}

static void Delayms(uint32 data)
{
    while(data--)
    {
        Delayus(1000);
    }
}

void sleep_task(uint32 data)
{
	Delayms(data);
}


bool MT6626_ReadByte(uint8 CW, uint16 *data)
{
#ifdef FUNC_RADIO_EN
	uint16_t TempData = 0;
	I2cReadNByte(FMI2cHandler, FM_WRITE_I2C_ADDR, CW, (uint8_t*)(&TempData), 2);
	*data = (*(uint8_t*)(&TempData) << 8) + *((uint8_t*)(&TempData) + 1);
	return TRUE;
#else
	return FALSE;
#endif
}


bool MT6626_WriteByte(uint8 CW, uint16 data)
{
	
#ifdef FUNC_RADIO_EN
	bool Rv;
	uint8_t LittleEndianData[2];
	LittleEndianData[0] = data >> 8;
	LittleEndianData[1] = data & 0xFF;
	Rv = I2cWriteNByte(FMI2cHandler, FM_WRITE_I2C_ADDR, CW, LittleEndianData, 2);
	return Rv;
#else
	return FALSE;
#endif
}

/***********************************************************************
*  Turn on/off tune (internal)
*
*  parameter-->ON_OFF: 1:ON, 0:OFF
*         
***********************************************************************/
void MT6626_TUNE_ON()
{
    uint16 dataRead;
    MT6626_ReadByte(FM_MAIN_CTRL, &dataRead);
    MT6626_WriteByte(FM_MAIN_CTRL, (dataRead & 0xFFFE) | TUNE);
}

/***********************************************************************
*  Seek on/off (internal)
*
*  parameter-->ON_OFF: 1:ON, 0:OFF
*         
***********************************************************************/
void MT6626_SEEK_ON()
{
    uint16 dataRead;
    MT6626_ReadByte(FM_MAIN_CTRL, &dataRead);
    MT6626_WriteByte(FM_MAIN_CTRL, (dataRead & 0xFFFD) | SEEK);
}

/***********************************************************************
*  Scan on/off (internal)
*
*  parameter-->ON_OFF: 1:ON, 0:OFF
*         
***********************************************************************/
void MT6626_SCAN_ON()
{
    uint16 dataRead;
    MT6626_ReadByte(FM_MAIN_CTRL, &dataRead);
    MT6626_WriteByte(FM_MAIN_CTRL, (dataRead & 0xFFFB) | SCAN);
}

/***********************************************************************
*  RAMP off (internal)
*         
***********************************************************************/
void MT6626_RampDown()
{
    uint16 dataRead;

    //Clear DSP state
    MT6626_ReadByte(FM_MAIN_CTRL, &dataRead);		
    MT6626_WriteByte(FM_MAIN_CTRL, (dataRead & 0xFFF0)); //clear rgf_tune/seek/scan/dsp_init
    //MT6626_WriteByte(FM_MAIN_CTRL, 0);
    
    //Set DSP ramp down state
    MT6626_ReadByte(FM_MAIN_CTRL, &dataRead);		
    MT6626_WriteByte(FM_MAIN_CTRL, (dataRead | RAMP_DOWN));
    
    //Check STC_DONE status flag (not the interrupt flag!)
    do
    {
        MT6626_ReadByte(FM_MAIN_INTR, &dataRead);
    } while((dataRead&FM_INTR_STC_DONE) == 0);
    //Clear DSP ramp down state
    MT6626_ReadByte(FM_MAIN_CTRL, &dataRead);		
    MT6626_WriteByte(FM_MAIN_CTRL, (dataRead & (~RAMP_DOWN)));
    
    MT6626_ReadByte(FM_MAIN_INTR, &dataRead);		
    MT6626_WriteByte(FM_MAIN_INTR, dataRead);//clear status flag

}

static void MT6626_writeFA(uint16* buff, uint8 fa)
{
    uint8 i = 0;
	if((fa & 0x4) != 0)
	{
		FM_MCUFA_OPEN = TRUE;
	}
	else
	{
		FM_MCUFA_OPEN = FALSE;
	}
	for(i = 0; i < 3; i++)
    {
        if((fa >> i) & 0x1)
		{
            *buff |= (1 << (12 + i));
		}
        else
		{
            *buff &= ~(1 << (12 + i));
		}
    }
}

/***********************************************************************
*  Set radio frequency (internal)
*
*  parameter-->CurFreq:set frequency
*         
***********************************************************************/
static bool MT6626_SetFreq(int32 CurFreq)
{
	uint8 tuneRetryCount = 0;
	uint32 CHAN = 0x0000;
	uint16 dataRead, cnt = 0, tempbuff = 0;
    
    MT6626_RampDown();
    
    _current_frequency = CurFreq;
    CHAN = (CurFreq - 640) << 1;
    MT6626_ReadByte(FM_CHANNEL_SET, &dataRead);

    if(Chip_Version == E1)
    {
        if(((uint8)((dataRead & 0x1000)>>12)) ^ (channel_parameter[CurFreq - 760] & 0x1))
        {
            MT6626_ReadByte(0x61, &tempbuff);
    		MT6626_WriteByte(0x60, 0x330F);
    		MT6626_WriteByte(0x61, 1); 
            MT6626_WriteByte(0x6e, 0x0);		
            MT6626_WriteByte(0x6e, 0x0);		
            MT6626_WriteByte(0x6e, 0x0);		
            MT6626_WriteByte(0x6e, 0x0);		
            MT6626_WriteByte(0x60, 0x0);		
            MT6626_WriteByte(0x60, 0x4000);
            MT6626_WriteByte(0x60, 0x0);		
            MT6626_WriteByte(0x60, 0x3000);		
            MT6626_WriteByte(0x60, 0x3001);		
            Delayms(3);
            MT6626_WriteByte(0x60, 0x3003);		
            MT6626_WriteByte(0x60, 0x3007);		
            MT6626_WriteByte(0x60, 0x300f);
            MT6626_WriteByte(0x61, tempbuff | 0x3);		
            MT6626_WriteByte(0x61, tempbuff | 0x2);		
			MT6626_WriteByte(0x6A, 0x20);
			MT6626_WriteByte(0x6B, 0x20);
            Delayms(200);	
        }
    }
    MT6626_writeFA(&dataRead, (channel_parameter[CurFreq - 760]));
    MT6626_WriteByte(FM_CHANNEL_SET, (dataRead & 0xFC00) | CHAN);
    
    MT6626_TUNE_ON();	
    
    do 
    {    
        MT6626_ReadByte(FM_MAIN_INTR, &dataRead);
        if((dataRead & FM_INTR_STC_DONE) == 0)
        {
            sleep_task(3);
        }        
        if(cnt++ > 20)
        {
            cnt = 0;
			//FMR_TRACE(FM_INFO_TuneTimes,"setfreq: poll many times!");
			if(tuneRetryCount > 10)
			{
				FMDrv_ChipReset();
			}
			tuneRetryCount++;
        }
    } while((dataRead & FM_INTR_STC_DONE) == 0);
    MT6626_WriteByte(FM_MAIN_INTR, dataRead | FM_INTR_STC_DONE);//clear status flag
    
#if defined(MT6626_FPGA)
    MT6626_WriteByte(0x54, 0x0001);
    MT6626_WriteByte(0x11, 0xb7d4);
    MT6626_WriteByte(0x54, 0x8000);
#endif

return TRUE;
}//MT6626_SetFreq

/***********************************************************************
*  Get RSSI Value (internal)
*
*  parameter-->
*  return dBuVemf
***********************************************************************/
int16 MT6626_GetCurRSSI(void)
{
    uint16 TmpReg;
    int16 RSSI,dBValue;
    MT6626_ReadByte(FM_R_RSSI, &TmpReg);
    RSSI = (int16)(TmpReg & 0x3FF);
    /*RS = RSSI
    *If RS>511, then RSSI(dBm)= (RS-1024)/16*6
    *                 else RSSI(dBm)= RS/16*6             */
    dBValue = (RSSI>511) ? (((RSSI - 1024) * 6 / 16) + 113) : (RSSI * 6 / 16 + 113);
    if(dBValue < 0)
    {
        dBValue = 0;
    }
    
    return dBValue;
}

/***********************************************************************
*  Get PAMD Value (internal)
*
*  parameter-->
***********************************************************************/
uint16 MT6626_GetCurPamd(void)
{
    uint16 TmpReg, PAMD, dBValue;
    MT6626_ReadByte(ADDR_PAMD, &TmpReg);
    /*PA = PAMD
    *If PA >= 256 then PAMD(dB) =  (PA-512)/16*6,
    *               else PAMD(dB) = PA/16*6                 */
    PAMD = TmpReg & 0x1FF;
    dBValue = (PAMD >= 256) ? ((512 - PAMD) * 6 / 16) : 0;
    
    return dBValue;
}
/***********************************************************************
*  Get Search freq (internal)
*
*  parameter-->
***********************************************************************/
void MT6626_GetSearchFreq(uint16* pfreq)
{
    *pfreq = (uint16)FreqKHz;
}

/*********************************************************************
*  Sete Antenna type
*  parameter-->Type: 1:Short Antenna, 0:Long Antenna
*
*********************************************************************/
bool MT6626_SetAntennaType(uint8 isShortATA)
{
    uint16 dataRead;
    
	is_shortAntenna = isShortATA;
    MT6626_ReadByte(FM_MAIN_CG2_CTRL, &dataRead);
    
    if(isShortATA)
    {
        dataRead |= ANTENNA_TYPE;
    }
    else
    {
        dataRead &= (~ANTENNA_TYPE);
    }
    
    MT6626_WriteByte(FM_MAIN_CG2_CTRL, dataRead);
    
    return TRUE;
}

/*********************************************************************
*  Get Antenna type
*  parameter
*  return-->1:Short Antenna, 0:Long Antenna
*********************************************************************/
bool MT6626_GetAntennaType(void)
{
    uint16 dataRead;
    
    MT6626_ReadByte(FM_MAIN_CG2_CTRL, &dataRead);
    if(dataRead&ANTENNA_TYPE)
	{
        return TRUE; //short antenna
	}
    else
	{
        return FALSE; //long antenna
	}
}

/*********************************************************************
*  DSP download procedure
*  parameter-->patch_download, coefficient_download
*
*********************************************************************/
#define BUFF_SIZE 10
#define OFFSET_REG 0x91
#define CONTROL_REG 0x90
#define DATA_REG 0x92
static void MT6626_ParameterDownloadToDsp(const uint16* dsp_array, Download_type type)
{
    uint32 CONTROL_CODE = 0;
    uint16 data_length = 0;  // in words
    uint16 i;

    switch(type) 
    {
        case rom_download:  //rom code
        case patch_download:  //patch
            CONTROL_CODE = 0x10;
            break;
        case coefficient_download:  //coeff
            CONTROL_CODE = 0xe;
            break;
        case hw_coefficient_download:	//HW coeff
            CONTROL_CODE = 0xd;
            break;
        default:
        break;
    }
	
    data_length = dsp_array[1] - dsp_array[0] + 1;        

    if(data_length > 0) 
    {
        MT6626_WriteByte(CONTROL_REG, 0);
        MT6626_WriteByte(OFFSET_REG, dsp_array[0]);		//Start address
        MT6626_WriteByte(CONTROL_REG, 0x40); 			//Reset download control
        MT6626_WriteByte(CONTROL_REG, CONTROL_CODE); 	//Set download control

        if(type == coefficient_download)
        {
			if(FMR_DEEMPHASIS_50us)
			{
				for(i = 0; i < data_length; i++) 
				{
					if(i == 86)
					{
						MT6626_WriteByte(DATA_REG, FMR_RSSI_THRESHOLD_LANT);
					} 
					else if(i == 88)
					{
						MT6626_WriteByte(DATA_REG, FMR_CQI_TH);
					} 
					else if(i == 292)
					{					
						MT6626_WriteByte(DATA_REG, 0x332B);
						MT6626_WriteByte(DATA_REG, 0x2545);
						MT6626_WriteByte(DATA_REG, 0x1344);
						MT6626_WriteByte(DATA_REG, 0x09F5);
						MT6626_WriteByte(DATA_REG, 0x0526);
						MT6626_WriteByte(DATA_REG, 0x02A9);
						MT6626_WriteByte(DATA_REG, 0x0160);
						MT6626_WriteByte(DATA_REG, 0x00B6);
						MT6626_WriteByte(DATA_REG, 0x005E);
						MT6626_WriteByte(DATA_REG, 0x0031);
						MT6626_WriteByte(DATA_REG, 0x0000);
						MT6626_WriteByte(DATA_REG, 0x0000);
						MT6626_WriteByte(DATA_REG, 0x0000);
						i += 12;
					}
					else if(i == 505)
					{
						MT6626_WriteByte(DATA_REG, FMR_RSSI_THRESHOLD_SANT);
					}
					else
					{
						MT6626_WriteByte(DATA_REG, dsp_array[2 + i]);
					}
				}
			}
			else
			{
				for(i = 0; i < data_length; i++) 
				{
					if(i == 86)
					{
						MT6626_WriteByte(DATA_REG, FMR_RSSI_THRESHOLD_LANT);
					} 
					else if(i == 88)
					{
						MT6626_WriteByte(DATA_REG, FMR_CQI_TH);
					} 
					else if(i == 505)
					{
						MT6626_WriteByte(DATA_REG, FMR_RSSI_THRESHOLD_SANT);
					}
					else
					{
					MT6626_WriteByte(DATA_REG, dsp_array[2 + i]);
					}
				}
			}
        }
		else
        {
			for(i = 0; i < data_length; i++) 
			{
				MT6626_WriteByte(DATA_REG, dsp_array[2 + i]);
			}
        }
    
    } 
    else
    {
        //print("MT6626_ParameterDownloadToDsp, file size ERROR!");
        //ASSERT(0);
    }
}
/***********************************************************************
*  FM Chip initial
*
*  parameter-->
***********************************************************************/
void FMDrv_ChipInit(void)
{
// Init Reset/Power Pin to FM
	//GpioClk32kIoConfig(1);
	if(!FMI2cHandler)
	{
		FMI2cHandler = I2cMasterCreate(I2C_SCL_PORT_INDEX, I2C_SCL_PIN_INDEX, I2C_SDA_PORT_INDEX, I2C_SDA_PIN_INDEX);
	}
	FMDrv_AutoDetect();   
}



bool MT6626hpInit(void)
{
	FMDrv_SetAntennaType(0);
	FMDrv_RDS_Disable();
	FMDrv_Force_Stereo();
	FMDrv_SetVolumeLevel(0x20);
	return TRUE;
}

/*detect 6626 is exist or not*/
void FMDrv_AutoDetect(void)
{
	int32 i = 0;
	uint16 tmp_reg;
	
	for(i = 0; i < POWER_ON_COMMAND_COUNT; i++) 
    { 
        if(PowerOnSetting[i].addr == 0xFFFD)
        {  //read status
            MT6626_ReadByte(PowerOnSetting[i].and, &tmp_reg);
            switch(PowerOnSetting[i].and)
            {
                case 0x62:
                    if(tmp_reg == 0x6626)
                    {
						FMChip_Exist = TRUE;
                    }
                    break;
                default:
                    break;
            }
        }
        else if(PowerOnSetting[i].addr == 0xFFFB)
        { //delay ms
            Delayms(PowerOnSetting[i].or);
        }
        else
        {
            if(PowerOnSetting[i].and != 0)
            {
                MT6626_ReadByte(PowerOnSetting[i].addr, &tmp_reg);
                tmp_reg &= PowerOnSetting[i].and;
                tmp_reg |= PowerOnSetting[i].or;	  	
            }
            else
            {
                tmp_reg = PowerOnSetting[i].or;
            }
            MT6626_WriteByte(PowerOnSetting[i].addr, tmp_reg);
        }
    }

	/*power off chip*/
    MT6626_WriteByte(0x60, 0x330F);
    MT6626_WriteByte(FM_MAIN_CG2_CTRL, 1); 
    for(i = 0; i < 4; i++)
    {
        MT6626_ReadByte(0x6E, &tmp_reg);		 
        MT6626_WriteByte(0x6E, (tmp_reg & 0xFFF8)); 
    }
    MT6626_WriteByte(FM_MAIN_CG1_CTRL, 0); 
    MT6626_WriteByte(FM_MAIN_CG1_CTRL, 0x4000); 
    MT6626_WriteByte(FM_MAIN_CG1_CTRL, 0); 
	FM_Detect_Done = TRUE;
}
/***********************************************************************
*  FMDrv_Get_stereo_mono
*
*  parameter-->
* RETURNS
*  0:mono 1:stereo
***********************************************************************/
uint8 FMDrv_Get_stereo_mono(void)
{
    uint16 TmpReg, value;
    MT6626_ReadByte(FM_R_RSSI, &TmpReg);
    value = (TmpReg & FM_BF_STEREO) >> 12;
    return value;
}

/***********************************************************************
*  Engineer mode function (API)
*
*  parameter-->group_idx: mono\stereo\RSSI_threshold\IF_count_delta
*              item_idx: sub select index
*              item_value: set parameter value
***********************************************************************/
void FMDrv_radio_item_info(uint16 group_idx, uint16 item_idx, uint32 item_value)
{
   uint16 dataRead;
	
	MT6626_ReadByte(0x60, &dataRead); 
	MT6626_WriteByte(0x60, dataRead & (0xFFF7)); 
	MT6626_ReadByte(0x75, &dataRead); 
	switch(group_idx)
	{
		case mono:
			if(item_value)
			{
				MT6626_WriteByte(0x75, dataRead | 0x0008);
			}
			else
			{
				MT6626_WriteByte(0x75, (dataRead & 0xFFF7));
			}
			break;
		case stereo:
			if(item_value)
			{
				MT6626_WriteByte(0x75, dataRead & 0xFFF7);
			}
			else
			{
				MT6626_WriteByte(0x75, dataRead | 0x0008);
			}
			break;
        case HCC_Enable:
            if(item_idx)
            {
                MT6626_WriteByte(0x75, dataRead | 0x0004);
            }
            else
            {
                MT6626_WriteByte(0x75, dataRead & (~0x0004));		
            }
			break;
		case Softmute_Enable:
			if(item_idx)
			{
				MT6626_WriteByte(0x75, dataRead | 0x0001);
			}
			else
			{
				MT6626_WriteByte(0x75, dataRead & (~0x0001));		
			}
			break;
		case RSSI_threshold:
		case PAMD_threshold:
		case De_emphasis:
		case HL_Side:
		case Demod_BW:
		case Dynamic_Limiter:
		case Softmute_Rate:
		case AFC_Enable:
		case Softmute_Level:
		case Analog_Volume:
		default:
			break;
	}
	MT6626_ReadByte(0x60, &dataRead); 
	MT6626_WriteByte(0x60, dataRead | 0x0008); 
}


bool FMDrv_IsChipValid(void)
{
	if(FM_Detect_Done == FALSE)
	{
		FMDrv_AutoDetect();
	}
    return FMChip_Exist;
}

uint16 FMDrv_ReadByte(uint8 addr)
{
    uint16 Data;
    MT6626_ReadByte(addr, &Data);
    return Data;
}

bool FMDrv_WriteByte(uint8 addr, uint16 data)
{
    MT6626_WriteByte(addr, data);
    return TRUE;
}

static int mt6626_set_bits(uint8 addr, uint16 bits, uint16 mask)
{
	int ret = 0;
    uint16 val;

    ret = MT6626_ReadByte(addr, &val);
    if(ret)
	{
        return ret;
	}


    val = ((val & (mask)) | bits);
    ret = MT6626_WriteByte(addr, val);
    return ret;
	
}

void FMDrv_SetVolumeLevel(uint8 vol)
{

	uint8 tmp_vol = vol & 0x3f;
	uint16 tmp = 0;
	MT6626_ReadByte(0x60, &tmp);
	
	MT6626_WriteByte(0x60, tmp & 0xFFF7); //0x60 D3 = 0

	tmp_vol = vol * 3;
	if(tmp_vol > MT6626_VOL_MAX)
	{
		tmp_vol = MT6626_VOL_MAX;
	}
	
	mt6626_set_bits(0x9C, (tmp_vol << 8), 0xC0FF);
   
	MT6626_WriteByte(0x60, tmp); //0x60 D3 = 1
	return;
}

void FMDrv_Mute(uint8 mute)
{
    uint16 dataRead;
    
    MT6626_ReadByte(FM_MAIN_CTRL,&dataRead);                           
    if(mute == 1)
	{
        MT6626_WriteByte(FM_MAIN_CTRL, (dataRead & 0xFFDF) | 0x0020);  
	}
    else
	{
        MT6626_WriteByte(FM_MAIN_CTRL, (dataRead & 0xFFDF)); 
	}
}

void FMDrv_GetIntr(uint16 *dataRead)
{
    MT6626_ReadByte(FM_MAIN_INTR, dataRead);
}

/***********************************************************************
*  FM Interrupt initialize
*
*  parameter-->
***********************************************************************/
void FMDrv_IntrInit(void)
{
	/*
#if defined(FM_INTERFACE_I2C)
#if (defined(FM_EINT_SUPPORT_SEARCH)) || (defined(__FM_RADIO_RDS_SUPPORT__))
    EINT_Registration(FM_EINT_NO, KAL_FALSE, fm_state, FMDrv_EINT_HISR, KAL_FALSE);
#endif
#endif
	*/
}

/*************************************************************
*  Radio power on Reset
*
*************************************************************/
void FMDrv_PowerOnReset(void)
{	
    int32 i;
    uint16 tmp_reg;
    DebugSwith = FALSE;
    
    /// Power On
    //trace(TRACE_INFO, FM_INFO_FM_COMMON_POWER_ON);
	
	
    //SerialCommInit();// pull I2C high before chip power on
/*
#if defined(FM_INTERFACE_I2C) && defined(BT_SUPPORT)
#if defined(__FMRADIO_I2S_SUPPORT__)
	   //BtRadio_Power_Handler(1, 1, 1);
#else
	  // BtRadio_Power_Handler(1, 1, 0);
#endif	   
#endif
*/
    for(i = 0; i < POWER_ON_COMMAND_COUNT; i++) 
    { 
        if(PowerOnSetting[i].addr == 0xFFFF) 
        { //polling status = 1  
            while(1)
            {		
                MT6626_ReadByte(PowerOnSetting[i].and, &tmp_reg);
                tmp_reg &= PowerOnSetting[i].or;
                if(tmp_reg)
				{
                    break;
				}
            }
        }   
        else if(PowerOnSetting[i].addr == 0xFFFE)
        {  //polling status = 0
            while(1)
            {		
                MT6626_ReadByte(PowerOnSetting[i].and, &tmp_reg);
                tmp_reg &= PowerOnSetting[i].or;
                if(!tmp_reg)
				{
                    break;
				}
            }      
        }
        else if(PowerOnSetting[i].addr == 0xFFFD)
        {  //read status
            MT6626_ReadByte(PowerOnSetting[i].and, &tmp_reg);
            switch(PowerOnSetting[i].and)
            {
                case 0x99:
					switch(tmp_reg)
                    {
                    	case 0x0:
							Chip_Version = E1;
							bin_patch = bin_patch_E1;
							bin_coeff = bin_coeff_E1;
						    break;
                        case 0x8A01:
						default:
							Chip_Version = E2;
							bin_patch = bin_patch_E2;
							bin_coeff = bin_coeff_E2;
							break;
                    }
					break;
                case 0x62:
                    Chip_ID = tmp_reg;    
                    //trace(TRACE_INFO, FM_INFO_FM_CHIP_ID, Chip_ID);
                    if(Chip_ID == 0xFFFF)
                    {
                        //print("MT6626 Power on reset, Chip ID is wrong!!!");
                        //ASSERT(0);
                    }
                    break;
                case 0x1C:
                    if(PowerOnSetting[i].or) 
                    {
                        MT6626_WriteByte(PowerOnSetting[i].and, (tmp_reg | 0x8000));
                    } 
                    else 
                    {
                        MT6626_WriteByte(PowerOnSetting[i].and, (tmp_reg & 0x7FFF));
                    }
                    break;
                
                default:
                    break;
            }
        }
        else if(PowerOnSetting[i].addr == 0xFFFB)
        { //delay ms
            Delayms(PowerOnSetting[i].or);
        }
        else if(PowerOnSetting[i].addr == 0xFFFA)
        { //delay us
            Delayus(PowerOnSetting[i].or);
        }
        else if(PowerOnSetting[i].addr == 0xFFF9)
        { //select the function that already coded
            switch(PowerOnSetting[i].or)
            {
                case 0x02:  //DSP path download
                    MT6626_ParameterDownloadToDsp(bin_patch, patch_download);
                    break;
                case 0x03:  //DSP coefficient download
                    MT6626_ParameterDownloadToDsp(bin_coeff, coefficient_download);
                    break;
                case 0x04:  //DSP HW coefficient download
                    MT6626_ParameterDownloadToDsp(bin_hw_coeff, hw_coefficient_download);
                    break;
                default:
                    break;
            }
        }
        else
        {
            if(PowerOnSetting[i].and != 0)
            {
                MT6626_ReadByte(PowerOnSetting[i].addr, &tmp_reg);
                tmp_reg &= PowerOnSetting[i].and;
                tmp_reg |= PowerOnSetting[i].or;	  	
            }
            else
            {
                tmp_reg = PowerOnSetting[i].or;
            }
            MT6626_WriteByte(PowerOnSetting[i].addr, tmp_reg);
        }
    }
	
    FMDrv_IntrInit();   
#if 1/*disable short antenna calibration*/ 	
	MT6626_ReadByte(FM_MAIN_CG1_CTRL, &tmp_reg);
	MT6626_WriteByte(FM_MAIN_CG1_CTRL, tmp_reg & (~0x0008));
	MT6626_WriteByte(0x23, 0x2000);
	MT6626_ReadByte(FM_MAIN_CG1_CTRL, &tmp_reg);
	MT6626_WriteByte(FM_MAIN_CG1_CTRL, tmp_reg | 0x0008);
#endif	
 
    _is_fm_on = TRUE;
    _current_frequency = -1;
    //DebugSwith = TRUE;
//print("FMDrv_PowerOnReset Done!");		
}

bool MT6626FMPowerOn(void)
{
	FMDrv_ChipInit();
#ifdef FUNC_RADIO_EN
	FMDrv_PowerOnReset();
	return TRUE;		
#else
	return FALSE;
#endif	
}


/*****************************************************************
*  Radio power off
*
*****************************************************************/
void FMDrv_PowerOffProc(void)
{
    int16 i;
    uint16 dataRead;
   
#ifndef USE_I2C
    uint8 tmp1;
#endif

  // EINT_Mask(FM_EINT_NO);
	/*Before MCUFA end, triggle BT EINT to wake up BT MCUSYS*/
	if(FM_MCUFA_OPEN)
	{
	    MT6626_ReadByte(FM_MAIN_EXTINTRMASK, &dataRead);	
		MT6626_WriteByte(FM_MAIN_EXTINTRMASK, (dataRead | 0x1000));//enable sw bt eint 
	    MT6626_ReadByte(FM_MAIN_INTR, &dataRead);	
		MT6626_WriteByte(FM_MAIN_INTR, (dataRead | FM_INTR_SW_INTR));//send sw  eint 
		Delayus(100);//delay 2 32k tick at least
	    MT6626_ReadByte(FM_MAIN_INTR, &dataRead);	
		MT6626_WriteByte(FM_MAIN_INTR, (dataRead & (~FM_INTR_SW_INTR)));//clear sw  eint 
	    MT6626_ReadByte(FM_MAIN_EXTINTRMASK, &dataRead);	
		MT6626_WriteByte(FM_MAIN_EXTINTRMASK, (dataRead & (~0x1000)));//disable sw bt eint 
	}
	
	/*SW work around for MCUFA issue. 
	 *if interrupt happen before doing rampdown, DSP can't switch MCUFA back well.
	 * In case read interrupt, and clean if interrupt found before rampdown. 
	 */
    MT6626_ReadByte(FM_MAIN_INTR, &dataRead);	
    if(dataRead & 0x1)
    {
        MT6626_WriteByte(FM_MAIN_INTR, dataRead);//clear status flag
    }
    MT6626_RampDown();
    
    MT6626_WriteByte(0x60, 0x330F);
    MT6626_WriteByte(FM_MAIN_CG2_CTRL, 1); 
    for(i = 0; i < 4; i++)
    {
        MT6626_ReadByte(0x6E, &dataRead);		 
        MT6626_WriteByte(0x6E, (dataRead & 0xFFF8)); 
    }
    MT6626_WriteByte(FM_MAIN_CG1_CTRL, 0); 
    MT6626_WriteByte(FM_MAIN_CG1_CTRL, 0x4000); 
    MT6626_WriteByte(FM_MAIN_CG1_CTRL, 0); 
    
#if defined(BT_SUPPORT)
   // BtRadio_Power_Handler(1, 0, 0);
#endif
    
    
    _is_fm_on = FALSE;
    _current_frequency = -1;
}

bool MT6626FMPowerOff(void)
{
	FMDrv_PowerOffProc();
	return TRUE;
}

/*********************************************************************
*  Radio set frquency
*
*  parameter-->curf:setting frequency value
                    input value: 875 - 1080 (87.5 MHz - 108.0 MHz)
*********************************************************************/
void FMDrv_SetFreq(int16 curf)  /* input value: 875 - 1080 (87.5 MHz - 108.0 MHz)*/
{

    //pstFMR_data->is_searching = FALSE;
    if(_is_fm_on != 1)
    {
        FMDrv_PowerOnReset();
    }
    
    //FMDrv_Mute(1);
    if(MT6626_SetFreq(curf) == 0) 
    {
        //ASSERT(0);
    }     
    FreqKHz = curf;
    //FMDrv_Mute(0);
}

/*********************************************************************
*  Set Antenna type
*  parameter-->Type: 1:Short Antenna, 0:Long Antenna
*
*********************************************************************/
bool FMDrv_SetAntennaType(uint8 ATAType)
{
	return MT6626_SetAntennaType(ATAType);
}

/*********************************************************************
*  Get Antenna type
*  parameter
*  return-->1:Short Antenna, 0:Long Antenna
*********************************************************************/
uint8 FMDrv_GetAntennaType(void)
{
	return MT6626_GetAntennaType();
}

/**********************************************************************
*  Get really signal level in current frequency
*
*  parameter-->curf:frequency value of radio now
**********************************************************************/
uint16 FMDrv_GetSigLvl(int16 curf)
{
    uint16 rssi;
    
    rssi = MT6626_GetCurRSSI();
    return rssi;
}

/**********************************************************************
*  Get PAMD in current frequency
*
*  parameter-->curf:frequency value of radio now
**********************************************************************/
uint16 FMDrv_GetPamdLvl(int16 curf)
{
	uint16 pamd, readtimes, total = 0, Average_Time = 0;

	if(curf != _current_frequency)
	{
		FMDrv_SetFreq(curf);
	}
	for(readtimes = 0; readtimes < 8; readtimes++)
	{
		pamd = MT6626_GetCurPamd();
		total += pamd;
        if(pamd != 0)
        {
            Average_Time++;
        }
		//sleep_task(10);
      Delayms(10);				
	}
   	if(Average_Time != 0)
   	{
	    return total/Average_Time;
   	}
    else
    {
        return 0;
    }
}
/*
range:20~50dB
return dB
*/
int16 FMDrv_GetMR(void)
{
	uint16 RegTemp;
	int16 MR;
    MT6626_ReadByte(0xBD, &RegTemp);
	RegTemp = (RegTemp & 0x1FF);
	
	if(RegTemp > 255)
	{
		MR = (int16)(RegTemp - 512) * 6 / 16;
	}
	else
	{

		MR = (int16)RegTemp * 6 / 16;
	}
	return MR;
}

void FMGetScanTbl(uint16* CH_TBL)
{

}

/**********************************************************************
*  Radio valid station,used in HW auto search frequency to verify 
*  valid positon
*
*  parameter-->freq: start frequency
               is_step_up:1-->forward, 0-->backward
               space:search step,0:200KHz, 1:100KHz
**********************************************************************/
void FMDrv_HWSearch(int16 freq, bool is_step_up, int16 space, bool is_preset)
{
    int32 targetFreq;
    uint8 UpDown_flag = 1;
    uint16 dataRead;
    uint8 _step;
	
    if(is_step_up)
	{
        UpDown_flag = 0;
	}
    HWSearch_flag = 0;
    Valid_flag = 0;
    FreqSt = FreqKHz;			
    
    //calculate starting frequency
    targetFreq = (int32)(freq);
    if(space)
    {// 100K
        _step = 1;
    }
    else
    { //200K
        _step = 2;
    }
    //- Modified code start  
    if(MT6626_SetFreq(targetFreq) == 0) 
    {
        //ASSERT(0);
    }

    MT6626_RampDown();
    //Delayms(50);
	
    // Setting before seek    
    MT6626_ReadByte(FM_MAIN_CFG2, &dataRead);  
    MT6626_WriteByte(FM_MAIN_CFG2, (dataRead & 0xFC00) | ((BAND_DN - 640) << 1));//set space(100k/200k)and band(875~1080)and up/down
    MT6626_ReadByte(FM_MAIN_CFG1, &dataRead);  
    MT6626_WriteByte(FM_MAIN_CFG1, (dataRead & 0x8800) | (UpDown_flag << 10) | (1 << (12 + _step)) | ((BAND_UP - 640) << 1));//set space(100k/200k)and band(875~1080)and up/down
    MT6626_ReadByte(FM_MAIN_CFG1, &dataRead);

    if(!is_preset)
    {	// this is not a auto_scan function !
        MT6626_WriteByte(FM_MAIN_CFG1, dataRead & 0xF7FF | 0x0800); //enable wrap, if it is not auto scan function
    }
	else
	{
        MT6626_WriteByte(FM_MAIN_CFG1, dataRead & 0xF7FF); //disable wrap, if it is auto scan function
    }
    
    if(!is_preset)
    {	// this is not a auto_scan function !
        MT6626_SEEK_ON();
    }
	else
	{
        MT6626_SCAN_ON();   
    }
}

uint8 FMDrv_HWSpolling(uint16* curf, uint8* is_valid)
{
    if(HWSearch_flag != 0)
    {
        MT6626_GetSearchFreq(curf); 
        if(Valid_flag == 1)
		{
            *is_valid = 1;
		}
        else
		{
            *is_valid = 0;
		}
        return 1;
    }
    else
	{
        return 0;
	}
}


/**********************************************************************
*  Radio valid station,used in auto search frequency to verify 
*  valid positon
*
*  parameter-->freq: destination frequency
               signalv1:not used 
               is_step_up:search direction,1. search up,0. search down
 *retrun: 0, dest freq is invalid, other valid
**********************************************************************/
uint8 FMDrv_ValidStop(int16 freq, int8 signalvl, bool is_step_up)
{
	uint8 cnt;
	uint16 RSSIValue = 0, PAMD = 0;
	FMDrv_SetFreq(freq);
    RSSIValue = FMDrv_GetSigLvl(freq);
	//trace(TRACE_INFO, FM_INFO_SortTuneRssi, freq, RSSIValue);
	if(RSSIValue > (((FMR_RSSI_THRESHOLD_LANT - 65536) * 6 / 16) + 113))
	{
		RSSIValue = MT6626_GetCurPamd();
		//trace(TRACE_INFO, FM_INFO_FM_PAMD_LVL, _current_frequency, RSSIValue);
		PAMD = FMR_CQI_TH & 0x1FF;
		PAMD = (PAMD >= 256) ? ((512 - PAMD) * 6 / 16) : 0;
		//trace(TRACE_INFO, FM_INFO_FM_PAMD_LVL, _current_frequency, PAMD);
		if(RSSIValue >= PAMD)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

void FMDrv_GetScanTbl(uint16* BitMapData, uint8 offset)
{  
    MT6626_ReadByte(RDS_DATA_REG, &BitMapData[offset]);  
}

void FMDrv_HCI_Event_Hdlr(bool isReadyRead)
{
    return;
}

uint32 FMDrv_GetCapArray(void)
{
    uint8 i = 0;
    uint16 regData = 0, capValue = 0;
    uint32 cookedCapValue = 0;

	uint32 chara[] = { 165, 330, 660, 1320,
                           2640, 5280, 10560, 21120 };
	
    MT6626_ReadByte(0x60, &regData);
    MT6626_WriteByte(0x60, regData & 0xFFF7);
	
    MT6626_ReadByte(0x25, &regData);
    
	capValue = regData >> 8;


	for(i = 0; i < 8; i++)
    {
        if((capValue >> i) & 0x0001)
        {
            cookedCapValue += chara[i];
        }
    }
	
    MT6626_ReadByte(0x60, &regData);
    MT6626_WriteByte(0x60, regData | 0x8);

	return cookedCapValue;
	
}
/***********************************************************************
*  FM Chip Reset
*
*  parameter-->
***********************************************************************/
void FMDrv_ChipReset(void)
{
    int16 temp_curFreq = _current_frequency;
    //BtRadio_Power_Handler_Force_Reset(1);
	//print("reset common 1 is done!");
	//BtRadio_Power_Handler(1, 0, 0);
	//print("reset common 2 is done!");
	FMDrv_PowerOnReset();
	//print("reset 1");
    FMDrv_SetAntennaType(is_shortAntenna);
	//print("reset 2");
	FMDrv_SetFreq(temp_curFreq);
	//print("reset 3");
}

bool FMDrv_Seek_Eliminate(uint16 rFreq, uint16 RSSI)
{
	uint16 RssiTemp, PAMD = 0;	
	bool EmptyChannel = FALSE;
	//FMDrv_Mute(1);
	FMDrv_SCAN_Sort_Tune_Internal(rFreq, &RssiTemp);
	if(RssiTemp < RSSI)
	{ 
	    //trace(TRACE_INFO, FM_INFO_SortTuneRssi, freq, RSSIValue);
	    //if(RSSI > (((FMR_RSSI_THRESHOLD_LANT-65536)*6/16)+113))
		 // if(RssiTemp > (((FMR_RSSI_THRESHOLD_LANT-65536)*6/16)+113))
			if(RSSI > (((FMR_RSSI_THRESHOLD_LANT - 65536) * 6 / 16) + 123)) //113
			{
				RssiTemp = MT6626_GetCurPamd();
				//trace(TRACE_INFO, FM_INFO_FM_PAMD_LVL, _current_frequency, RSSIValue);
				PAMD = FMR_CQI_TH&0x1FF;
				PAMD = (PAMD >= 256) ? ((512 - PAMD) * 6 / 16) : 0;
				//trace(TRACE_INFO, FM_INFO_FM_PAMD_LVL, _current_frequency, PAMD);
				if(RssiTemp >= PAMD)
				{
					EmptyChannel = TRUE;
				}
			}
	}
	//ramp down
	MT6626_RampDown();
	//dsiable mute
	//FMDrv_Mute(0);
	return EmptyChannel;
}


bool FMDrv_GetTuneResult(uint16 rFreq)
{
	uint16 RSSI, PAMD = 0;;
	RSSI = FMDrv_GetSigLvl(rFreq);
	return FMDrv_Seek_Eliminate(rFreq, RSSI);
}

void FMDrv_SCAN_Sort_Tune_Internal(uint16 freq, uint16* p_RSSI)
{
	uint32 CHAN = 0x0000;
	uint16 dataRead, RSSIValue = 0, tempbuff = 0;
	uint8 cnt = 0, GetTime = 5; //GetTime = 4
	 
    //ramp down
    MT6626_RampDown();
    CHAN = (freq - 640) << 1;
    MT6626_ReadByte(FM_CHANNEL_SET, &dataRead);
    if(Chip_Version == E1)
    {
        if(((uint8)((dataRead & 0x1000)>>12)) ^ (channel_parameter[freq - 760] & 0x1))
        {
            MT6626_ReadByte(0x61, &tempbuff);
    		MT6626_WriteByte(0x60, 0x330F);
    		MT6626_WriteByte(0x61, 1); 
            MT6626_WriteByte(0x6e, 0x0);		
            MT6626_WriteByte(0x6e, 0x0);		
            MT6626_WriteByte(0x6e, 0x0);		
            MT6626_WriteByte(0x6e, 0x0);		
            MT6626_WriteByte(0x60, 0x0);		
            MT6626_WriteByte(0x60, 0x4000);
            MT6626_WriteByte(0x60, 0x0);		
            MT6626_WriteByte(0x60, 0x3000);		
            MT6626_WriteByte(0x60, 0x3001);		
            Delayms(3);
            MT6626_WriteByte(0x60, 0x3003);		
            MT6626_WriteByte(0x60, 0x3007);		
            MT6626_WriteByte(0x60, 0x300f);
            MT6626_WriteByte(0x61, tempbuff | 0x3);		
            MT6626_WriteByte(0x61, tempbuff | 0x2);		
			MT6626_WriteByte(0x6A, 0x20);
			MT6626_WriteByte(0x6B, 0x20);
            Delayms(200);	
        }
    }
    MT6626_writeFA(&dataRead, (channel_parameter[freq - 760]));
    MT6626_WriteByte(FM_CHANNEL_SET, (dataRead&0xFC00)|CHAN);
    
    MT6626_TUNE_ON();	
		
	do 
    { 
		#if 0
		if(pstFMR_data->bHWsearchStop)/*if search abort, cancel polling*/
		{
			return;
		}
		#endif
        MT6626_ReadByte(FM_MAIN_INTR, &dataRead);        
        if((dataRead & FM_INTR_STC_DONE) == 0)
        {
            Delayms(3);
        }        
    } while((dataRead & FM_INTR_STC_DONE) == 0);
	
		
	MT6626_WriteByte(FM_MAIN_INTR, dataRead | FM_INTR_STC_DONE);//clear status flag

   	for(cnt = 0; cnt < GetTime; cnt++)
   	{
   	    RSSIValue += FMDrv_GetSigLvl(freq);
        Delayus(2250);
   	}
    *p_RSSI = RSSIValue / GetTime;
	//trace(TRACE_INFO, FM_INFO_SortTuneRssi, freq, RSSIValue / GetTime);
    
    return;
}
/**APIs for EM**/
uint32 FMDrv_GetRSSITH(uint8 type)/*0: long ant. 1: short ant.*/
{
	uint32 RSSI_TH;
	if(type == 0)
	{
		RSSI_TH = FMR_RSSI_THRESHOLD_LANT;
	}
	else
	{
		RSSI_TH = FMR_RSSI_THRESHOLD_SANT;
	}
	return RSSI_TH;
}
void FMDrv_SetRSSITH(uint8 type, uint32 value)/*0: long ant. 1: short ant.*/
{
	/*if(type == 0)
	{
		FMR_RSSI_THRESHOLD_LANT = value;
	}
	else
	{
		FMR_RSSI_THRESHOLD_SANT = value;
	}*/
}
int32 FMDrv_RSSI_Hex2Db(uint32 Hex)
{
	int32 dBValue;
	
	dBValue = Hex & 0x3FF;
	dBValue = (int32)((dBValue > 511) ? (((float)dBValue - 1024) * 6 / 16 + 113) : ((float)dBValue * 6 / 16 + 113));
    
    return dBValue;
}
uint32 FMDrv_RSSI_Db2Hex(int32 Db)
{
	int32 dBValue;
	uint32 HexValue;
	dBValue = (Db - 113) * 16 / 6;
	HexValue = (uint32)((dBValue < 0) ? (dBValue + 0x10000) : (dBValue + 0xFC00));
	return HexValue;
}
void FMDrv_Force_Mono(void)
{
	uint16 RegTemp;
	
	MT6626_ReadByte(0x60, &RegTemp);
	MT6626_WriteByte(0x60, RegTemp & (~0x0008));/*change to SW control*/
	
	MT6626_ReadByte(0x75, &RegTemp);
	MT6626_WriteByte(0x75, RegTemp | 0x0008);/*force MONO*/
	
	MT6626_ReadByte(0x60, &RegTemp);
	MT6626_WriteByte(0x60, RegTemp | 0x0008);/*change to DSp control*/
}
void FMDrv_Force_Stereo(void)
{
	uint16 RegTemp;

	MT6626_ReadByte(0x60, &RegTemp);
	MT6626_WriteByte(0x60, RegTemp & (~0x0008));/*change to SW control*/
	
	MT6626_ReadByte(0x75, &RegTemp);
	MT6626_WriteByte(0x75, RegTemp & (~0x0008));/*auto modulate*/
	
	MT6626_ReadByte(0x60, &RegTemp);
	MT6626_WriteByte(0x60, RegTemp | 0x0008);/*change to DSp control*/
	
	#if 1 //temp test 
	FMDrv_SetVolumeLevel(0x20);
	#endif
}


void MT6626FMStereo(bool StereoFlag)
{
	 if(StereoFlag) //stereo
	 {
		 FMDrv_Force_Stereo();
	 }
	 else
	 {
		 FMDrv_Force_Mono();
	 }
	
}

void FMDrv_RDS_Disable(void)
{
	uint16 dataRead;

	//kal_char debug_buff[50];
	//sprintf(debug_buff, "FMDrv_RDS_Disable");
	//kal_print((kal_char*)debug_buff);

	MT6626_ReadByte(FM_MAIN_CTRL, &dataRead);
	MT6626_WriteByte(FM_MAIN_CTRL, (dataRead & 0xFFEF));
}

/*
void mtk_fm_test(uint8 channel)
{
    switch(channel)
    {
    case '1':
        FMDrv_SetFreq(899);
        break;

    case '2':
        FMDrv_SetFreq(947);
        break;

    case '3':
        FMDrv_SetFreq(879);
        break;
    case '4':
        FMDrv_SetFreq(990);
        break;
    case '5':
        FMDrv_SetFreq(1017);
        break;
    case '6':
        FMDrv_SetFreq(1072);
        break;
    case '7':
        FMDrv_SetFreq(1077);
        break;

    default:
        FMDrv_SetFreq(899);
        break;
    }
		FMDrv_Force_Stereo();
    FMDrv_SetVolumeLevel(0x20);
    Delayms(1000);
}
*/


#endif //#ifdef RADIO_MT6626_EN


