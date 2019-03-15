/******************************************************************************
 * @file    get_bootup_info.c
 * @author  Sean
 * @version V1.0.0
 * @date    16-Jan-2014
 * @maintainer lujiangang
 * @brief   get information from boot
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */
#include	"wakeup.h"
#include	"clk.h"
#include	"spi_flash.h"

#define OTP_31K_BASE_ADDR 0x7C0000
#define APP_FUN_ADDR      0x7F70
#define OTP_VERSION_ADDR  0x0400


typedef struct _APP_FUN_SEL
{
	__IO uint32_t	Reserved: 16;
	__IO uint32_t	RemapType: 4;
	__IO uint32_t	UpdateDev: 6;
	__IO uint32_t	ExCrystal: 5;
	__IO uint32_t	PorUpdataDis: 1;

} APP_FUN_SEL;
APP_FUN_SEL AppFun __ATTRIBUTE__(AT(OTP_31K_BASE_ADDR + APP_FUN_ADDR));

typedef struct _OTP_VERSION_SEL
{
	__IO uint32_t	ChipType: 8;
	__IO uint32_t	SdkV: 8;
	__IO uint32_t	PatchV: 8;
	__IO uint32_t	BootV: 8;

} OTP_VERSION_SEL;
OTP_VERSION_SEL OtpVersion __ATTRIBUTE__(AT(OTP_31K_BASE_ADDR + OTP_VERSION_ADDR));

/**
 * @brief  Get POR update disable state.0:enable,1:disable
 * @param  None
 * @return POR update disable state
 */
bool PorUpdateDisable(void)
{
	return AppFun.PorUpdataDis;
}

/**
 * @brief  External Crystal configure.0:32.768k,1~30:1M~30M
 * @param  None
 * @return External Crystal configure
 */
uint8_t ExCrystalConfig(void)
{
	return AppFun.ExCrystal;
}

/**
 * @brief  Update device configure
 * Bit[5:4]U-disk.configure PHY1/2.
 * 00:no U-disk
 * 01:Phy1
 * 10:Phy2
 * 11:Phy1 Phy2
 * default:10

 * Bit[3:0]SD.configure SD0/SD1/SD2/SD3.
 * 0000:no SD
 * 0001:SD0
 * 0010:SD1
 * ...
 * 1111:SD0 SD1 SD2 SD3
 * default:0001
 * use the suitable configure

 * @param  None
 * @return Update device configure
 */
uint8_t UpdateDevConfig(void)
{
	return AppFun.UpdateDev;
}

/**
 * @brief  Get remap tpye.0:OTP only,1:OTP+FLASH,2:OTP+SDRAM,3:FLASH ONLY
 * @param  None
 * @return Remap type
 */
uint8_t GetRemapType(void)
{
	return AppFun.RemapType;
}

/**
 * @brief  Get chip type.0x12 discribes O18.
 * @param  None
 * @return Chip type
 */
uint8_t GetChipType(void)
{
	return OtpVersion.ChipType;
}



/**
 * @brief  Get patch version
 * @param  None
 * @return Patch version
 */
uint8_t GetPatchVersion(void)
{
	return OtpVersion.PatchV;
}

/**
 * @brief  Get Boot version
 * @param  None
 * @return Boot version
 */
uint8_t GetBootVersion(void)
{
	return OtpVersion.BootV;
}

/**
 * @brief  Get Boot extend version
 * @param  None
 * @return Boot extend version
 */
uint8_t GetBootVersionEx(void)
{		
	int ExVer = OtpVersion.SdkV & 0xDF; //to upper
	return (!ExVer) ? 0x20 : ExVer;
}

/**
 * @brief  Get Sdk version
 * @param  None
 * @return Sdk version
 */
uint32_t GetSdkVer(void)
{
	uint32_t SdkVer;
	SpiFlashRead(0x94, (uint8_t*)&SdkVer, 4);
	return SdkVer;
}

/**
 * @brief  Get wakeup source name
 * @param  WakeupSrc wakeup source index 
 * @return wakeup source name
 */
const char* GetWakeupSrcName(unsigned int WakeupSrc)
{
	/*
	 * ignore the combination reset source.for example,POR+SYSRESET signal when debugging reset 
	 * with	system reset
	 */
	WakeupSrc = __RBIT(WakeupSrc);
	WakeupSrc = __CLZ(WakeupSrc);
	WakeupSrc = 1 << WakeupSrc;
	switch(WakeupSrc)
	{
		case	WAKEUP_FLAG_POR_LDOIN:
				return "POR";
		case	WAKEUP_FLAG_WDG:
				return "WDG";
		case 	WAKEUP_FLAG_RTC:
			    return "RTC_WAKEUP";
		case 	WAKEUP_FLAG_POWERKEY:
			    return "POWER_KEY";
		case 	WAKEUP_FLAG_SYSRST:
			    return "SYS_REST";
		case 	WAKEUP_FLAG_RSTPIN:
			    return "REST_PIN";
		case 	WAKEUP_FLAG_GPIOA10:
			    return "GPIO_A10";
		case 	WAKEUP_FLAG_GPIOB5:
			    return "GPIO_B5";
		case 	WAKEUP_FLAG_GPIOB6:
			    return "GPIO_B6";
		case 	WAKEUP_FLAG_GPIOB7:
			    return "GPIO_B7";
		case 	WAKEUP_FLAG_GPIOB22:
			    return "GPIO_B22";
		case 	WAKEUP_FLAG_GPIOB23:
			    return "GPIO_B23";
		case 	WAKEUP_FLAG_GPIOB24:
			    return "GPIO_B24";
		case 	WAKEUP_FLAG_GPIOB25:
			    return "GPIO_B25";
		case 	WAKEUP_FLAG_GPIOC2:
			    return "GPIO_C2";
		case 	WAKEUP_FLAG_GPIOC7:
			    return "GPIO_C7";
		case 	WAKEUP_FLAG_GPIOC8:
			    return "GPIO_C8";
		case 	WAKEUP_FLAG_GPIOC9:
			    return "GPIO_C9";
		case 	WAKEUP_FLAG_POR_RTC:
			    return "RTC_PWON";
		case 	WAKEUP_FLAG_POR_POWERKEY:
			    return "POR_PWK";
		case 	WAKEUP_FLAG_POR_8SRST:
				return "POR_8SRST";
		default:
				return "UNKNOWN";
	}
}

/**
 * @brief  get code size
 * @param  None 
 * @return code size in byte version
 */
long GetCodeSize(void)
{
	long Size;
	SpiFlashRead(0x80, (uint8_t*)&Size, 4);
       return Size;
}

/**
 * @brief  get code encryption information
 * @param  None 
 * @return code engryption information printf
 */
const char* GetCodeEncryptedInfo(void)
{
	char Encrypted;
	SpiFlashRead(0xFF, (unsigned char*)&Encrypted, 1);
    return 0x55 == Encrypted ? "Yes" : (0xFF == Encrypted ? "No" : "Err");
}


/**
 * @brief  Get current spi flash clock frequency
 * @param  None
 * @return spi clk frequency
 */
uint32_t ClkGetFshcClkFreq(void)
{
	uint8_t	fshc_clk_sel;
	uint32_t	FshcClkFreq;
	
	fshc_clk_sel = ClkFshcClkSelGet();
	switch(fshc_clk_sel)
	{
		case 0: //pll 60M
			FshcClkFreq = 60000000;
			break;
		
		case 1: //sys clk
			FshcClkFreq = ClkGetCurrentSysClkFreq();
			break;
		
		case 2: //pll 80M
			FshcClkFreq = 80000000;
			break;
		
		case 3: //pll 48M
			FshcClkFreq = 48000000;
			break;
		
		default:
			FshcClkFreq = 0xffffffff;
			break;
	}

	return FshcClkFreq;		
}

/**
 * @brief  Set external 32KHz oscillator calibration
 * @param  None
 * @return None
 */
void Osc32kExtCapCalibrate(void)
{
    unsigned short Val;
    
    SpiFlashRead(0xA0, (unsigned char*)&Val, 2);
    ClkOsc32kNoNeedExtCapacitanceSet((char)Val, (char)(Val >> 8));    
}


