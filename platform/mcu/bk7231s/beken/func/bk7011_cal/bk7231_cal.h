#ifndef _BK7231_CAL_H_
#define _BK7231_CAL_H_

#if (CFG_SOC_NAME == SOC_BK7231)

#include "uart_pub.h"

#define CAL_DEBUG          0

#if CAL_DEBUG
#define CAL_PRT       null_prf
#define CAL_WARN      null_prf
#define CAL_FATAL     fatal_prf
#define CAL_TIM_PRT   os_printf
#define CAL_FLASH_PRT os_printf
#else
#define CAL_PRT       null_prf
#define CAL_WARN      null_prf
#define CAL_FATAL     null_prf
#define CAL_TIM_PRT   null_prf
#define CAL_FLASH_PRT null_prf
#endif

#define REGTRXV2A
#define BK7011_VER_A

//#define _11MBPS_MAX_POWER

void delay05us(INT32 num);

#define trx_reg_is_write(st_trxreg)     while(BK7011RCBEKEN.REG0x1->value & st_trxreg) 	{cpu_delay(1);}

#define DETECT_LOOPCNT		        10
#define GOLD_OUTPUT_POWER		    56
#define UNSIGNEDOFFSET10		    512
#define I_Q_CAP_DIF					32
#define CONSTANT_RCIQ				117
#define SUMNUMBERS					4
#define MINOFFSET			    	16

#define cpu_delay(val)            delay(MAX(1, val/100))
#define DELAY1US				  100

#define cal_delay(val)            delay05us(MAX(1, val))	// 8us
#define CAL_DELAY05US			  2		// 20170503 2:1.5us
#define CAL_TX_NUM                50
#define CAL_RX_NUM                5

#define cal_delay_100us(val)      delay100us(MAX(1, val))	// 200us
#define CAL_DELAY100US			  1  //20160804  1:100us 2:200us		// 20170503 1:150us 2:300us

#define st_TRXREG00			(1<<0)
#define st_TRXREG01			(1<<1)
#define st_TRXREG02			(1<<2)
#define st_TRXREG03			(1<<3)
#define st_TRXREG04			(1<<4)
#define st_TRXREG05			(1<<5)
#define st_TRXREG06			(1<<6)
#define st_TRXREG07			(1<<7)
#define st_TRXREG08			(1<<8)
#define st_TRXREG09			(1<<9)
#define st_TRXREG0A			(1<<10)
#define st_TRXREG0B			(1<<11)
#define st_TRXREG0C			(1<<12)
#define st_TRXREG0D			(1<<13)
#define st_TRXREG0E			(1<<14)
#define st_TRXREG0F			(1<<15)
#define st_TRXREG10			(1<<16)
#define st_TRXREG11			(1<<17)
#define st_TRXREG12			(1<<18)
#define st_TRXREG13			(1<<19)
#define st_TRXREG14			(1<<20)
#define st_TRXREG15			(1<<21)
#define st_TRXREG16			(1<<22)
#define st_TRXREG17			(1<<23)
#define st_TRXREG18			(1<<24)
#define st_TRXREG19			(1<<25)
#define st_TRXREG1A			(1<<26)
#define st_TRXREG1B			(1<<27)

#define abs(a)                ((a) < 0 ?(-1*(a)):(a))

#ifndef __BK7011RCBEKEN_H__
#define __BK7011RCBEKEN_H__

#define RC_BEKEN_BASE		0x01050000

/// REG0x0
typedef union
{
    struct
    {
        volatile unsigned int ch0en        : 1;  /**< Enable BK7011 (1: Enable) */
        volatile unsigned int Reserved     : 2;  /**< NC */
        volatile unsigned int rcen         : 1;  /**< Enable BK7011 (1: Enable) */
        volatile unsigned int Reserved_    : 4;  /**< NC */
        volatile unsigned int ch0ld        : 1;  /**< BK7011 LD输出值（是否锁定） */
        volatile unsigned int Reserved__   : 3;  /**< NC */
        volatile unsigned int ch0shdnstat  : 1;  /**< BK7011是否处于shutdown状态 */
        volatile unsigned int Reserved___  : 3;  /**< NC */
        volatile unsigned int rcstate      : 3;  /**< 当前RC的状态;0x00=SPI_RESET ;0x01=SHUTDOWN ;0x02=WAIT_SPI (SPI正在操作);0x03=WAIT_LOCK（RFPLL正在Lock）;0x04=ACTIVE */
        volatile unsigned int Reserved____ : 11; /**< NC */
        volatile unsigned int spireset     : 1;  /**< Reset BK7011的SPI寄存器 */
        volatile unsigned int forceenable  : 1;  /**< 强制控制BK7011的接口信号 */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x0_TypeDef;

/// REG0x1
typedef union
{
    struct
    {
        volatile unsigned int trxregstat : 28; /**< TRx Register Stat.;0x0: register is idle;0x1: register is updating. Can not write register again */
        volatile unsigned int prescaler  : 4;  /**< SPI时钟频率控制=RC_Clock (80 MHz)/2/PRESCALE */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x1_TypeDef;

/// REG0x5
typedef union
{
    struct
    {
        volatile unsigned int ch0outpower : 8;  /**< No description */
        volatile unsigned int Reserved    : 24; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x5_TypeDef;

/// REG0x8
typedef union
{
    struct
    {
        volatile unsigned int CH0RXONDELAY  : 8; /**< No description */
        volatile unsigned int Reserved      : 8; /**< NC */
        volatile unsigned int CH0RXOFFDELAY : 8; /**< No description */
        volatile unsigned int Reserved_     : 8; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x8_TypeDef;

/// REG0xB
typedef union
{
    struct
    {
        volatile unsigned int CH0TXONDELAY  : 8; /**< No description */
        volatile unsigned int Reserved      : 8; /**< NC */
        volatile unsigned int CH0TXOFFDELAY : 8; /**< No description */
        volatile unsigned int Reserved_     : 8; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0xB_TypeDef;

/// REG0xE
typedef union
{
    struct
    {
        volatile unsigned int CH0PAONDELAY  : 8; /**< No description */
        volatile unsigned int Reserved      : 8; /**< NC */
        volatile unsigned int CH0PAOFFDELAY : 8; /**< No description */
        volatile unsigned int Reserved_     : 8; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0xE_TypeDef;

/// REG0x11
typedef union
{
    struct
    {
        volatile unsigned int CH0SHDNONDELAY  : 8; /**< No description */
        volatile unsigned int Reserved        : 8; /**< NC */
        volatile unsigned int CH0SHDNOFFDELAY : 8; /**< No description */
        volatile unsigned int Reserved_       : 8; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x11_TypeDef;

/// REG0x19
typedef union
{
    struct
    {
        volatile unsigned int FCH0SHDN  : 1;  /**< 强制模式下的SHDN */
        volatile unsigned int FCH0RXEN  : 1;  /**< 强制模式下的RXEN */
        volatile unsigned int FCH0TXEN  : 1;  /**< 强制模式下的TXEN */
        volatile unsigned int FCH0RXHP  : 1;  /**< 强制模式下的RXHP */
        volatile unsigned int Reserved  : 4;  /**< NC */
        volatile unsigned int FCH0B     : 8;  /**< 强制模式下增益控制 */
        volatile unsigned int FCH0EN    : 1;  /**< 强制控制RF接口信号 */
        volatile unsigned int Reserved_ : 15; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x19_TypeDef;

/// REG0x1C
typedef union
{
    struct
    {
        volatile unsigned int FRXON    : 1;  /**< 强制模式下RX ON */
        volatile unsigned int FTXON    : 1;  /**< 强制模式下TX ON */
        volatile unsigned int Reserved : 30; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x1C_TypeDef;

/// REG0x1E
typedef union
{
    struct
    {
        volatile unsigned int FERXONDEL : 12; /**< No description */
        volatile unsigned int Reserved  : 20; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x1E_TypeDef;

/// REG0x3C
typedef union
{
    struct
    {
        volatile unsigned int RXAVGQRD    : 12; /**< Q路计算结果 */
        volatile unsigned int RXAVGIRD    : 12; /**< I路计算结果 */
        volatile unsigned int Reserved    : 4;  /**< NC */
        volatile unsigned int RXHPFBYPASS : 1;  /**< RX HPF bypass */
        volatile unsigned int RXIQSWAP    : 1;  /**< RX IQ SWAP */
        volatile unsigned int RXAVGMODE   : 1;  /**< 0：求取信号的平均值;1：求取信号绝对值得平均值 */
        volatile unsigned int RXDCCALEN   : 1;  /**< 使能RX DC计算(1) */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x3C_TypeDef;

/// REG0x3E
typedef union
{
    struct
    {
        volatile unsigned int RXDCQRD  : 12; /**< Q路DC计算结果 */
        volatile unsigned int RXDCIRD  : 12; /**< I路DC计算结果 */
        volatile unsigned int Reserved : 6;  /**< NC */
        volatile unsigned int RXCOMPEN : 1;  /**< 使能Mismatch补偿(1) */
        volatile unsigned int RXCALEN  : 1;  /**< 使能Mismatch计算(1) */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x3E_TypeDef;

/// REG0x3F
typedef union
{
    struct
    {
        volatile unsigned int RXPHASEERRRD : 10; /**< 估计出来的相位误差，phase_err_est*2^9，有符号 */
        volatile unsigned int Reserved     : 6;  /**< NC */
        volatile unsigned int RXAMPERRRD   : 10; /**< 估计出来的幅度误差，amp_err_est*2^9，有符号 */
        volatile unsigned int Reserved_    : 6;  /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x3F_TypeDef;

/// REG0x40
typedef union
{
    struct
    {
        volatile unsigned int RXTY2RD  : 10; /**< 估计出来的TY2，(ty2-0.5)*2^10，有符号 */
        volatile unsigned int Reserved : 22; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x40_TypeDef;

/// REG0x41
typedef union
{
    struct
    {
        volatile unsigned int RXDCQWR  : 12; /**< 补偿时使用的Q路DC */
        volatile unsigned int RXDCIWR  : 12; /**< 补偿时使用的I路DC */
        volatile unsigned int Reserved : 8;  /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x41_TypeDef;

/// REG0x42
typedef union
{
    struct
    {
        volatile unsigned int RXPHASEERRWR : 10; /**< 补偿时使用的phase_err*ty2*2^9 */
        volatile unsigned int Reserved     : 6;  /**< NC */
        volatile unsigned int RXAMPERRWR   : 10; /**< 补偿时使用的ty2/(1+amp_err)*2^9 */
        volatile unsigned int Reserved_    : 6;  /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x42_TypeDef;

/// REG0x4C
typedef union
{
    struct
    {
        volatile unsigned int QCONSTANT   : 10; /**< 当发射常数时，Q的值 */
        volatile unsigned int Reserved    : 6;  /**< NC */
        volatile unsigned int ICONSTANT   : 10; /**< 当发射常数时，I的值 */
        volatile unsigned int DPDCOMPEN   : 1;  /**< DPD Compensation Enable */
        volatile unsigned int DPDCALEN    : 1;  /**< DPD Calculation Enable */
        volatile unsigned int DPDEN       : 1;  /**< DPD Enable] */
        volatile unsigned int TXCOMPDIS   : 1;  /**< 发送补偿功能禁止：;0x0: 使能发送补偿;0x1: 禁止发送补偿 */
        volatile unsigned int TESTPATTERN : 2;  /**< 0：正常模式，发射正常来自Modem的数据 */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x4C_TypeDef;

/// REG0x4D
typedef union
{
    struct
    {
        volatile unsigned int Reserved  : 12; /**< NC */
        volatile unsigned int TXSINAMP  : 4;  /**< 正弦波幅度调整系数。小数表示，x.yyy。范围是[0 15/8] */
        volatile unsigned int TXSINMODE : 2;  /**< 0：I/Q路发射复数正弦波;1：仅仅I路发射，Q路发射0;2：仅仅Q路发射，I路发射0;3：保留 */
        volatile unsigned int Reserved_ : 4;  /**< NC */
        volatile unsigned int TXSINF    : 10; /**< 发射正弦波频率;TX_SIN_F=2*pi*F/80e6*2^8，F是发射频率 */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x4D_TypeDef;

/// REG0x4F
typedef union
{
    struct
    {
        volatile unsigned int TXQDCCOMP : 10; /**< Q DC offset，[-512 511] */
        volatile unsigned int Reserved  : 6;  /**< NC */
        volatile unsigned int TXIDCCOMP : 10; /**< I DC offset，[-512 511] */
        volatile unsigned int Reserved_ : 6;  /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x4F_TypeDef;

/// REG0x50
typedef union
{
    struct
    {
        volatile unsigned int TXQGAINCOMP : 10; /**< 0 ~ 1023/1024，step:1/1024 */
        volatile unsigned int Reserved    : 6;  /**< NC */
        volatile unsigned int TXIGAINCOMP : 10; /**<  0 ~ 1023/1024，step:1/1024 */
        volatile unsigned int Reserved_   : 6;  /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x50_TypeDef;

/// REG0x51
typedef union
{
    struct
    {
        volatile unsigned int TXTY2       : 10; /**< 512/1024 ~ (512+1023)/1024，step:1/1024 */
        volatile unsigned int Reserved    : 6;  /**< NC */
        volatile unsigned int TXPHASECOMP : 10; /**< -512/1024~ 511/1024，step:1/1024 */
        volatile unsigned int Reserved_   : 6;  /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x51_TypeDef;

/// REG0x52
typedef union
{
    struct
    {
        volatile unsigned int Reserved         : 6;  /**< NC */
        volatile unsigned int IQCONSTANTIQCALP : 10; /**< IQ校准，IQ CONSTANT 输入值正值 */
        volatile unsigned int TXPREGAIN        : 5;  /**< IQ信号增益控制;0：1.75 dB；;…. 31：-6dB;; */
        volatile unsigned int IQCONSTANTPOUT   : 10; /**< 功率校准，IQ CONSTANT 输入值 */
        volatile unsigned int TXIQSWAP         : 1;  /**<  IQ Swap */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x52_TypeDef;

/// REG0x54
typedef union
{
    struct
    {
        volatile unsigned int STRXADCIQ  : 1; /**< RX ADC IQ校准 Settling time 0:300us;1:600us */
        volatile unsigned int TSSIPOUTTH : 8; /**< 功率校准，最佳输出功率对应的TSSI门限 */
        volatile unsigned int DSELVA     : 1; /**< TSSI or AGC gain setting for versiong A, output 0: TSSI 1:AGC gain setting */
        volatile unsigned int TXDCN      : 2; /**< TX DC 两分法第一次执行步骤 0:3次，1:4次，2:5次，3:6次 */
        volatile unsigned int STRXADC    : 2; /**< RX ADC Settling time 0:100us;1:200us;2:300us;3:400us */
        volatile unsigned int STSARADC   : 2; /**< SAR ADC Settling time 0:0.5us;1:1us;2:1.5us;3:2us */
        volatile unsigned int TSSIRD     : 8; /**< TSSI Value. Just valid during tssi_cal_en assert */
        volatile unsigned int AGCPGARD   : 4; /**< AGC的PGA结果 */
        volatile unsigned int AGCBUFRD   : 1; /**< AGC的BUF结果 */
        volatile unsigned int AGCLNARD   : 2; /**< AGC的LNA结果 */
        volatile unsigned int AGCMAPMODE : 1; /**< 0：映射方式一;1：映射方式二 */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x54_TypeDef;

/// REG0x55
typedef union
{
    struct
    {
        volatile unsigned int RXSNRNOISE : 9; /**< rx noise for sinar test */
        volatile unsigned int Reserved   : 7; /**< NC */
        volatile unsigned int RXSNRSIG   : 9; /**< rx signal for sinar test */
        volatile unsigned int Reserved_  : 7; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x55_TypeDef;

/// REG0x5C
typedef union
{
    struct
    {
        volatile unsigned int trxspiintval : 10; /**< TRX SPI Transfer Interval. Unit is 12.5ns */
        volatile unsigned int Reserved     : 14; /**< NC */
        volatile unsigned int Reserved_    : 7;  /**< Reserved */
        volatile unsigned int trxbankrpt   : 1;  /**< TRX Bank Report. 0x0:bank0; 0x1:bank1 */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x5C_TypeDef;

/// REG0x6A
typedef union
{
    struct
    {
        volatile unsigned int trxaddaregstat : 6;  /**< ADC&DAC Register Status.;0x0: register is idle;0x1: register is updating. Can not write register again */
        volatile unsigned int Reserved       : 26; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x6A_TypeDef;

/// BK7011RCBEKEN
struct BK7011RCBEKEN_TypeDef
{
    volatile BK7011_RC_BEKEN_REG0x0_TypeDef  *REG0x0;
    volatile BK7011_RC_BEKEN_REG0x1_TypeDef  *REG0x1;
    volatile BK7011_RC_BEKEN_REG0x5_TypeDef  *REG0x5;
    volatile BK7011_RC_BEKEN_REG0x8_TypeDef  *REG0x8;
    volatile BK7011_RC_BEKEN_REG0xB_TypeDef  *REG0xB;
    volatile BK7011_RC_BEKEN_REG0xE_TypeDef  *REG0xE;
    volatile BK7011_RC_BEKEN_REG0x11_TypeDef *REG0x11;
    volatile BK7011_RC_BEKEN_REG0x19_TypeDef *REG0x19;
    volatile BK7011_RC_BEKEN_REG0x1C_TypeDef *REG0x1C;
    volatile BK7011_RC_BEKEN_REG0x1E_TypeDef *REG0x1E;
    volatile BK7011_RC_BEKEN_REG0x3C_TypeDef *REG0x3C;
    volatile BK7011_RC_BEKEN_REG0x3E_TypeDef *REG0x3E;
    volatile BK7011_RC_BEKEN_REG0x3F_TypeDef *REG0x3F;
    volatile BK7011_RC_BEKEN_REG0x40_TypeDef *REG0x40;
    volatile BK7011_RC_BEKEN_REG0x41_TypeDef *REG0x41;
    volatile BK7011_RC_BEKEN_REG0x42_TypeDef *REG0x42;
    volatile BK7011_RC_BEKEN_REG0x4C_TypeDef *REG0x4C;
    volatile BK7011_RC_BEKEN_REG0x4D_TypeDef *REG0x4D;
    volatile BK7011_RC_BEKEN_REG0x4F_TypeDef *REG0x4F;
    volatile BK7011_RC_BEKEN_REG0x50_TypeDef *REG0x50;
    volatile BK7011_RC_BEKEN_REG0x51_TypeDef *REG0x51;
    volatile BK7011_RC_BEKEN_REG0x52_TypeDef *REG0x52;
    volatile BK7011_RC_BEKEN_REG0x54_TypeDef *REG0x54;
    volatile BK7011_RC_BEKEN_REG0x55_TypeDef *REG0x55;
    volatile BK7011_RC_BEKEN_REG0x5C_TypeDef *REG0x5C;
    volatile BK7011_RC_BEKEN_REG0x6A_TypeDef *REG0x6A;
};
#endif

#ifndef __BK7011TRxV2A_H__
#define __BK7011TRxV2A_H__

#define TRX_BEKEN_BASE		0x01050080

/// REG0x0
typedef union
{
    struct
    {
        volatile unsigned int pwdpll       : 1; /**< pwdpll */
        volatile unsigned int cp20         : 3; /**< cp curent control 0to 350uA 50uA step */
        volatile unsigned int spideten     : 1; /**< unlock detect enable fron spi 1:enable */
        volatile unsigned int hvref10      : 2; /**< high vth control for unlock detect 00:0.85V;01:0.9V;10:0.95V;11:1.05V */
        volatile unsigned int lvref10      : 2; /**< low vth control for unlock detect 00:0.2V;01:0.3V;10:0.35V;11:0.4V */
        volatile unsigned int Rzctrl26M    : 1; /**< Rz ctrl in 26M mode:1:normal;0:add 14K */
        volatile unsigned int LoopRzctrl30 : 4; /**< Rz ctrl:2K to 17K,1K step */
        volatile unsigned int rpc10        : 2; /**< second pole Rp ctrl:00:30K;01:10K;10:22K;11:2K */
        volatile unsigned int nsyn         : 1; /**< N divider rst */
        volatile unsigned int cksel10      : 2; /**< 0:26M;1:40M;2:24M;3:19.2M */
        volatile unsigned int spitrig      : 1; /**< SPI band selection trigger signal */
        volatile unsigned int CKEn         : 1; /**< 1: Enable all clock phases(480M/160M); 0 : Disable all clocks; */
        volatile unsigned int band41       : 4; /**< band manual value */
        volatile unsigned int bandmanual   : 1; /**< 1:band manual;0:band auto */
        volatile unsigned int dsptrig      : 1; /**< band selection trigger signal */
        volatile unsigned int xtalIctrl    : 1; /**< 0:large current;1:small current */
        volatile unsigned int ckTestEn     : 1; /**< 1: Enable clock test path; 0 : Disable clock test path; */
        volatile unsigned int xamp31       : 3; /**< xtal OSC amp control */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x0_TypeDef;

/// REG0x1
typedef union
{
    struct
    {
        volatile unsigned int acoef80       : 9; /**< 数字滤波器积分支路的系数 */
        volatile unsigned int bcoef20       : 3; /**< 数字滤波器propotional支路的系数 */
        volatile unsigned int bckphoffset50 : 6; /**< 在PD的输入端调整div时钟的延迟 */
        volatile unsigned int rckphoffset50 : 6; /**< 在PD的输入端调整ref时钟的延迟 */
        volatile unsigned int bypass52M     : 1; /**< bypass 52M的时钟，输出26M的时钟 */
        volatile unsigned int tst2xpden     : 1; /**< 选择测试要输出的信号，1为PD的输出，0为52M的时钟 */
        volatile unsigned int tst2xen       : 1; /**< 使能测试输出 */
        volatile unsigned int calspitrig    : 1; /**< 通过寄存器触发频率的校准，触发的时候，先写为0，再写为1 */
        volatile unsigned int bypassdmsb    : 1; /**< 数字滤波器工作的时候，bypass掉Dmsb支路 */
        volatile unsigned int manual        : 1; /**< 对Dmsb和Dlsb改为手动 */
        volatile unsigned int frecalbypass  : 1; /**< bypass 掉频率的自动校准功能，改用手动 */
        volatile unsigned int rstnspi       : 1; /**< 通过寄存器reset */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x1_TypeDef;

/// REG0x2
typedef union
{
    struct
    {
        volatile unsigned int arbpden        : 1;  /**< 通过寄存器使能arbiter BBPD */
        volatile unsigned int endoubler      : 1;  /**< 通过寄存器使能doubler */
        volatile unsigned int trackenspi     : 1;  /**< 通过寄存器使能开环算法 */
        volatile unsigned int Dcal80         : 9;  /**< 通过寄存器手动控制振荡器的Dcal */
        volatile unsigned int Dmsbspi20      : 3;  /**< 通过寄存器手动控制振荡器的Dmsb */
        volatile unsigned int Dlsbspi40      : 5;  /**< 通过寄存器手动控制振荡器的Dlsb */
        volatile unsigned int updatecycle110 : 12; /**< 采用开环算法的时候数据调整的时间周期 */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x2_TypeDef;

/// REG0x3
typedef union
{
    struct
    {
        volatile unsigned int numdn110       : 12; /**< 控制相位滞后的次数 */
        volatile unsigned int numup110       : 12; /**< 控制相位超前的次数 */
        volatile unsigned int arbpddelay30   : 4;  /**< arbiter BBPD中的delay 控制 */
        volatile unsigned int hysen          : 1;  /**< 使能相位的迟滞功能 */
        volatile unsigned int phacalbypass   : 1;  /**< 启用数字滤波器的时候，校准完成后可以不使能相位的tracking,从而测试校准的结果是否正确 */
        volatile unsigned int f2xldovsel3v10 : 2;  /**< 控制LDO的输出电压，两个LDO共享一组控制位，该控制位已经经过level shift的转换，00：1.2V；01 10：1.3V；11：1.4V */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x3_TypeDef;

/// REG0x4
typedef union
{
    struct
    {
        volatile unsigned int manual    : 1; /**< vco manual band control enable 1:manual */
        volatile unsigned int bandm60   : 7; /**< vco band manual control */
        volatile unsigned int calresc10 : 2; /**< vco nwell voltage for calibration:00,0.5V;01,0.6V;10,0.7V;11,0.8V */
        volatile unsigned int rvco20    : 3; /**< rfpll vco bias current control */
        volatile unsigned int cpldo10   : 2; /**< Charge Pump LDO voltage control. */
        volatile unsigned int NC        : 1; /**< No description */
        volatile unsigned int trswspi   : 1; /**< trsw spi control:0,spi;1:cpu */
        volatile unsigned int trswen    : 1; /**< enable cpu trsw control 1:enable */
        volatile unsigned int nrsten    : 1; /**< N counter reset enable */
        volatile unsigned int reset     : 1; /**< reset rfpll fb divider loading */
        volatile unsigned int pwdlogen  : 1; /**< power down rfpll LO generation */
        volatile unsigned int pwdvco    : 1; /**< power down rfpll vco */
        volatile unsigned int pwdpll    : 1; /**< power down rfpll pll */
        volatile unsigned int nint70    : 8; /**< power down rfpll vco */
        volatile unsigned int intmode   : 1; /**< power down rfpll pll */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x4_TypeDef;

/// REG0x5
typedef union
{
    struct
    {
        volatile unsigned int errdetspien : 1; /**< spi enable rfpll unlock detection */
        volatile unsigned int spitrigger  : 1; /**< VCO calibration trigger from spi */
        volatile unsigned int ckrefsel    : 1; /**< reference clock freq select: 0, 26M; 1, 52M */
        volatile unsigned int ckpsel      : 1; /**< reference clock pol select */
        volatile unsigned int locap30     : 4; /**< LO Mixer load cap control */
        volatile unsigned int lmngm20     : 3; /**< LO Mixer negative resistor control */
        volatile unsigned int sdiv210     : 2; /**< rpll 3.2G div2 input dc bias control */
        volatile unsigned int pllldo10    : 2; /**< pll ldo output voltage control: 00, 1.2V; 01 1.3V; 10, 1.4V;11 1.5V */
        volatile unsigned int vcoldo10    : 2; /**< vco ldo output voltage control: 00, 1.2V; 01 1.3V; 10, 1.4V;11 1.5V */
        volatile unsigned int lwvref10    : 2; /**< unlock detection voltage control(low) */
        volatile unsigned int lnvref10    : 2; /**< cdyn detection voltage control(low) */
        volatile unsigned int hwvref10    : 2; /**< unlock detection voltage control(high) */
        volatile unsigned int hnvref10    : 2; /**< cdyn detection voltage control(high) */
        volatile unsigned int captunedir  : 1; /**< cap tune direction */
        volatile unsigned int pwdcpamp10  : 2; /**< 00,cp with op fb; 01, cp wi up/dn connect together; 11, cp wi up/dn connect to gnd/vdd */
        volatile unsigned int dcal30      : 4; /**< rfpll cp bias current calibration */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x5_TypeDef;

/// REG0x6
typedef union
{
    struct
    {
        volatile unsigned int vcmsel10     : 2; /**< ADC OP CM voltage adjustment(0:0.625 */
        volatile unsigned int tmpEn        : 1; /**< 1: Enable temperature sensor; 0 : Disable temperature sensor; */
        volatile unsigned int NC           : 1; /**< Reserved */
        volatile unsigned int lpfcapcalq50 : 6; /**< lpf Q path calibiration input */
        volatile unsigned int lpfcapcali50 : 6; /**< lpf I path calibiration input */
        volatile unsigned int dcocq        : 8; /**< dcoc Q path input */
        volatile unsigned int dcoci        : 8; /**< dcoc I path input */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x6_TypeDef;

/// REG0x7
typedef union
{
    struct
    {
        volatile unsigned int digdcoen    : 1; /**< dcoc input selection,1: digital 0: spi */
        volatile unsigned int spilpfrxg30 : 4; /**< rxif gain, 0000-1111 0dB - 45dB step:3dB */
        volatile unsigned int autorxifgen : 1; /**< rxif gain control selection, 1: auto control 0: spi */
        volatile unsigned int pwdxtalbuf  : 1; /**< 1: Power-down crystal oscillator buffer;;0: Enable crystal oscillator buffer; */
        volatile unsigned int pwdxtalcore : 1; /**< 1: Power-down crystal oscillator core;;0: Enable crystal oscillator core; */
        volatile unsigned int lpfvcmic10  : 2; /**< control auxiliary pmos pair gate voltage,default 01 */
        volatile unsigned int lpftxTest   : 1; /**< tx test control bit,1 : tx test mode 0: default */
        volatile unsigned int xtalenref   : 1; /**< 1: Enable crystal oscillator output as the reference to RF PLL only;;0: Enable crystal oscillator output as the reference to frequency doubler only. */
        volatile unsigned int voc1v210    : 2; /**< SPI/Digital regulator output programming; 00:1.2;01:1.3;10,1.4;11:1.5 */
        volatile unsigned int swm1v210    : 2; /**< Path selection of the switched output in low-power mode,00: shutdown, 01: high res, 10:res/2,11:res/3 */
        volatile unsigned int NC          : 1; /**< NC */
        volatile unsigned int chin60      : 7; /**< substitute for cpu channel value */
        volatile unsigned int cksdmsel20  : 3; /**< rfpll sigma delta clock select:;0: 26MHz;;1: 40MHz;;2: 24MHz;;3: 19.2MHz;;4: 52MHz;;5:104MHz; */
        volatile unsigned int ckbandsel10 : 2; /**< vco band calibration clock select:;0: 26MHz;;1: 40MHz;;2: 24MHz;;3: 19.2MHz; */
        volatile unsigned int var10       : 2; /**< Kvco control: 0:40MHz/V;1:60MHz/V;2:60MHz/V;3:80MHz/V */
        volatile unsigned int enckcali    : 1; /**< 0:VCO calibration clock do not power down;1: auto power down vco calibration clock; */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x7_TypeDef;

/// REG0x8
typedef union
{
    struct
    {
        volatile unsigned int vsrxmixldo10 : 2; /**< rx mixer LDO vout 00-11 1.2-1.5V */
        volatile unsigned int vsrxlnaldo10 : 2; /**< rx lna LDO vout 00-11 1.2-1.5V */
        volatile unsigned int isrxref10    : 2; /**< rx ref ib selection 00-11 10-15uA */
        volatile unsigned int isrxlna20    : 3; /**< rx lna ib selection 000-111 50-110uA */
        volatile unsigned int isrxlnac10   : 2; /**< rx lnacali ib selection 00-11 40-70uA */
        volatile unsigned int isrxdiv10    : 2; /**< rx div ib selection 00-11 10-25uA */
        volatile unsigned int vbsrxlo20    : 3; /**< rx lo biasing voltage: 000-111 0.4-0.75V */
        volatile unsigned int vresrxlo     : 1; /**< rx lo vb ref voltage: 0-1 0.4-0.5V */
        volatile unsigned int ibsrxi2v10   : 2; /**< rx i2v ib selection 00-11 5-15uA */
        volatile unsigned int vcmsrxi2v    : 1; /**< rx i2v vcm selction 0-1 0.6-0.5V */
        volatile unsigned int isrssigm10   : 2; /**< rx rssi gm ib selection 00-11 40-55uA */
        volatile unsigned int stbrssi10    : 2; /**< rx rssi stable time 00-11 150-450ns step:100ns */
        volatile unsigned int rssith50     : 6; /**< rx rssi threshhold Pant=-29dBm(th=000111) 0:-37dBm step:1dB */
        volatile unsigned int nc           : 2; /**< reserved */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x8_TypeDef;

/// REG0x9
typedef union
{
    struct
    {
        volatile unsigned int agcrxfeEn    : 1;  /**< enble rxfe agc by mcu, 0/1 gctr by spi/mcu */
        volatile unsigned int grxlnaspi10  : 2;  /**< rx lna gain 00-11 0/+6/+12/+18dBr */
        volatile unsigned int grxi2vspi10  : 2;  /**< rx i2v gain 00-11 0/+6/+6/+12dBr */
        volatile unsigned int grssi10      : 2;  /**< rx rssi option gain */
        volatile unsigned int grxlnacali20 : 3;  /**< rx lan cali gain,000-111 -50~-5.5 caliin->i2vout step:6dB */
        volatile unsigned int rssiten      : 1;  /**< rssi analog v output test enable */
        volatile unsigned int dcaprxi2v10  : 2;  /**< V2T: I2V feedback cap tuning 00-11: + 0/1p/2p/3p */
        volatile unsigned int drxfbrm      : 1;  /**< V2T: feedback mathing res on */
        volatile unsigned int dcaptrsw20   : 3;  /**< V2T: 7/0 @ tx/rx mode */
        volatile unsigned int nc           : 15; /**< reserved */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x9_TypeDef;

/// REG0xA
typedef union
{
    struct
    {
        volatile unsigned int disrefPA10  : 2; /**< PA reference bias current programming for Constant-gm bias circuit,10u to 16.5uA */
        volatile unsigned int disrefpga10 : 2; /**< RFPGA reference bias current programming for Constant-gm bias circuit,10u to 16.5uA */
        volatile unsigned int dbpab30     : 4; /**< PA class-B input bias level programming */
        volatile unsigned int dbpaa30     : 4; /**< PA class-A input bias level programming */
        volatile unsigned int dbcpa10     : 2; /**< PA cascode device bias level programming,00(2.2v), 01(2.5v), 10(2.8v),11(3.0v) */
        volatile unsigned int noConn10    : 2; /**< No Connected */
        volatile unsigned int dcapbuf30   : 4; /**< PA buf output capacitor tuning */
        volatile unsigned int dbbuf30     : 4; /**< PA buf input device bias level programming */
        volatile unsigned int dbcbuf10    : 2; /**< PA buf cascode device bias level programming,00(2.2v), 01(2.5v), 10(2.8v),11(3.0v) */
        volatile unsigned int noConn10_   : 2; /**< No Connected */
        volatile unsigned int dtr30       : 4; /**< PA ramping-up time selection,0.13us(1111) to 3.45us(0000) */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0xA_TypeDef;

/// REG0xB
typedef union
{
    struct
    {
        volatile unsigned int dcpga20    : 3; /**< RFPGA cascode device biaslevel programming */
        volatile unsigned int noConn     : 1; /**< No Connected */
        volatile unsigned int dbpga30    : 4; /**< RFPGA input device bias level programming */
        volatile unsigned int dcappga30  : 4; /**< RFPGA output capacitor tuning */
        volatile unsigned int dcorMod30  : 4; /**< Modulator&RFPGA gain tuning for Pout calibration */
        volatile unsigned int dvlo20     : 3; /**< Modulator LO bias level programming,0.9v~1.07v,100(1.0v) */
        volatile unsigned int noConn_    : 1; /**< No Connected */
        volatile unsigned int dvcmo20    : 3; /**< Modulator V-I common mode voltage selection,010(vdd/2) */
        volatile unsigned int noConn__   : 1; /**< No Connected */
        volatile unsigned int dcapmod30  : 4; /**< Modulator output capacitor tuning */
        volatile unsigned int gctrlmod30 : 4; /**< Modulator gain control,3dB/step,0001(default gain+3),0010,0100(default gain-3),1000(default gain-6) */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0xB_TypeDef;

/// REG0xC
typedef union
{
    struct
    {
        volatile unsigned int gctrlpga20  : 3; /**< RFPGA gain control. 111(max),110(max-3),101(max-6),100(max-9),011(max-12),010(max-18),001(max-24),000(max-30) */
        volatile unsigned int noConn      : 1; /**< No Connected */
        volatile unsigned int dgainbuf20  : 3; /**< PA buf gain control,10dB(1111)~-5dB(001),4.5dB/step */
        volatile unsigned int noConn_     : 1; /**< No Connected */
        volatile unsigned int dgainPA20   : 3; /**< PA gain control,9dB(111)~-4dB(001),5dB/step */
        volatile unsigned int enTSSItst   : 1; /**< TSSI test enable */
        volatile unsigned int dcorPA30    : 4; /**< PA gain tuning for Pout calibration */
        volatile unsigned int vregsel10   : 2; /**< RFLDO output voltage selection.00:1.2v; 01:1.3v; 10:1.4v; 11:1.5v */
        volatile unsigned int TSSIiref10  : 2; /**< TSSI reference bias current programming for Constant-gm bias circuit,10u to 16.5uA */
        volatile unsigned int gtssi10     : 2; /**< TSSI output gain programming,4K(00)/5K(01)/6K(10)/7K(11) */
        volatile unsigned int stbtssi10   : 2; /**< TSSI output stable time programming,0.15us(00) to 0.45us(11) */
        volatile unsigned int PcaliATTs10 : 2; /**< TSSI Pout calibration input attenuation selection,00(-6dB)~11(-24dB),6dB/step */
        volatile unsigned int enPcaliGm   : 1; /**< TSSI Pout calibration option to enable Gm1 */
        volatile unsigned int enPcali     : 1; /**< TSSI Pout calibration enable */
        volatile unsigned int enIQcali    : 1; /**< TSSI IQ calibration enable */
        volatile unsigned int enDCcaliGm1 : 1; /**< TSSI DC calibration Gm1 block enable for -30~-15dBm input range */
        volatile unsigned int enDCcali    : 1; /**< DC/IQ calibration enable.enDCcali=1 for DC or IQ calibration */
        volatile unsigned int TSSIsel     : 1; /**< TSSI output selection.TSSIsel=1 for Pout calibration;TSSIsel=0 for DC/IQ calibration */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0xC_TypeDef;

/// REG0xD
typedef union
{
    struct
    {
        volatile unsigned int dacselection : 1; /**< 0: external dac, 1: internal dac */
        volatile unsigned int lpfrxbw      : 1; /**< lpf rx bandwidth selection 1: high bandwidth(19MHz) 0: low bandwidth(9MHz); */
        volatile unsigned int lpftxbw      : 1; /**< lpf tx bandwidth selection 1: high bandwidth(29MHz) 0: low bandwidth(14MHz); */
        volatile unsigned int lpftrxsw     : 1; /**< LPF tx rx switch(1:tx,0:rx) */
        volatile unsigned int enlpf        : 1; /**< LPF enable */
        volatile unsigned int enif         : 1; /**< ADC, DAC Ldo enable */
        volatile unsigned int endcoc       : 1; /**< DCOC DAC enable */
        volatile unsigned int enrxadc      : 1; /**< RX ADC enable */
        volatile unsigned int entxdac      : 1; /**< TX dac enable */
        volatile unsigned int entxdacbias  : 1; /**< TX dac bias enable */
        volatile unsigned int enrxrssi     : 1; /**< RX RF Rssi enable */
        volatile unsigned int enrxref      : 1; /**< RX RF Vref enable */
        volatile unsigned int enrxi2v      : 1; /**< RX I2V enable */
        volatile unsigned int enrxmix      : 1; /**< RX mixer enable */
        volatile unsigned int enlnacal     : 1; /**< RX look back lna input enable */
        volatile unsigned int enlna        : 1; /**< Rxfe lna enable */
        volatile unsigned int txvinsel     : 1; /**< Txfe input selection(0,cal when need bypass TX filter,1:normal) */
        volatile unsigned int entssi       : 1; /**< Txfe tssi enable */
        volatile unsigned int entssiadc    : 1; /**< Txfe tssi adc enable */
        volatile unsigned int entxferef    : 1; /**< Txfe reference enable */
        volatile unsigned int entxfebias   : 1; /**< Txfe bias enable */
        volatile unsigned int entxv2i      : 1; /**< Tx v2i enable */
        volatile unsigned int entxlo       : 1; /**< Tx LO enable */
        volatile unsigned int entxpga      : 1; /**< TX PGA enable */
        volatile unsigned int enpa         : 1; /**< Tx PA enable */
        volatile unsigned int enrxsw       : 1; /**< RX switch enable */
        volatile unsigned int entxsw       : 1; /**< TX switch enable */
        volatile unsigned int trswpll      : 1; /**< rf pll tx rx switch */
        volatile unsigned int enrfpll      : 1; /**< rf pll enable */
        volatile unsigned int endobuler    : 1; /**< dobuler enable */
        volatile unsigned int endpll       : 1; /**< digital pll enable */
        volatile unsigned int enxtal       : 1; /**< xtal enable signal */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0xD_TypeDef;

/// REG0xE
typedef union
{
    struct
    {
        volatile unsigned int dacselection : 1; /**< 0: external dac, 1: internal dac */
        volatile unsigned int lpfrxbw      : 1; /**< lpf rx bandwidth selection 1: high bandwidth(19MHz) 0: low bandwidth(9MHz); */
        volatile unsigned int lpftxbw      : 1; /**< lpf tx bandwidth selection 1: high bandwidth(29MHz) 0: low bandwidth(14MHz); */
        volatile unsigned int lpftrxsw     : 1; /**< LPF tx rx switch(1:tx,0:rx) */
        volatile unsigned int enlpf        : 1; /**< LPF enable */
        volatile unsigned int enif         : 1; /**< ADC, DAC Ldo enable */
        volatile unsigned int endcoc       : 1; /**< DCOC DAC enable */
        volatile unsigned int enrxadc      : 1; /**< RX ADC enable */
        volatile unsigned int entxdac      : 1; /**< TX dac enable */
        volatile unsigned int entxdacbias  : 1; /**< TX dac bias enable */
        volatile unsigned int enrxrssi     : 1; /**< RX RF Rssi enable */
        volatile unsigned int enrxref      : 1; /**< RX RF Vref enable */
        volatile unsigned int enrxi2v      : 1; /**< RX I2V enable */
        volatile unsigned int enrxmix      : 1; /**< RX mixer enable */
        volatile unsigned int enlnacal     : 1; /**< RX look back lna input enable */
        volatile unsigned int enlna        : 1; /**< Rxfe lna enable */
        volatile unsigned int txvinsel     : 1; /**< Txfe input selection(0,cal when need bypass TX filter,1:normal) */
        volatile unsigned int entssi       : 1; /**< Txfe tssi enable */
        volatile unsigned int entssiadc    : 1; /**< Txfe tssi adc enable */
        volatile unsigned int entxferef    : 1; /**< Txfe reference enable */
        volatile unsigned int entxfebias   : 1; /**< Txfe bias enable */
        volatile unsigned int entxv2i      : 1; /**< Tx v2i enable */
        volatile unsigned int entxlo       : 1; /**< Tx LO enable */
        volatile unsigned int entxpga      : 1; /**< TX PGA enable */
        volatile unsigned int enpa         : 1; /**< Tx PA enable */
        volatile unsigned int enrxsw       : 1; /**< RX switch enable */
        volatile unsigned int entxsw       : 1; /**< TX switch enable */
        volatile unsigned int trswpll      : 1; /**< rf pll tx rx switch */
        volatile unsigned int enrfpll      : 1; /**< rf pll enable */
        volatile unsigned int endobuler    : 1; /**< dobuler enable */
        volatile unsigned int endpll       : 1; /**< digital pll enable */
        volatile unsigned int enxtal       : 1; /**< xtal enable signal */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0xE_TypeDef;

/// REG0xF
typedef union
{
    struct
    {
        volatile unsigned int regbanksel   : 1;  /**< Register Bank Select. 0x0: Bank0 (Reg00~Reg14); ; 0x1: Bank1 (Reg16~Reg30) */
        volatile unsigned int clkdacsel    : 1;  /**< DAC Clock Select. 0x0: 80MHz; 0x1: 160MHz */
        volatile unsigned int clkadcsel    : 1;  /**< ADCClock Select. 0x0: 40MHz; 0x1: 80MHz */
        volatile unsigned int NC           : 1;  /**< NC */
        volatile unsigned int NC_          : 1;  /**< NC */
        volatile unsigned int sinadtxen    : 1;  /**< TX Sinad Enable */
        volatile unsigned int sinadrxen    : 1;  /**< RX Sinad Detect Enable */
        volatile unsigned int tssicalen    : 1;  /**< TSSI Calibration Enable, enable 40M clock */
        volatile unsigned int NC__         : 5;  /**< NC */
        volatile unsigned int sinadhpfcoef : 2;  /**< HPF Coefficence for RX Siand Detect. 0x0: 1/8 0x1: 1/32; 0x2: 1/128; 0x3: 1/1024 */
        volatile unsigned int clkdacinv    : 1;  /**< DAC Clock Invert */
        volatile unsigned int clkadcinv    : 1;  /**< ADC Clock Invert */
        volatile unsigned int reserved     : 11; /**< Reserved */
        volatile unsigned int sysldoen     : 1;  /**< enable sysldo */
        volatile unsigned int sysldolp     : 1;  /**< sysldo low power enable */
        volatile unsigned int endspvdd     : 1;  /**< dsp vdd enable */
        volatile unsigned int digldolp     : 1;  /**< digldo low power enable */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0xF_TypeDef;

/// REG0x10
typedef union
{
    struct
    {
        volatile unsigned int dacselection : 1; /**< 0: external dac, 1: internal dac */
        volatile unsigned int lpfrxbw      : 1; /**< lpf rx bandwidth selection 1: high bandwidth(19MHz) 0: low bandwidth(9MHz); */
        volatile unsigned int lpftxbw      : 1; /**< lpf tx bandwidth selection 1: high bandwidth(29MHz) 0: low bandwidth(14MHz); */
        volatile unsigned int lpftrxsw     : 1; /**< LPF tx rx switch(1:tx,0:rx) */
        volatile unsigned int enlpf        : 1; /**< LPF enable */
        volatile unsigned int enif         : 1; /**< ADC, DAC Ldo enable */
        volatile unsigned int endcoc       : 1; /**< DCOC DAC enable */
        volatile unsigned int enrxadc      : 1; /**< RX ADC enable */
        volatile unsigned int entxdac      : 1; /**< TX dac enable */
        volatile unsigned int entxdacbias  : 1; /**< TX dac bias enable */
        volatile unsigned int enrxrssi     : 1; /**< RX RF Rssi enable */
        volatile unsigned int enrxref      : 1; /**< RX RF Vref enable */
        volatile unsigned int enrxi2v      : 1; /**< RX I2V enable */
        volatile unsigned int enrxmix      : 1; /**< RX mixer enable */
        volatile unsigned int enlnacal     : 1; /**< RX look back lna input enable */
        volatile unsigned int enlna        : 1; /**< Rxfe lna enable */
        volatile unsigned int txvinsel     : 1; /**< Txfe input selection(0,cal when need bypass TX filter,1:normal) */
        volatile unsigned int entssi       : 1; /**< Txfe tssi enable */
        volatile unsigned int entssiadc    : 1; /**< Txfe tssi adc enable */
        volatile unsigned int entxferef    : 1; /**< Txfe reference enable */
        volatile unsigned int entxfebias   : 1; /**< Txfe bias enable */
        volatile unsigned int entxv2i      : 1; /**< Tx v2i enable */
        volatile unsigned int entxlo       : 1; /**< Tx LO enable */
        volatile unsigned int entxpga      : 1; /**< TX PGA enable */
        volatile unsigned int enpa         : 1; /**< Tx PA enable */
        volatile unsigned int enrxsw       : 1; /**< RX switch enable */
        volatile unsigned int entxsw       : 1; /**< TX switch enable */
        volatile unsigned int trswpll      : 1; /**< rf pll tx rx switch */
        volatile unsigned int enrfpll      : 1; /**< rf pll enable */
        volatile unsigned int endobuler    : 1; /**< dobuler enable */
        volatile unsigned int endpll       : 1; /**< digital pll enable */
        volatile unsigned int enxtal       : 1; /**< xtal enable signal */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x10_TypeDef;

/// REG0x11
typedef union
{
    struct
    {
        volatile unsigned int txsinadtable : 11; /**< TX Sinad Table;D[0] = 0: Write I Path Table Data;D[0] = 1: Write Q Path Table Data;LSB：D[1] ;Both I and Q need 16 number data */
        volatile unsigned int reserved     : 21; /**< Reserved */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x11_TypeDef;

/// REG0x12
typedef union
{
    struct
    {
        volatile unsigned int lpfouttsten     : 1; /**< lpf output test enable,0:disenable,1:enable */
        volatile unsigned int lpfintsten      : 1; /**< lpf input test enable,0:disenable,1:enable */
        volatile unsigned int ifbufic20       : 3; /**< test buffer bias current control,0:low, 1:high */
        volatile unsigned int ifadcvcmsel     : 2; /**< if adc reference voltage selection. */
        volatile unsigned int ldodpll10       : 2; /**< digital pll and xtal ldo voutselection. 00:low, 11:high */
        volatile unsigned int NC              : 1; /**< Reserved bit; */
        volatile unsigned int buftstselection : 1; /**< buffer input selection:1:lpf, 0:rssidactst,rssitst,tssitst,adcstst1 */
        volatile unsigned int entstbufldo     : 1; /**< enable test buffer ldo, 0:disenable,1:enable */
        volatile unsigned int NC_             : 1; /**< Reserved bit; */
        volatile unsigned int dldovctrl10     : 2; /**< dig do output control voltage: 0:1.2V 0.1V/step */
        volatile unsigned int adcinSel        : 1; /**< SAR ADC input selection;0:TSSI, 1:Temperature */
        volatile unsigned int adciselr20      : 3; /**< adc reference bias current selection,0:0.45x,1:0.5x,2:0.55X,3:0.6x,4:0.9x,5:1X,6:1.1X,7:1.2X */
        volatile unsigned int adciselc20      : 3; /**< adc core bias current selection,0:5x,1:0.6x,2:0.7X,3:0.8x,4:1.0x,5:1.2X,6:1.4X,7:1.7X */
        volatile unsigned int adcrefbwsel     : 1; /**< adc bandwidth selection.0:40MHz, 1:80MHz */
        volatile unsigned int adcten          : 1; /**< adc test enable(1:only I work, 0:both path work) */
        volatile unsigned int fictrl30        : 4; /**< lpf bias current selection, 0:low, 15:high */
        volatile unsigned int flvcm10         : 2; /**< lpf common mode voltage selection,00:low , 11: high */
        volatile unsigned int ldoifsel3v10    : 2; /**< If ldo vout selection:00:low, 11: high */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x12_TypeDef;

/// REG0x13
typedef union
{
    struct
    {
        volatile unsigned int r2rx10   : 2; /**< rx mode loopfilter R2 control */
        volatile unsigned int c1rx10   : 2; /**< rx mode loopfilter C1 control */
        volatile unsigned int rzrx40   : 5; /**< rx mode loopfilter Rz control */
        volatile unsigned int icprx40  : 5; /**< rx mode cp current control */
        volatile unsigned int icpoff20 : 3; /**< cp offset current control */
        volatile unsigned int icpsel   : 1; /**< icp bias select: 0, internal; 1, external */
        volatile unsigned int r2tx10   : 2; /**< tx mode loopfilter R2 control */
        volatile unsigned int c1tx10   : 2; /**< tx mode loopfilter C1 control */
        volatile unsigned int rztx40   : 5; /**< tx mode loopfilter Rz control */
        volatile unsigned int icptx40  : 5; /**< tx mode cp current control */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x13_TypeDef;

/// REG0x14
typedef union
{
    struct
    {
        volatile unsigned int dci0db : 8; /**< RX dcoc I path input for 0dB of PGA */
        volatile unsigned int dcq0db : 8; /**< RX dcoc Q path input for 0dB of PGA */
        volatile unsigned int dci3db : 8; /**< RX dcoc I path input for 3dB of PGA */
        volatile unsigned int dcq3db : 8; /**< RX dcoc Q path input for 3dB of PGA */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x14_TypeDef;

/// REG0x15
typedef union
{
    struct
    {
        volatile unsigned int dci6db : 8; /**< RX dcoc I path input for 6dB of PGA */
        volatile unsigned int dcq6db : 8; /**< RX dcoc Q path input for 6dB of PGA */
        volatile unsigned int dci9db : 8; /**< RX dcoc I path input for 9dB of PGA */
        volatile unsigned int dcq9db : 8; /**< RX dcoc Q path input for 9dB of PGA */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x15_TypeDef;

/// REG0x16
typedef union
{
    struct
    {
        volatile unsigned int dci12db : 8; /**< RX dcoc I path input for 12dB of PGA */
        volatile unsigned int dcq12db : 8; /**< RX dcoc Q path input for 12dB of PGA */
        volatile unsigned int dci15db : 8; /**< RX dcoc I path input for 15dB of PGA */
        volatile unsigned int dcq15db : 8; /**< RX dcoc Q path input for 15dB of PGA */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x16_TypeDef;

/// REG0x17
typedef union
{
    struct
    {
        volatile unsigned int dci18db : 8; /**< RX dcoc I path input for 18dB of PGA */
        volatile unsigned int dcq18db : 8; /**< RX dcoc Q path input for 18dB of PGA */
        volatile unsigned int dci21db : 8; /**< RX dcoc I path input for 21dB of PGA */
        volatile unsigned int dcq21db : 8; /**< RX dcoc Q path input for 21dB of PGA */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x17_TypeDef;

/// REG0x18
typedef union
{
    struct
    {
        volatile unsigned int dci24db : 8; /**< RX dcoc I path input for 24dB of PGA */
        volatile unsigned int dcq24db : 8; /**< RX dcoc Q path input for 24dB of PGA */
        volatile unsigned int dci27db : 8; /**< RX dcoc I path input for 27dB of PGA */
        volatile unsigned int dcq27db : 8; /**< RX dcoc Q path input for 27dB of PGA */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x18_TypeDef;

/// REG0x19
typedef union
{
    struct
    {
        volatile unsigned int dci30db : 8; /**< RX dcoc I path input for 30dB of PGA */
        volatile unsigned int dcq30db : 8; /**< RX dcoc Q path input for 30dB of PGA */
        volatile unsigned int dci33db : 8; /**< RX dcoc I path input for 33dB of PGA */
        volatile unsigned int dcq33db : 8; /**< RX dcoc Q path input for 33dB of PGA */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x19_TypeDef;

/// REG0x1A
typedef union
{
    struct
    {
        volatile unsigned int dci36db : 8; /**< RX dcoc I path input for 36dB of PGA */
        volatile unsigned int dcq36db : 8; /**< RX dcoc Q path input for 36dB of PGA */
        volatile unsigned int dci39db : 8; /**< RX dcoc I path input for 39dB of PGA */
        volatile unsigned int dcq39db : 8; /**< RX dcoc Q path input for 39dB of PGA */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x1A_TypeDef;

/// REG0x1B
typedef union
{
    struct
    {
        volatile unsigned int dci42db : 8; /**< RX dcoc I path input for 42dB of PGA */
        volatile unsigned int dcq42db : 8; /**< RX dcoc Q path input for 42dB of PGA */
        volatile unsigned int dci45db : 8; /**< RX dcoc I path input for 45dB of PGA */
        volatile unsigned int dcq45db : 8; /**< RX dcoc Q path input for 45dB of PGA */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x1B_TypeDef;

/// REG0x1C
typedef union
{
    struct
    {
        volatile unsigned int dacoutqspi90 : 10; /**< DAC Q channel input in analog test only; */
        volatile unsigned int NC           : 2;  /**< Reserved bits; */
        volatile unsigned int dacoutispi90 : 10; /**< DAC I channel input in analog test only; */
        volatile unsigned int NC_          : 10; /**< Reserved bits; */
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x1C_TypeDef;   // trx reg28   just for test
#define BK7011_TRxV2ABASE				0X00800080

/// BK7011TRxV2A
struct BK7011TRxV2A_TypeDef
{
    volatile BK7011_TRxV2A_REG0x0_TypeDef  *REG0x0;
    volatile BK7011_TRxV2A_REG0x1_TypeDef  *REG0x1;
    volatile BK7011_TRxV2A_REG0x2_TypeDef  *REG0x2;
    volatile BK7011_TRxV2A_REG0x3_TypeDef  *REG0x3;
    volatile BK7011_TRxV2A_REG0x4_TypeDef  *REG0x4;
    volatile BK7011_TRxV2A_REG0x5_TypeDef  *REG0x5;
    volatile BK7011_TRxV2A_REG0x6_TypeDef  *REG0x6;
    volatile BK7011_TRxV2A_REG0x7_TypeDef  *REG0x7;
    volatile BK7011_TRxV2A_REG0x8_TypeDef  *REG0x8;
    volatile BK7011_TRxV2A_REG0x9_TypeDef  *REG0x9;
    volatile BK7011_TRxV2A_REG0xA_TypeDef  *REG0xA;
    volatile BK7011_TRxV2A_REG0xB_TypeDef  *REG0xB;
    volatile BK7011_TRxV2A_REG0xC_TypeDef  *REG0xC;
    volatile BK7011_TRxV2A_REG0xD_TypeDef  *REG0xD;
    volatile BK7011_TRxV2A_REG0xE_TypeDef  *REG0xE;
    volatile BK7011_TRxV2A_REG0xF_TypeDef  *REG0xF;
    volatile BK7011_TRxV2A_REG0x10_TypeDef *REG0x10;
    volatile BK7011_TRxV2A_REG0x11_TypeDef *REG0x11;
    volatile BK7011_TRxV2A_REG0x12_TypeDef *REG0x12;
    volatile BK7011_TRxV2A_REG0x13_TypeDef *REG0x13;
    volatile BK7011_TRxV2A_REG0x14_TypeDef *REG0x14;
    volatile BK7011_TRxV2A_REG0x15_TypeDef *REG0x15;
    volatile BK7011_TRxV2A_REG0x16_TypeDef *REG0x16;
    volatile BK7011_TRxV2A_REG0x17_TypeDef *REG0x17;
    volatile BK7011_TRxV2A_REG0x18_TypeDef *REG0x18;
    volatile BK7011_TRxV2A_REG0x19_TypeDef *REG0x19;
    volatile BK7011_TRxV2A_REG0x1A_TypeDef *REG0x1A;
    volatile BK7011_TRxV2A_REG0x1B_TypeDef *REG0x1B;
    volatile BK7011_TRxV2A_REG0x1C_TypeDef *REG0x1C;
};
#endif

/// POWER
typedef struct
{
    unsigned int rega_8_11  : 4; 
    unsigned int regc_0_2   : 3; 
    unsigned int regc_4_6   : 3; 
    unsigned int regc_8_10  : 3;     
    unsigned int regb_28_31 : 4; 
    unsigned int pregain    : 5;
    unsigned int unuse      : 10;
} PWR_REGS;

/*******************************************************************************
* Function Declarations
*******************************************************************************/
#endif // (CFG_SOC_NAME == SOC_BK7231)

#endif // _BK7231_CAL_H_