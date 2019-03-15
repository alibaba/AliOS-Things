/************************************************Copyright(c)***********************************
**                                   Quintic(Nanjing) Microelectronics Co,Ltd.
**
**                                   http://www.quinticcorp.com
**
**--------------------File Info----------------------------------------------------------------
** File Name:                  qndriver.h
** subversion number:   160
** maintainer: Sam
**----------------------------------------------------------------------------------------
************************************************************************************************/

#ifndef  __QN8035_H__
#define  __QN8035_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#define FREQ2CHREG(freq)   ((freq << 1) - 1200)//((freq-6000)/5)//((freq << 1) - 1235)//((freq-6175)/5)
#define CHREG2FREQ(ch)     ((ch + 1200) >> 1)//(ch*5+6000)//((ch + 1235) >> 1)//(ch*5+6175)
// only used for engineering normally!
#define _QNCOMMON_H_
//=====================================
#define QND_SINE_WAVE_CLOCK             0x00    //inject sine-wave clock  
#define QND_DIGITAL_CLOCK               0x80

#define PILOT_CCA                       1  // 0: disable stereo judgement for the country has many mono FM stations
#define QND_STEP_CONSTANT               0x40

#define CH_SETUP_DELAY_TIME             50


//=====================================
/* modify this according to I2C device address when you use standard I2C function like I2C_XXXX except AI2C_XXXX*/

/************end*********************/
#define QND_80xxAPI	__declspec(dllimport)

#define QND_MODE_SLEEP      0
#define QND_MODE_WAKEUP     1
#define QND_MODE_DEFAULT    2
// RX / TX value is using upper 8 bit

#define QND_MODE_RX         0x8000
#define QND_MODE_TX         0x4000
// AM / FM value is using lower 8 bit
// need to check datasheet to get right bit
#define QND_MODE_FM         0x0000
#define QND_MODE_AM         0x40

#define BAND_FM		0
#define BAND_AM		1
// tune
#define QND_FSTEP_50KHZ      0
#define QND_FSTEP_100KHZ      1
#define QND_FSTEP_200KHZ      2
// output format
#define QND_OUTPUT_ANALOG     0
#define QND_OUTPUT_IIS        1

// stereo mode
#define QND_TX_AUDIO_MONO              0x10
#define QND_TX_AUDIO_STEREO            0x00

#define QND_RX_AUDIO_MONO              0x20
#define QND_RX_AUDIO_STEREO            0x00

#define  QND_CONFIG_MONO               0x01
#define  QND_CONFIG_MUTE			   0x02
#define  QND_CONFIG_SOFTCLIP		   0x03
#define  QND_CONFIG_AUTOAGC			   0x04
#define  QND_CONFIG_AGCGAIN			   0x05

#define  QND_CONFIG_EQUALIZER		   0x06
#define  QND_CONFIG_VOLUME			   0x07
#define	 QND_CONFIG_BASS_QUALITY       0x08
#define  QND_CONFIG_BASS_FREQ		   0x09
#define  QND_CONFIG_BASS_GAIN		   0x0a
#define  QND_CONFIG_MID_QUALITY        0x0b
#define  QND_CONFIG_MID_FREQ           0x0c
#define  QND_CONFIG_MID_GAIN           0x0d
#define  QND_CONFIG_TREBLE_FREQ        0x0e
#define  QND_CONFIG_TREBLE_GAIN        0x0f

#define  QND_ENABLE_EQUALIZER          0x10
#define  QND_DISABLE_EQUALIZER         0x00


#define  QND_CONFIG_AUDIOPEAK_DEV      0x11
#define  QND_CONFIG_PILOT_DEV          0x12
#define  QND_CONFIG_RDS_DEV            0x13
#define  RDSCOSTAS		0x1E
// input format
#define QND_INPUT_ANALOG     0
#define QND_INPUT_IIS        1

// i2s mode

#define QND_EQUALIZE_BASS	0x00
#define QND_EQUALIZE_MID	0x01
#define QND_EQUALIZE_TREBLE 0x02
// RDS, TMC
#define QND_EUROPE_FLEXIBILITY_DISABLE  0
#define QND_EUROPE_FLEXIBILITY_ENABLE   1
#define QND_RDS_OFF              0
#define QND_RDS_ON               1
#define QND_RDS_BUFFER_NOT_READY 0
#define QND_RDS_BUFFER_READY     1


#define CHIPID_QN8000	0x00
#define CHIPID_QN8005	0x20
#define CHIPID_QN8005B1 0x21
#define CHIPID_QN8006	0x30
#define CHIPID_QN8007B1 0x11
#define CHIPID_QN8007	0x10
#define CHIPID_QN8006A1 0x30
#define CHIPID_QN8006B1 0x31
#define CHIPID_QN8016   0xe0
#define CHIPID_QN8016_1 0xb0
#define CHIPID_QN8015   0xa0
#define CHIPID_QN8065   0xa0
#define CHIPID_QN8067   0xd0
#define CHIPID_QN8065N  0xa0
#define CHIPID_QN8027   0x40
#define CHIPID_QN8025   0x80

//For antena impedance match
#define QND_HIGH_IMPEDANCE 		1
#define QND_LOW_IMPEDANCE 		0


#define CCA_PILOT_READ_DELAY            60
#define CCA_PILOT_SNR_FILTER            25
#define CCA_PILOT_READ_COUNT_DELAY      2
#define CCA_PILOT_READ_COUNT            10
#define CCA_PILOT_TH                    8

#define _QNCONFIG_H_

/**********************************************************************************************
// flag configuration
***********************************************************************************************/
/************************MCU platform selection*****************/
// Target compiler: 8051 (Keil or others)
#define QND_C51

/***************************** CHIP ID ******************************/

/********************* country selection**************/
#define COUNTRY_CHINA			0
#define COUNTRY_USA				1
#define COUNTRY_JAPAN			2

#define 	QND_FMSTEP_100KHZ	0
#define 	QND_FMSTEP_200KHZ	1
#define 	QND_FMSTEP_50KHZ	2
/************************EDN******************************/

/********************* minor feature selection*********************/

#define SMSTART_VAL                     12
#define HCCSTART_VAL                    18
#define SNCSTART_VAL                    51

#define QND_BAND_NUM     6
#define RXCCA_MASK                      0x03
#define RX_CCA                          0x02
#define STATUS1         				0x04
#define RXCCA_FAIL                      0x08

enum
{
    // MSB (8bits) of the word: RSSI Threshold
    // LSB (8bits) of the word: SNR Threshold
    // eg: 0x1E06 => RSSI_TH = 0x1E, SNR_TH = 0x06
    // notice: the rang of RSSI is 0x0A ~ 0x3F
    // notice: the rang of SNR is 0x00 ~ 0x3F

    CCA_SENSITIVITY_LEVEL_0 = 0x1E06,
    CCA_SENSITIVITY_LEVEL_1 = 0x1E07,   //if using the pilot as CCA,reference this item.
    CCA_SENSITIVITY_LEVEL_2 = 0x1E08,
    CCA_SENSITIVITY_LEVEL_3 = 0x1E09,
    CCA_SENSITIVITY_LEVEL_4 = 0x1E0A,   //if not using the pilot as CCA,reference this item.
    CCA_SENSITIVITY_LEVEL_5 = 0x1E0B,
    CCA_SENSITIVITY_LEVEL_6 = 0x1E0C,
    CCA_SENSITIVITY_LEVEL_7 = 0x1E0D,
    CCA_SENSITIVITY_LEVEL_8 = 0x1E0E,
    CCA_SENSITIVITY_LEVEL_9 = 0x1E0F
};

#define QND_DIGITAL_CLOCK               0x80    //inject digital clock,default is inject digital clock

/**********************************************************************************************
// limitation configuration
***********************************************************************************************/

#define QND_READ_RSSI_DELAY    10
// auto scan
#define QND_MP_THRESHOLD       0x28

#define _QNREG_H_
#define	XTAL_DIV0						0x15
#define	XTAL_DIV1						0x16
#define	XTAL_DIV2						0x17

#define GAIN_SEL						0x1B
#define CCA1							0x27
#define SYSTEM_CTL2						0x1D
#define PLT1                            0x2F
#define	PLT2                            0x30
#define	CCA_CNT1					    0x37
#define	CCA_CNT2					    0x38
#define	CCA_SNR_TH_1					0x39
#define	CCA_SNR_TH_2					0x3A


#define REG_REF                         0x49

#define SYSTEM1		0x00
#define SNR            	0x02
#define STATUS2		0x13
#define RDSD0		0x0b
#define CH			0x07
#define CH_STEP		0x0a
#define CCOND1		0x33     //?
//#define SMSTART		0x3b
//#define SNCSTART	0x3c
//#define HCCSTART	0x3d

#define	SMSTART							0x34
#define	SNCSTART						0x35
#define	HCCSTART						0x36

#define	CCA			0x01
//  ?? #define CCA2		0x2f
#define RSSISIG		0x03
#define CH_START	0x08
#define CH_STOP		0x09
//? #define CCA4		0x30
//? #define CCA5		0x31
//?#define RSSIMP		0x21
///#define SYSTEM2		0x00	    ???
#define CID2		0x06
//? #define RDSEN		  0x08
//#define TXREQ		  0x20
//??#define MUTE		  0x03
//#define UNMUTE		  0x00
#define STNBY		  0x20
#define IMR			  0x40
#define CHSC		  0x02
#define CCA_CH_DIS	  0x01
#define CH_CH         0x03
#define CH_CH_STOP    0x30
#define CH_CH_START   0x0c
#define RX_MONO_MASK  0x04
#define RDS_RXUPD     0x80
#define RDSSYNC       0x10
#define ST_MO_RX                        0x01
#define _QNSYS_H_
// external driver interface
// logical layer
/*****************************************************************************
Driver API Macro Definition
*****************************************************************************/
#define QNM_SetRxThreshold(db) \
        QN8025WriteReg((uint16_t)CCA, db)

/*
  System General Control
*/
//extern void WaitMs(uint16_t ms);
//extern uint8_t QND_WriteReg(uint8_t adr, uint8_t value);

//#define QN8025_MUTE				0x80
//#define QN8025_TC75US				0x40

#define QN8035_DEF_FREQ          		875

bool QN8025WriteReg(uint16_t addr, uint8_t dat);
uint8_t QNDReadReg(uint8_t regaddr);
void QNFSetRegBit(uint8_t regaddr, uint8_t bitMask, uint8_t dataval);
uint8_t QNFSetCh(uint16_t freq);
//void QNFRXInit(void);
//void QNFConfigScan(uint16_t start, uint16_t stop, uint8_t step);
void QN8035VolumeSet(void);
void QN8035Mute(bool MUTEFLAG);
void QNChipInitialization(void);
void QN8035SetFreq(uint16_t Frep);
bool QN8035Init(void);
bool QN8035PowerOn(void);
bool QN8035PowerDown(void);
//void QNDSetSysMode(uint16_t mode);
void QN8035RXSetTH(void);
void QN8035SearchSet(uint16_t freq);
bool QN8035SearchRead(uint16_t Freq);
bool QN8035ReadID(void);
void QN8035VolSet(uint8_t vol);
void QN8035OutputHIZ(bool HIZOnOff);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif

