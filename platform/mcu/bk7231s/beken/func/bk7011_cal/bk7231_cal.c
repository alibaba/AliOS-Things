#include "include.h"

#if (CFG_SOC_NAME == SOC_BK7231)
#include "arm_arch.h"
#include "target_util_pub.h"
#include "mem_pub.h"
#include "drv_model_pub.h"
#include "sys_ctrl_pub.h"
#include "phy.h"
#include "bk7011_cal_pub.h"
#include "bk7011_cal.h"
#include <string.h>
#include "flash_pub.h"
#include "cmd_evm.h"
#include "temp_detect_pub.h"
#include "power_save_pub.h"

#define CAL_RESULT_TO_FLASH		           0
#define CAL_RESULT_FLASH_ADDR		       0x000F1000UL


#if CFG_SUPPORT_CALIBRATION
#define TX_WANTED_POWER_CAL                 0
#define TX_IQ_POWER_CAL                     1
#define TX_IQ_LOOPBACK_POWER_CAL            2
#define TX_DC_CAL                           0
#define TX_DC_LOOPBACK_CAL                  1
#define TX_GAIN_IMB_CAL                     0
#define TX_GAIN_LOOPBACK_IMB_CAL            1
#define TX_PHASE_IMB_CAL                    0
#define TX_PHASE_LOOPBACK_IMB_CAL           1

static INT32 gtx_dc_n = 0;//the times of first dc cal.
static UINT32 gst_rx_adc = CAL_DELAY100US;
static UINT32 gst_sar_adc = CAL_DELAY05US;

#define BK7231_56PIN_BOARD  1
#define BK7231_32PIN_BOARD  2
#define BK7231_32PIN_TZH_BOARD  3

#define BOARD  BK7231_32PIN_TZH_BOARD

#if (BOARD == BK7231_56PIN_BOARD)
#define TRX_REG_0XA_VAL         0x83703274
#define TRX_REG_0XB_VAL         0x20240077
#define TRX_REG_0XC_VAL         0x01A267EE
#define TRX_REG_0XD_VAL         0xDDF90339
#define TSSI_POUT_TH            0x50   //0x50:RF components for best power,0x43:for best sens
#else
#if (BOARD == BK7231_32PIN_BOARD)
#define TRX_REG_0XA_VAL         0x83703274
#define TRX_REG_0XB_VAL         0x20246077
#define TRX_REG_0XC_VAL         0x01A287EE
#define TRX_REG_0XD_VAL         0xDDF90339
#define TSSI_POUT_TH            0x80
#else
#if (BOARD == BK7231_32PIN_TZH_BOARD)
#define TRX_REG_0XA_VAL         0x83703274
#define TRX_REG_0XB_VAL         0x20248077
#define TRX_REG_0XC_VAL         0x01A287EE
#define TRX_REG_0XD_VAL         0xDDF90339
#define TSSI_POUT_TH            0x60
#endif
#endif
#endif

#if 1
#define TXIQ_IMB_TSSI_TH             105
#endif

#if 1
#define TXIQ_IMB_TSSI_TH_LOOPBACK             90
#endif

#define No_TXPOWERCAL   0
#if 1
#define TXIQ_TSSI_TH             0x40
#endif
#define TRX_REG_0XD_TX_IQ_VAL    0xD9FF0338   //0214 
#define TRX_REG_0XD_TX_LOOPBACK_IQ_VAL    0xD9FE7FF1   //0214 

#define TRX_REG_0XC_TXLO_LEAKAGE_VAL     0x1A28188

#define TRX_REG_0XC_RXIQ_VAL    0x1A245ED  // 1A244ED    //0215 increasse gain due to load antenna   01A244DD
#define TRX_REG_0XE_RXIQ_VAL    0xD9F87FF1   //0214  FDF87FF1


#if 1
#define gi_dc_tx_pa_dgainPA20  7
#define gi_dc_tx_pa_dgainbuf20  6
#define gi_gain_tx_pa_dgainPA20  2
#define gi_gain_tx_pa_dgainbuf20  4
#endif

#if 1
#define gi_dc_tx_loopback_pa_dgainPA20  7
#define gi_dc_tx_loopback_pa_dgainbuf20  7
#define gi_gain_tx_loopback_pa_dgainPA20  6
#define gi_gain_tx_loopback_pa_dgainbuf20  5
#define gi_phase_tx_loopback_pa_dgainPA20  6
#define gi_phase_tx_loopback_pa_dgainbuf20  6
#endif

#if 1
#define gi_cal_rx_iq_pa_dgainPA20  4
#define gi_cal_rx_iq_pa_dgainbuf20  5
#endif

#define CALIBRATE_TIMES     2000
#undef  CALIBRATE_TIMES         // by gwf

#ifdef CALIBRATE_TIMES
int calibrate_time = 0;

int *p_gbias_after_cal_array = NULL;
int *p_gav_tssi_array = NULL;
int *p_gtx_ifilter_corner_array = NULL;
int *p_gtx_qfilter_corner_array = NULL;
int *p_rx_amp_err_rd_array = NULL;
int *p_rx_phase_err_rd_array = NULL;
int *p_rx_ty2_rd_array = NULL;

int *p_gtx_i_dc_comp_array = NULL;
int *p_gtx_q_dc_comp_array = NULL;
int *p_gtx_i_gain_comp_array = NULL;
int *p_gtx_q_gain_comp_array = NULL;
int *p_gtx_phase_comp_array = NULL;

int *p_gtx_i_dc_comp_temp_array = NULL;
int *p_gtx_q_dc_comp_temp_array = NULL;
int *p_gtx_i_gain_comp_temp_array = NULL;
int *p_gtx_q_gain_comp_temp_array = NULL;
int *p_gtx_phase_comp_temp_array = NULL;

int *p_g_rx_dc_gain_tab_array = NULL;
#endif

static INT32 bk7011_trx_val[32] =
{
    0x0811EF5E,    // 0
    0x01000000,
    0x00000000,
    0x00000000,
    0x38025E88,
    0x2555EC7A,    // 5
    0x5FA44104,//0x5FA4410C,
    0x201848F5,//20170315 by yiming
    #if CFG_SYS_REDUCE_NORMAL_POWER
    0x076AA885,//0x076CA9C9 by wangjian
    #else
    0x076CA9C8,
    #endif
    0x000003AF,    // 9,from 3AF to 3BF ,add fixed 6dB gain
    TRX_REG_0XA_VAL,//0x83373330,
    TRX_REG_0XB_VAL,//0x4A444B24,
    TRX_REG_0XC_VAL,//0x01A187EE,   // 0x01A147FE is use for high temp calib
    0xDDF90339,// 0xD
    0xDA01BCF0,// 0xE
    0x00018000,// 0xF ADC sample rate clk 40M
    0xD0000000,
    0x00000000,
    0xF0090481,//0x12,from 0xF0090501 to 0xF0090481 ad/da LDO voltage reduced 50mv
    0x7B305ECC,
    0x827C827C,    // 0x14
    0x86788678,
    0x8C748C74,
    0xA45F9868,
    0xA45FA45F,
    0xA55EA45F,    // 0x19
    0xA55DA55E,
    0xA55DA55D,
    0x20000000     // 0x1C
};

static INT32 bk7011_rc_val[32] =
{
    0x00000009,//00 REG_0x0,  0x0000;
    0xF0000000,//01 REG_0x1,  0x0004;
    0x00000030,//02 REG_0x5,  0x0014;
    0x00010001,//03 REG_0x8,  0x0020;
    0x000100e0,//04 REG_0xB,  0x002C;
    0x00010070,//05 REG_0xE,  0x0038;
    0x00010001,//06 REG_0x11, 0x0044;
    0x00010005,//07 REG_0x19, 0x0064;
    0x00000002,//08 REG_0x1c, 0x0070;
    0x0000012c,//09 REG_0x1E, 0x0078;
    0x1002DF4B,//10 REG_0x3C, 0x00F0;
    0x40000000,//11 REG_0x3E, 0x00F8;   // enable comp bit
    0x03E803CB,//12 REG_0x3F, 0x00FC;
    0x00000001,//13 REG_0x40, 0x0100;
    0x00000000,//14 REG_0x41, 0x0104;
    0x02000041,//15 REG_0x42, 0x0108;
    0x018B018B,//16 REG_0x4C, 0x0130;//20170321
    0x2CC02000,//17 REG_0x4D, 0x0134;
    0x02000200,//18 REG_0x4F, 0x013C;
    0x03FF03FF,//19 REG_0x50, 0x0140;
    0x02000200,//20 REG_0x51, 0x0144;
    0x50079D40,//21 REG_0x52, 0x0148;  0x4A443B24   0x53479D40
    (0x00025600 | ((TSSI_POUT_TH) << 1)), //22 REG_0x54, 0x0150;  0x00025646-1us  2E600  20170503 reduce RX ADC  time 2D600
    0x00000000,//23 REG_0x55, 0x0154;
    0x80000064,//24 REG_0x5C, 0x0170;
};

//static UINT8 gstat_cal = 1;  // 1: calibration not done, 0: done

INT32 gconst_iqcal_p = 117;
INT32 gconst_pout = 0x210;
INT32 gav_tssi = 0;
INT32 gav_tssi_temp = 0;
INT32 grc_reg_map[32];
UINT8 gbias_after_cal = 0;

INT32 gtx_dcorMod = 0x4;
INT32 gtx_dcorPA = 0x8;
INT32 gtx_pre_gain = 0x7;

INT32 gtx_dcorMod_temp = 0x4;
INT32 gtx_dcorPA_temp = 0x8;
INT32 gtx_pre_gain_temp = 0x7;

INT32 gtx_dcorMod_temp_loopback = 0x4;
INT32 gtx_dcorPA_temp_loopback = 0x8;
INT32 gtx_pre_gain_temp_loopback = 0x7;

INT32 gtx_i_dc_comp_loopback = 0x202;
INT32 gtx_q_dc_comp_loopback = 0x1ff;

INT32 gtx_i_gain_comp_loopback = 0x3ff;
INT32 gtx_q_gain_comp_loopback = 0x3f3;

INT32 gtx_i_dc_comp = 0x202;
INT32 gtx_q_dc_comp = 0x1ff;

INT32 gtx_i_gain_comp = 0x3ff;
INT32 gtx_q_gain_comp = 0x3f3;

INT32 gtx_ifilter_corner = 0x10;
INT32 gtx_qfilter_corner = 0x10;

INT32 gtx_phase_comp_loopback = 0x200;
INT32 gtx_phase_ty2_loopback = 0x200;

INT32 gtx_phase_comp = 0x200;
INT32 gtx_phase_ty2 = 0x200;

INT32 gtx_power_cal_mode = TX_WANTED_POWER_CAL;
INT32 gtx_dc_cal_mode = TX_DC_CAL;
INT32 gtx_gain_imb_cal_mode = TX_GAIN_IMB_CAL;
INT32 gtx_phase_imb_cal_mode = TX_PHASE_IMB_CAL;

INT32 g_rx_dc_gain_tab[8] =
{
    0x827C827C,
    0x86788678,
    0x8C748C74,
    0xA45F9868,
    0xA45FA45F,
    0xA55EA45F,
    0xA55DA55E,
    0xA55DA55D
};

INT32 grx_amp_err_wr = 0x200;
INT32 grx_phase_err_wr = 0x041;

#define PWRI(gain, rb_28_31, rc_8_10, rc_4_6, rc_0_2, ra_8_11)      \
{                                     \
     .unuse      = 0,            \
     .pregain    = gain,         \
     .regb_28_31 = rb_28_31,     \
     .regc_8_10  = rc_8_10,      \
     .regc_4_6   = rc_4_6,       \
     .regc_0_2   = rc_0_2,       \
     .rega_8_11  = ra_8_11       \
} 

PWR_REGS cfg_tab_b[32] =                                                     
{                                                                            
    // pregain REGB<31:28> REGC<8:10> REGC<6:4> REGC<2:0> REGA<11:8>           
PWRI(  0x00 ,    0xF,         2,        3,         6,        2       ),   // 0 
PWRI(  0x0A ,    0xF,         2,        4,         6,        2       ),   // 1 
PWRI(  0x05 ,    0xF,         2,        4,         6,        2       ),   // 2 
PWRI(  0x00 ,    0xF,         2,        4,         6,        2       ),   // 3 
PWRI(  0x0B ,    0xF,         3,        4,         6,        2       ),   // 4 
PWRI(  0x07 ,    0xF,         3,        4,         6,        2       ),   // 5 
PWRI(  0x02 ,    0xF,         3,        4,         6,        2       ),   // 6 
PWRI(  0x07 ,    0xF,         3,        5,         6,        2       ),   // 7 
PWRI(  0x05 ,    0xF,         3,        5,         6,        2       ),   // 8 
PWRI(  0x02 ,    0xF,         3,        5,         6,        2       ),   // 9 
PWRI(  0x00 ,    0xF,         3,        5,         6,        2       ),   // 10
PWRI(  0x00 ,    0xF,         4,        5,         6,        4       ),   // 11
PWRI(  0x0B ,    0xF,         5,        5,         6,        4       ),   // 12
PWRI(  0x0A ,    0xF,         6,        6,         5,        4       ),   // 13
PWRI(  0x08 ,    0xF,         6,        6,         5,        4       ),   // 14
PWRI(  0x05 ,    0xF,         6,        6,         5,        4       ),   // 15
PWRI(  0x02 ,    0xF,         6,        6,         5,        4       ),   // 16
PWRI(  0x0E ,    0xF,         6,        6,         6,        4       ),   // 17
PWRI(  0x0C ,    0xF,         6,        6,         6,        4       ),   // 18
PWRI(  0x0A ,    0xF,         6,        6,         6,        4       ),   // 19
PWRI(  0x08 ,    0xF,         6,        6,         6,        4       ),   // 20
PWRI(  0x05 ,    0xF,         6,        6,         6,        4       ),   // 21
PWRI(  0x0C ,    0xF,         7,        6,         6,        4       ),   // 22
PWRI(  0x09 ,    0xF,         7,        6,         6,        4       ),   // 23
PWRI(  0x07 ,    0xF,         7,        6,         6,        4       ),   // 24
PWRI(  0x05 ,    0xF,         7,        6,         6,        4       ),   // 25
PWRI(  0x0E ,    0xF,         7,        6,         7,        5       ),   // 26
PWRI(  0x0B ,    0xF,         7,        6,         7,        5       ),   // 27
PWRI(  0x00 ,    0xF,         7,        6,         6,        7       ),   // 28
PWRI(  0x00 ,    0xF,         7,        7,         6,        7       ),   // 29
PWRI(  0x08 ,    0xF,         7,        7,         7,        7       ),   // 30
PWRI(  0x00 ,    0xF,         7,        7,         7,        7       ),   // 31
}; 

PWR_REGS cfg_tab_g[32] = {
    // pregain REGB<31:28> REGC<8:10> REGC<6:4> REGC<2:0> REGA<11:8>           
PWRI(  0xA ,      4,	        4,	      4,         5,        2      ),   // 0 
PWRI(  0x4 ,      4,	        4,	      4,         5,        2      ),   // 1 
PWRI(  0x2 ,      4,	        5,	      5,         4,        2      ),   // 2 
PWRI(  0x4 ,      4,	        6,	      5,         4,        2      ),   // 3 
PWRI(  0x0 ,      4,	        6,	      5,         4,        2      ),   // 4 
PWRI(  0xA ,      4,	        6,	      5,         5,        2      ),   // 5 
PWRI(  0x9 ,      4,	        6,	      5,         5,        2      ),   // 6 
PWRI(  0x7 ,      4,	        6,	      5,         5,        2      ),   // 7 
PWRI(  0x5 ,      4,	        6,	      5,         5,        2      ),   // 8 
PWRI(  0x3 ,      4,	        6,	      5,         5,        2      ),   // 9 
PWRI(  0x1 ,      4,	        6,	      5,         5,        2      ),   // 10
PWRI(  0x4 ,      4,	        7,	      5,         5,        2      ),   // 11
PWRI(  0x2 ,      4,	        7,	      5,         5,        2      ),   // 12
PWRI(  0x0 ,      4,	        7,	      5,         5,        2      ),   // 13
PWRI(  0x4 ,      4,	        7,	      6,         5,        2      ),   // 14
PWRI(  0x2 ,      4,	        7,	      6,         5,        2      ),   // 15
PWRI(  0x0 ,      4,	        7,	      6,         5,        2      ),   // 16
PWRI(  0xB ,      4,	        7,	      6,         6,        2      ),   // 17
PWRI(  0x8 ,      4,	        7,	      6,         6,        2      ),   // 18
PWRI(  0x5 ,      4,	        7,	      6,         6,        2      ),   // 19
PWRI(  0x3 ,      4,	        7,	      6,         6,        2      ),   // 20
PWRI(  0x7 ,      4,	        7,	      7,         6,        2      ),   // 21
PWRI(  0x4 ,      4,	        7,	      7,         6,        2      ),   // 22
PWRI(  0x2 ,      4,	        7,	      7,         6,        2      ),   // 23
PWRI(  0x0 ,      4,	        7,	      7,         6,        2      ),   // 24
PWRI(  0x2 ,      2,	        7,	      6,         6,        2      ),   // 25
PWRI(  0x4 ,      2,	        7,	      7,         6,        2      ),   // 26
PWRI(  0x2 ,      2,	        7,	      7,         6,        2      ),   // 27
PWRI(  0x0 ,      2,	        7,	      7,         6,        2      ),   // 28
PWRI(  0x6 ,      5,	        7,	      6,         6,        2      ),   // 29
PWRI(  0x0 ,      5,	        7,	      6,         6,        2      ),   // 30
PWRI(  0x0 ,      5,	        7,	      7,         6,        2      ),   // 31
};

struct BK7011RCBEKEN_TypeDef BK7011RCBEKEN =
{
    (volatile BK7011_RC_BEKEN_REG0x0_TypeDef *) (RC_BEKEN_BASE + 0 * 4),
    (volatile BK7011_RC_BEKEN_REG0x1_TypeDef *) (RC_BEKEN_BASE + 1 * 4),
    (volatile BK7011_RC_BEKEN_REG0x5_TypeDef *) (RC_BEKEN_BASE + 5 * 4),
    (volatile BK7011_RC_BEKEN_REG0x8_TypeDef *) (RC_BEKEN_BASE + 8 * 4),
    (volatile BK7011_RC_BEKEN_REG0xB_TypeDef *) (RC_BEKEN_BASE + 11 * 4),
    (volatile BK7011_RC_BEKEN_REG0xE_TypeDef *) (RC_BEKEN_BASE + 14 * 4),
    (volatile BK7011_RC_BEKEN_REG0x11_TypeDef *)(RC_BEKEN_BASE + 17 * 4),
    (volatile BK7011_RC_BEKEN_REG0x19_TypeDef *)(RC_BEKEN_BASE + 25 * 4),
    (volatile BK7011_RC_BEKEN_REG0x1C_TypeDef *)(RC_BEKEN_BASE + 28 * 4),
    (volatile BK7011_RC_BEKEN_REG0x1E_TypeDef *)(RC_BEKEN_BASE + 30 * 4),
    (volatile BK7011_RC_BEKEN_REG0x3C_TypeDef *)(RC_BEKEN_BASE + 60 * 4),
    (volatile BK7011_RC_BEKEN_REG0x3E_TypeDef *)(RC_BEKEN_BASE + 62 * 4),
    (volatile BK7011_RC_BEKEN_REG0x3F_TypeDef *)(RC_BEKEN_BASE + 63 * 4),
    (volatile BK7011_RC_BEKEN_REG0x40_TypeDef *)(RC_BEKEN_BASE + 64 * 4),
    (volatile BK7011_RC_BEKEN_REG0x41_TypeDef *)(RC_BEKEN_BASE + 65 * 4),
    (volatile BK7011_RC_BEKEN_REG0x42_TypeDef *)(RC_BEKEN_BASE + 66 * 4),
    (volatile BK7011_RC_BEKEN_REG0x4C_TypeDef *)(RC_BEKEN_BASE + 76 * 4),
    (volatile BK7011_RC_BEKEN_REG0x4D_TypeDef *)(RC_BEKEN_BASE + 77 * 4),
    (volatile BK7011_RC_BEKEN_REG0x4F_TypeDef *)(RC_BEKEN_BASE + 79 * 4),
    (volatile BK7011_RC_BEKEN_REG0x50_TypeDef *)(RC_BEKEN_BASE + 80 * 4),
    (volatile BK7011_RC_BEKEN_REG0x51_TypeDef *)(RC_BEKEN_BASE + 81 * 4),
    (volatile BK7011_RC_BEKEN_REG0x52_TypeDef *)(RC_BEKEN_BASE + 82 * 4),
    (volatile BK7011_RC_BEKEN_REG0x54_TypeDef *)(RC_BEKEN_BASE + 84 * 4),
    (volatile BK7011_RC_BEKEN_REG0x55_TypeDef *)(RC_BEKEN_BASE + 85 * 4),
    (volatile BK7011_RC_BEKEN_REG0x5C_TypeDef *)(RC_BEKEN_BASE + 92 * 4),
    (volatile BK7011_RC_BEKEN_REG0x6A_TypeDef *)(RC_BEKEN_BASE + 106 * 4),
};

struct BK7011TRxV2A_TypeDef BK7011TRX =
{
    (BK7011_TRxV2A_REG0x0_TypeDef *)(&grc_reg_map[0]),
    (BK7011_TRxV2A_REG0x1_TypeDef *)(&grc_reg_map[1]),
    (BK7011_TRxV2A_REG0x2_TypeDef *)(&grc_reg_map[2]),
    (BK7011_TRxV2A_REG0x3_TypeDef *)(&grc_reg_map[3]),
    (BK7011_TRxV2A_REG0x4_TypeDef *)(&grc_reg_map[4]),
    (BK7011_TRxV2A_REG0x5_TypeDef *)(&grc_reg_map[5]),
    (BK7011_TRxV2A_REG0x6_TypeDef *)(&grc_reg_map[6]),
    (BK7011_TRxV2A_REG0x7_TypeDef *)(&grc_reg_map[7]),
    (BK7011_TRxV2A_REG0x8_TypeDef *)(&grc_reg_map[8]),
    (BK7011_TRxV2A_REG0x9_TypeDef *)(&grc_reg_map[9]),
    (BK7011_TRxV2A_REG0xA_TypeDef *)(&grc_reg_map[10]),
    (BK7011_TRxV2A_REG0xB_TypeDef *)(&grc_reg_map[11]),
    (BK7011_TRxV2A_REG0xC_TypeDef *)(&grc_reg_map[12]),
    (BK7011_TRxV2A_REG0xD_TypeDef *)(&grc_reg_map[13]),
    (BK7011_TRxV2A_REG0xE_TypeDef *)(&grc_reg_map[14]),
    (BK7011_TRxV2A_REG0xF_TypeDef *)(&grc_reg_map[15]),
    (BK7011_TRxV2A_REG0x10_TypeDef *)(&grc_reg_map[16]),
    (BK7011_TRxV2A_REG0x11_TypeDef *)(&grc_reg_map[17]),
    (BK7011_TRxV2A_REG0x12_TypeDef *)(&grc_reg_map[18]),
    (BK7011_TRxV2A_REG0x13_TypeDef *)(&grc_reg_map[19]),
    (BK7011_TRxV2A_REG0x14_TypeDef *)(&grc_reg_map[20]),
    (BK7011_TRxV2A_REG0x15_TypeDef *)(&grc_reg_map[21]),
    (BK7011_TRxV2A_REG0x16_TypeDef *)(&grc_reg_map[22]),
    (BK7011_TRxV2A_REG0x17_TypeDef *)(&grc_reg_map[23]),
    (BK7011_TRxV2A_REG0x18_TypeDef *)(&grc_reg_map[24]),
    (BK7011_TRxV2A_REG0x19_TypeDef *)(&grc_reg_map[25]),
    (BK7011_TRxV2A_REG0x1A_TypeDef *)(&grc_reg_map[26]),
    (BK7011_TRxV2A_REG0x1B_TypeDef *)(&grc_reg_map[27]),
    (BK7011_TRxV2A_REG0x1C_TypeDef *)(&grc_reg_map[28]),
};

struct BK7011TRxV2A_TypeDef BK7011TRXONLY =
{
    (volatile BK7011_TRxV2A_REG0x0_TypeDef *) (TRX_BEKEN_BASE + 0 * 4),
    (volatile BK7011_TRxV2A_REG0x1_TypeDef *) (TRX_BEKEN_BASE + 1 * 4),
    (volatile BK7011_TRxV2A_REG0x2_TypeDef *) (TRX_BEKEN_BASE + 2 * 4),
    (volatile BK7011_TRxV2A_REG0x3_TypeDef *) (TRX_BEKEN_BASE + 3 * 4),
    (volatile BK7011_TRxV2A_REG0x4_TypeDef *) (TRX_BEKEN_BASE + 4 * 4),
    (volatile BK7011_TRxV2A_REG0x5_TypeDef *) (TRX_BEKEN_BASE + 5 * 4),
    (volatile BK7011_TRxV2A_REG0x6_TypeDef *) (TRX_BEKEN_BASE + 6 * 4),
    (volatile BK7011_TRxV2A_REG0x7_TypeDef *) (TRX_BEKEN_BASE + 7 * 4),
    (volatile BK7011_TRxV2A_REG0x8_TypeDef *) (TRX_BEKEN_BASE + 8 * 4),
    (volatile BK7011_TRxV2A_REG0x9_TypeDef *) (TRX_BEKEN_BASE + 9 * 4),
    (volatile BK7011_TRxV2A_REG0xA_TypeDef *) (TRX_BEKEN_BASE + 10 * 4),
    (volatile BK7011_TRxV2A_REG0xB_TypeDef *) (TRX_BEKEN_BASE + 11 * 4),
    (volatile BK7011_TRxV2A_REG0xC_TypeDef *) (TRX_BEKEN_BASE + 12 * 4),
    (volatile BK7011_TRxV2A_REG0xD_TypeDef *) (TRX_BEKEN_BASE + 13 * 4),
    (volatile BK7011_TRxV2A_REG0xE_TypeDef *) (TRX_BEKEN_BASE + 14 * 4),
    (volatile BK7011_TRxV2A_REG0xF_TypeDef *) (TRX_BEKEN_BASE + 15 * 4),
    (volatile BK7011_TRxV2A_REG0x10_TypeDef *)(TRX_BEKEN_BASE + 16 * 4),
    (volatile BK7011_TRxV2A_REG0x11_TypeDef *)(TRX_BEKEN_BASE + 17 * 4),
    (volatile BK7011_TRxV2A_REG0x12_TypeDef *)(TRX_BEKEN_BASE + 18 * 4),
    (volatile BK7011_TRxV2A_REG0x13_TypeDef *)(TRX_BEKEN_BASE + 19 * 4),
    (volatile BK7011_TRxV2A_REG0x14_TypeDef *)(TRX_BEKEN_BASE + 20 * 4),
    (volatile BK7011_TRxV2A_REG0x15_TypeDef *)(TRX_BEKEN_BASE + 21 * 4),
    (volatile BK7011_TRxV2A_REG0x16_TypeDef *)(TRX_BEKEN_BASE + 22 * 4),
    (volatile BK7011_TRxV2A_REG0x17_TypeDef *)(TRX_BEKEN_BASE + 23 * 4),
    (volatile BK7011_TRxV2A_REG0x18_TypeDef *)(TRX_BEKEN_BASE + 24 * 4),
    (volatile BK7011_TRxV2A_REG0x19_TypeDef *)(TRX_BEKEN_BASE + 25 * 4),
    (volatile BK7011_TRxV2A_REG0x1A_TypeDef *)(TRX_BEKEN_BASE + 26 * 4),
    (volatile BK7011_TRxV2A_REG0x1B_TypeDef *)(TRX_BEKEN_BASE + 27 * 4),
    (volatile BK7011_TRxV2A_REG0x1C_TypeDef *)(TRX_BEKEN_BASE + 28 * 4),
};

void bk7011_cal_pll(void);

void delay05us(INT32 num)
{
    volatile INT32 i, j;
    //REG_WRITE((0x0802800+(18*4)), 0x02);

    for(i = 0; i < num; i ++)
    {
        for(j = 0; j < 5; j ++)
            ;
    }
    //REG_WRITE((0x0802800+(18*4)), 0x00);
}

void delay100us(INT32 num)
{
    volatile INT32 i, j;
    //REG_WRITE((0x0802800+(18*4)), 0x02);

    for(i = 0; i < num; i ++)
    {
        for(j = 0; j < 1050; j ++)
            ;
    }
    //REG_WRITE((0x0802800+(18*4)), 0x00);
}


#define CAL_WR_TRXREGS(reg)    do{\
                                    while(BK7011RCBEKEN.REG0x1->value & (0x1 << reg));\
                                    BK7011TRXONLY.REG##reg->value = BK7011TRX.REG##reg->value;\
                                    cal_delay(6);\
                                    while(BK7011RCBEKEN.REG0x1->value & (0x1 << reg));\
                                }while(0)


void rwnx_cal_load_default_result(void)
{
    gtx_dcorMod = (bk7011_trx_val[11] >> 12) & 0xf;
    gtx_dcorPA = (bk7011_trx_val[12] >> 12) & 0xf;
    gtx_pre_gain = (bk7011_rc_val[21] >> 16) & 0x1f;

    gtx_i_dc_comp = (bk7011_rc_val[18] >> 16) & 0x3ff;
    gtx_q_dc_comp = bk7011_rc_val[18] & 0x3ff;

    gtx_i_gain_comp = (bk7011_rc_val[19] >> 16) & 0x3ff;
    gtx_q_gain_comp = bk7011_rc_val[19] & 0x3ff;

    gtx_ifilter_corner = (bk7011_trx_val[6] >> 10) & 0x3f;
    gtx_qfilter_corner = (bk7011_trx_val[6] >> 4) & 0x3f;
    gtx_phase_comp = (bk7011_rc_val[20] >> 16) & 0x3ff;
    gtx_phase_ty2 = bk7011_rc_val[20] & 0x3ff;

    g_rx_dc_gain_tab[0] = bk7011_trx_val[20];
    g_rx_dc_gain_tab[1] = bk7011_trx_val[21];
    g_rx_dc_gain_tab[2] = bk7011_trx_val[22];
    g_rx_dc_gain_tab[3] = bk7011_trx_val[23];
    g_rx_dc_gain_tab[4] = bk7011_trx_val[24];
    g_rx_dc_gain_tab[5] = bk7011_trx_val[25];
    g_rx_dc_gain_tab[6] = bk7011_trx_val[26];
    g_rx_dc_gain_tab[7] = bk7011_trx_val[27];

    grx_amp_err_wr = (bk7011_rc_val[15] >> 16) & 0x3ff;
    grx_phase_err_wr = bk7011_rc_val[15] & 0x3ff;

    //gstat_cal = (bk7011_rc_val[16] >> 29) & 0x1;
}

#ifdef CALIBRATE_TIMES
void calibration_result_print(void)
{
    int i, j;
    int max, min;


    os_printf("gtx_dcorMod_temp: 0x%x\r\n", gtx_dcorMod_temp);
    os_printf("gtx_dcorPA_temp: 0x%x\r\n", gtx_dcorPA_temp);
    os_printf("gtx_pre_gain_temp: 0x%x\r\n", gtx_pre_gain_temp);

    os_printf("gtx_dcorMod_temp_loopback: 0x%x\r\n", gtx_dcorMod_temp_loopback);
    os_printf("gtx_dcorPA_temp_loopback: 0x%x\r\n", gtx_dcorPA_temp_loopback);
    os_printf("gtx_pre_gain_temp_loopback: 0x%x\r\n", gtx_pre_gain_temp_loopback);

    max = -1100, min = 1100;
    for (i = 0; i < calibrate_time; i++)
    {
        if (max < p_gbias_after_cal_array[i])
        {
            max = p_gbias_after_cal_array[i];
        }
        if (min > p_gbias_after_cal_array[i])
        {
            min = p_gbias_after_cal_array[i];
        }
    }
    os_printf("gbias_after_cal: min = %d, max = %d, max-min = %d\r\n", min, max, max - min);

    max = -1100, min = 1100;
    for (i = 0; i < calibrate_time; i++)
    {
        if (max < p_gav_tssi_array[i])
        {
            max = p_gav_tssi_array[i];
        }
        if (min > p_gav_tssi_array[i])
        {
            min = p_gav_tssi_array[i];
        }
    }
    os_printf("gav_tssi: min = %d, max = %d, max-min = %d\r\n", min, max, max - min);

    os_printf("\r\n");

    max = -1100, min = 1100;
    for (i = 0; i < calibrate_time; i++)
    {
        if (max < p_gtx_ifilter_corner_array[i])
        {
            max = p_gtx_ifilter_corner_array[i];
        }
        if (min > p_gtx_ifilter_corner_array[i])
        {
            min = p_gtx_ifilter_corner_array[i];
        }
    }
    os_printf("gtx_ifilter_corner: min = %d, max = %d, max-min = %d\r\n", min, max, max - min);

    max = -1100, min = 1100;
    for (i = 0; i < calibrate_time; i++)
    {
        if (max < p_gtx_qfilter_corner_array[i])
        {
            max = p_gtx_qfilter_corner_array[i];
        }
        if (min > p_gtx_qfilter_corner_array[i])
        {
            min = p_gtx_qfilter_corner_array[i];
        }
    }
    os_printf("gtx_qfilter_corner: min = %d, max = %d, max-min = %d\r\n", min, max, max - min);

    os_printf("\r\n");

    max = -1100, min = 1100;
    for (i = 0; i < calibrate_time; i++)
    {
        if (max < p_gtx_i_dc_comp_array[i])
        {
            max = p_gtx_i_dc_comp_array[i];
        }
        if (min > p_gtx_i_dc_comp_array[i])
        {
            min = p_gtx_i_dc_comp_array[i];
        }
    }
    os_printf("tx_i_dc_comp: min = %d, max = %d, max-min = %d\r\n", min, max, max - min);

    max = -1100, min = 1100;
    for (i = 0; i < calibrate_time; i++)
    {
        if (max < p_gtx_q_dc_comp_array[i])
        {
            max = p_gtx_q_dc_comp_array[i];
        }
        if (min > p_gtx_q_dc_comp_array[i])
        {
            min = p_gtx_q_dc_comp_array[i];
        }
    }
    os_printf("tx_q_dc_comp: min = %d, max = %d, max-min = %d\r\n", min, max, max - min);

    max = -1100, min = 1100;
    for (i = 0; i < calibrate_time; i++)
    {
        if (max < p_gtx_i_gain_comp_array[i])
        {
            max = p_gtx_i_gain_comp_array[i];
        }
        if (min > p_gtx_i_gain_comp_array[i])
        {
            min = p_gtx_i_gain_comp_array[i];
        }
    }
    os_printf("tx_i_gain_comp: min = %d, max = %d, max-min = %d\r\n", min, max, max - min);

    max = -1100, min = 1100;
    for (i = 0; i < calibrate_time; i++)
    {
        if (max < p_gtx_q_gain_comp_array[i])
        {
            max = p_gtx_q_gain_comp_array[i];
        }
        if (min > p_gtx_q_gain_comp_array[i])
        {
            min = p_gtx_q_gain_comp_array[i];
        }
    }
    os_printf("tx_q_gain_comp: min = %d, max = %d, max-min = %d\r\n", min, max, max - min);

    max = -1100, min = 1100;
    for (i = 0; i < calibrate_time; i++)
    {
        if (max < p_gtx_phase_comp_array[i])
        {
            max = p_gtx_phase_comp_array[i];
        }
        if (min > p_gtx_phase_comp_array[i])
        {
            min = p_gtx_phase_comp_array[i];
        }
    }
    os_printf("tx_phase_comp: min = %d, max = %d, max-min = %d\r\n", min, max, max - min);

    os_printf("\r\n");

    max = -1100, min = 1100;
    for (i = 0; i < calibrate_time; i++)
    {
        if (max < p_gtx_i_dc_comp_temp_array[i])
        {
            max = p_gtx_i_dc_comp_temp_array[i];
        }
        if (min > p_gtx_i_dc_comp_temp_array[i])
        {
            min = p_gtx_i_dc_comp_temp_array[i];
        }
    }
    os_printf("tx_i_dc_comp_temp: min = %d, max = %d, max-min = %d\r\n", min, max, max - min);

    max = -1100, min = 1100;
    for (i = 0; i < calibrate_time; i++)
    {
        if (max < p_gtx_q_dc_comp_temp_array[i])
        {
            max = p_gtx_q_dc_comp_temp_array[i];
        }
        if (min > p_gtx_q_dc_comp_temp_array[i])
        {
            min = p_gtx_q_dc_comp_temp_array[i];
        }
    }
    os_printf("tx_q_dc_comp_temp: min = %d, max = %d, max-min = %d\r\n", min, max, max - min);


    max = -1100, min = 1100;
    for (i = 0; i < calibrate_time; i++)
    {
        if (max < p_gtx_i_gain_comp_temp_array[i])
        {
            max = p_gtx_i_gain_comp_temp_array[i];
        }
        if (min > p_gtx_i_gain_comp_temp_array[i])
        {
            min = p_gtx_i_gain_comp_temp_array[i];
        }
    }
    os_printf("tx_i_gain_comp_temp: min = %d, max = %d, max-min = %d\r\n", min, max, max - min);

    max = -1100, min = 1100;
    for (i = 0; i < calibrate_time; i++)
    {
        if (max < p_gtx_q_gain_comp_temp_array[i])
        {
            max = p_gtx_q_gain_comp_temp_array[i];
        }
        if (min > p_gtx_q_gain_comp_temp_array[i])
        {
            min = p_gtx_q_gain_comp_temp_array[i];
        }
    }
    os_printf("tx_q_gain_comp_temp: min = %d, max = %d, max-min = %d\r\n", min, max, max - min);

    max = -1100, min = 1100;
    for (i = 0; i < calibrate_time; i++)
    {
        if (max < p_gtx_phase_comp_temp_array[i])
        {
            max = p_gtx_phase_comp_temp_array[i];
        }
        if (min > p_gtx_phase_comp_temp_array[i])
        {
            min = p_gtx_phase_comp_temp_array[i];
        }
    }
    os_printf("tx_phase_comp_temp: min = %d, max = %d, max-min = %d\r\n", min, max, max - min);

    os_printf("\r\n");

    max = -1100, min = 1100;
    for (i = 0; i < calibrate_time; i++)
    {
        if (max < p_rx_amp_err_rd_array[i])
        {
            max = p_rx_amp_err_rd_array[i];
        }
        if (min > p_rx_amp_err_rd_array[i])
        {
            min = p_rx_amp_err_rd_array[i];
        }
    }
    os_printf("rx_amp_err_rd: min = %d, max = %d, max-min = %d\r\n", min, max, max - min);

    max = -1100, min = 1100;
    for (i = 0; i < calibrate_time; i++)
    {
        if (max < p_rx_phase_err_rd_array[i])
        {
            max = p_rx_phase_err_rd_array[i];
        }
        if (min > p_rx_phase_err_rd_array[i])
        {
            min = p_rx_phase_err_rd_array[i];
        }
    }
    os_printf("rx_phase_err_rd: min = %d, max = %d, max-min = %d\r\n", min, max, max - min);

    max = -1100, min = 1100;
    for (i = 0; i < calibrate_time; i++)
    {
        if (max < p_rx_ty2_rd_array[i])
        {
            max = p_rx_ty2_rd_array[i];
        }
        if (min > p_rx_ty2_rd_array[i])
        {
            min = p_rx_ty2_rd_array[i];
        }
    }
    os_printf("rx_ty2_rd: min = %d, max = %d, max-min = %d\r\n", min, max, max - min);

    for (j = 0; j < 8; j++)
    {
        int iTemp;
        os_printf("\r\n");

        max = -1100, min = 1100;
        for (i = 0; i < calibrate_time; i++)
        {
            iTemp = p_g_rx_dc_gain_tab_array[j * CALIBRATE_TIMES + i];
            if (max < (iTemp & 0x00FF))
            {
                max = iTemp & 0x00FF;
            }
            if (min > (iTemp & 0x00FF))
            {
                min = iTemp & 0x00FF;
            }
        }
        os_printf("g_rx_dc_gain_tab[%d].i_%ddb: min = %d, max = %d, max-min = %d\r\n", j, j * 6, min, max, max - min);

        max = -1100, min = 1100;
        for (i = 0; i < calibrate_time; i++)
        {
            iTemp = p_g_rx_dc_gain_tab_array[j * CALIBRATE_TIMES + i] >> 8;
            if (max < (iTemp & 0x00FF))
            {
                max = iTemp & 0x00FF;
            }
            if (min > (iTemp & 0x00FF))
            {
                min = iTemp & 0x00FF;
            }
        }
        os_printf("g_rx_dc_gain_tab[%d].q_%ddb: min = %d, max = %d, max-min = %d\r\n", j, j * 6, min, max, max - min);

        max = -1100, min = 1100;
        for (i = 0; i < calibrate_time; i++)
        {
            iTemp = p_g_rx_dc_gain_tab_array[j * CALIBRATE_TIMES + i] >> 16;
            if (max < (iTemp & 0x00FF))
            {
                max = iTemp & 0x00FF;
            }
            if (min > (iTemp & 0x00FF))
            {
                min = iTemp & 0x00FF;
            }
        }
        os_printf("g_rx_dc_gain_tab[%d].i_%ddb: min = %d, max = %d, max-min = %d\r\n", j, j * 6 + 3, min, max, max - min);

        max = -1100, min = 1100;
        for (i = 0; i < calibrate_time; i++)
        {
            iTemp = p_g_rx_dc_gain_tab_array[j * CALIBRATE_TIMES + i] >> 24;
            if (max < (iTemp & 0x00FF))
            {
                max = iTemp & 0x00FF;
            }
            if (min > (iTemp & 0x00FF))
            {
                min = iTemp & 0x00FF;
            }
        }
        os_printf("g_rx_dc_gain_tab[%d].q_%ddb: min = %d, max = %d, max-min = %d\r\n", j, j * 6 + 3, min, max, max - min);
    }
}

void calibration_auto_test(void)
{
    int i;
    int gbias_after_cal_array[CALIBRATE_TIMES];
    int gav_tssi_array[CALIBRATE_TIMES];
    int rx_amp_err_rd_array[CALIBRATE_TIMES];
    int rx_phase_err_rd_array[CALIBRATE_TIMES];
    int rx_ty2_rd_array[CALIBRATE_TIMES];
    int gtx_ifilter_corner_array[CALIBRATE_TIMES];
    int gtx_qfilter_corner_array[CALIBRATE_TIMES];

    int gtx_i_dc_comp_array[CALIBRATE_TIMES];
    int gtx_q_dc_comp_array[CALIBRATE_TIMES];
    int gtx_i_gain_comp_array[CALIBRATE_TIMES];
    int gtx_q_gain_comp_array[CALIBRATE_TIMES];
    int gtx_phase_comp_array[CALIBRATE_TIMES];

    int gtx_i_dc_comp_temp_array[CALIBRATE_TIMES];
    int gtx_q_dc_comp_temp_array[CALIBRATE_TIMES];
    int gtx_i_gain_comp_temp_array[CALIBRATE_TIMES];
    int gtx_q_gain_comp_temp_array[CALIBRATE_TIMES];
    int gtx_phase_comp_temp_array[CALIBRATE_TIMES];

    int g_rx_dc_gain_tab_array[8][CALIBRATE_TIMES];

    p_gbias_after_cal_array = gbias_after_cal_array;
    p_gav_tssi_array = gav_tssi_array;
    p_rx_amp_err_rd_array = rx_amp_err_rd_array;
    p_rx_ty2_rd_array = rx_ty2_rd_array;
    p_rx_phase_err_rd_array = rx_phase_err_rd_array;
    p_gtx_ifilter_corner_array = gtx_ifilter_corner_array;
    p_gtx_qfilter_corner_array = gtx_qfilter_corner_array;
    p_gtx_i_dc_comp_array = gtx_i_dc_comp_array;
    p_gtx_q_dc_comp_array = gtx_q_dc_comp_array;
    p_gtx_i_gain_comp_array = gtx_i_gain_comp_array;
    p_gtx_q_gain_comp_array = gtx_q_gain_comp_array;
    p_gtx_phase_comp_array = gtx_phase_comp_array;
    p_gtx_i_dc_comp_temp_array = gtx_i_dc_comp_temp_array;
    p_gtx_q_dc_comp_temp_array = gtx_q_dc_comp_temp_array;
    p_gtx_i_gain_comp_temp_array = gtx_i_gain_comp_temp_array;
    p_gtx_q_gain_comp_temp_array = gtx_q_gain_comp_temp_array;
    p_gtx_phase_comp_temp_array = gtx_phase_comp_temp_array;
    p_g_rx_dc_gain_tab_array = &(g_rx_dc_gain_tab_array[0][0]);

    calibrate_time = 0;
    os_printf("\r\n\r\n***********************************************************************\r\n"
              "calibration_auto_test: \r\n");
    for (i = 0; i < CALIBRATE_TIMES; i++)
    {
        calibration_main();
    }
    calibration_result_print();
    os_printf("***********************************************************************\r\n");

    p_gbias_after_cal_array = NULL;
    p_gav_tssi_array = NULL;
    p_rx_amp_err_rd_array = NULL;
    p_rx_phase_err_rd_array = NULL;
    p_rx_ty2_rd_array = NULL;
    p_gtx_i_dc_comp_array = NULL;
    p_gtx_q_dc_comp_array = NULL;
    p_gtx_i_gain_comp_array = NULL;
    p_gtx_q_gain_comp_array = NULL;
    p_gtx_phase_comp_array = NULL;
    p_gtx_ifilter_corner_array = NULL;
    p_gtx_qfilter_corner_array = NULL;
    p_gtx_i_dc_comp_temp_array = NULL;
    p_gtx_q_dc_comp_temp_array = NULL;
    p_gtx_i_gain_comp_temp_array = NULL;
    p_gtx_q_gain_comp_temp_array = NULL;
    p_gtx_phase_comp_temp_array = NULL;
    p_g_rx_dc_gain_tab_array = NULL;
}
#else
void calibration_auto_test(void)
{
}
#endif

void rwnx_cal_read_current_cal_result(void)
{
#ifdef CALIBRATE_TIMES
    if (p_gtx_phase_comp_array != NULL)
    {
        p_gbias_after_cal_array[calibrate_time] = gbias_after_cal;
        p_gav_tssi_array[calibrate_time] = gav_tssi;
        p_gtx_phase_comp_array[calibrate_time] = gtx_phase_comp;
        p_gtx_i_dc_comp_array[calibrate_time] = gtx_i_dc_comp;
        p_gtx_q_dc_comp_array[calibrate_time] = gtx_q_dc_comp;
        p_gtx_i_gain_comp_array[calibrate_time] = gtx_i_gain_comp;
        p_gtx_q_gain_comp_array[calibrate_time] = gtx_q_gain_comp;
        p_gtx_ifilter_corner_array[calibrate_time] = gtx_ifilter_corner;
        p_gtx_qfilter_corner_array[calibrate_time] = gtx_qfilter_corner;

        p_g_rx_dc_gain_tab_array[0 * CALIBRATE_TIMES + calibrate_time] = g_rx_dc_gain_tab[0];
        p_g_rx_dc_gain_tab_array[1 * CALIBRATE_TIMES + calibrate_time] = g_rx_dc_gain_tab[1];
        p_g_rx_dc_gain_tab_array[2 * CALIBRATE_TIMES + calibrate_time] = g_rx_dc_gain_tab[2];
        p_g_rx_dc_gain_tab_array[3 * CALIBRATE_TIMES + calibrate_time] = g_rx_dc_gain_tab[3];
        p_g_rx_dc_gain_tab_array[4 * CALIBRATE_TIMES + calibrate_time] = g_rx_dc_gain_tab[4];
        p_g_rx_dc_gain_tab_array[5 * CALIBRATE_TIMES + calibrate_time] = g_rx_dc_gain_tab[5];
        p_g_rx_dc_gain_tab_array[6 * CALIBRATE_TIMES + calibrate_time] = g_rx_dc_gain_tab[6];
        p_g_rx_dc_gain_tab_array[7 * CALIBRATE_TIMES + calibrate_time] = g_rx_dc_gain_tab[7];
        calibrate_time ++;
    }
    else
#endif
    {
        CAL_FATAL("*********** finally result **********\r\n");
        CAL_FATAL("gbias_after_cal: 0x%x\r\n", gbias_after_cal);
        CAL_FATAL("gav_tssi: 0x%x\r\n", gav_tssi);
        CAL_FATAL("gtx_q_dc_comp:0x%x\r\n", gtx_q_dc_comp);
        CAL_FATAL("gtx_i_dc_comp:0x%x\r\n", gtx_i_dc_comp);
        CAL_FATAL("gtx_i_gain_comp:%d\r\n", gtx_i_gain_comp);
        CAL_FATAL("gtx_q_gain_comp:%d\r\n", gtx_q_gain_comp);
        CAL_FATAL("gtx_phase_comp:%d\r\n", gtx_phase_comp);
        CAL_FATAL("gtx_phase_ty2:%d\r\n", gtx_phase_ty2);
        CAL_FATAL("gtx_ifilter_corner over: 0x%x\r\n", gtx_ifilter_corner);
        CAL_FATAL("gtx_qfilter_corner over: 0x%x\r\n", gtx_qfilter_corner);
        CAL_FATAL("gtx_dcorMod:0x%x, gtx_dcorPA:0x%x\r\n", gtx_dcorMod, gtx_dcorPA);
        CAL_FATAL("gtx_pre_gain:0x%x\r\n", gtx_pre_gain);
        CAL_FATAL("g_rx_dc_gain_tab 0 over: 0x%x\r\n", g_rx_dc_gain_tab[0]);
        CAL_FATAL("g_rx_dc_gain_tab 1 over: 0x%x\r\n", g_rx_dc_gain_tab[1]);
        CAL_FATAL("g_rx_dc_gain_tab 2 over: 0x%x\r\n", g_rx_dc_gain_tab[2]);
        CAL_FATAL("g_rx_dc_gain_tab 3 over: 0x%x\r\n", g_rx_dc_gain_tab[3]);
        CAL_FATAL("g_rx_dc_gain_tab 4 over: 0x%x\r\n", g_rx_dc_gain_tab[4]);
        CAL_FATAL("g_rx_dc_gain_tab 5 over: 0x%x\r\n", g_rx_dc_gain_tab[5]);
        CAL_FATAL("g_rx_dc_gain_tab 6 over: 0x%x\r\n", g_rx_dc_gain_tab[6]);
        CAL_FATAL("g_rx_dc_gain_tab 7 over: 0x%x\r\n", g_rx_dc_gain_tab[7]);

        CAL_FATAL("grx_amp_err_wr:0x%03x\r\n", grx_amp_err_wr);
        CAL_FATAL("grx_phase_err_wr:0x%03x\r\n", grx_phase_err_wr);
        CAL_FATAL("**************************************\r\n");
    }
}

#if CFG_SUPPORT_MANUAL_CALI
static UINT32 rwnx_cal_translate_tx_rate(UINT32 rate)
{
    UINT32 param;

    switch(rate)
    {
    case 0 :
        param = 1;
        break;  // 1Mbps
    case 1 :
        param = 2;
        break;  // 2Mbps
    case 2 :
        param = 5;
        break;	// 5.5Mbps
    case 3:
        param = 11;
        break;	// 11Mbps
    case 4:
        param = 6;
        break;	// 6Mbps
    case 5 :
        param = 9;
        break;	// 9Mbps
    case 6:
        param = 12;
        break;	// 12Mbps
    case 7:
        param = 18;
        break;	// 18Mbps
    case 8:
        param = 24;
        break;	// 24Mbps
    case 9:
        param = 36;
        break;	// 36Mbps
    case 10:
        param = 48;
        break;	// 48Mbps
    case 11:
        param = 54;
        break;	// 54Mbps
    default:
        param = rate;
        break;	// 54Mbps
    }

    return param;
}

static UINT32 rwnx_cal_translate_tx_rate_for_n(UINT32 rate, UINT32 bandwidth)
{
    UINT32 param;
   
    switch(rate)
    {
    case 128 :
    case 129 :
    case 130 :
    case 131 :
    case 132 :
    case 133 :        
        param = rate - 123;
        break;	// MCS0-5 the same rate indx as 9M to 36M
		
    case 134:
    case 135:
        if(bandwidth == PHY_CHNL_BW_20)
            param = 11;  // MCS6-7the same rate indx as to 54M  - band:20M
        else 
            param = 12;  // MCS6-7the same rate indx as to 135M - band:40M
        break;	
    
    default:
        param = rate;
        break;	
    }

    return param;
}
#endif

//static INT32 cur_rate;
void rwnx_cal_set_txpwr_by_rate(INT32 rate, UINT32 test_mode)
{
    UINT32 ret;
    UINT32 pwr_gain;

#if CFG_SUPPORT_MANUAL_CALI 
    struct phy_channel_info info;
    UINT32 channel, bandwidth;   // PHY_CHNL_BW_20,  PHY_CHNL_BW_40:

    phy_get_channel(&info, 0);
    bandwidth = (info.info1 >> 8) & 0xff;
    
    channel = (BK7011TRXONLY.REG0x7->bits.chin60 - 7)/5;
    if(channel > 14)
        channel = 14;    
    if(!manual_cal_get_txpwr(rwnx_cal_translate_tx_rate(rate), 
        channel, bandwidth, &pwr_gain)) 
    {
        // unable get txpwr from manual cal
        return;
    }

    ret = manual_cal_get_pwr_idx_shift(rate, bandwidth, &pwr_gain);
    if(!ret ){
        // unable get txpwr from manual cal
        return;
    } else if(ret == 1) {
        rwnx_cal_set_txpwr(pwr_gain, EVM_DEFUALT_B_RATE);
    } else if(ret == 2) {
        rwnx_cal_set_txpwr(pwr_gain, EVM_DEFUALT_RATE);
    }
    
    if(test_mode)
    {
        os_printf("add extral movement in test\r\n"); 

         #if CFG_USE_TEMPERATURE_DETECT
         temp_detect_uninit();
         #endif
        
        BK7011TRX.REG0x6->bits.lpfcapcalq50 = 0x3f;
        BK7011TRX.REG0x6->bits.lpfcapcali50 = 0x3f; 
        CAL_WR_TRXREGS(0x6);

        BK7011TRX.REG0xB->value = 0xF0248033;
        CAL_WR_TRXREGS(0xB);   

        BK7011TRX.REG0xC->value = 0x1A287CC;
        CAL_WR_TRXREGS(0xC); 

        BK7011TRX.REG0x13->bits.icpoff20 = 0;
        CAL_WR_TRXREGS(0x13);  

        if(rate <= 3) {
            BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = ((bk7011_rc_val[18]>>16) & 0x3ff) + 0x20;
            BK7011RCBEKEN.REG0x52->bits.TXPREGAIN = 0;
        }
    } else {
        BK7011TRX.REG0x6->value = bk7011_trx_val[6];
        CAL_WR_TRXREGS(0x6);

        BK7011TRX.REG0xB->value = bk7011_trx_val[11];
        CAL_WR_TRXREGS(0xB);   

        BK7011TRX.REG0xC->value = bk7011_trx_val[12];
        CAL_WR_TRXREGS(0xC); 

        BK7011TRX.REG0x13->value = bk7011_trx_val[19];
        CAL_WR_TRXREGS(0x13); 

        BK7011RCBEKEN.REG0x4F->value = bk7011_rc_val[18];

        BK7011RCBEKEN.REG0x52->value = bk7011_rc_val[21];
    }
#endif
}

#if CFG_SUPPORT_MANUAL_CALI
void rwnx_cal_set_txpwr(UINT32 pwr_gain, UINT32 grate)
{
    const PWR_REGS *pcfg;

    if(pwr_gain > 32) {
        os_printf("set_txpwr unknow pwr idx:%d \r\n", pwr_gain); 
        return;
    }

    if(grate == EVM_DEFUALT_B_RATE) {
    // for b
        pcfg = cfg_tab_b + pwr_gain;
    } else if(grate == EVM_DEFUALT_RATE) {
    // for g
        pcfg = cfg_tab_g + pwr_gain;
    } else {
        os_printf("set_txpwr unknow rate:%d \r\n", grate);  
        return;
    }

    os_printf("idx:%02d,r:%03d- pg:%02x, %01x, %01x, %01x, %01x, %01x\r\n", pwr_gain, grate,
        pcfg->pregain, pcfg->regb_28_31, pcfg->regc_8_10,pcfg->regc_4_6, pcfg->regc_0_2, pcfg->rega_8_11);

    BK7011RCBEKEN.REG0x52->bits.TXPREGAIN = gtx_pre_gain = pcfg->pregain;
    bk7011_rc_val[21] = BK7011RCBEKEN.REG0x52->value;

    BK7011TRX.REG0xA->bits.dbpaa30 = pcfg->rega_8_11;
    CAL_WR_TRXREGS(0xA);
    bk7011_trx_val[10] = BK7011TRXONLY.REG0xA->value ;

    BK7011TRX.REG0xB->bits.gctrlmod30 = pcfg->regb_28_31;
    CAL_WR_TRXREGS(0xB);
    bk7011_trx_val[11] = BK7011TRXONLY.REG0xB->value ;

    BK7011TRX.REG0xC->bits.gctrlpga20 = pcfg->regc_0_2;
    BK7011TRX.REG0xC->bits.dgainbuf20 = pcfg->regc_4_6;
    BK7011TRX.REG0xC->bits.dgainPA20 = pcfg->regc_8_10;     
    CAL_WR_TRXREGS(0xC);
    bk7011_trx_val[12] = BK7011TRXONLY.REG0xC->value ;    
}   

void rwnx_cal_set_reg_mod_pa(UINT16 reg_mod, UINT16 reg_pa)
{
    CHECK_OPERATE_RF_REG_IF_IN_SLEEP();

    gtx_dcorMod = (INT32)reg_mod,
    gtx_dcorPA = (INT32)reg_pa;
    BK7011TRXONLY.REG0xB->bits.dcorMod30 = gtx_dcorMod;
    BK7011TRXONLY.REG0xC->bits.dcorPA30 = gtx_dcorPA;    
    bk7011_trx_val[11] = BK7011TRXONLY.REG0xB->value;
    bk7011_trx_val[12] = BK7011TRXONLY.REG0xC->value; 

    CHECK_OPERATE_RF_REG_IF_IN_SLEEP_END();
    
}
#endif

#if CFG_USE_TEMPERATURE_DETECT
void rwnx_cal_do_temp_detect(UINT16 cur_val, UINT16 thre, UINT16 *last)
{
    TMP_PWR_PTR tmp_pwr_ptr;
    tmp_pwr_ptr = manual_cal_set_tmp_pwr(cur_val, thre, last);
    if(tmp_pwr_ptr) 
    {
        rwnx_cal_set_reg_mod_pa(tmp_pwr_ptr->mod, tmp_pwr_ptr->pa);
    }
}
#endif // CFG_USE_TEMPERATURE_DETECT

void rwnx_tx_cal_save_cal_result(void)
{
    // Manual calibration not used PA & MOD, but use pre_gain
    #if CFG_SUPPORT_MANUAL_CALI 
    gtx_dcorMod = 0x8;
    gtx_dcorPA = 0x8;
    #if CFG_USE_TEMPERATURE_DETECT
    manual_cal_tmp_pwr_init_reg(gtx_dcorMod, gtx_dcorPA);
    #endif  // CFG_USE_TEMPERATURE_DETECT
    #endif

    bk7011_trx_val[11] = (bk7011_trx_val[11] & (~(0xf << 12))) | (((0xf)&gtx_dcorMod) << 12);
    bk7011_trx_val[12] = (bk7011_trx_val[12] & (~(0xf << 12))) | (((0xf)&gtx_dcorPA) << 12);
    bk7011_rc_val[21] = (bk7011_rc_val[21] & (~(0x1f << 16))) | (((0x1f)&gtx_pre_gain) << 16);

    bk7011_rc_val[18] = (bk7011_rc_val[18] & (~(0x3ff << 16))) | (((0x3ff)&gtx_i_dc_comp) << 16);
    bk7011_rc_val[18] = (bk7011_rc_val[18] & (~0x3ff)) | ((0x3ff)&gtx_q_dc_comp);

    bk7011_rc_val[19] = (bk7011_rc_val[19] & (~(0x3ff << 16))) | (((0x3ff)&gtx_i_gain_comp) << 16);
    bk7011_rc_val[19] = (bk7011_rc_val[19] & (~0x3ff)) | ((0x3ff)&gtx_q_gain_comp);

    bk7011_trx_val[6] = (bk7011_trx_val[6] & (~(0x3f << 10))) | (((0x3f)&gtx_ifilter_corner) << 10);
    bk7011_trx_val[6] = (bk7011_trx_val[6] & (~(0x3f << 4))) | (((0x3f)&gtx_qfilter_corner) << 4);

    bk7011_rc_val[20] = (bk7011_rc_val[20] & (~(0x3ff << 16))) | (((0x3ff)&gtx_phase_comp) << 16);
    bk7011_rc_val[20] = (bk7011_rc_val[20] & (~0x3ff)) | ((0x3ff)&gtx_phase_ty2);

    //if(gstat_cal)
    //bk7011_rc_val[16] = bk7011_rc_val[16] | (1 << 29);
    //else
    //bk7011_rc_val[16] = bk7011_rc_val[16] & (~(1 << 29));
}

void rwnx_rx_cal_save_cal_result(void)
{
    bk7011_trx_val[20] = g_rx_dc_gain_tab[0];
    bk7011_trx_val[21] = g_rx_dc_gain_tab[1];
    bk7011_trx_val[22] = g_rx_dc_gain_tab[2];
    bk7011_trx_val[23] = g_rx_dc_gain_tab[3];
    bk7011_trx_val[24] = g_rx_dc_gain_tab[4];
    bk7011_trx_val[25] = g_rx_dc_gain_tab[5];
    bk7011_trx_val[26] = g_rx_dc_gain_tab[6];
    bk7011_trx_val[27] = g_rx_dc_gain_tab[7];

    bk7011_rc_val[15] = (bk7011_rc_val[15] & (~(0x3ff << 16))) | (((0x3ff)&grx_amp_err_wr) << 16);
    bk7011_rc_val[15] = (bk7011_rc_val[15] & (~0x3ff)) | ((0x3ff)&grx_phase_err_wr);
}

/*******************************************************************************
* Function Implemantation
*******************************************************************************/
void bk7011_read_cal_param(void)
{
    gtx_dc_n = (BK7011RCBEKEN.REG0x54->bits.TXDCN & 0x03) + 2;
    gst_sar_adc = ((BK7011RCBEKEN.REG0x54->bits.STSARADC & 0x03) + 1) * CAL_DELAY05US;
    gst_rx_adc = ((BK7011RCBEKEN.REG0x54->bits.STRXADC & 0x03) + 1) *  CAL_DELAY100US;
    gconst_iqcal_p = BK7011RCBEKEN.REG0x52->bits.IQCONSTANTIQCALP - 512;
    gconst_iqcal_p =  abs(gconst_iqcal_p);
    gconst_pout = BK7011RCBEKEN.REG0x52->bits.IQCONSTANTPOUT;

    return;
}

INT32 rwnx_cal_load_trx_rcbekn_reg_val(void)
{
#if (CFG_RUNNING_PLATFORM == FPGA_PLATFORM)

#else
    BK7011RCBEKEN.REG0x0->value  = bk7011_rc_val[0];
    BK7011RCBEKEN.REG0x1->value  = bk7011_rc_val[1];
    BK7011RCBEKEN.REG0x5->value  = bk7011_rc_val[2];
    BK7011RCBEKEN.REG0x8->value  = bk7011_rc_val[3];
    BK7011RCBEKEN.REG0xB->value  = bk7011_rc_val[4];
    BK7011RCBEKEN.REG0xE->value  = bk7011_rc_val[5];
    BK7011RCBEKEN.REG0x11->value = bk7011_rc_val[6];
    BK7011RCBEKEN.REG0x19->value = bk7011_rc_val[7];
    BK7011RCBEKEN.REG0x1C->value = bk7011_rc_val[8];
    BK7011RCBEKEN.REG0x0->value  = bk7011_rc_val[0];
    BK7011RCBEKEN.REG0x1E->value = bk7011_rc_val[9];

    /**********NEW ADDED************/
    BK7011RCBEKEN.REG0x3C->value = bk7011_rc_val[10];
    BK7011RCBEKEN.REG0x3E->value = bk7011_rc_val[11];
    BK7011RCBEKEN.REG0x3F->value = bk7011_rc_val[12];
    BK7011RCBEKEN.REG0x40->value = bk7011_rc_val[13];
    BK7011RCBEKEN.REG0x41->value = bk7011_rc_val[14];
    BK7011RCBEKEN.REG0x42->value = bk7011_rc_val[15];
    BK7011RCBEKEN.REG0x4C->value = bk7011_rc_val[16];
    BK7011RCBEKEN.REG0x4D->value = bk7011_rc_val[17];
    BK7011RCBEKEN.REG0x4F->value = bk7011_rc_val[18];
    BK7011RCBEKEN.REG0x50->value = bk7011_rc_val[19];
    BK7011RCBEKEN.REG0x51->value = bk7011_rc_val[20];
    BK7011RCBEKEN.REG0x52->value = bk7011_rc_val[21];
    BK7011RCBEKEN.REG0x54->value = bk7011_rc_val[22];
    BK7011RCBEKEN.REG0x55->value = bk7011_rc_val[23];
    BK7011RCBEKEN.REG0x5C->value = bk7011_rc_val[24];

    //BK7011RCBEKEN.REG0x3C->bits.RXIQSWAP = 1; /* I/Q SWAP*/

#endif

    os_memcpy(grc_reg_map, bk7011_trx_val, sizeof(INT32) * 29);
    while(BK7011RCBEKEN.REG0x1->value & 0x0FFFFFFF)
    {
        cpu_delay(1);
    }

    BK7011TRXONLY.REG0x0->value = bk7011_trx_val[0];
    BK7011TRXONLY.REG0x1->value = bk7011_trx_val[1];
    BK7011TRXONLY.REG0x2->value = bk7011_trx_val[2];
    BK7011TRXONLY.REG0x3->value = bk7011_trx_val[3];
    BK7011TRXONLY.REG0x4->value = bk7011_trx_val[4];
    BK7011TRXONLY.REG0x5->value = bk7011_trx_val[5];
    BK7011TRXONLY.REG0x6->value = bk7011_trx_val[6];
    BK7011TRXONLY.REG0x7->value = bk7011_trx_val[7];
    BK7011TRXONLY.REG0x8->value = bk7011_trx_val[8];
    BK7011TRXONLY.REG0x9->value = bk7011_trx_val[9];
    BK7011TRXONLY.REG0xA->value = bk7011_trx_val[10];
    BK7011TRXONLY.REG0xB->value = bk7011_trx_val[11];
    BK7011TRXONLY.REG0xC->value = bk7011_trx_val[12];
    BK7011TRXONLY.REG0xD->value = bk7011_trx_val[13];
    BK7011TRXONLY.REG0xE->value = bk7011_trx_val[14];
    BK7011TRXONLY.REG0xF->value = bk7011_trx_val[15];
    BK7011TRXONLY.REG0x10->value = bk7011_trx_val[16];
    BK7011TRXONLY.REG0x11->value = bk7011_trx_val[17];
    BK7011TRXONLY.REG0x12->value = bk7011_trx_val[18];
    BK7011TRXONLY.REG0x13->value = bk7011_trx_val[19];
    BK7011TRXONLY.REG0x14->value = bk7011_trx_val[20];
    BK7011TRXONLY.REG0x15->value = bk7011_trx_val[21];
    BK7011TRXONLY.REG0x16->value = bk7011_trx_val[22];
    BK7011TRXONLY.REG0x17->value = bk7011_trx_val[23];
    BK7011TRXONLY.REG0x18->value = bk7011_trx_val[24];
    BK7011TRXONLY.REG0x19->value = bk7011_trx_val[25];
    BK7011TRXONLY.REG0x1A->value = bk7011_trx_val[26];
    BK7011TRXONLY.REG0x1B->value = bk7011_trx_val[27];

    while(BK7011RCBEKEN.REG0x1->value & 0x0FFFFFFF)
    {
        cpu_delay(1);
    }

#ifdef BK7011_VER_A
    //os_memcpy(gadda_map, bk7011_adda_val, sizeof(INT32) * 6);

    //BK7011ADDA.REG0x0->value = bk7011_adda_val[0];
    //BK7011ADDA.REG0x1->value = bk7011_adda_val[1];
    //BK7011ADDA.REG0x2->value = bk7011_adda_val[2];
    //BK7011ADDA.REG0x3->value = bk7011_adda_val[3];
    //BK7011ADDA.REG0x4->value = bk7011_adda_val[4];
    //BK7011ADDA.REG0x5->value = bk7011_adda_val[5];
    //BK7011ADDAMAP.REG0x0->value = bk7011_adda_val[0];
    //BK7011ADDAMAP.REG0x1->value = bk7011_adda_val[1];
    //BK7011ADDAMAP.REG0x2->value = bk7011_adda_val[2];
    //BK7011ADDAMAP.REG0x3->value = bk7011_adda_val[3];
    //BK7011ADDAMAP.REG0x4->value = bk7011_adda_val[4];
    //BK7011ADDAMAP.REG0x5->value = bk7011_adda_val[5];
#endif

#ifdef BK7011_VER_A
    //BK7011RCBEKEN.REG0x54->bits.DSELVA = 0;
#endif

    //BK7011ICU.REG0x3->bits.addaclkinv = 1;
    //BK7011ICU.REG0x3->bits.txdacgain = 1

#if No_TXPOWERCAL
    BK7011TRX.REG0xB->value = TRX_REG_0XB_VAL;
    CAL_WR_TRXREGS(0xB);
    BK7011TRX.REG0xC->value = TRX_REG_0XC_VAL;
    CAL_WR_TRXREGS(0xC);

    BK7011RCBEKEN.REG0x52->bits.TXPREGAIN = 7;
#endif

    // cal rf pll when reload trx and rc beken value
    bk7011_cal_pll();
    return 0;
}

INT32 rwnx_cal_save_trx_rcbekn_reg_val(void)
{
#if (CFG_RUNNING_PLATFORM == FPGA_PLATFORM)

#else
    bk7011_rc_val[0] = BK7011RCBEKEN.REG0x0->value ;
    bk7011_rc_val[1] = BK7011RCBEKEN.REG0x1->value ;
    bk7011_rc_val[2] = BK7011RCBEKEN.REG0x5->value ;
    bk7011_rc_val[3] = BK7011RCBEKEN.REG0x8->value ;
    bk7011_rc_val[4] = BK7011RCBEKEN.REG0xB->value ;
    bk7011_rc_val[5] = BK7011RCBEKEN.REG0xE->value ;
    bk7011_rc_val[6] = BK7011RCBEKEN.REG0x11->value;
    bk7011_rc_val[7] = BK7011RCBEKEN.REG0x19->value;
    bk7011_rc_val[8] = BK7011RCBEKEN.REG0x1C->value;
    bk7011_rc_val[0] = BK7011RCBEKEN.REG0x0->value ;
    bk7011_rc_val[9] = BK7011RCBEKEN.REG0x1E->value;

    /**********NEW ADDED************/
    bk7011_rc_val[10] = BK7011RCBEKEN.REG0x3C->value;
    bk7011_rc_val[11] = BK7011RCBEKEN.REG0x3E->value;
    bk7011_rc_val[12] = BK7011RCBEKEN.REG0x3F->value;
    bk7011_rc_val[13] = BK7011RCBEKEN.REG0x40->value;
    bk7011_rc_val[14] = BK7011RCBEKEN.REG0x41->value;
    bk7011_rc_val[15] = BK7011RCBEKEN.REG0x42->value;
    bk7011_rc_val[16] = BK7011RCBEKEN.REG0x4C->value;
    bk7011_rc_val[17] = BK7011RCBEKEN.REG0x4D->value;
    bk7011_rc_val[18] = BK7011RCBEKEN.REG0x4F->value;
    bk7011_rc_val[19] = BK7011RCBEKEN.REG0x50->value;
    bk7011_rc_val[20] = BK7011RCBEKEN.REG0x51->value;
    bk7011_rc_val[21] = BK7011RCBEKEN.REG0x52->value;
    bk7011_rc_val[22] = BK7011RCBEKEN.REG0x54->value;
    bk7011_rc_val[23] = BK7011RCBEKEN.REG0x55->value;
    bk7011_rc_val[24] = BK7011RCBEKEN.REG0x5C->value;

    //BK7011RCBEKEN.REG0x3C->bits.RXIQSWAP = 1; /* I/Q SWAP*/

#endif

    bk7011_trx_val[0]  = BK7011TRXONLY.REG0x0->value ;
    bk7011_trx_val[1]  = BK7011TRXONLY.REG0x1->value ;
    bk7011_trx_val[2]  = BK7011TRXONLY.REG0x2->value ;
    bk7011_trx_val[3]  = BK7011TRXONLY.REG0x3->value ;
    bk7011_trx_val[4]  = BK7011TRXONLY.REG0x4->value ;
    bk7011_trx_val[5]  = BK7011TRXONLY.REG0x5->value ;
    bk7011_trx_val[6]  = BK7011TRXONLY.REG0x6->value ;
    bk7011_trx_val[7]  = BK7011TRXONLY.REG0x7->value ;
    bk7011_trx_val[8]  = BK7011TRXONLY.REG0x8->value ;
    bk7011_trx_val[9]  = BK7011TRXONLY.REG0x9->value ;
    bk7011_trx_val[10] = BK7011TRXONLY.REG0xA->value ;
    bk7011_trx_val[11] = BK7011TRXONLY.REG0xB->value ;
    bk7011_trx_val[12] = BK7011TRXONLY.REG0xC->value ;
    bk7011_trx_val[13] = BK7011TRXONLY.REG0xD->value ;
    bk7011_trx_val[14] = BK7011TRXONLY.REG0xE->value ;
    bk7011_trx_val[15] = BK7011TRXONLY.REG0xF->value ;
    bk7011_trx_val[16] = BK7011TRXONLY.REG0x10->value;
    bk7011_trx_val[17] = BK7011TRXONLY.REG0x11->value;
    bk7011_trx_val[18] = BK7011TRXONLY.REG0x12->value;
    bk7011_trx_val[19] = BK7011TRXONLY.REG0x13->value;
    bk7011_trx_val[20] = BK7011TRXONLY.REG0x14->value;
    bk7011_trx_val[21] = BK7011TRXONLY.REG0x15->value;
    bk7011_trx_val[22] = BK7011TRXONLY.REG0x16->value;
    bk7011_trx_val[23] = BK7011TRXONLY.REG0x17->value;
    bk7011_trx_val[24] = BK7011TRXONLY.REG0x18->value;
    bk7011_trx_val[25] = BK7011TRXONLY.REG0x19->value;
    bk7011_trx_val[26] = BK7011TRXONLY.REG0x1A->value;
    bk7011_trx_val[27] = BK7011TRXONLY.REG0x1B->value;

    os_memcpy(grc_reg_map, bk7011_trx_val, sizeof(INT32) * 29);
    while(BK7011RCBEKEN.REG0x1->value & 0x0FFFFFFF)
    {
        cpu_delay(1);
    }

    return 0;
}

void bk7011_cal_ready(void)
{
    rwnx_cal_load_trx_rcbekn_reg_val();

    bk7011_read_cal_param();
    rwnx_cal_load_default_result();

    cpu_delay(1000);
    /*step 4.2*/
    BK7011RCBEKEN.REG0x4C->bits.TXCOMPDIS = 0;
    //gstat_cal = 0; // calibration start

    return;
}

void bk7011_cal_dpll(void)
{
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_CALI_DPLL, NULL);
}

#define BIAS_DIFF_VAL1       (4u)
#define BIAS_DIFF_VAL2       (2u)
void bk7011_cal_bias(void)
{
    UINT32 param, param2;
    //    BK7011TRX.REG0xF->bits.biascalmanual = 0;
    param = PARAM_BIAS_CAL_MANUAL_BIT;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BIAS_REG_CLEAN, &param);

    //    BK7011TRX.REG0xF->bits.biascaltrig = 0;
    param = PARAM_BIAS_CAL_TRIGGER_BIT;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BIAS_REG_CLEAN, &param);
    //trx_reg_is_write(st_TRXREG0F);
    //BK7011TRXONLY.REG0xF->value = BK7011TRX.REG0xF->value;
    cpu_delay(100);
    //    BK7011TRX.REG0xF->bits.biascaltrig = 1;
    param = PARAM_BIAS_CAL_TRIGGER_BIT;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BIAS_REG_SET, &param);

    //trx_reg_is_write(st_TRXREG0F);
    //BK7011TRXONLY.REG0xF->value = BK7011TRX.REG0xF->value;
    cpu_delay(DELAY1US * 40);//40us = 30 + 10;

    //Read SYS_CTRL.REG0x4C->bias_cal_out
    param = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BIAS_REG_READ, &param);
    param = (param >> PARAM_BIAS_CAL_OUT_POSI) & PARAM_BIAS_CAL_OUT_MASK;

    //First, Write SYS_CTRL.REG0x4C->ldo_val_man = bias_cal_out + BIAS_DIFF_VAL1
    param += BIAS_DIFF_VAL1;
    param2 = param;
    if (param > 0x1f) param = 0x1f;
    param = ((param & PARAM_BIAS_CAL_OUT_MASK) << PARAM_LDO_VAL_MANUAL_POSI)
            | PARAM_BIAS_CAL_MANUAL_BIT;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BIAS_REG_WRITE, &param);

    //Second, Write SYS_CTRL.REG0x4C->ldo_val_man = ldo_val_man - BIAS_DIFF_VAL2
    param = param2 - BIAS_DIFF_VAL2;
    if (param > 0x1f) param = 0x1f;
    gbias_after_cal = param;
    param = ((param & PARAM_BIAS_CAL_OUT_MASK) << PARAM_LDO_VAL_MANUAL_POSI)
            | PARAM_BIAS_CAL_MANUAL_BIT;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BIAS_REG_WRITE, &param);


    //param = ((0x3 & PARAM_VSEL_SYS_LDO_MASK)<< PARAM_VSEL_SYS_LDO_POSI);
    //sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_ANALOG_CTRL4_SET, &param);

    return;
}

void bk7011_cal_pll(void)
{
#if 1
    //    uint32_t loop = 0, val;

    //    do
    {
#if 0

#else

        /*reg0x10 enrfpll = 1*/
        BK7011TRX.REG0x10->bits.enrfpll = 1;
        BK7011TRX.REG0x10->bits.endpll = 1;
        CAL_WR_TRXREGS(0x10);

        /*reg0x00 spitrig = 0->1->0*/
        BK7011TRX.REG0x0->bits.spitrig = 0;
        CAL_WR_TRXREGS(0x0);
        BK7011TRX.REG0x0->bits.spitrig = 1;
        CAL_WR_TRXREGS(0x0);
        BK7011TRX.REG0x0->bits.spitrig = 0;
        CAL_WR_TRXREGS(0x0);

        /*reg0x05 spitrigger = 0->1->0*/
        BK7011TRX.REG0x5->bits.spitrigger = 0;
        BK7011TRX.REG0x5->bits.errdetspien = 0;
        CAL_WR_TRXREGS(0x5);
        BK7011TRX.REG0x5->bits.spitrigger = 1;
        CAL_WR_TRXREGS(0x5);
        BK7011TRX.REG0x5->bits.spitrigger = 0;
        CAL_WR_TRXREGS(0x5);
        BK7011TRX.REG0x5->bits.errdetspien = 1;
        CAL_WR_TRXREGS(0x5);
#endif

        cpu_delay(DELAY1US * 10);

    }

#endif
}


void bk7011_tx_cal_en(void)
{
    BK7011RCBEKEN.REG0x0->bits.forceenable = 1;
    cpu_delay(1);
    BK7011RCBEKEN.REG0x19->bits.FCH0EN = 1;
    BK7011RCBEKEN.REG0x19->bits.FCH0SHDN = 1;
    BK7011RCBEKEN.REG0x19->bits.FCH0RXEN = 0;
    BK7011RCBEKEN.REG0x19->bits.FCH0TXEN = 1;
    BK7011RCBEKEN.REG0x1C->bits.FRXON = 0;
    BK7011RCBEKEN.REG0x1C->bits.FTXON = 1;
    //BK7011RCBEKEN.REG0x4D->bits.TXSINF = 0x1A;

    //BK7011ICU.REG0x3->value = 0x00010002;
    return;
}

static INT32 bk7011_get_tx_output_power(void)
{
    int i;
    INT32 tssioutpower = 0;

    for (i = 0; i < 4; i++)
    {
        cal_delay(1 * gst_sar_adc);
        tssioutpower += BK7011RCBEKEN.REG0x54->bits.TSSIRD;
    }

    if(gtx_power_cal_mode == TX_WANTED_POWER_CAL) //
    {

        tssioutpower = tssioutpower / 4 - TSSI_POUT_TH - gav_tssi_temp;
    }
    else if(gtx_power_cal_mode == TX_IQ_POWER_CAL) //
    {

        tssioutpower = tssioutpower / 4 - TXIQ_IMB_TSSI_TH - gav_tssi_temp;
    }
    else
    {
        tssioutpower = tssioutpower / 4 - TXIQ_IMB_TSSI_TH_LOOPBACK - gav_tssi_temp;
    }


    tssioutpower = abs(tssioutpower);
    //tssioutpower = abs((INT32)(((INT32)BK7011RCBEKEN.REG0x54->bits.TSSIRD) - gconst_tssi_pout_th));

    return tssioutpower;
}


static INT32 bk7011_set_tx_pa(INT32 val1, INT32 val2, INT32 val3, INT32 val4)
{
    BK7011TRX.REG0xC->bits.dgainPA20 = val1;
    BK7011TRX.REG0xC->bits.dgainbuf20 = val2;
    BK7011TRX.REG0xC->bits.gctrlpga20 = val3;
    BK7011TRX.REG0xB->bits.gctrlmod30 = val4;
    CAL_WR_TRXREGS(0xC);
    CAL_WR_TRXREGS(0xB);

    return 0;
}

#define GET_AV_TSSI_CNT         4

INT32 bk7011_cal_tx_output_power(INT32 *val)
{
    INT32 gold_index = 0;
    INT32 tssilow = 0;
    INT32 tssihigh = 0;
    INT32 index;
    INT16 high, low, tx_fre_gain;

    INT32 cnt = 0;

    //BK7011ADDAMAP.REG0x5->bits.vc40 = 0x1f;
    //BK7011ADDA.REG0x5->value = BK7011ADDAMAP.REG0x5->value;

    // bk7011_rc_val[12]-16:20=7,    0x53479D40,   21 REG_0x52
    BK7011RCBEKEN.REG0x52->bits.TXPREGAIN = 7;

    BK7011TRX.REG0xB->value = TRX_REG_0XB_VAL;
    CAL_WR_TRXREGS(0xB);

    BK7011RCBEKEN.REG0x4C->bits.ICONSTANT = 0x200;
    BK7011RCBEKEN.REG0x4C->bits.QCONSTANT = 0x200;
    BK7011TRX.REG0xC->value = TRX_REG_0XC_TXLO_LEAKAGE_VAL;
    BK7011RCBEKEN.REG0x4C->bits.TESTPATTERN = 1;
    if (gtx_power_cal_mode == TX_WANTED_POWER_CAL)
    {
        BK7011TRX.REG0xD->value = TRX_REG_0XD_VAL;
        BK7011TRX.REG0xA->value = TRX_REG_0XA_VAL;//by yiming 20170315;
        BK7011TRX.REG0xB->value = TRX_REG_0XB_VAL;//;
        BK7011TRX.REG0xC->value = TRX_REG_0XC_VAL;//;
        BK7011TRX.REG0xC->bits.TSSIsel = 1;
        BK7011TRX.REG0xC->bits.enDCcali = 0;
        BK7011TRX.REG0xC->bits.enIQcali = 0;
        BK7011TRX.REG0xC->bits.enPcali = 1;
    }
    else if(gtx_power_cal_mode == TX_IQ_POWER_CAL)
    {
        BK7011TRX.REG0xD->value = TRX_REG_0XD_TX_IQ_VAL;
        BK7011TRX.REG0xA->value = 0x83703274;//by yiming 20170315;
        BK7011TRX.REG0xB->value = 0x20246077;//;
        BK7011TRX.REG0xC->value = 0x01A282CE;//;0x01A287EE
        bk7011_set_tx_pa(gi_gain_tx_pa_dgainPA20, gi_gain_tx_pa_dgainbuf20, 5, 8);
        BK7011TRX.REG0xC->bits.TSSIsel = 0;
        BK7011TRX.REG0xC->bits.enDCcali = 1;
        BK7011TRX.REG0xC->bits.enIQcali = 1;
        BK7011TRX.REG0xC->bits.enPcali = 0;
    }
    else  //gtx_power_cal_mode == TX_IQ_LOOPBACK_POWER_CAL
    {
        BK7011TRX.REG0xD->value = TRX_REG_0XD_TX_LOOPBACK_IQ_VAL;
        BK7011TRX.REG0xA->value = 0x83703274;//by yiming 20170315;
        BK7011TRX.REG0xB->value = 0x20246077;//;
        BK7011TRX.REG0xC->value = 0x01A282CE;//;0x01A287EE
        bk7011_set_tx_pa(gi_gain_tx_loopback_pa_dgainPA20, gi_gain_tx_loopback_pa_dgainbuf20, 5, 8);
        BK7011TRX.REG0xC->bits.TSSIsel = 0;
        BK7011TRX.REG0xC->bits.enDCcali = 1;
        BK7011TRX.REG0xC->bits.enIQcali = 1;
        BK7011TRX.REG0xC->bits.enPcali = 0;
    }

    BK7011TRX.REG0xD->bits.entssi = 1;
    BK7011TRX.REG0xD->bits.entssiadc = 1;//20170503 enable TSSI
    BK7011TRX.REG0xC->bits.enDCcaliGm1 = 0;
    BK7011TRX.REG0xC->bits.enPcaliGm = 0;
    BK7011TRX.REG0xF->bits.tssicalen = 1;
    CAL_WR_TRXREGS(0xA);
    CAL_WR_TRXREGS(0xD);
    CAL_WR_TRXREGS(0xC);
    CAL_WR_TRXREGS(0xF);
    BK7011TRX.REG0xB->bits.dcorMod30 = 0;
    CAL_WR_TRXREGS(0xB);
    cal_delay(150);//for reg C,D,F write spi=80/16=5M,total time is 21.6us
    cal_delay(5 * gst_sar_adc);

    gav_tssi_temp = 0;
    for(cnt = 0; cnt < GET_AV_TSSI_CNT; cnt++)
    {
        cal_delay(1 * gst_sar_adc);
        gav_tssi_temp += BK7011RCBEKEN.REG0x54->bits.TSSIRD;
    }
    cnt = 0;
    gav_tssi_temp /= GET_AV_TSSI_CNT;//Get small power tssi of each sample to remove tssi dc
    if (gtx_power_cal_mode == TX_WANTED_POWER_CAL)
    {
        gav_tssi = gav_tssi_temp;
    }

    if (gtx_power_cal_mode == TX_WANTED_POWER_CAL)
    {
        BK7011RCBEKEN.REG0x4C->bits.ICONSTANT = gconst_pout;
        BK7011RCBEKEN.REG0x4C->bits.QCONSTANT = gconst_pout;
        BK7011TRX.REG0xC->value = TRX_REG_0XC_VAL;//;
        CAL_WR_TRXREGS(0xC);
        BK7011TRX.REG0xC->bits.TSSIsel = 1;
        BK7011TRX.REG0xC->bits.enDCcali = 0;
        BK7011TRX.REG0xC->bits.enIQcali = 0;
        BK7011TRX.REG0xC->bits.enPcali = 1;

    }
    else if(gtx_power_cal_mode == TX_IQ_POWER_CAL)
    {
        BK7011RCBEKEN.REG0x4C->bits.ICONSTANT = 0x240;
        BK7011RCBEKEN.REG0x4C->bits.QCONSTANT = 0x240;
        BK7011TRX.REG0xC->value = 0x01A282CE;//;0x01A287EE
        CAL_WR_TRXREGS(0xC);
        bk7011_set_tx_pa(gi_gain_tx_pa_dgainPA20, gi_gain_tx_pa_dgainbuf20, 5, 8);
        BK7011TRX.REG0xC->bits.TSSIsel = 0;
        BK7011TRX.REG0xC->bits.enDCcali = 1;
        BK7011TRX.REG0xC->bits.enIQcali = 1;
        BK7011TRX.REG0xC->bits.enPcali = 0;
    }
    else  //gtx_power_cal_mode == TX_IQ_LOOPBACK_POWER_CAL
    {
        BK7011RCBEKEN.REG0x4C->bits.ICONSTANT = 0x260;//by yiming 20170315;  250
        BK7011RCBEKEN.REG0x4C->bits.QCONSTANT = 0x260;
        BK7011TRX.REG0xC->value = 0x01A282CE;//;0x01A287EE
        CAL_WR_TRXREGS(0xC);
        bk7011_set_tx_pa(gi_gain_tx_loopback_pa_dgainPA20, gi_gain_tx_loopback_pa_dgainbuf20, 5, 8);
        BK7011TRX.REG0xC->bits.TSSIsel = 0;
        BK7011TRX.REG0xC->bits.enDCcali = 1;
        BK7011TRX.REG0xC->bits.enIQcali = 1;
        BK7011TRX.REG0xC->bits.enPcali = 0;
    }

    BK7011TRX.REG0xC->bits.enDCcaliGm1 = 0;
    BK7011TRX.REG0xC->bits.enPcaliGm = 0;
    BK7011TRX.REG0xF->bits.tssicalen = 1;
    BK7011TRX.REG0xC->bits.dcorPA30 = 8;

    CAL_WR_TRXREGS(0xA);
    CAL_WR_TRXREGS(0xD);
    CAL_WR_TRXREGS(0xC);
    CAL_WR_TRXREGS(0xF);
    BK7011RCBEKEN.REG0x4C->bits.TESTPATTERN = 1;

    low = 0;
    high = 8;

    BK7011TRX.REG0xB->bits.dcorMod30 = low;
    CAL_WR_TRXREGS(0xB);
    cal_delay(CAL_TX_NUM);//first sar dac delay needs double time
    tssilow = bk7011_get_tx_output_power();


    BK7011TRX.REG0xB->bits.dcorMod30 = high;
    CAL_WR_TRXREGS(0xB);
    tssihigh = bk7011_get_tx_output_power();

    do
    {
        CAL_PRT("cnt:%d, index:%d, tssilow:0x%x-%d, tssihigh:0x%x-%d\r\n",
                cnt++, index, tssilow, low, tssihigh, high);
        if(tssilow < tssihigh)
        {
            index = low;
            high = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7011TRX.REG0xB->bits.dcorMod30 = high;
            CAL_WR_TRXREGS(0xB);
            tssihigh = bk7011_get_tx_output_power();
        }
        else
        {
            index = high;
            low = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7011TRX.REG0xB->bits.dcorMod30 = low;
            CAL_WR_TRXREGS(0xB);
            tssilow = bk7011_get_tx_output_power();
        }
    }
    while((high - low) > 1);

    index = ((tssilow < tssihigh) ? low : high);
    if (gtx_power_cal_mode == TX_WANTED_POWER_CAL)
    {
        gtx_dcorMod = index;
    }
#ifdef CALIBRATE_TIMES
    else if(gtx_power_cal_mode == TX_IQ_POWER_CAL)
    {
        if (p_gbias_after_cal_array == NULL)
        {
            CAL_TIM_PRT("bk7011 TX IQ Cal. Output Power: \r\ntx_dcorMod= %d, ", index);
        }
        gtx_dcorMod_temp = index;

    }
    else
    {
        if (p_gbias_after_cal_array == NULL)
        {
            CAL_TIM_PRT("bk7011 TX IQ Cal. Output Power: \r\ntx_dcorMod= %d, ", index);
        }
        gtx_dcorMod_temp_loopback = index;
    }

#endif

    CAL_PRT("gtx_dcorMod over: 0x%x\r\n", gtx_dcorMod);
    CAL_PRT("cnt:%d, index:%d, tssilow:0x%x-%d, tssihigh:0x%x-%d\r\n",
            cnt++, index, tssilow, low, tssihigh, high);

    BK7011TRX.REG0xB->bits.dcorMod30 = index;
    CAL_WR_TRXREGS(0xB);
    gold_index = index << 8;
    cal_delay(6);

    //BK7011TRX.REG0xB->bits.dcorMod30 = index;  // dcormod30 optimum
    //trx_reg_is_write(st_TRXREG0B);
    //BK7011TRXONLY.REG0xB->value = BK7011TRX.REG0xB->value;
    low = 0;
    high = 8;
    BK7011TRX.REG0xC->bits.dcorPA30 = low;
    CAL_WR_TRXREGS(0xC);
    tssilow = bk7011_get_tx_output_power();

    BK7011TRX.REG0xC->bits.dcorPA30 = high;
    CAL_WR_TRXREGS(0xC);
    tssihigh = bk7011_get_tx_output_power();

    do
    {
        CAL_PRT("cnt:%d, index:%d, tssilow:0x%x-%d, tssihigh:0x%x-%d\r\n",
                cnt++, index, tssilow, low, tssihigh, high);
        if(tssilow < tssihigh)
        {
            index = low;
            high = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7011TRX.REG0xC->bits.dcorPA30 = high;
            CAL_WR_TRXREGS(0xC);
            tssihigh = bk7011_get_tx_output_power();
        }
        else
        {
            index = high;
            low = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7011TRX.REG0xC->bits.dcorPA30 = low;
            CAL_WR_TRXREGS(0xC);
            tssilow = bk7011_get_tx_output_power();
        }
    }
    while((high - low) > 1);

    index = ((tssilow < tssihigh) ? low : high);
    if (gtx_power_cal_mode == TX_WANTED_POWER_CAL)
    {
        gtx_dcorPA = index;
    }
#ifdef CALIBRATE_TIMES
    else if(gtx_power_cal_mode == TX_IQ_POWER_CAL)
    {
        if (p_gbias_after_cal_array == NULL)
        {
            CAL_TIM_PRT("tx_dcorPA= %d\r\n", index);
        }
        gtx_dcorPA_temp = index;
    }
    else
    {
        if (p_gbias_after_cal_array == NULL)
        {
            CAL_TIM_PRT("tx_dcorPA= %d\r\n", index);
        }
        gtx_dcorPA_temp_loopback = index;
    }
#endif
    BK7011TRX.REG0xC->bits.dcorPA30 = index;
    CAL_WR_TRXREGS(0xC);
    gold_index += index;

    //method2:  first searching "dcorPA30",then serching "dcormod30";
    /****************************************************************
    *
    *****************************************************************/
    *val = gold_index;

    CAL_PRT("gtx_dcorMod:0x%x, gtx_dcorPA:0x%x\r\n", gtx_dcorMod, gtx_dcorPA);

#if 1
    tx_fre_gain = BK7011RCBEKEN.REG0x52->bits.TXPREGAIN;
    //CAL_WARN("tx_fre_gain:0x%x\r\n", tx_fre_gain);

    if(tx_fre_gain > 2)
        low = tx_fre_gain - 2;
    else
        low = 0;
    high = tx_fre_gain + 2;


    BK7011RCBEKEN.REG0x52->bits.TXPREGAIN = low;

    //cpu_delay(1);
    //    tssilow = bk7011_get_tx_output_power_pregain();
    tssilow = bk7011_get_tx_output_power();

    BK7011RCBEKEN.REG0x52->bits.TXPREGAIN = high;
    //cpu_delay(1);
    //    tssihigh = bk7011_get_tx_output_power_pregain();
    tssihigh = bk7011_get_tx_output_power();

    do
    {
        if(tssilow < tssihigh)
        {
            index = low;
            high = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7011RCBEKEN.REG0x52->bits.TXPREGAIN = high;
            //cpu_delay(1);
            //           tssihigh = bk7011_get_tx_output_power_pregain();
            tssihigh = bk7011_get_tx_output_power();
        }
        else
        {
            index = high;
            low = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7011RCBEKEN.REG0x52->bits.TXPREGAIN = low;
            //cpu_delay(1);
            //           tssilow = bk7011_get_tx_output_power_pregain();
            tssilow = bk7011_get_tx_output_power();
        }

    }
    while((high - low) > 1);

    index = ((tssilow < tssihigh) ? low : high);

    BK7011RCBEKEN.REG0x52->bits.TXPREGAIN = index;
    if (gtx_power_cal_mode == TX_WANTED_POWER_CAL)
    {
        gtx_pre_gain = index;
    }
#ifdef CALIBRATE_TIMES
    else if(gtx_power_cal_mode == TX_IQ_POWER_CAL)
    {
        if (p_gbias_after_cal_array == NULL)
        {
            CAL_TIM_PRT("tx_pre_gain= %d\r\n", index);
        }
        gtx_pre_gain_temp = index;
    }
    else
    {
        if (p_gbias_after_cal_array == NULL)
        {
            CAL_TIM_PRT("tx_pre_gain= %d\r\n", index);
        }
        gtx_pre_gain_temp_loopback = index;

    }
#endif

    CAL_PRT("gtx_pre_gain:0x%x\r\n", gtx_pre_gain);
#endif

#if 0
    if(gtx_dcorMod >= 0xa  && gtx_dcorPA >= 0xc)
        while(1);
#endif

    return (gold_index);
}

#if 0
void bk7011_micopwr_config_tssi_read_prepare(void)
{
    BK7011TRX.REG0xC->bits.TSSIsel = 1;
    BK7011TRX.REG0xC->bits.enDCcali = 0;
    BK7011TRX.REG0xC->bits.enIQcali = 0;
    BK7011TRX.REG0xC->bits.enPcali = 1;
    BK7011TRX.REG0xD->bits.entssi = 1;
    BK7011TRX.REG0xD->bits.entssiadc = 1;//20170503 enable TSSI
    BK7011TRX.REG0xC->bits.enDCcaliGm1 = 0;
    BK7011TRX.REG0xC->bits.enPcaliGm = 0;
    BK7011TRX.REG0xF->bits.tssicalen = 1;

    CAL_WR_TRXREGS(0xD);
    CAL_WR_TRXREGS(0xC);
    CAL_WR_TRXREGS(0xF);
}

#include "math.h"
#define TSSI_TAB_LEN    40
#define TEMP_TAB_LEN    10
//UINT8 tssi_tab[TSSI_TAB_LEN];
UINT8 temp_tab[TSSI_TAB_LEN];

void bk7011_micopwr_tssi_read(void)
{
    UINT32 i;
    REG_WRITE((0x0802800 + (18 * 4)), 0x00);
    for(i = 0; i < TSSI_TAB_LEN; i++)
    {
        tssi_tab[i] = BK7011RCBEKEN.REG0x54->bits.TSSIRD & 0xff;
    }
    //REG_WRITE((0x0802800 + (18 * 4)), 0x00);
}

void bk7011_micopwr_tssi_show(void)
{
    UINT32 i, s = 0;
    double sd = 0;
    os_printf("\r\n tssi tab: \r\n");
    for(i = 0; i < TSSI_TAB_LEN; i++)
    {
        os_printf("%d, ", tssi_tab[i]);
        if((i + 1) % 16 == 0)
            os_printf("\r\n");
    }

    for(i = 0; i < TSSI_TAB_LEN; i++)
    {
        s += tssi_tab[i] * tssi_tab[i];
    }
    sd = sqrt((double)(s / TSSI_TAB_LEN));
    os_printf("\r\nsd = %f\r\n", sd);
}
#endif

static INT32 bk7011_update_tx_power(void)
{
#ifdef CALIBRATE_TIMES
    bk7011_set_tx_pa(gi_dc_tx_pa_dgainPA20, gi_dc_tx_pa_dgainbuf20, 6, 2);// mod gain should be same as the powser cal. setting
#else
    bk7011_set_tx_pa(gi_dc_tx_pa_dgainPA20, gi_dc_tx_pa_dgainbuf20, 6, 2);
#endif
    return 0;
}

static INT32 bk7011_update_tx_loopback_power(void)
{
#ifdef CALIBRATE_TIMES
    bk7011_set_tx_pa(gi_dc_tx_loopback_pa_dgainPA20, gi_dc_tx_loopback_pa_dgainbuf20, 5, 8);
#else
    bk7011_set_tx_pa(gi_dc_tx_loopback_pa_dgainPA20, gi_dc_tx_loopback_pa_dgainbuf20, 5, 8);
#endif
    return 0;
}

static INT32 bk7011_get_tx_dc(void)
{
    INT32 detect_dc = 0;
    INT16 i;



    cpu_delay(200);//18us

    for(i = 0; i < SUMNUMBERS; i++)
    {
        cal_delay(5 * gst_sar_adc);
        detect_dc += BK7011RCBEKEN.REG0x54->bits.TSSIRD;
    }

    return detect_dc;
}
/*
static INT32 bk7011_get_tx_dc_1(void)
{
    INT32 detect_dc = 0;
    INT16 i;


    //BK7011TRXONLY.REG0xD->value = BK7011TRX.REG0xD->value;
    CAL_WR_TRXREGS(0xC);
    CAL_WR_TRXREGS(0xF);

    cpu_delay(200);
    for(i = 0; i < SUMNUMBERS; i++)
    {
        cal_delay(5 * gst_sar_adc);
        detect_dc += BK7011RCBEKEN.REG0x54->bits.TSSIRD;
    }
    return detect_dc;
}
*/

INT32 bk7011_cal_tx_dc(INT32 *val)
{
    INT32 detect_dc_low = 0;
    INT32 detect_dc_high = 0;
    INT16 high, low;
    INT32 index, gold_index = 0;
    INT32 i_index, q_index;
    INT32 srchcnt = 0;
    INT16 search_thrd = 64;//DC search range search_thrd=64 gtx_dc_n=0,search_thrd=512 gtx_dc_n=3,search_thrd=256 gtx_dc_n=2,  search_thrd=128 gtx_dc_n=1
    /*step 4*/


    if(gtx_dc_cal_mode == TX_DC_CAL)
    {
        BK7011TRX.REG0xD->value = TRX_REG_0XD_TX_IQ_VAL;  //BK7011TRXREG0xD;//
        CAL_WR_TRXREGS(0xD);
        bk7011_update_tx_power();
    }
    else// (gtx_dc_cal_mode == TX_DC_LOOPBACK_CAL)
    {
        BK7011TRX.REG0xD->value = TRX_REG_0XD_TX_LOOPBACK_IQ_VAL;
        CAL_WR_TRXREGS(0xD);
        bk7011_update_tx_loopback_power();
    }




#ifdef CALIBRATE_TIMES
    if (p_gbias_after_cal_array == NULL)
    {
        if(gtx_dc_cal_mode == TX_DC_CAL)
        {
            CAL_TIM_PRT("\r\nbk7011_cal_tx_dc: \r\n");
        }
        else// (gtx_dc_cal_mode == TX_DC_LOOPBACK_CAL)
        {
            CAL_TIM_PRT("\r\nbk7011_cal_tx_loopback_dc: \r\n");
        }
        CAL_TIM_PRT("dgainPA20  = %d\r\n", BK7011TRX.REG0xC->bits.dgainPA20 );
        CAL_TIM_PRT("dgainbuf20 =%d\r\n",  BK7011TRX.REG0xC->bits.dgainbuf20);
        CAL_TIM_PRT("gctrlpga20  = %d\r\n", BK7011TRX.REG0xC->bits.gctrlpga20 );
        CAL_TIM_PRT("gctrlmod30 = %d\r\n", BK7011TRX.REG0xB->bits.gctrlmod30);
    }
#endif

    //[-512 511]---->[0,1023];

    // I DC calibration;
    BK7011RCBEKEN.REG0x4F->bits.TXQDCCOMP = UNSIGNEDOFFSET10 + 0; //default

    if(gtx_dc_cal_mode == TX_DC_CAL)
    {
        low = UNSIGNEDOFFSET10 - MINOFFSET ;
        high = UNSIGNEDOFFSET10 + MINOFFSET ;
    }
    else// (gtx_dc_cal_mode == TX_DC_LOOPBACK_CAL)
    {
        low = UNSIGNEDOFFSET10 - 3 * MINOFFSET ;
        high = UNSIGNEDOFFSET10 + 3 * MINOFFSET ;
    }


    BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = low;

    //20170330
    BK7011RCBEKEN.REG0x4C->bits.ICONSTANT = UNSIGNEDOFFSET10  - 1;
    BK7011RCBEKEN.REG0x4C->bits.QCONSTANT = UNSIGNEDOFFSET10 - 1 ;
    BK7011RCBEKEN.REG0x4C->bits.TESTPATTERN = 1;
    BK7011TRX.REG0xC->bits.TSSIsel = 0;
    BK7011TRX.REG0xC->bits.enDCcali = 1;
    BK7011TRX.REG0xC->bits.enDCcaliGm1 = 0;//inverse 20141014
    BK7011TRX.REG0xC->bits.enIQcali = 0;
    BK7011TRX.REG0xC->bits.enPcali = 0;
    BK7011TRX.REG0xC->bits.enPcaliGm = 0;
    BK7011TRX.REG0xF->bits.tssicalen = 1;
    //20170330

    //BK7011TRXONLY.REG0xD->value = BK7011TRX.REG0xD->value;
    CAL_WR_TRXREGS(0xC);
    CAL_WR_TRXREGS(0xF);
    cal_delay(CAL_TX_NUM);//first sar dac delay needs double time

    detect_dc_low = bk7011_get_tx_dc();


    BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = high;
    detect_dc_high = bk7011_get_tx_dc();
    //Step 1 3~6 search;
    srchcnt = 0;



    if(detect_dc_low < detect_dc_high)
    {
        high = 511;
        low = high - search_thrd;
    }
    else
    {
        low = 512;
        high = low + search_thrd;
    }

    BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = high;
    detect_dc_high = bk7011_get_tx_dc();
    BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = low;
    detect_dc_low = bk7011_get_tx_dc();

    do
    {
        if(detect_dc_low < detect_dc_high)
        {
            index = low;
            high = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = high;
            detect_dc_high = bk7011_get_tx_dc();

        }
        else
        {
            index = high;
            low = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = low;
            detect_dc_low = bk7011_get_tx_dc();
        }
        srchcnt++;
        if(srchcnt > gtx_dc_n)
            break;
    }
    while((high - low) > 1);

    //Step 2  search;

    BK7011TRX.REG0xC->bits.enDCcaliGm1 = 1;//20170330 YIMING


    BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = low;
    detect_dc_low = bk7011_get_tx_dc();
    BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = high;
    detect_dc_high = bk7011_get_tx_dc();

    do
    {
        if(detect_dc_low < detect_dc_high)
        {
            index = low;
            high = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = high;
            detect_dc_high = bk7011_get_tx_dc();

        }
        else
        {
            index = high;
            low = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = low;
            detect_dc_low = bk7011_get_tx_dc();
        }
    }
    while((high - low) > 1);
    i_index = ((detect_dc_low < detect_dc_high) ? low : high);

    // Q DC calibration;
    //Step 1 3~6 search;
    if(gtx_dc_cal_mode == TX_DC_CAL)
    {
        low = UNSIGNEDOFFSET10 - MINOFFSET ;
        high = UNSIGNEDOFFSET10 + MINOFFSET ;
    }
    else// (gtx_dc_cal_mode == TX_DC_LOOPBACK_CAL)
    {
        low = UNSIGNEDOFFSET10 - 3 * MINOFFSET ;
        high = UNSIGNEDOFFSET10 + 3 * MINOFFSET ;
    }

    //20170330

    BK7011TRX.REG0xC->bits.enDCcaliGm1 = 0;//inverse 20141014

    //20170330
    srchcnt = 0;
    BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = i_index; //default

    BK7011RCBEKEN.REG0x4F->bits.TXQDCCOMP = low;
    detect_dc_low = bk7011_get_tx_dc();


    BK7011RCBEKEN.REG0x4F->bits.TXQDCCOMP = high;
    detect_dc_high = bk7011_get_tx_dc();


    if(detect_dc_low < detect_dc_high)
    {

        high = 511;
        low = high - search_thrd;
    }
    else
    {
        low = 512;
        high = low + search_thrd;
    }

    BK7011RCBEKEN.REG0x4F->bits.TXQDCCOMP = high;
    detect_dc_high = bk7011_get_tx_dc();
    BK7011RCBEKEN.REG0x4F->bits.TXQDCCOMP = low;
    detect_dc_low = bk7011_get_tx_dc();

    do
    {

        if(detect_dc_low < detect_dc_high)
        {
            index = low;
            high = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7011RCBEKEN.REG0x4F->bits.TXQDCCOMP = high;
            detect_dc_high = bk7011_get_tx_dc();

        }
        else
        {
            index = high;
            low = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7011RCBEKEN.REG0x4F->bits.TXQDCCOMP = low;
            detect_dc_low = bk7011_get_tx_dc();
        }
        srchcnt++;
        if(srchcnt > gtx_dc_n)
            break;
    }
    while((high - low) > 1);

    //Step 2  search;

    BK7011TRX.REG0xC->bits.enDCcaliGm1 = 1;//20170330 YIMING


    BK7011RCBEKEN.REG0x4F->bits.TXQDCCOMP = low;
    detect_dc_low = bk7011_get_tx_dc();
    BK7011RCBEKEN.REG0x4F->bits.TXQDCCOMP = high;
    detect_dc_high = bk7011_get_tx_dc();
    do
    {



        if(detect_dc_low < detect_dc_high)
        {
            index = low;
            high = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7011RCBEKEN.REG0x4F->bits.TXQDCCOMP = high;
            detect_dc_high = bk7011_get_tx_dc();

        }
        else
        {
            index = high;
            low = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7011RCBEKEN.REG0x4F->bits.TXQDCCOMP = low;
            detect_dc_low = bk7011_get_tx_dc();
        }
    }
    while((high - low) > 1);

    q_index = ((detect_dc_low < detect_dc_high) ? low : high);



    if(gtx_dc_cal_mode == TX_DC_CAL)
    {
        gtx_q_dc_comp = q_index;
        CAL_WARN("gtx_q_dc_comp:0x%x\r\n", gtx_q_dc_comp);
    }
    else// (gtx_dc_cal_mode == TX_DC_LOOPBACK_CAL)
    {
        gtx_q_dc_comp_loopback = q_index;
#ifdef CALIBRATE_TIMES
        if (p_gtx_q_dc_comp_temp_array != NULL)
        {
            p_gtx_q_dc_comp_temp_array[calibrate_time] = q_index;
        }
#endif
    }
    gold_index += q_index;

    // 2nd  I DC calibration;
    //Step 1 3~6 search;
    if(gtx_dc_cal_mode == TX_DC_CAL)
    {
        low = UNSIGNEDOFFSET10 - MINOFFSET ;
        high = UNSIGNEDOFFSET10 + MINOFFSET ;
    }
    else// (gtx_dc_cal_mode == TX_DC_LOOPBACK_CAL)
    {
        low = UNSIGNEDOFFSET10 - 3 * MINOFFSET ;
        high = UNSIGNEDOFFSET10 + 3 * MINOFFSET ;
    }
    //20170330

    BK7011TRX.REG0xC->bits.enDCcaliGm1 = 0;//inverse 20141014

    //20170330


    srchcnt = 0;
    BK7011RCBEKEN.REG0x4F->bits.TXQDCCOMP = q_index; // optimum

    BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = low;
    detect_dc_low = bk7011_get_tx_dc();


    BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = high;
    detect_dc_high = bk7011_get_tx_dc();
    //low = 0;
    //high = 1023;





    if(detect_dc_low < detect_dc_high)
    {

        high = 511;
        low = high - search_thrd;
    }
    else
    {
        low = 512;
        high = low + search_thrd;
    }

    BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = high;
    detect_dc_high = bk7011_get_tx_dc();
    BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = low;
    detect_dc_low = bk7011_get_tx_dc();

    do
    {


        if(detect_dc_low < detect_dc_high)
        {
            index = low;
            high = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = high;
            detect_dc_high = bk7011_get_tx_dc();
        }
        else
        {
            index = high;
            low = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = low;
            detect_dc_low = bk7011_get_tx_dc();
        }
        srchcnt++;
        if(srchcnt > gtx_dc_n) break;
    }
    while((high - low) > 1);

    //Step 2  search;

    BK7011TRX.REG0xC->bits.enDCcaliGm1 = 1;//20170330 YIMING


    BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = low;
    detect_dc_low = bk7011_get_tx_dc();
    BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = high;
    detect_dc_high = bk7011_get_tx_dc();
    do
    {

        if(detect_dc_low < detect_dc_high)
        {
            index = low;
            high = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = high;
            detect_dc_high = bk7011_get_tx_dc();

        }
        else
        {
            index = high;
            low = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = low;
            detect_dc_low = bk7011_get_tx_dc();
        }
    }
    while((high - low) > 1);


    i_index = ((detect_dc_low < detect_dc_high) ? low : high);
    BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = i_index;
    if(gtx_dc_cal_mode == TX_DC_CAL)
    {
        gtx_i_dc_comp = i_index;
        CAL_WARN("gtx_i_dc_comp:0x%x\r\n", gtx_i_dc_comp);
    }
    else// (gtx_dc_cal_mode == TX_DC_LOOPBACK_CAL)
    {
        gtx_i_dc_comp_loopback = i_index;
#ifdef CALIBRATE_TIMES
        if (p_gtx_i_dc_comp_temp_array != NULL)
        {
            p_gtx_i_dc_comp_temp_array[calibrate_time] = i_index;
        }
#endif
    }

    gold_index += (i_index << 16);
    *val = gold_index;
    (void)index;
    return gold_index;
}


#define TSSI_RD_TIMES		8
static INT32 bk7011_get_tx_i_gain(void)
{
    int i;
    INT32 detector_i_gain_p, detector_i_gain_n, detector_i_gain;

    BK7011RCBEKEN.REG0x4C->bits.ICONSTANT = UNSIGNEDOFFSET10 + gconst_iqcal_p;
    BK7011RCBEKEN.REG0x4C->bits.QCONSTANT = UNSIGNEDOFFSET10 + 0;
    detector_i_gain_p = 0;
    for (i = 0; i < TSSI_RD_TIMES; i++)
    {
        cal_delay(1 * gst_sar_adc);
        detector_i_gain_p += BK7011RCBEKEN.REG0x54->bits.TSSIRD;
    }

    BK7011RCBEKEN.REG0x4C->bits.ICONSTANT = UNSIGNEDOFFSET10 - gconst_iqcal_p;
    BK7011RCBEKEN.REG0x4C->bits.QCONSTANT = UNSIGNEDOFFSET10 + 0;
    detector_i_gain_n = 0;
    for (i = 0; i < TSSI_RD_TIMES; i++)
    {
        cal_delay(1 * gst_sar_adc);
        detector_i_gain_n += BK7011RCBEKEN.REG0x54->bits.TSSIRD;
    }

    detector_i_gain = detector_i_gain_p + detector_i_gain_n;
    return detector_i_gain;
}
static INT32 bk7011_get_tx_q_gain(void)
{
    int i;
    INT32 detector_q_gain_p, detector_q_gain_n, detector_q_gain;

    BK7011RCBEKEN.REG0x4C->bits.ICONSTANT = UNSIGNEDOFFSET10 + 0;
    BK7011RCBEKEN.REG0x4C->bits.QCONSTANT = UNSIGNEDOFFSET10 + gconst_iqcal_p;
    detector_q_gain_p = 0;
    for (i = 0; i < TSSI_RD_TIMES; i++)
    {
        cal_delay(1 * gst_sar_adc);
        detector_q_gain_p += BK7011RCBEKEN.REG0x54->bits.TSSIRD;
    }

    BK7011RCBEKEN.REG0x4C->bits.ICONSTANT = UNSIGNEDOFFSET10 + 0;
    BK7011RCBEKEN.REG0x4C->bits.QCONSTANT = UNSIGNEDOFFSET10 - gconst_iqcal_p;
    detector_q_gain_n = 0;
    for (i = 0; i < TSSI_RD_TIMES; i++)
    {
        cal_delay(1 * gst_sar_adc);
        detector_q_gain_n += BK7011RCBEKEN.REG0x54->bits.TSSIRD;
    }

    detector_q_gain = detector_q_gain_p + detector_q_gain_n;
    return detector_q_gain;
}
static INT32 bk7011_get_tx_i_phase(void)
{
    int i;
    INT32 detector_i_phase_n, detector_i_phase_p, detector_i_phase;


    BK7011RCBEKEN.REG0x4C->bits.ICONSTANT = UNSIGNEDOFFSET10 + gconst_iqcal_p;
    BK7011RCBEKEN.REG0x4C->bits.QCONSTANT = UNSIGNEDOFFSET10 - gconst_iqcal_p;
    detector_i_phase_p = 0;
    for (i = 0; i < TSSI_RD_TIMES; i++)
    {
        cal_delay(1 * gst_sar_adc);
        detector_i_phase_p += BK7011RCBEKEN.REG0x54->bits.TSSIRD;
    }

    BK7011RCBEKEN.REG0x4C->bits.ICONSTANT = UNSIGNEDOFFSET10 - gconst_iqcal_p;
    BK7011RCBEKEN.REG0x4C->bits.QCONSTANT = UNSIGNEDOFFSET10 + gconst_iqcal_p;
    detector_i_phase_n = 0;
    for (i = 0; i < TSSI_RD_TIMES; i++)
    {
        cal_delay(1 * gst_sar_adc);
        detector_i_phase_n += BK7011RCBEKEN.REG0x54->bits.TSSIRD;
    }

    detector_i_phase = detector_i_phase_p + detector_i_phase_n;
    return detector_i_phase;
}
static INT32 bk7011_get_tx_q_phase(void)
{
    int i;
    INT32 detector_q_phase_n, detector_q_phase_p, detector_q_phase;

    BK7011RCBEKEN.REG0x4C->bits.ICONSTANT = UNSIGNEDOFFSET10 + gconst_iqcal_p;
    BK7011RCBEKEN.REG0x4C->bits.QCONSTANT = UNSIGNEDOFFSET10 + gconst_iqcal_p;
    detector_q_phase_p = 0;
    for (i = 0; i < TSSI_RD_TIMES; i++)
    {
        cal_delay(1 * gst_sar_adc);
        detector_q_phase_p += BK7011RCBEKEN.REG0x54->bits.TSSIRD;
    }

    BK7011RCBEKEN.REG0x4C->bits.ICONSTANT = UNSIGNEDOFFSET10 - gconst_iqcal_p;
    BK7011RCBEKEN.REG0x4C->bits.QCONSTANT = UNSIGNEDOFFSET10 - gconst_iqcal_p;
    detector_q_phase_n = 0;
    for (i = 0; i < TSSI_RD_TIMES; i++)
    {
        cal_delay(1 * gst_sar_adc);
        detector_q_phase_n += BK7011RCBEKEN.REG0x54->bits.TSSIRD;
    }

    detector_q_phase = detector_q_phase_p + detector_q_phase_n;
    return detector_q_phase;
}
static INT32 bk7011_get_rx_i_avg_signed(void)
{
    INT32 val;
#ifdef BK7011_VER_A
    val = BK7011RCBEKEN.REG0x3C->bits.RXAVGIRD;
#else
    val = BK7011RCBEKEN.REG0x3C->bits.RXAVGIRD;
#endif

    if(val & 0x00000800)
    {
        val |= 0xfffff000;
    }

    return abs(val);
}
/*
static INT32 bk7011_get_rx_q_avg_signed(void)
{
    INT32 val;

#ifdef BK7011_VER_A
    val = BK7011RCBEKEN.REG0x3C->bits.RXAVGQRD;
#else
    val = BK7011RCBEKEN.REG0x3C->bits.RXAVGQRD;
#endif

    if(val & 0x00000800)
    {
        val |= 0xfffff000;
    }
    return abs(val);
}
*/
INT32 bk7011_cal_tx_gain_imbalance(INT32 *val)
{
    INT32 detect_gain_low = 0;
    INT32 detect_gain_high = 0;
    INT16 high, low;
    INT32 index = 0, gold_index = 0;
    INT32 detector_i_gain;
    INT32 detector_q_gain;
    if(gtx_gain_imb_cal_mode == TX_GAIN_IMB_CAL)
    {
        BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = gtx_i_dc_comp;
        BK7011RCBEKEN.REG0x4F->bits.TXQDCCOMP = gtx_q_dc_comp;
        BK7011TRX.REG0xD->value = TRX_REG_0XD_TX_IQ_VAL;//BK7011TRXREG0xD;//0214 close the TX switch to ellimite the antenna infect
        CAL_WR_TRXREGS(0xD);
    }
    else
    {
        BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = gtx_i_dc_comp_loopback;
        BK7011RCBEKEN.REG0x4F->bits.TXQDCCOMP = gtx_q_dc_comp_loopback;
        BK7011TRX.REG0xD->value = TRX_REG_0XD_TX_LOOPBACK_IQ_VAL;
        CAL_WR_TRXREGS(0xD);

    }

    BK7011RCBEKEN.REG0x50->bits.TXIGAINCOMP = 1023;
    BK7011RCBEKEN.REG0x50->bits.TXQGAINCOMP = 1023;
    BK7011RCBEKEN.REG0x4C->bits.TESTPATTERN = 1;


#ifdef CALIBRATE_TIMES
    if(gtx_gain_imb_cal_mode == TX_GAIN_IMB_CAL)
    {
        bk7011_set_tx_pa(gi_gain_tx_pa_dgainPA20, gi_gain_tx_pa_dgainbuf20, 5, 8);
    }
    else
    {
        bk7011_set_tx_pa(gi_gain_tx_loopback_pa_dgainPA20, gi_gain_tx_loopback_pa_dgainbuf20, 5, 8);
    }
    if (p_gbias_after_cal_array == NULL)
    {
        if(gtx_gain_imb_cal_mode == TX_GAIN_IMB_CAL)
        {
            CAL_TIM_PRT("\r\nbk7011_cal_tx_gain_imbalance\r\n");
        }
        else
        {
            CAL_TIM_PRT("\r\nbk7011_cal_tx_loopback_gain_imbalance\r\n");
        }

        CAL_TIM_PRT("dgainPA20  = %d\r\n", BK7011TRX.REG0xC->bits.dgainPA20 );
        CAL_TIM_PRT("dgainbuf20 =%d\r\n",  BK7011TRX.REG0xC->bits.dgainbuf20);
        CAL_TIM_PRT("gctrlpga20  = %d\r\n", BK7011TRX.REG0xC->bits.gctrlpga20 );
        CAL_TIM_PRT("gctrlmod30 = %d\r\n", BK7011TRX.REG0xB->bits.gctrlmod30);
    }
#else
    if(gtx_gain_imb_cal_mode == TX_GAIN_IMB_CAL)
    {
        bk7011_set_tx_pa(gi_gain_tx_pa_dgainPA20, gi_gain_tx_pa_dgainbuf20, 5, 8);
    }
    else
    {
        bk7011_set_tx_pa(gi_gain_tx_loopback_pa_dgainPA20, gi_gain_tx_loopback_pa_dgainbuf20, 5, 8);
    }
#endif


    BK7011TRX.REG0xC->bits.TSSIsel = 0;
    BK7011TRX.REG0xC->bits.enDCcali = 1;
    BK7011TRX.REG0xC->bits.enDCcaliGm1 = 0;
    BK7011TRX.REG0xC->bits.enIQcali = 1;
    BK7011TRX.REG0xC->bits.enPcali = 0;
    BK7011TRX.REG0xC->bits.enPcaliGm = 0;
    BK7011TRX.REG0xF->bits.tssicalen = 1;
    CAL_WR_TRXREGS(0xC);
    CAL_WR_TRXREGS(0xB);
    CAL_WR_TRXREGS(0xF);

    if(gtx_gain_imb_cal_mode == TX_GAIN_IMB_CAL)
    {
        gtx_i_gain_comp = BK7011RCBEKEN.REG0x50->bits.TXIGAINCOMP;
        gtx_q_gain_comp = BK7011RCBEKEN.REG0x50->bits.TXQGAINCOMP;
    }
    else
    {
        gtx_i_gain_comp_loopback = BK7011RCBEKEN.REG0x50->bits.TXIGAINCOMP;
        gtx_q_gain_comp_loopback = BK7011RCBEKEN.REG0x50->bits.TXQGAINCOMP;
    }

    cpu_delay(500);
    detector_i_gain = bk7011_get_tx_i_gain();
    detector_q_gain = bk7011_get_tx_q_gain();

    if(abs(detector_q_gain - detector_i_gain) < 3)
    {
        *val = 0;
        if(gtx_gain_imb_cal_mode == TX_GAIN_IMB_CAL)
        {
            gtx_i_gain_comp = BK7011RCBEKEN.REG0x50->bits.TXIGAINCOMP;
            gtx_q_gain_comp = BK7011RCBEKEN.REG0x50->bits.TXQGAINCOMP;
        }
        else
        {
            gtx_i_gain_comp_loopback = BK7011RCBEKEN.REG0x50->bits.TXIGAINCOMP;
            gtx_q_gain_comp_loopback = BK7011RCBEKEN.REG0x50->bits.TXQGAINCOMP;
        }

        return (BK7011RCBEKEN.REG0x50->bits.TXQGAINCOMP + (BK7011RCBEKEN.REG0x50->bits.TXIGAINCOMP  << 16));
    }

    if(detector_i_gain > detector_q_gain) // TX_Q_GAIN_COMP NOT CHANGED
    {
        BK7011RCBEKEN.REG0x50->bits.TXQGAINCOMP = 1023;
        low = 0;
        BK7011RCBEKEN.REG0x50->bits.TXIGAINCOMP = low;
        detector_i_gain = bk7011_get_tx_i_gain();
        detector_q_gain = bk7011_get_tx_q_gain();

        detect_gain_low = abs(detector_i_gain - detector_q_gain);

        high = 1023;
        BK7011RCBEKEN.REG0x50->bits.TXIGAINCOMP = high;
        detector_i_gain = bk7011_get_tx_i_gain();
        detector_q_gain = bk7011_get_tx_q_gain();

        detect_gain_high = abs(detector_i_gain - detector_q_gain);
        do
        {
            if(detect_gain_low < detect_gain_high)
            {
                index = low;
                high = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
                BK7011RCBEKEN.REG0x50->bits.TXIGAINCOMP = high;
                detector_i_gain = bk7011_get_tx_i_gain();
                detector_q_gain = bk7011_get_tx_q_gain();
                detect_gain_high = abs(detector_i_gain - detector_q_gain);
            }
            else
            {
                index = high;
                low = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
                BK7011RCBEKEN.REG0x50->bits.TXIGAINCOMP = low;
                detector_i_gain = bk7011_get_tx_i_gain();
                detector_q_gain = bk7011_get_tx_q_gain();
                detect_gain_low = abs(detector_i_gain - detector_q_gain);
            }

        }
        while((high - low) > 1);
        index = ((detect_gain_low < detect_gain_high) ? low : high);
        BK7011RCBEKEN.REG0x50->bits.TXIGAINCOMP = index;

        if(gtx_gain_imb_cal_mode == TX_GAIN_IMB_CAL)
        {
            gtx_i_gain_comp = index;
        }
        else
        {
            gtx_i_gain_comp_loopback = index;
        }
        gold_index = (index << 16) + 1023;
    }
    else  //// TX_I_GAIN_COMP NOT CHANGED
    {
        BK7011RCBEKEN.REG0x50->bits.TXIGAINCOMP = 1023;
        low = 0;
        BK7011RCBEKEN.REG0x50->bits.TXQGAINCOMP = low;
        detector_i_gain = bk7011_get_tx_i_gain();
        detector_q_gain = bk7011_get_tx_q_gain();

        detect_gain_low = abs(detector_i_gain - detector_q_gain);

        high = 1023;
        BK7011RCBEKEN.REG0x50->bits.TXQGAINCOMP = high;
        detector_i_gain = bk7011_get_tx_i_gain();
        detector_q_gain = bk7011_get_tx_q_gain();

        detect_gain_high = abs(detector_i_gain - detector_q_gain);
        do
        {
            if(detect_gain_low < detect_gain_high)
            {
                index = low;
                high = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
                BK7011RCBEKEN.REG0x50->bits.TXQGAINCOMP = high;
                detector_i_gain = bk7011_get_tx_i_gain();
                detector_q_gain = bk7011_get_tx_q_gain();
                detect_gain_high = abs(detector_i_gain - detector_q_gain);
            }
            else
            {
                index = high;
                low = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
                BK7011RCBEKEN.REG0x50->bits.TXQGAINCOMP = low;
                detector_i_gain = bk7011_get_tx_i_gain();
                detector_q_gain = bk7011_get_tx_q_gain();
                detect_gain_low = abs(detector_i_gain - detector_q_gain);
            }

        }
        while((high - low) > 1);
        index = ((detect_gain_low < detect_gain_high) ? low : high);
        BK7011RCBEKEN.REG0x50->bits.TXQGAINCOMP = index;
        if(gtx_gain_imb_cal_mode == TX_GAIN_IMB_CAL)
        {
            gtx_q_gain_comp = index;
        }
        else
        {
            gtx_q_gain_comp_loopback = index;
        }
        gold_index += (1023 << 16) + index;
    }

    *val = gold_index;
    if(gtx_gain_imb_cal_mode == TX_GAIN_IMB_CAL)
    {
        CAL_WARN("gtx_i_gain_comp:%d\r\n", gtx_i_gain_comp);
        CAL_WARN("gtx_q_gain_comp:%d\r\n", gtx_q_gain_comp);
    }

    return gold_index;
}


static INT32 bk7011_cal_tx_ty2(INT32 tx_phase_comp)
{
    float ty1, ty1_sqr, ty2;
    INT32 tx_ty2;

    ty1 = -1.0 * ((tx_phase_comp - 512) * (tx_phase_comp - 512)) / (1024.0 * 1024.0);
    ty1_sqr = ty1 * ty1;
    ty2 = 1 - ty1 / 2 + 3 * ty1_sqr / 8;
    tx_ty2 = (INT32)((ty2 - 0.5) * 1024 + 0.5);

    return tx_ty2;
}

INT32 bk7011_cal_tx_phase_imbalance(INT32 *val)
{
    INT32 detect_phase_low = 0;
    INT32 detect_phase_high = 0;
    INT16 high, low;
    INT32 index = 0, gold_index = 0;
    INT32 detector_i_phase;
    INT32 detector_q_phase;

    if(gtx_phase_imb_cal_mode == TX_PHASE_IMB_CAL)
    {
        BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = gtx_i_dc_comp;
        BK7011RCBEKEN.REG0x4F->bits.TXQDCCOMP = gtx_q_dc_comp;
        BK7011RCBEKEN.REG0x50->bits.TXIGAINCOMP = gtx_i_gain_comp;
        BK7011RCBEKEN.REG0x50->bits.TXQGAINCOMP = gtx_q_gain_comp;
        BK7011TRX.REG0xD->value = TRX_REG_0XD_TX_IQ_VAL;//BK7011TRXREG0xD;//0214 close the TX switch to ellimite the antenna infect
        CAL_WR_TRXREGS(0xD);
    }
    else
    {
        BK7011RCBEKEN.REG0x4F->bits.TXIDCCOMP = gtx_i_dc_comp_loopback;
        BK7011RCBEKEN.REG0x4F->bits.TXQDCCOMP = gtx_q_dc_comp_loopback;
        BK7011RCBEKEN.REG0x50->bits.TXIGAINCOMP = gtx_i_gain_comp_loopback;
        BK7011RCBEKEN.REG0x50->bits.TXQGAINCOMP = gtx_q_gain_comp_loopback;
        BK7011TRX.REG0xD->value = TRX_REG_0XD_TX_LOOPBACK_IQ_VAL;
        CAL_WR_TRXREGS(0xD);

    }

    BK7011RCBEKEN.REG0x51->bits.TXPHASECOMP = 512;
    BK7011RCBEKEN.REG0x51->bits.TXTY2 = 512;
    BK7011RCBEKEN.REG0x4C->bits.TESTPATTERN = 1;

#ifdef CALIBRATE_TIMES
    if(gtx_phase_imb_cal_mode == TX_PHASE_IMB_CAL)
    {
        bk7011_set_tx_pa(gi_gain_tx_pa_dgainPA20, gi_gain_tx_pa_dgainbuf20, 5, 8);
    }
    else
    {
        bk7011_set_tx_pa(gi_phase_tx_loopback_pa_dgainPA20, gi_phase_tx_loopback_pa_dgainbuf20, 5, 8);

    }

    if (p_gbias_after_cal_array == NULL)
    {
        if(gtx_phase_imb_cal_mode == TX_PHASE_IMB_CAL)
        {
            CAL_TIM_PRT("\r\nbk7011_cal_tx_phase_imbalance: \r\n");
        }
        else
        {
            CAL_TIM_PRT("\r\nbk7011_cal_tx_loopback_phase_imbalance: \r\n");

        }

        CAL_TIM_PRT("dgainPA20  = %d\r\n", BK7011TRX.REG0xC->bits.dgainPA20 );
        CAL_TIM_PRT("dgainbuf20 =%d\r\n",  BK7011TRX.REG0xC->bits.dgainbuf20);
        CAL_TIM_PRT("gctrlpga20  = %d\r\n", BK7011TRX.REG0xC->bits.gctrlpga20 );
        CAL_TIM_PRT("gctrlmod30 = %d\r\n", BK7011TRX.REG0xB->bits.gctrlmod30);
    }
#else
    if(gtx_phase_imb_cal_mode == TX_PHASE_IMB_CAL)
    {
        bk7011_set_tx_pa(gi_gain_tx_pa_dgainPA20, gi_gain_tx_pa_dgainbuf20, 5, 8);
    }
    else
    {
        bk7011_set_tx_pa(gi_phase_tx_loopback_pa_dgainPA20, gi_phase_tx_loopback_pa_dgainbuf20, 5, 8);
    }
#endif


    BK7011TRX.REG0xC->bits.TSSIsel = 0;
    BK7011TRX.REG0xC->bits.enDCcali = 1;
    BK7011TRX.REG0xC->bits.enDCcaliGm1 = 0;
    BK7011TRX.REG0xC->bits.enIQcali = 1;
    BK7011TRX.REG0xC->bits.enPcali = 0;
    BK7011TRX.REG0xC->bits.enPcaliGm = 0;
    BK7011TRX.REG0xF->bits.tssicalen = 1;

    CAL_WR_TRXREGS(0xC);
    CAL_WR_TRXREGS(0xB);
    CAL_WR_TRXREGS(0xF);

    low = bk7011_cal_tx_ty2(512);
    low = 1;
    BK7011RCBEKEN.REG0x51->bits.TXPHASECOMP =  low;
    BK7011RCBEKEN.REG0x51->bits.TXTY2 = bk7011_cal_tx_ty2( low);
    detector_i_phase = bk7011_get_tx_i_phase();
    detector_q_phase = bk7011_get_tx_q_phase();
    detect_phase_low = abs(detector_i_phase - detector_q_phase);

    high = 1023;
    BK7011RCBEKEN.REG0x51->bits.TXPHASECOMP =  high;
    BK7011RCBEKEN.REG0x51->bits.TXTY2 = bk7011_cal_tx_ty2( high);
    detector_i_phase = bk7011_get_tx_i_phase();
    detector_q_phase = bk7011_get_tx_q_phase();
    detect_phase_high = abs(detector_i_phase - detector_q_phase);

    do
    {
        if(detect_phase_low < detect_phase_high)
        {
            index = low;
            high = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7011RCBEKEN.REG0x51->bits.TXPHASECOMP =  high;
            BK7011RCBEKEN.REG0x51->bits.TXTY2 = bk7011_cal_tx_ty2( high);
            detector_i_phase = bk7011_get_tx_i_phase();
            detector_q_phase = bk7011_get_tx_q_phase();
            detect_phase_high = abs(detector_i_phase - detector_q_phase);
        }
        else
        {
            index = high;
            low = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7011RCBEKEN.REG0x51->bits.TXPHASECOMP =  low;
            BK7011RCBEKEN.REG0x51->bits.TXTY2 = bk7011_cal_tx_ty2( low);
            detector_i_phase = bk7011_get_tx_i_phase();
            detector_q_phase = bk7011_get_tx_q_phase();
            detect_phase_low = abs(detector_i_phase - detector_q_phase);
        }
    }
    while((high - low) > 1);
    index = ((detect_phase_low < detect_phase_high) ? low : high);
    BK7011RCBEKEN.REG0x51->bits.TXPHASECOMP = index;

    if(gtx_phase_imb_cal_mode == TX_PHASE_IMB_CAL)
    {
        gtx_phase_comp =  BK7011RCBEKEN.REG0x51->bits.TXPHASECOMP;
        gtx_phase_ty2 = BK7011RCBEKEN.REG0x51->bits.TXTY2;

        CAL_WARN("gtx_phase_comp:%d\r\n", gtx_phase_comp);
        CAL_WARN("gtx_phase_ty2:%d\r\n", gtx_phase_ty2);
    }
    else
    {
        gtx_phase_comp_loopback =  BK7011RCBEKEN.REG0x51->bits.TXPHASECOMP;
        gtx_phase_ty2_loopback = BK7011RCBEKEN.REG0x51->bits.TXTY2;

#ifdef CALIBRATE_TIMES
        if (p_gtx_phase_comp_temp_array != NULL)
        {
            p_gtx_phase_comp_temp_array[calibrate_time] = gtx_phase_comp_loopback;
        }
#endif

        CAL_WARN("tx_phase_comp:%d\r\n", gtx_phase_comp_loopback);
        CAL_WARN("tx_phase_ty2:%d\r\n", gtx_phase_ty2_loopback);

    }

    gold_index = index;
    return gold_index;
}


static float bk7011_get_tx_filter_i_ratio(void)
{
    INT32 rx_avg_i_14M, rx_avg_i_500K;
    float rx_avg_ratio;
    BK7011RCBEKEN.REG0x4D->bits.TXSINF = 358; // 14MHz;
    cal_delay_100us(gst_rx_adc);
    rx_avg_i_14M = bk7011_get_rx_i_avg_signed();
    BK7011RCBEKEN.REG0x4D->bits.TXSINF = 13; // 500KHz;
    cal_delay_100us(gst_rx_adc);
    rx_avg_i_500K = bk7011_get_rx_i_avg_signed();
    if(rx_avg_i_14M > 0)
    {
        rx_avg_ratio = abs(1.0 * rx_avg_i_500K / rx_avg_i_14M - 1.414);
        return rx_avg_ratio;
    }
    else
    {
        return -1.0;
    }
}

INT32 bk7011_cal_tx_filter_corner(INT32 *val)
{

    float tx_avg_ratio_low = 0.0;
    float tx_avg_ratio_high = 0.0;
    INT16 high, low;
    INT32 index = 0, gold_index = 0;

    BK7011RCBEKEN.REG0x1C->bits.FRXON = 1;
    // I CAL
    BK7011TRX.REG0x6->bits.lpfcapcali50 = 0x20;
    BK7011TRX.REG0xD->value = 0xFC4E03B9;//BK7011TRXREG0xD;//0xE00F02B9;//0xFC4E03B9;//
    CAL_WR_TRXREGS(0x6);
    CAL_WR_TRXREGS(0xD);

    //12/10/2014 for D version
    BK7011TRX.REG0xF->bits.tssicalen = 0;
    BK7011TRX.REG0xF->bits.sinadrxen = 0;
    CAL_WR_TRXREGS(0xF);


    BK7011RCBEKEN.REG0x4C->bits.TESTPATTERN = 2;
    BK7011RCBEKEN.REG0x4D->bits.TXSINMODE = 1;
    BK7011RCBEKEN.REG0x4D->bits.TXSINAMP = 4;
    BK7011RCBEKEN.REG0x3C->bits.RXDCCALEN = 1;
    BK7011RCBEKEN.REG0x3C->bits.RXAVGMODE = 1;


    //cpu_delay(gst_rx_adc);

    low = 0;
    BK7011TRX.REG0x6->bits.lpfcapcali50 = low;
    CAL_WR_TRXREGS(0x6);

    tx_avg_ratio_low = bk7011_get_tx_filter_i_ratio();

    high = 63;
    BK7011TRX.REG0x6->bits.lpfcapcali50 = high;
    CAL_WR_TRXREGS(0x6);

    tx_avg_ratio_high = bk7011_get_tx_filter_i_ratio();

    do
    {
        if(tx_avg_ratio_low < tx_avg_ratio_high)
        {
            index = low;
            high = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7011TRX.REG0x6->bits.lpfcapcali50 = high;
            CAL_WR_TRXREGS(0x6);
            tx_avg_ratio_high = bk7011_get_tx_filter_i_ratio();
        }
        else
        {
            index = high;
            low = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7011TRX.REG0x6->bits.lpfcapcali50 = low;
            CAL_WR_TRXREGS(0x6);
            tx_avg_ratio_low = bk7011_get_tx_filter_i_ratio();
        }
    }
    while((high - low) > 1);
    index = ((tx_avg_ratio_low < tx_avg_ratio_high) ? low : high);
    gold_index = index << 8;
    gtx_ifilter_corner = index;
    // Q CAL

    BK7011TRX.REG0x6->bits.lpfcapcali50 = gtx_ifilter_corner;
    BK7011TRX.REG0x6->bits.lpfcapcalq50 = gtx_ifilter_corner;//160405 iq used same value
    gtx_qfilter_corner = gtx_ifilter_corner;
    CAL_WARN("gtx_ifilter_corner over: 0x%x\r\n", gtx_ifilter_corner);
    CAL_WARN("gtx_qfilter_corner over: 0x%x\r\n", gtx_qfilter_corner);

    CAL_WR_TRXREGS(0x6);

    *val = gold_index;
    return (gold_index);
}

void bk7011_rx_cal_en(void)
{
    BK7011RCBEKEN.REG0x3E->bits.RXCOMPEN = 0;

    BK7011RCBEKEN.REG0x1C->bits.FRXON = 1;
    BK7011RCBEKEN.REG0x1C->bits.FTXON = 0;

    BK7011RCBEKEN.REG0x0->bits.forceenable = 1;
    cpu_delay(1);
    BK7011RCBEKEN.REG0x19->bits.FCH0SHDN = 1;
    BK7011RCBEKEN.REG0x19->bits.FCH0TXEN = 0;
    BK7011RCBEKEN.REG0x19->bits.FCH0RXEN = 1;
    BK7011RCBEKEN.REG0x19->bits.FCH0EN = 1;

    // ADC clock change to 80M
    BK7011TRX.REG0xF->bits.clkadcsel = 0;//20170331   1  --> 0
    CAL_WR_TRXREGS(0xF);

    BK7011TRX.REG0x12->bits.adcrefbwsel = 0;//20170331  1-->0
    CAL_WR_TRXREGS(0x12);
}

INT32 bk7011_cal_rx_dc(void)
{
    INT32 index = 0;
    INT32 i, j, k, t, curr, value;


    /*step 2*/
    BK7011RCBEKEN.REG0x3C->bits.RXDCCALEN = 1;
    BK7011RCBEKEN.REG0x3C->bits.RXAVGMODE = 0;
    BK7011RCBEKEN.REG0x19->bits.FCH0RXHP = 0; //huaming.jiang 20141017
    BK7011RCBEKEN.REG0x1C->bits.FTXON = 0;
    BK7011RCBEKEN.REG0x1C->bits.FRXON = 1;

    BK7011TRX.REG0x7->bits.chin60 = 0x64;
    CAL_WR_TRXREGS(0x7);
    delay100us(1);//delay 100us for RFPLL

    BK7011TRX.REG0xE->value = 0xDA01BCF0; /// D801BCF0;//170217
    CAL_WR_TRXREGS(0xE);

    for(i = 0; i < 16; i ++)
    {
        BK7011RCBEKEN.REG0x19->bits.FCH0B = (0x70 | i);
        for(j = 0; j < 2; j ++)
        {
            index = 128;
            k = 6;
            do
            {
                //set dc offset
                value = (*((volatile unsigned long *)(TRX_BEKEN_BASE + (0x14 + i / 2) * 4)));
                curr = ~(0xff << (16 * (i % 2) + 8 * j));
                value &= curr;
                curr = (index << (16 * (i % 2) + 8 * j));
                value |= curr;
                (*((volatile unsigned long *)(TRX_BEKEN_BASE + (0x14 + i / 2) * 4))) = value;
                while(BK7011RCBEKEN.REG0x1->value & 0xfffffff);
                cal_delay_100us(gst_rx_adc);

                //read dc avg, and calc mean
                value = 0;
                for(t = 0; t < 10; t ++)
                {
                    if(j == 0)  curr = BK7011RCBEKEN.REG0x3C->bits.RXAVGIRD;
                    else        curr = BK7011RCBEKEN.REG0x3C->bits.RXAVGQRD;
                    if(curr >= 2048) curr -= 4096;
                    value += curr;
                    cpu_delay(100);
                }
                curr = value / 10;

                //calc new dc offset
                if(curr > 0) index += (0x1 << k);
                else         index -= (0x1 << k);
                k --;
            }
            while((k >= 0) && ((curr >= 16) || (curr <= -16)));
            if(k < 0)
            {
                value = (*((volatile unsigned long *)(TRX_BEKEN_BASE + (0x14 + i / 2) * 4)));
                curr = ~(0xff << (16 * (i % 2) + 8 * j));
                value &= curr;
                curr = (index << (16 * (i % 2) + 8 * j));
                value |= curr;
                (*((volatile unsigned long *)(TRX_BEKEN_BASE + (0x14 + i / 2) * 4))) = value;
                while(BK7011RCBEKEN.REG0x1->value & 0xfffffff);
            }
        }
    }

    g_rx_dc_gain_tab[0] = BK7011TRXONLY.REG0x14->value;
    g_rx_dc_gain_tab[1] = BK7011TRXONLY.REG0x15->value;
    g_rx_dc_gain_tab[2] = BK7011TRXONLY.REG0x16->value;
    g_rx_dc_gain_tab[3] = BK7011TRXONLY.REG0x17->value;
    g_rx_dc_gain_tab[4] = BK7011TRXONLY.REG0x18->value;
    g_rx_dc_gain_tab[5] = BK7011TRXONLY.REG0x19->value;
    g_rx_dc_gain_tab[6] = BK7011TRXONLY.REG0x1A->value;
    g_rx_dc_gain_tab[7] = BK7011TRXONLY.REG0x1B->value;
    CAL_WARN("g_rx_dc_gain_tab 0 over: 0x%x\r\n", g_rx_dc_gain_tab[0]);
    CAL_WARN("g_rx_dc_gain_tab 1 over: 0x%x\r\n", g_rx_dc_gain_tab[1]);
    CAL_WARN("g_rx_dc_gain_tab 2 over: 0x%x\r\n", g_rx_dc_gain_tab[2]);
    CAL_WARN("g_rx_dc_gain_tab 3 over: 0x%x\r\n", g_rx_dc_gain_tab[3]);
    CAL_WARN("g_rx_dc_gain_tab 4 over: 0x%x\r\n", g_rx_dc_gain_tab[4]);
    CAL_WARN("g_rx_dc_gain_tab 5 over: 0x%x\r\n", g_rx_dc_gain_tab[5]);
    CAL_WARN("g_rx_dc_gain_tab 6 over: 0x%x\r\n", g_rx_dc_gain_tab[6]);
    CAL_WARN("g_rx_dc_gain_tab 7 over: 0x%x\r\n", g_rx_dc_gain_tab[7]);

    BK7011RCBEKEN.REG0x3C->bits.RXDCCALEN = 0;

    BK7011TRX.REG0x7->bits.chin60 = 0xc;
    CAL_WR_TRXREGS(0x7);
    delay100us(1);//delay 100us for RFPLL

    return 0;
}

INT32 bk7011_cal_rx_iq(INT32 *val)
{
    INT32 rx_amp_err_rd, rx_phase_err_rd, rx_ty2_rd;
    INT32 rx_amp_err_wr;
    INT32 rx_phase_err_wr;
    float amp_err, phase_err, ty2_err;
    INT32 gold_index = 0;
    INT32 i, curr, value, value1, value2;

    /*step 1*/
    BK7011RCBEKEN.REG0x1C->bits.FRXON = 1;
    BK7011RCBEKEN.REG0x1C->bits.FTXON = 1;

    BK7011TRX.REG0xF->bits.tssicalen = 0;
    BK7011TRX.REG0xF->bits.sinadrxen = 0;
    CAL_WR_TRXREGS(0xF);

    BK7011TRX.REG0xE->value = TRX_REG_0XE_RXIQ_VAL;
    CAL_WR_TRXREGS(0xE);
    bk7011_set_tx_pa(gi_cal_rx_iq_pa_dgainPA20, gi_cal_rx_iq_pa_dgainbuf20, 4, 4);

    BK7011RCBEKEN.REG0x19->bits.FCH0B = 0x1a;//decrease 6dB

    /*searching...*/
    BK7011RCBEKEN.REG0x3E->bits.RXCALEN = 1;
    BK7011RCBEKEN.REG0x4C->bits.TESTPATTERN = 2;
    BK7011RCBEKEN.REG0x4D->bits.TXSINMODE = 0;
    BK7011RCBEKEN.REG0x4D->bits.TXSINAMP = 0x04;  //  increase 6dB
    BK7011RCBEKEN.REG0x4D->bits.TXSINF = 179; // 7MHz;
    cal_delay_100us(6 * gst_rx_adc);
    cpu_delay(500 * DELAY1US);

    BK7011RCBEKEN.REG0x41->bits.RXDCIWR = 0x0;
    BK7011RCBEKEN.REG0x41->bits.RXDCQWR = 0x0;

    value = 0;
    value1 = 0;
    value2 = 0;
    for(i = 0; i < 2; i ++)
    {
        curr = BK7011RCBEKEN.REG0x3F->bits.RXAMPERRRD;
        value += curr - ((curr < 512) ? 0 : 1024);
        curr = BK7011RCBEKEN.REG0x3F->bits.RXPHASEERRRD;
        value1 += curr - ((curr < 512) ? 0 : 1024);
        curr = BK7011RCBEKEN.REG0x40->bits.RXTY2RD;
        value2 += curr - ((curr < 512) ? 0 : 1024);
        cpu_delay(gst_rx_adc);
    }
    rx_amp_err_rd = value / 2;
    rx_phase_err_rd = value1 / 2;
    rx_ty2_rd = value2 / 2;

#ifdef CALIBRATE_TIMES      // by gwf
    if (p_rx_amp_err_rd_array != NULL)
    {
        int rx_amp_err_rd_temp;
        int rx_phase_err_rd_temp;
        int rx_ty2_rd_temp;
        if (rx_amp_err_rd & 0x200)
        {
            rx_amp_err_rd_temp = rx_amp_err_rd | 0xFFFFFC00;
        }
        else
        {
            rx_amp_err_rd_temp = rx_amp_err_rd & 0x000003FF;
        }
        if (rx_phase_err_rd & 0x200)
        {
            rx_phase_err_rd_temp = rx_phase_err_rd | 0xFFFFFC00;
        }
        else
        {
            rx_phase_err_rd_temp = rx_phase_err_rd & 0x000003FF;
        }
        if (rx_ty2_rd & 0x200)
        {
            rx_ty2_rd_temp = rx_ty2_rd | 0xFFFFFC00;
        }
        else
        {
            rx_ty2_rd_temp = rx_ty2_rd & 0x000003FF;
        }
        p_rx_amp_err_rd_array[calibrate_time] = rx_amp_err_rd_temp;
        p_rx_phase_err_rd_array[calibrate_time] = rx_phase_err_rd_temp;
        p_rx_ty2_rd_array[calibrate_time] = rx_ty2_rd_temp;
    }
    else
#endif
    {
        CAL_FATAL("[rx_iq]rx_amp_err_rd: 0x%03x\r\n", rx_amp_err_rd );
        CAL_FATAL("[rx_iq]rx_phase_err_rd: 0x%03x\r\n", rx_phase_err_rd );
        CAL_FATAL("[rx_iq]rx_ty2_rd: 0x%03x\r\n", rx_ty2_rd );
    }

    amp_err = 1.0 * rx_amp_err_rd / 1024;
    phase_err = 1.0 * rx_phase_err_rd / 1024;
    ty2_err = 1.0 * rx_ty2_rd / 1024;

    rx_amp_err_wr = (INT32) (512 * (ty2_err + 1) / (amp_err + 1));
    rx_phase_err_wr = (INT32) (512 * phase_err * (ty2_err + 1));

    BK7011RCBEKEN.REG0x42->bits.RXPHASEERRWR = rx_phase_err_wr;
    BK7011RCBEKEN.REG0x42->bits.RXAMPERRWR = rx_amp_err_wr;
    BK7011RCBEKEN.REG0x3E->bits.RXCOMPEN = 1;
    BK7011RCBEKEN.REG0x3E->bits.RXCALEN = 0;

    grx_amp_err_wr = rx_amp_err_wr;
    grx_phase_err_wr = rx_phase_err_wr;

    CAL_WARN("grx_amp_err_wr:0x%03x\r\n", grx_amp_err_wr);
    CAL_WARN("grx_phase_err_wr:0x%03x\r\n", grx_phase_err_wr);

    gold_index = (rx_amp_err_wr << 16 ) + rx_phase_err_wr;
    *val = gold_index;

    BK7011TRX.REG0x9->bits.agcrxfeEn = 1; //enable agc
    BK7011TRX.REG0x7->bits.autorxifgen = 1;//ensable agc
    CAL_WR_TRXREGS(0x7);
    CAL_WR_TRXREGS(0x9);

    return gold_index;
}

void bk7011_set_rx_avg_dc(void)
{
    INT32 rx_dc_i_rd, rx_dc_q_rd;

    BK7011RCBEKEN.REG0x3E->bits.RXCOMPEN = 0;
    BK7011RCBEKEN.REG0x3C->bits.RXAVGMODE = 0;
    BK7011RCBEKEN.REG0x3C->bits.RXDCCALEN = 1;
    cal_delay_100us(gst_rx_adc);
    BK7011RCBEKEN.REG0x3C->bits.RXDCCALEN = 0;

    rx_dc_i_rd = BK7011RCBEKEN.REG0x3C->bits.RXAVGIRD;
    rx_dc_q_rd = BK7011RCBEKEN.REG0x3C->bits.RXAVGQRD;
    BK7011RCBEKEN.REG0x41->bits.RXDCIWR = rx_dc_i_rd;
    BK7011RCBEKEN.REG0x41->bits.RXDCQWR = rx_dc_q_rd;

    BK7011RCBEKEN.REG0x3E->bits.RXCOMPEN = 1;

    return;
}

INT32 bk7011_load_calibration_cfg(void)
{
    BK7011RCBEKEN.REG0x0->value  = BK7011RCBEKEN.REG0x0->value;
    BK7011RCBEKEN.REG0x1->value  = BK7011RCBEKEN.REG0x1->value;
    BK7011RCBEKEN.REG0x5->value  = BK7011RCBEKEN.REG0x5->value ;
    BK7011RCBEKEN.REG0x8->value  = BK7011RCBEKEN.REG0x8->value;
    BK7011RCBEKEN.REG0xB->value  = BK7011RCBEKEN.REG0xB->value;
    BK7011RCBEKEN.REG0x11->value = BK7011RCBEKEN.REG0x11->value;
    BK7011RCBEKEN.REG0x19->value = BK7011RCBEKEN.REG0x19->value;
    BK7011RCBEKEN.REG0x1E->value = BK7011RCBEKEN.REG0x1E->value;

    /**********NEW ADDED************/
    BK7011RCBEKEN.REG0x3C->value = BK7011RCBEKEN.REG0x3C->value;
    BK7011RCBEKEN.REG0x3E->value = BK7011RCBEKEN.REG0x3E->value;
    BK7011RCBEKEN.REG0x3F->value = BK7011RCBEKEN.REG0x3F->value;
    BK7011RCBEKEN.REG0x40->value = BK7011RCBEKEN.REG0x40->value;
    BK7011RCBEKEN.REG0x41->value = BK7011RCBEKEN.REG0x41->value;
    BK7011RCBEKEN.REG0x42->value = BK7011RCBEKEN.REG0x42->value ;
    BK7011RCBEKEN.REG0x4C->value = BK7011RCBEKEN.REG0x4C->value;
    BK7011RCBEKEN.REG0x4D->value = BK7011RCBEKEN.REG0x4D->value;
    BK7011RCBEKEN.REG0x4F->value = BK7011RCBEKEN.REG0x4F->value;
    BK7011RCBEKEN.REG0x50->value = BK7011RCBEKEN.REG0x50->value;
    BK7011RCBEKEN.REG0x51->value = BK7011RCBEKEN.REG0x51->value;
    BK7011RCBEKEN.REG0x52->value = BK7011RCBEKEN.REG0x52->value;
    BK7011RCBEKEN.REG0x54->value = BK7011RCBEKEN.REG0x54->value;
    BK7011RCBEKEN.REG0x5C->value = BK7011RCBEKEN.REG0x5C->value;

    while(BK7011RCBEKEN.REG0x1->value & 0x0FFFFFFF)
    {
        cpu_delay(1);
    }
    BK7011TRXONLY.REG0x0->value = grc_reg_map[0];
    BK7011TRXONLY.REG0x1->value = grc_reg_map[1];
    BK7011TRXONLY.REG0x2->value = grc_reg_map[2];
    BK7011TRXONLY.REG0x3->value = grc_reg_map[3];
    BK7011TRXONLY.REG0x4->value = grc_reg_map[4];
    BK7011TRXONLY.REG0x6->value = grc_reg_map[6];
    BK7011TRXONLY.REG0x7->value = grc_reg_map[7];
    BK7011TRXONLY.REG0x8->value = grc_reg_map[8];
    BK7011TRXONLY.REG0x9->value = grc_reg_map[9];
    BK7011TRXONLY.REG0xA->value = grc_reg_map[10];
    BK7011TRXONLY.REG0xB->value = grc_reg_map[11];
    BK7011TRXONLY.REG0xC->value = grc_reg_map[12];
    BK7011TRXONLY.REG0xD->value = grc_reg_map[13];
    BK7011TRXONLY.REG0xE->value = grc_reg_map[14];
    BK7011TRXONLY.REG0x11->value = grc_reg_map[17];
    BK7011TRXONLY.REG0x12->value = grc_reg_map[18];
    BK7011TRXONLY.REG0x13->value = grc_reg_map[19];
    BK7011TRXONLY.REG0x14->value = grc_reg_map[20];
    BK7011TRXONLY.REG0x15->value = grc_reg_map[21];
    BK7011TRXONLY.REG0x16->value = grc_reg_map[22];
    BK7011TRXONLY.REG0x17->value = grc_reg_map[23];
    BK7011TRXONLY.REG0x18->value = grc_reg_map[24];
    BK7011TRXONLY.REG0x19->value = grc_reg_map[25];
    BK7011TRXONLY.REG0x1A->value = grc_reg_map[26];
    BK7011TRXONLY.REG0x1B->value = grc_reg_map[27];

    while(BK7011RCBEKEN.REG0x1->value & 0x0FFFFFFF)
    {
        cpu_delay(1);
    }

    BK7011TRX.REG0x7->bits.autorxifgen = 0;
    BK7011TRX.REG0x7->bits.digdcoen = 0;
    BK7011TRX.REG0x7->bits.spilpfrxg30 = 6;
    CAL_WR_TRXREGS(0x7);

    BK7011TRX.REG0x6->bits.lpfcapcali50 = gtx_ifilter_corner;
    BK7011TRX.REG0x6->bits.lpfcapcalq50 = gtx_ifilter_corner;
    CAL_WR_TRXREGS(0x6);

    bk7011_set_rx_avg_dc(); // 11/11/2014

    return 0;
}

void bk7011_set_tx_after_cal(void)
{
    BK7011RCBEKEN.REG0x0->bits.forceenable = 0;
    cpu_delay(1);
    BK7011RCBEKEN.REG0x19->bits.FCH0EN = 1;
    BK7011RCBEKEN.REG0x19->bits.FCH0SHDN = 1;
    BK7011RCBEKEN.REG0x19->bits.FCH0RXEN = 0;
    BK7011RCBEKEN.REG0x19->bits.FCH0TXEN = 1;
    BK7011RCBEKEN.REG0x1C->bits.FRXON = 0;
    BK7011RCBEKEN.REG0x1C->bits.FTXON = 1;
    BK7011RCBEKEN.REG0x4C->bits.TESTPATTERN = 0;
}

void bk7011_set_rx_after_cal(void)
{
    BK7011RCBEKEN.REG0x0->bits.forceenable = 0;
    cpu_delay(1);
    BK7011RCBEKEN.REG0x19->bits.FCH0EN = 1;
    BK7011RCBEKEN.REG0x19->bits.FCH0SHDN = 1;
    BK7011RCBEKEN.REG0x19->bits.FCH0RXEN = 1;
    BK7011RCBEKEN.REG0x19->bits.FCH0TXEN = 0;
    BK7011RCBEKEN.REG0x1C->bits.FRXON = 1;
    BK7011RCBEKEN.REG0x1C->bits.FTXON = 0;
    BK7011RCBEKEN.REG0x4C->bits.TESTPATTERN = 0;

    BK7011TRX.REG0xE->value = 0xDA01BCF0;
    CAL_WR_TRXREGS(0xE);

    // ADC clock change to 40M
    BK7011TRX.REG0xF->bits.clkadcsel = 0;
    CAL_WR_TRXREGS(0xF);

    BK7011TRX.REG0x12->bits.adcrefbwsel = 0;
    CAL_WR_TRXREGS(0x12);

}

extern void mpb_tx_mode(void);

#define CALI_DPD_TEST       0
#if CALI_DPD_TEST
#define I_TABLE_ADDR        0x01050400
#define Q_TABLE_ADDR        0x01050600

static UINT16 i_table_val[256] =
{
    0, 6, 13, 19, 26, 35, 40, 47, 52, 57, 68, 73, 76, 82, 88, 91, 96, 102, 107, 107, 118, 118, 120, 127, 132, 134, 139, 141, 146, 149, 152, 158, 161, 161, 163, 164, 168, 172, 172, 176, 181, 177, 179, 181, 185, 187, 189, 185, 191, 195, 196, 195, 196, 197, 203, 198, 204, 201, 207, 199, 206, 207, 207, 207, 207, 210, 210, 212, 214, 215, 215, 215, 206, 216, 215, 221, 217, 219, 215, 219, 222, 222, 225, 229, 225, 223, 228, 226, 226, 229, 229, 226, 225, 227, 226, 226, 228, 232, 230, 229, 230, 231, 230, 231, 234, 235, 236, 238, 241, 244, 245, 247, 248, 251, 252, 255, 255, 258, 259, 262, 263, 265, 267, 268, 271, 272, 275, 275, 278, 280, 282, 284, 287, 288, 291, 293, 295, 297, 299, 301, 304, 306, 308, 310, 312, 314, 317, 319, 321, 323, 325, 327, 330, 332, 334, 336, 338, 341, 343, 345, 347, 349, 351, 354, 356, 358, 360, 362, 364, 367, 369, 371, 373, 375, 377, 380, 382, 384, 386, 388, 390, 393, 395, 397, 399, 401, 403, 406, 408, 410, 412, 414, 416, 419, 421, 423, 425, 427, 429, 432, 434, 436, 438, 440, 442, 445, 447, 449, 451, 453, 455, 458, 460, 462, 464, 466, 468, 471, 473, 475, 477, 479, 481, 484, 486, 488, 490, 492, 495, 497, 499, 501, 503, 505, 508, 510, 512, 514, 516, 518, 521, 523, 525, 527, 529, 531, 534, 536, 538, 540, 542, 544, 547, 549, 551, 562
};

static UINT16 q_table_val[256] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 2, 1, 5, 8, 5, 9, 6, 5, 7, 4, 7, 8, 17, 13, 12, 14, 15, 12, 12, 18, 12, 13, 16, 16, 17, 19, 20, 24, 22, 30, 23, 21, 24, 30, 27, 26, 24, 27, 26, 30, 28, 30, 32, 31, 31, 32, 32, 33, 35, 35, 33, 35, 34, 32, 32, 32, 34, 33, 32, 31, 32, 30, 33, 29, 30, 29, 30, 29, 29, 28, 27, 29, 27, 28, 26, 26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 37, 37, 37, 37, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 41, 41, 41, 41, 41, 42, 42, 42, 42, 42, 43, 43, 43, 43, 43, 44, 44, 44, 44, 44, 45, 45, 45, 45, 45, 46, 46, 46, 46, 46, 47, 47, 47, 47, 48, 48, 48, 48, 48, 49, 49, 49, 49, 49, 50, 50, 50, 50, 50, 51, 51, 51, 51, 51, 52, 52, 48
};

UINT32 bk7211_cal_tx_dpd_load(void)
{

    UINT16 *i_tbl_addr = (UINT16 *)I_TABLE_ADDR;
    UINT16 *q_tbl_addr = (UINT16 *)Q_TABLE_ADDR;
    UINT16 k;

    BK7011RCBEKEN.REG0x4C->bits.DPDEN = 0;

    os_memcpy(i_tbl_addr, (UINT16 *)&i_table_val[0], 256 * 2);
    os_memcpy(q_tbl_addr, (UINT16 *)&q_table_val[0], 256 * 2);

#if 1
    for(k = 0; k < 256; k++)
    {
        i_tbl_addr[k] = 1;
        q_tbl_addr[k] = 0;
    }
#endif
    return 0;

}
#endif

#if CAL_RESULT_TO_FLASH
void write_cal_result_to_flash(void)
{
    UINT32 param;
    UINT32 param1;
    char cTemp[sizeof(bk7011_rc_val) + sizeof(bk7011_trx_val)];
    char cTemp1[sizeof(bk7011_rc_val) + sizeof(bk7011_trx_val)];

    CAL_FLASH_PRT("write cal result to flash\r\n");

	#if CFG_SUPPORT_ALIOS
	hal_flash_dis_secure(0, 0, 0);
	#else
    bk_flash_enable_security(FLASH_PROTECT_NONE);
	#endif

    flash_erase_sector(CAL_RESULT_FLASH_ADDR);

    memcpy(cTemp, bk7011_rc_val, sizeof(bk7011_rc_val));
    memcpy(cTemp + sizeof(bk7011_rc_val), bk7011_trx_val, sizeof(bk7011_trx_val));

    flash_write(cTemp, sizeof(cTemp), CAL_RESULT_FLASH_ADDR);
    flash_read(cTemp1, sizeof(cTemp1), CAL_RESULT_FLASH_ADDR);
    if (memcmp(cTemp, cTemp1,  sizeof(cTemp1)) == 0)
    {
        CAL_FLASH_PRT("memcmp OK\r\n");
    }
    else
    {
        CAL_FLASH_PRT("memcmp fail\r\n");
    }

	#if CFG_SUPPORT_ALIOS
	hal_flash_enable_secure(0, 0, 0);
	#else
    bk_flash_enable_security(FLASH_UNPROTECT_LAST_BLOCK);
	#endif

    os_printf("write cal result to flash OK\r\n");
}
#endif

char read_cal_result_from_flash(void)
{
#if CAL_RESULT_TO_FLASH
    char cTemp[sizeof(bk7011_rc_val) + sizeof(bk7011_trx_val)];

    CAL_FLASH_PRT("read_cal_result_from_flash\r\n");
    memset(cTemp, 0, sizeof(cTemp));
    flash_read(cTemp, sizeof(cTemp), CAL_RESULT_FLASH_ADDR);
    if ((cTemp[0] == 0xFF) && (cTemp[4] == 0xFF) && (cTemp[8] == 0xFF) && (cTemp[12] == 0xFF))
    {
        return 0;
    }
    else
    {
        memcpy(bk7011_rc_val, cTemp, sizeof(bk7011_rc_val));
        memcpy(bk7011_trx_val, cTemp + sizeof(bk7011_rc_val), sizeof(bk7011_trx_val));
        rwnx_cal_load_default_result();
        rwnx_cal_load_trx_rcbekn_reg_val();
        os_printf("read cal result from flash OK\r\n");

        return 1;
    }
#else
    return 0;
#endif
}

#if CAL_RESULT_TO_FLASH
void flash_test(void)
{
    UINT32 param;
    char cTemp[0x1000];
    char cTemp1[0x1000];
    int i;

    flash_read(cTemp, 0x1000, 0xF4000);
    os_printf("cTemp:\r\n");
    for (i = 0; i < 0x1000; i++)
    {
        os_printf("%x ", cTemp[i]);
    }
    os_printf("\r\n");

	#if CFG_SUPPORT_ALIOS
	hal_flash_dis_secure(0, 0, 0);
	#else
    bk_flash_enable_security(FLASH_PROTECT_NONE);
	#endif
    param = 0xF4000;
    flash_ctrl(CMD_FLASH_ERASE_SECTOR, &param);

    flash_read(cTemp, 0x1000, 0xF4000);
    os_printf("cTemp:\r\n");
    for (i = 0; i < 0x1000; i++)
    {
        os_printf("%x ", cTemp[i]);
    }
    os_printf("\r\n");

    for (i = 0; i < 0x1000; i++)
    {
        cTemp[i] = i;
    }
    os_printf("cTemp:\r\n");
    for (i = 0; i < 0x1000; i++)
    {
        os_printf("%x ", cTemp[i]);
    }
    os_printf("\r\n");

    flash_write(cTemp, 0x1000, 0xF4000);

    memset(cTemp1, 0, 0x1000);
    os_printf("cTemp1:\r\n");
    for (i = 0; i < 0x1000; i++)
    {
        os_printf("%x ", cTemp1[i]);
    }
    os_printf("\r\n");
    flash_read(cTemp1, 0x1000, 0xF4000);

    os_printf("cTemp1:\r\n");
    for (i = 0; i < 0x1000; i++)
    {
        os_printf("%x ", cTemp1[i]);
    }

    os_printf("\r\n");
    if (memcmp(cTemp, cTemp1, sizeof(cTemp)) == 0)
    {
        os_printf("memcmp OK\r\n");
    }
    else
    {
        os_printf("memcmp ERROR\r\n");
    }
}
#endif

void sctrl_dpll_int_open(void);
void calibration_main(void)
{
    INT32 goldval[32] = {0};

    BK7011RCBEKEN.REG0x4C->bits.TXCOMPDIS = 0;
    bk7011_cal_ready();
    bk7011_cal_bias();

    bk7011_cal_dpll();
    sctrl_dpll_int_open();

    bk7011_cal_pll();

    BK7011TRX.REG0xA->value = TRX_REG_0XA_VAL;
    CAL_WR_TRXREGS(0xA);
    BK7011TRX.REG0xB->value = TRX_REG_0XB_VAL;
    CAL_WR_TRXREGS(0xB);
    BK7011TRX.REG0xC->value = TRX_REG_0XC_VAL;
    CAL_WR_TRXREGS(0xC);

    if (read_cal_result_from_flash() == 1)
    {
        return;
    }

    BK7011TRX.REG0x7->bits.chin60 = 0x00;//2400MHz  descrease the external interference
    CAL_WR_TRXREGS(0x7);
    delay100us(1);//delay 100us for RFPLL

    bk7011_tx_cal_en();
    gtx_power_cal_mode = TX_IQ_POWER_CAL;
    bk7011_cal_tx_output_power(goldval);  //

    gtx_dc_cal_mode = TX_DC_CAL;
    bk7011_cal_tx_dc(goldval);

    gtx_gain_imb_cal_mode = TX_GAIN_IMB_CAL;
    bk7011_cal_tx_gain_imbalance(goldval);

    gtx_phase_imb_cal_mode = TX_PHASE_IMB_CAL;
    bk7011_cal_tx_phase_imbalance(goldval);


    BK7011TRX.REG0x7->bits.chin60 = 0x5d;//2495MHz
    CAL_WR_TRXREGS(0x7);
    delay100us(1);//delay 100us for RFPLL

    bk7011_cal_tx_filter_corner(goldval);
    bk7011_cal_bias();

    gtx_power_cal_mode = TX_WANTED_POWER_CAL;

    BK7011TRX.REG0x7->bits.chin60 = 0xc;//fix to this channel for power cal.
    CAL_WR_TRXREGS(0x7);
    delay100us(1);//delay 100us for RFPLL

    //bk7011_cal_tx_output_power(goldval);  // Actual Power cal.

    bk7011_set_tx_after_cal();
    rwnx_tx_cal_save_cal_result();//by yiming 20170315

    bk7011_rx_cal_en();
    bk7011_cal_rx_dc();

    bk7011_tx_cal_en();


    BK7011TRX.REG0x7->bits.chin60 = 0x5d;//2495MHz
    CAL_WR_TRXREGS(0x7);
    delay100us(1);//delay 100us for RFPLL

    gtx_power_cal_mode = TX_IQ_LOOPBACK_POWER_CAL;
    bk7011_cal_tx_output_power(goldval);

    gtx_dc_cal_mode = TX_DC_LOOPBACK_CAL;
    bk7011_cal_tx_dc(goldval);//Loopback DC cal.


    gtx_gain_imb_cal_mode = TX_GAIN_LOOPBACK_IMB_CAL;
    bk7011_cal_tx_gain_imbalance(goldval);

#ifdef CALIBRATE_TIMES
    if (p_gtx_i_gain_comp_temp_array != NULL)
    {
        p_gtx_i_gain_comp_temp_array[calibrate_time] = ( (*((volatile unsigned long * )(0x01050000 + 0x50 * 4))) >> 16) & 0x03FF ;
        p_gtx_q_gain_comp_temp_array[calibrate_time] = (*((volatile unsigned long * )(0x01050000 + 0x50 * 4))) & 0x03FF ;
    }
#endif

    gtx_phase_imb_cal_mode = TX_PHASE_LOOPBACK_IMB_CAL;
    bk7011_cal_tx_phase_imbalance(goldval);

    bk7011_rx_cal_en();
    bk7011_cal_rx_iq(goldval);

    bk7011_load_calibration_cfg();
    bk7011_set_rx_after_cal();

    rwnx_rx_cal_save_cal_result();
    rwnx_cal_load_default_result();
    rwnx_cal_read_current_cal_result();
    BK7011RCBEKEN.REG0x4C->bits.TXCOMPDIS = 0;

    rwnx_tx_cal_save_cal_result();//by yiming 20170315
    rwnx_cal_load_trx_rcbekn_reg_val();

#if CALI_DPD_TEST
    rwnx_cal_load_trx_rcbekn_reg_val();
    bk7211_cal_tx_dpd_load();
#endif

#if CAL_RESULT_TO_FLASH
    write_cal_result_to_flash();
#endif
    return ;
}

void do_calibration_in_temp_dect(void)
{
    INT32 goldval[32] = {0};

    BK7011TRX.REG0x7->bits.chin60 = 0xc;//fix to this channel for power cal.
    CAL_WR_TRXREGS(0x7);
    delay100us(1);//delay 100us for RFPLL

    bk7011_read_cal_param();
    rwnx_cal_load_default_result();
    cpu_delay(10);//cpu_delay(10);  20160801

    bk7011_cal_bias();
    bk7011_cal_dpll();
    sctrl_dpll_int_open();

    BK7011RCBEKEN.REG0x4C->bits.TXCOMPDIS = 0;

    bk7011_tx_cal_en();

    gtx_power_cal_mode = TX_WANTED_POWER_CAL;
    //bk7011_cal_tx_output_power(goldval);  // second time

    bk7011_set_tx_after_cal();

    //rwnx_cal_read_current_cal_result();
    rwnx_tx_cal_save_cal_result();
    rwnx_cal_load_default_result();
    rwnx_cal_read_current_cal_result();
    BK7011RCBEKEN.REG0x4C->bits.TXCOMPDIS = 0;

    return ;
}

UINT32 gctrlpga20 = 0;
void turnoff_PA_in_temp_dect(void)
{
    CHECK_OPERATE_RF_REG_IF_IN_SLEEP();

    gctrlpga20 = BK7011TRXONLY.REG0xC->bits.gctrlpga20;
    BK7011TRX.REG0xC->bits.gctrlpga20 = 0;
    CAL_WR_TRXREGS(0xC);
    CHECK_OPERATE_RF_REG_IF_IN_SLEEP_END();
}

void turnon_PA_in_temp_dect(void)
{
    CHECK_OPERATE_RF_REG_IF_IN_SLEEP();

    BK7011TRX.REG0xC->bits.gctrlpga20 = gctrlpga20;
    CAL_WR_TRXREGS(0xC);
    CHECK_OPERATE_RF_REG_IF_IN_SLEEP_END();
}

#else  /* CFG_SUPPORT_CALIBRATION */
#endif  /* CFG_SUPPORT_CALIBRATION */

#endif // (CFG_SOC_NAME == SOC_BK7231)
// eof

