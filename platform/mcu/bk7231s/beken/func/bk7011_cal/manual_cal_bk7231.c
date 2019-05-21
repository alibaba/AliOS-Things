#include "include.h"

#if (CFG_SOC_NAME == SOC_BK7231)
#include "arm_arch.h"
#if CFG_SUPPORT_MANUAL_CALI
#include "target_util_pub.h"
#include "mem_pub.h"

#include "drv_model_pub.h"
#include "sys_ctrl_pub.h"
#include "phy.h"

#include "bk7011_cal_pub.h"
#include "bk7011_cal.h"

#include <string.h>
#include "flash_pub.h"
#include "temp_detect_pub.h"
#include "saradc_pub.h"
#include "param_config.h"

#include "rtos_pub.h"
#if CFG_SUPPORT_ALIOS
#include "hal/soc/soc.h"
#else
#include "BkDriverFlash.h"
#endif
#include "power_save_pub.h"

#define TXPWR_DEFAULT_TAB                 1

/* support max work cuurent<250mA@3.3V */
#define CFG_SUPPORT_LOW_MAX_CURRENT       1

#define MCAL_SAVE_MAC_ADDR                1

#define WLAN_2_4_G_CHANNEL_NUM            (14)
#define MAX_RATE_FOR_B_5M                 (5)     // 5.5M
#define MAX_RATE_FOR_B                    (11)    // 11M
#define MAX_RATE_FOR_G                    (54)    // 54M
#define MAX_RATE_MIX_N                    (128)   // MCS0
#define MAX_RATE_FOR_N                    (135)   // MCS7

#define MOD_DIST_G_BW_N20                 (2)
#define MOD_DIST_G_BW_N40                 (4)

#define TXPWR_ELEM_INUSED                 (0)
#define TXPWR_ELEM_UNUSED                 (1)

#define BK_FLASH_SECTOR_SIZE              (4*1024)  
#define BK_FLASH_WRITE_CHECK_TIMES        3

#define MCAL_DEBUG                        1
#include "uart_pub.h"
#if MCAL_DEBUG
#define MCAL_PRT      os_printf
#define MCAL_WARN     os_printf
#define MCAL_FATAL    fatal_prf
#else
#define MCAL_PRT      null_prf
#define MCAL_WARN     null_prf
#define MCAL_FATAL    null_prf
#endif

#define BK_FLASH_OPT_TLV_HEADER           (0x00564c54)   // ASIC TLV
typedef enum{
    TXID                        = 0x11111100,
    TXPWR_TAB_TAB               = 0x22222200,
    TXEND                       = 0xeeeeeeee,
    TXNON                       = 0xffffffff
}TXSTRUCT;

#define DEFAULT_TXID_ID           (12345678)
#define DEFAULT_TXID_THERMAL      (315)
#define DEFAULT_TXID_CHANNEL      (22222222)
#undef  DEFAULT_TXID_XTAL
#define DEFAULT_TXID_XTAL         (33333333)
typedef enum{
    TXID_ID                     = TXID+1,
    TXID_MAC,
    TXID_THERMAL,
    TXID_CHANNEL,
    TXID_XTAL,
    TXID_ADC,
    TXID_END,
    TXID_NON                    = TXID+0xFF
}TXIDList;

typedef enum{
    TXPWR_ENABLE_ID             = TXPWR_TAB_TAB+1,
    TXPWR_TAB_B_ID,
    TXPWR_TAB_G_ID,
    TXPWR_TAB_N_ID,
    TXPWR_TAB_DIF_GN20_ID,
    TXPWR_TAB_DIF_GN40_ID,
    TXPWR_END,
    TXPWR_NON                   = TXPWR_TAB_TAB+0xFF
}TXPWR_ELEM_ID;

typedef enum {
    TXPWR_NONE_RD               = 0u,
    TXPWR_TAB_B_RD              = 0x1u,
    TXPWR_TAB_G_RD              = 0x2u,
    TXPWR_TAB_N_RD              = 0x4u,
} TXPWR_IS_RD;


/* bit[7]: TXPWR flag 
 *          0:  invalid
 *          1:  used
 * bit[4:0]: 5bit TXPWR pwr_gain;
 */
#define FLAG_MASK                       (0x1u)
#define FLAG_POSI                       (7)
#define GAIN_MASK                       (0x1fu)
#define GET_TXPWR_GAIN(p)               ((p)->value & GAIN_MASK)
#define SET_TXPWR_GAIN(p, gain)         {(p)->value &= (~GAIN_MASK); \
                                         (p)->value |= (gain&GAIN_MASK);}

#define GET_TXPWR_FLAG(p)               (((p)->value>>FLAG_POSI)&FLAG_MASK)
#define SET_TXPWR_FLAG(p, flag)         {(p)->value &= (~(FLAG_MASK<<FLAG_POSI)); \
                                         (p)->value |= ((flag&FLAG_MASK)<<FLAG_POSI);}
#define INIT_TXPWR_VALUE(gain, flag)    (((flag&FLAG_MASK)<<FLAG_POSI)|(gain&GAIN_MASK))
typedef struct txpwr_st {
    UINT8 value;
} TXPWR_ST, *TXPWR_PTR;

typedef struct txpwr_elem_st
{
    UINT32 type;
    UINT32 len;
} TXPWR_ELEM_ST, *TXPWR_ELEM_PTR;

typedef struct tag_txpwr_st {
    TXPWR_ELEM_ST head;
}TAG_TXPWR_ST, *TAG_TXPWR_PTR;

typedef struct tag_enable_st {
    TXPWR_ELEM_ST head;
    UINT32 flag;
}TAG_ENABLE_ST, *TAG_ENABLE_PTR;

typedef struct tag_dif_st {
    TXPWR_ELEM_ST head;
    UINT32 differ;
}TAG_DIF_ST, *TAG_DIF_PTR;

typedef struct tag_txpwr_tab_st {
    TXPWR_ELEM_ST head;
    TXPWR_ST tab[WLAN_2_4_G_CHANNEL_NUM];
} __attribute__((packed)) TAG_TXPWR_TAB_ST, *TAG_TXPWR_TAB_PTR;

typedef struct tag_common_st {
    TXPWR_ELEM_ST head;
    UINT32 value;
} TAG_COMM_ST, *TAG_COMM_PTR;

typedef struct tag_chip_mac_st {
    TXPWR_ELEM_ST head;
    UINT8 arry[6];
} __attribute__((packed)) TAG_CHIPMAC_ST, *TAG_CHIPMAC_PTR;

#if TXPWR_DEFAULT_TAB

#if CFG_SUPPORT_LOW_MAX_CURRENT //Max default work current<250mA@3.3V
    const TXPWR_ST gtxpwr_tab_def_b[WLAN_2_4_G_CHANNEL_NUM] = {
        INIT_TXPWR_VALUE(9, TXPWR_ELEM_INUSED),  // ch1  inused
        INIT_TXPWR_VALUE(10, TXPWR_ELEM_UNUSED),
        INIT_TXPWR_VALUE(10, TXPWR_ELEM_UNUSED),    
        INIT_TXPWR_VALUE(10, TXPWR_ELEM_UNUSED),  // ch4
        INIT_TXPWR_VALUE(11, TXPWR_ELEM_UNUSED),
        INIT_TXPWR_VALUE(11, TXPWR_ELEM_UNUSED),
        INIT_TXPWR_VALUE(11, TXPWR_ELEM_UNUSED),  // ch7
        INIT_TXPWR_VALUE(12, TXPWR_ELEM_UNUSED),
        INIT_TXPWR_VALUE(12, TXPWR_ELEM_UNUSED), 
        INIT_TXPWR_VALUE(12, TXPWR_ELEM_UNUSED),  // ch10
        INIT_TXPWR_VALUE(12, TXPWR_ELEM_UNUSED),
        INIT_TXPWR_VALUE(12, TXPWR_ELEM_UNUSED),
        INIT_TXPWR_VALUE(12, TXPWR_ELEM_INUSED),  // ch13  inused
        INIT_TXPWR_VALUE(12, TXPWR_ELEM_UNUSED),
     };
#else
    const TXPWR_ST gtxpwr_tab_def_b[WLAN_2_4_G_CHANNEL_NUM] = {
        INIT_TXPWR_VALUE(19, TXPWR_ELEM_INUSED),  // ch1  inused
        INIT_TXPWR_VALUE(20, TXPWR_ELEM_UNUSED),
        INIT_TXPWR_VALUE(21, TXPWR_ELEM_UNUSED),    
        INIT_TXPWR_VALUE(21, TXPWR_ELEM_UNUSED),  // ch4
        INIT_TXPWR_VALUE(22, TXPWR_ELEM_UNUSED),
        INIT_TXPWR_VALUE(22, TXPWR_ELEM_UNUSED),
        INIT_TXPWR_VALUE(22, TXPWR_ELEM_UNUSED),  // ch7
        INIT_TXPWR_VALUE(23, TXPWR_ELEM_UNUSED),
        INIT_TXPWR_VALUE(23, TXPWR_ELEM_UNUSED), 
        INIT_TXPWR_VALUE(23, TXPWR_ELEM_UNUSED),  // ch10
        INIT_TXPWR_VALUE(24, TXPWR_ELEM_UNUSED),
        INIT_TXPWR_VALUE(24, TXPWR_ELEM_UNUSED),
        INIT_TXPWR_VALUE(24, TXPWR_ELEM_INUSED),  // ch13  inused
        INIT_TXPWR_VALUE(24, TXPWR_ELEM_UNUSED),
    };
#endif

const TXPWR_ST gtxpwr_tab_def_g[WLAN_2_4_G_CHANNEL_NUM] = {
    INIT_TXPWR_VALUE(11, TXPWR_ELEM_INUSED),  // ch1  inused 
    INIT_TXPWR_VALUE(12, TXPWR_ELEM_UNUSED),
    INIT_TXPWR_VALUE(12, TXPWR_ELEM_UNUSED),    
    INIT_TXPWR_VALUE(12, TXPWR_ELEM_UNUSED),  // ch4
    INIT_TXPWR_VALUE(13, TXPWR_ELEM_UNUSED),
    INIT_TXPWR_VALUE(13, TXPWR_ELEM_UNUSED),
    INIT_TXPWR_VALUE(13, TXPWR_ELEM_UNUSED),  // ch7
    INIT_TXPWR_VALUE(14, TXPWR_ELEM_UNUSED),
    INIT_TXPWR_VALUE(14, TXPWR_ELEM_UNUSED), 
    INIT_TXPWR_VALUE(14, TXPWR_ELEM_UNUSED),  // ch10
    INIT_TXPWR_VALUE(15, TXPWR_ELEM_UNUSED),
    INIT_TXPWR_VALUE(15, TXPWR_ELEM_UNUSED),
    INIT_TXPWR_VALUE(15, TXPWR_ELEM_INUSED),  // ch13  inused 
    INIT_TXPWR_VALUE(15, TXPWR_ELEM_UNUSED),
};

const TXPWR_ST gtxpwr_tab_def_n_40[WLAN_2_4_G_CHANNEL_NUM] = {
    INIT_TXPWR_VALUE(7, TXPWR_ELEM_UNUSED),  
    INIT_TXPWR_VALUE(8, TXPWR_ELEM_UNUSED),
    INIT_TXPWR_VALUE(8, TXPWR_ELEM_UNUSED),  // ch3   
    INIT_TXPWR_VALUE(8, TXPWR_ELEM_UNUSED),  
    INIT_TXPWR_VALUE(9, TXPWR_ELEM_UNUSED),
    INIT_TXPWR_VALUE(9, TXPWR_ELEM_UNUSED),
    INIT_TXPWR_VALUE(9, TXPWR_ELEM_UNUSED),  // ch7
    INIT_TXPWR_VALUE(10, TXPWR_ELEM_UNUSED),
    INIT_TXPWR_VALUE(10, TXPWR_ELEM_UNUSED), 
    INIT_TXPWR_VALUE(10, TXPWR_ELEM_UNUSED),  
    INIT_TXPWR_VALUE(11, TXPWR_ELEM_UNUSED),  // ch11  
    INIT_TXPWR_VALUE(11, TXPWR_ELEM_UNUSED),
    INIT_TXPWR_VALUE(11, TXPWR_ELEM_UNUSED),  
    INIT_TXPWR_VALUE(11, TXPWR_ELEM_UNUSED),
};
#endif

TXPWR_ST gtxpwr_tab_b[WLAN_2_4_G_CHANNEL_NUM];
TXPWR_ST gtxpwr_tab_g[WLAN_2_4_G_CHANNEL_NUM];
TXPWR_ST gtxpwr_tab_n[WLAN_2_4_G_CHANNEL_NUM];
TXPWR_ST gtxpwr_tab_n_40[WLAN_2_4_G_CHANNEL_NUM];
UINT32 g_dif_g_n20 = MOD_DIST_G_BW_N20;
UINT32 g_dif_g_n40 = MOD_DIST_G_BW_N40;
UINT32 g_cur_temp = DEFAULT_TXID_THERMAL;

static int manual_cal_fit_txpwr_tab_n_20(UINT32 differ);
static int manual_cal_fit_txpwr_tab_n_40(UINT32 differ);
static void manual_cal_adjust_fitting(TXPWR_PTR ptr, UINT16 dist);

void manual_cal_save_txpwr(UINT32 rate, UINT32 channel, UINT32 pwr_gain)
{
    TXPWR_PTR txpwr_tab_ptr = NULL;
    UINT8  is_g_rate = 0;

    if((channel == 0) || (channel > WLAN_2_4_G_CHANNEL_NUM)) {
        MCAL_WARN("Manual cal wrong channel:%d\r\n", channel);
        return;
    }
    
    channel -= 1;

    if((rate <= MAX_RATE_FOR_B_5M) || (rate == MAX_RATE_FOR_B)) {
        txpwr_tab_ptr = &gtxpwr_tab_b[channel];
    } else if(rate <= MAX_RATE_FOR_G) {
        txpwr_tab_ptr = &gtxpwr_tab_g[channel];
        is_g_rate = 1;
    } else if(rate <= MAX_RATE_FOR_N && rate >= MAX_RATE_MIX_N) {
        txpwr_tab_ptr = &gtxpwr_tab_n_40[channel];
    } else {
        MCAL_FATAL("Manual cal wrong rate:%d\r\n", rate);
        return;
    }
    
    SET_TXPWR_GAIN(txpwr_tab_ptr, pwr_gain);
    SET_TXPWR_FLAG(txpwr_tab_ptr, TXPWR_ELEM_INUSED);  

    if(is_g_rate) {
        // need save n20 
        os_memcpy(&gtxpwr_tab_n[channel], &gtxpwr_tab_g[channel], sizeof(TXPWR_ST));
        manual_cal_adjust_fitting(&gtxpwr_tab_n[channel], g_dif_g_n20);
        MCAL_PRT("fit n20 tab with dist:%d\r\n", g_dif_g_n20);
    }
}

#if CFG_SUPPORT_LOW_MAX_CURRENT//Max default work current<250mA@3.3V
    const UINT16 shift_tab_b[4] = {0, 0, 0, 0}; // 11M base,5.5M,2M,1M
    const UINT16 shift_tab_g[8] = {0, 2, 4, 4, 4, 4, 4, 4}; // 54M base -  6M
    const UINT16 shift_tab_n[1] = {0}; // for MCS7
#else
    const UINT16 shift_tab_b[4] = {0, 2, 4, 6}; // 11M base,5.5M,2M,1M
    const UINT16 shift_tab_g[8] = {0, 2, 4, 6, 8, 10, 12, 14}; // 54M base -  6M
    const UINT16 shift_tab_n[1] = {0}; // for MCS7
#endif

UINT32 manual_cal_get_pwr_idx_shift(UINT32 rate, UINT32 bandwidth, UINT32 *pwr_gain)
{
    const UINT16 *shift;
    UINT32 idex, ret = 2;

    if(bandwidth == PHY_CHNL_BW_20) 
    {
        if(rate <= 3){
            shift = shift_tab_b;
            idex = 3 - rate;
            ret = 1;
        }
        else if(rate <= 11 ){  // for g: 6-54M -- 3-11
            shift = shift_tab_g;
            idex = 11 - rate;
        }else if((rate >= 128) && (rate < 135)){ // for MCS0-6
            shift = shift_tab_g;
            idex = 134 - rate;
        }else if(rate == 135){ // for MCS7
            shift = shift_tab_n;
            idex = 0;
        }else{
            MCAL_FATAL("\r\nget_pwr_idx_shift wrong rate:%d\r\n", rate);
            return 0;
        }
    }
    else {
        if((rate >= 128) && (rate < 135)){
            shift = shift_tab_g;
            idex = 134 - rate;
        }else if(rate == 135){ // for MCS7
            shift = shift_tab_n;
            idex = 0;
        }else{
            MCAL_FATAL("\r\nget_pwr_idx_shift wrong rate:%d\r\n", rate);
            return 0;
        }        
    }

    MCAL_PRT("get_pwr_info: idx: %d, pwr:%d", shift[idex], *pwr_gain);
   
    idex = shift[idex] + *pwr_gain;
    *pwr_gain = (idex > 31)? 31: idex;
    
    MCAL_PRT("--pwr:%d\r\n", *pwr_gain);
    
    return ret;
}

int manual_cal_get_txpwr(UINT32 rate, UINT32 channel, UINT32 bandwidth, UINT32 *pwr_gain)
{
    TXPWR_PTR txpwr_tab_ptr = NULL;

    if((channel == 0) || (channel > WLAN_2_4_G_CHANNEL_NUM)) {
        MCAL_WARN("Manual cal wrong channel:%d\r\n", channel);
        return 0;
    }
    
    if((bandwidth != PHY_CHNL_BW_20) && (bandwidth != PHY_CHNL_BW_40)) {
        MCAL_WARN("Manual cal wrong bandwidth:%d\r\n", bandwidth);
        return 0;
    }
   
    channel -= 1;

    if(bandwidth == PHY_CHNL_BW_20) {
        if((rate <= MAX_RATE_FOR_B_5M) || (rate == MAX_RATE_FOR_B)) {
            txpwr_tab_ptr = &gtxpwr_tab_b[channel];
        } else if(rate < MAX_RATE_FOR_N) {  
            // MAX_RATE_FOR_N is right, for only MCS7 need change
            txpwr_tab_ptr = &gtxpwr_tab_g[channel];
        } else if(rate == MAX_RATE_FOR_N){
            txpwr_tab_ptr = &gtxpwr_tab_n[channel];    
        } else {
            MCAL_FATAL("\r\nManual cal wrong rate:%d\r\n", rate);
            return 0;
        }
    } else {
        if(rate < MAX_RATE_FOR_N && rate >= MAX_RATE_MIX_N) {  
            // MAX_RATE_FOR_N is right, for only MCS7 need change
            txpwr_tab_ptr = &gtxpwr_tab_g[channel];
        } else if(rate == MAX_RATE_FOR_N) {  
            // MAX_RATE_FOR_N_40 
            txpwr_tab_ptr = &gtxpwr_tab_n_40[channel];
        } else {
            MCAL_FATAL("\r\nManual cal wrong rate with BW40? %d:%d\r\n", bandwidth, rate);
            txpwr_tab_ptr = &gtxpwr_tab_g[channel];
        }
    }

    *pwr_gain = GET_TXPWR_GAIN(txpwr_tab_ptr);
     MCAL_PRT("get txpwrtab gain:%d,ch:%d\r\n", *pwr_gain, channel+1);
	 
     return 1;
}


void manual_cal_show_txpwr_tab(void)
{
    TXPWR_PTR txpwr_tab_ptr = NULL;
    UINT32 i;
    
    MCAL_PRT("txpwr table for b:\r\n");
    for(i=0; i<WLAN_2_4_G_CHANNEL_NUM; i++) {
        txpwr_tab_ptr = &gtxpwr_tab_b[i];
        MCAL_PRT("ch:%2d: value:%02x, flag:%01d, gain:%02d\r\n", 
            i+1, txpwr_tab_ptr->value, 
            GET_TXPWR_FLAG(txpwr_tab_ptr), GET_TXPWR_GAIN(txpwr_tab_ptr));
    }

    MCAL_PRT("\r\ntxpwr table for g:\r\n");
    for(i=0; i<WLAN_2_4_G_CHANNEL_NUM; i++) {
        txpwr_tab_ptr = &gtxpwr_tab_g[i];
        MCAL_PRT("ch:%2d: value:%02x, flag:%01d, gain:%02d\r\n", 
            i+1, txpwr_tab_ptr->value, 
            GET_TXPWR_FLAG(txpwr_tab_ptr), GET_TXPWR_GAIN(txpwr_tab_ptr));
    }  

    MCAL_PRT("\r\ntxpwr table for n20:\r\n");
    for(i=0; i<WLAN_2_4_G_CHANNEL_NUM; i++) {
        txpwr_tab_ptr = &gtxpwr_tab_n[i];
        MCAL_PRT("ch:%2d: value:%02x, flag:%01d, gain:%02d\r\n", 
            i+1, txpwr_tab_ptr->value, 
            GET_TXPWR_FLAG(txpwr_tab_ptr), GET_TXPWR_GAIN(txpwr_tab_ptr));
    }  

    MCAL_PRT("\r\ntxpwr table for n40:\r\n");
    for(i=0; i<WLAN_2_4_G_CHANNEL_NUM; i++) {
        txpwr_tab_ptr = &gtxpwr_tab_n_40[i];
        MCAL_PRT("ch:%2d: value:%02x, flag:%01d, gain:%02d\r\n", 
            i+1, txpwr_tab_ptr->value, 
            GET_TXPWR_FLAG(txpwr_tab_ptr), GET_TXPWR_GAIN(txpwr_tab_ptr));
    }

    MCAL_PRT("\r\n sys temper:%d\r\n", g_cur_temp);
}

TXPWR_IS_RD manual_cal_txpwr_tab_is_fitted(void)
{
    return (TXPWR_TAB_B_RD | TXPWR_TAB_G_RD | TXPWR_TAB_N_RD);
}

static void manual_cal_adjust_fitting(TXPWR_PTR ptr, UINT16 dist)
{   
    UINT16 gain;
    //if(GET_TXPWR_FLAG(ptr) == TXPWR_ELEM_UNUSED) 
    //    return;
    
    gain = GET_TXPWR_GAIN(ptr);
    gain = (gain > dist)? gain - dist : 0;

    SET_TXPWR_GAIN(ptr, gain);
}

static void manual_cal_do_fitting_base(TXPWR_PTR dst, TXPWR_PTR srclow, TXPWR_PTR srchigh)
{
    UINT16 gain = 0, dist;
    
    if((GET_TXPWR_FLAG(srclow) == TXPWR_ELEM_UNUSED) 
        || (GET_TXPWR_FLAG(srchigh) == TXPWR_ELEM_UNUSED))
        return;

    if(GET_TXPWR_GAIN(srclow) > GET_TXPWR_GAIN(srchigh)) {
        dist = GET_TXPWR_GAIN(srclow) - GET_TXPWR_GAIN(srchigh);
        gain = (UINT16)((float)(dist) / 3.0 + 0.5) + GET_TXPWR_GAIN(srchigh) + 1;
        //gain = (UINT16)((float)(dist) / 2.0 + 0.5) + GET_TXPWR_GAIN(srchigh);
    }
    else {
        dist = GET_TXPWR_GAIN(srchigh) - GET_TXPWR_GAIN(srclow);
        gain = (UINT16)((float)(dist) / 3.0 + 0.5) + GET_TXPWR_GAIN(srclow) + 1;
        //gain = (UINT16)((float)(dist) / 2.0 + 0.5) + GET_TXPWR_GAIN(srclow);
    }
    
    SET_TXPWR_GAIN(dst, gain);
    SET_TXPWR_FLAG(dst, TXPWR_ELEM_INUSED);
}

static void manual_cal_do_fitting(TXPWR_PTR dst, TXPWR_PTR srclow, TXPWR_PTR srchigh)
{
    UINT16 gain = 0;
    
    if((GET_TXPWR_FLAG(srclow) == TXPWR_ELEM_UNUSED) 
        || (GET_TXPWR_FLAG(srchigh) == TXPWR_ELEM_UNUSED))
        return;
    
    gain = (UINT16)((float)(GET_TXPWR_GAIN(srclow) + GET_TXPWR_GAIN(srchigh)) / 2 + 0.5);
    SET_TXPWR_GAIN(dst, gain);
    SET_TXPWR_FLAG(dst, TXPWR_ELEM_INUSED);
}

UINT32 manual_cal_fitting_txpwr_tab(void)
{
    UINT32 ret = 0, i;
    TXPWR_PTR tab_ptr = NULL;

    // for b, check ch1, ch7, ch13 is in used
    tab_ptr = gtxpwr_tab_b;
    if((GET_TXPWR_FLAG(&tab_ptr[0]) == TXPWR_ELEM_UNUSED)  
      ||(GET_TXPWR_FLAG(&tab_ptr[12]) == TXPWR_ELEM_UNUSED) ){  
        MCAL_WARN("txpwr table for b fitting failed!, ch1 ch13 unused\r\n");
    } else {
        if(GET_TXPWR_FLAG(&tab_ptr[6]) == TXPWR_ELEM_UNUSED) {
            // fitting ch7, use ch1, ch13  
            manual_cal_do_fitting_base(&tab_ptr[6], &tab_ptr[0], &tab_ptr[12]);
        }
        // fitting ch4, use ch1, ch7  
        manual_cal_do_fitting(&tab_ptr[3], &tab_ptr[0], &tab_ptr[6]);
        // fitting ch2, use ch1, ch4  
        manual_cal_do_fitting(&tab_ptr[1], &tab_ptr[0], &tab_ptr[3]);
        // fitting ch3, use ch2, ch4  
        manual_cal_do_fitting(&tab_ptr[2], &tab_ptr[1], &tab_ptr[3]); 
        // fitting ch5, use ch4, ch7  
        manual_cal_do_fitting(&tab_ptr[4], &tab_ptr[3], &tab_ptr[6]);
        // fitting ch6, use ch5, ch7  
        manual_cal_do_fitting(&tab_ptr[5], &tab_ptr[4], &tab_ptr[6]); 

        // fitting ch10, use ch7, ch13  
        manual_cal_do_fitting(&tab_ptr[9], &tab_ptr[6], &tab_ptr[12]);
        // fitting ch8, use ch7, ch10  
        manual_cal_do_fitting(&tab_ptr[7], &tab_ptr[6], &tab_ptr[9]);
        // fitting ch9, use ch8, ch10  
        manual_cal_do_fitting(&tab_ptr[8], &tab_ptr[7], &tab_ptr[9]); 
        // fitting ch11, use ch10, ch13  
        manual_cal_do_fitting(&tab_ptr[10], &tab_ptr[9], &tab_ptr[12]);
        // fitting ch12, use ch11, ch13 
        manual_cal_do_fitting(&tab_ptr[11], &tab_ptr[10], &tab_ptr[12]);

        // fitting ch14, the same as ch13
        manual_cal_do_fitting(&tab_ptr[13], &tab_ptr[12], &tab_ptr[12]); 

        // clear flag, only base change set used
        for(i=0; i<WLAN_2_4_G_CHANNEL_NUM; i++){
            if((i==0) || (i==12))
                continue;
            SET_TXPWR_FLAG(&tab_ptr[i], TXPWR_ELEM_UNUSED);
        }
        ret = 1;
    }

    // for g
    tab_ptr = gtxpwr_tab_g;
    if((GET_TXPWR_FLAG(&tab_ptr[0]) == TXPWR_ELEM_UNUSED)  
      ||(GET_TXPWR_FLAG(&tab_ptr[12]) == TXPWR_ELEM_UNUSED) ){ 
        MCAL_WARN("txpwr table for g fitting failed!, ch1 ch13 unused\r\n");
    } else {
        if(GET_TXPWR_FLAG(&tab_ptr[6]) == TXPWR_ELEM_UNUSED) {
            // fitting ch7, use ch1, ch13  
            manual_cal_do_fitting_base(&tab_ptr[6], &tab_ptr[0], &tab_ptr[12]);
        }
    
        // fitting ch4, use ch1, ch7  
        manual_cal_do_fitting(&tab_ptr[3], &tab_ptr[0], &tab_ptr[6]);
        // fitting ch2, use ch1, ch4  
        manual_cal_do_fitting(&tab_ptr[1], &tab_ptr[0], &tab_ptr[3]);
        // fitting ch3, use ch2, ch4  
        manual_cal_do_fitting(&tab_ptr[2], &tab_ptr[1], &tab_ptr[3]); 
        // fitting ch5, use ch4, ch7  
        manual_cal_do_fitting(&tab_ptr[4], &tab_ptr[3], &tab_ptr[6]);
        // fitting ch6, use ch5, ch7  
        manual_cal_do_fitting(&tab_ptr[5], &tab_ptr[4], &tab_ptr[6]); 

        // fitting ch10, use ch7, ch13  
        manual_cal_do_fitting(&tab_ptr[9], &tab_ptr[6], &tab_ptr[12]);
        // fitting ch8, use ch7, ch10  
        manual_cal_do_fitting(&tab_ptr[7], &tab_ptr[6], &tab_ptr[9]);
        // fitting ch9, use ch8, ch10  
        manual_cal_do_fitting(&tab_ptr[8], &tab_ptr[7], &tab_ptr[9]); 
        // fitting ch11, use ch10, ch13  
        manual_cal_do_fitting(&tab_ptr[10], &tab_ptr[9], &tab_ptr[12]);
        // fitting ch12, use ch11, ch13 
        manual_cal_do_fitting(&tab_ptr[11], &tab_ptr[10], &tab_ptr[12]);

        // fitting ch14, the same as ch13
        manual_cal_do_fitting(&tab_ptr[13], &tab_ptr[12], &tab_ptr[12]);

        // clear flag, only base change set used
        for(i=0; i<WLAN_2_4_G_CHANNEL_NUM; i++){
            if((i==0) || (i==12))
                continue;
            SET_TXPWR_FLAG(&tab_ptr[i], TXPWR_ELEM_UNUSED);
        }
        
        // fit n20 synch
        manual_cal_fit_txpwr_tab_n_20(g_dif_g_n20);
 
        ret = 1;
    }


    // for n 40
    tab_ptr = gtxpwr_tab_n_40;
    if((GET_TXPWR_FLAG(&tab_ptr[2]) == TXPWR_ELEM_UNUSED)   // ch3
      //||(GET_TXPWR_FLAG(&tab_ptr[6]) == TXPWR_ELEM_UNUSED)  // ch7
      ||(GET_TXPWR_FLAG(&tab_ptr[10]) == TXPWR_ELEM_UNUSED) ){  // ch11
        MCAL_WARN("txpwr table for n40 ch3 ch11 unused, use g for fitting\r\n");
        
        // fit n40 synch
        manual_cal_fit_txpwr_tab_n_40(g_dif_g_n40);
    } else {
        if(GET_TXPWR_FLAG(&tab_ptr[6]) == TXPWR_ELEM_UNUSED) {
            // fitting ch7, use ch1, ch13  
            manual_cal_do_fitting_base(&tab_ptr[6], &tab_ptr[2], &tab_ptr[10]);
        }

        // fitting ch4, use ch3, ch7  
        manual_cal_do_fitting(&tab_ptr[3], &tab_ptr[2], &tab_ptr[6]);
        // fitting ch2, the same as ch3
        manual_cal_do_fitting(&tab_ptr[1], &tab_ptr[2], &tab_ptr[2]);
        // fitting ch1, the same as ch3  
        manual_cal_do_fitting(&tab_ptr[0], &tab_ptr[2], &tab_ptr[2]); 
        // fitting ch5, use ch4, ch7  
        manual_cal_do_fitting(&tab_ptr[4], &tab_ptr[3], &tab_ptr[6]);
        // fitting ch6, use ch5, ch7  
        manual_cal_do_fitting(&tab_ptr[5], &tab_ptr[4], &tab_ptr[6]); 

        // fitting ch10, use ch7, ch11  
        manual_cal_do_fitting(&tab_ptr[9], &tab_ptr[6], &tab_ptr[10]);
        // fitting ch8, use ch7, ch10  
        manual_cal_do_fitting(&tab_ptr[7], &tab_ptr[6], &tab_ptr[9]);
        // fitting ch9, use ch8, ch10  
        manual_cal_do_fitting(&tab_ptr[8], &tab_ptr[7], &tab_ptr[9]); 

        // fitting ch12, the same as ch11
        manual_cal_do_fitting(&tab_ptr[11], &tab_ptr[10], &tab_ptr[10]);
        // fitting ch13, the same as ch11
        manual_cal_do_fitting(&tab_ptr[12], &tab_ptr[10], &tab_ptr[10]);
        // fitting ch14, the same as ch11
        manual_cal_do_fitting(&tab_ptr[13], &tab_ptr[10], &tab_ptr[10]);
        ret = 1;

        // clear flag, only base change set used 
        for(i=0; i<WLAN_2_4_G_CHANNEL_NUM; i++){
            if((i==2) || (i==10))
                continue;
            SET_TXPWR_FLAG(&tab_ptr[i], TXPWR_ELEM_UNUSED);
        }
    }

    return ret;
}

static int manual_cal_fit_txpwr_tab_n_20(UINT32 differ)
{
    TXPWR_PTR tab_ptr_temp, tab_ptr = NULL;
    UINT32 i = 0;
    
    tab_ptr = gtxpwr_tab_n;
    tab_ptr_temp = gtxpwr_tab_g;
    
    for(i=0; i<WLAN_2_4_G_CHANNEL_NUM; i++) {
        os_memcpy(&tab_ptr[i], &tab_ptr_temp[i], sizeof(TXPWR_ST));
        manual_cal_adjust_fitting(&tab_ptr[i], differ);
    }
}

static int manual_cal_fit_txpwr_tab_n_40(UINT32 differ)
{
    TXPWR_PTR tab_ptr_temp, tab_ptr = NULL;
    UINT32 i = 0;
    
    tab_ptr = gtxpwr_tab_n_40;
    tab_ptr_temp = gtxpwr_tab_g;
    
    for(i=0; i<WLAN_2_4_G_CHANNEL_NUM; i++) {
        os_memcpy(&tab_ptr[i], &tab_ptr_temp[i], sizeof(TXPWR_ST));
        manual_cal_adjust_fitting(&tab_ptr[i], differ);
    }
}

void manual_cal_set_dif_g_n20(UINT32 diff)
{
    if(diff == g_dif_g_n20) {
        MCAL_PRT("no need to do, same with g_dif_g_n20:%d\r\n", g_dif_g_n20);
        return;
    }
    
    if(diff > 31)
        diff = 31;
    
    g_dif_g_n20 = (diff&GAIN_MASK);
    manual_cal_fit_txpwr_tab_n_20(g_dif_g_n20);
}

void manual_cal_set_dif_g_n40(UINT32 diff)
{
    if(diff == g_dif_g_n40) {
        MCAL_PRT("no need to do, same with g_dif_g_n40:%d\r\n", g_dif_g_n40);
        return;
    }

    if(diff > 31)
        diff = 31;
    
    g_dif_g_n40 = (diff&GAIN_MASK);
    manual_cal_fit_txpwr_tab_n_40(g_dif_g_n40);
}

////////////////////////////////////////////////////////////////////////////////
static UINT32 manual_cal_search_opt_tab(UINT32 *len)
{
    UINT32 ret = 0, status;
    DD_HANDLE flash_handle;
    TXPWR_ELEM_ST head;
	#if CFG_SUPPORT_ALIOS
	hal_logic_partition_t *pt = hal_flash_get_info(HAL_PARTITION_RF_FIRMWARE);
	#else
	bk_logic_partition_t *pt = bk_flash_get_info(BK_PARTITION_RF_FIRMWARE);
	#endif
		
    *len = 0;
    flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);
    ddev_read(flash_handle, (char *)&head, sizeof(TXPWR_ELEM_ST), pt->partition_start_addr);//TXPWR_TAB_FLASH_ADDR);
    if(BK_FLASH_OPT_TLV_HEADER == head.type){
        *len = head.len + sizeof(TAG_TXPWR_ST);
        ret = 1;
    } 
    ddev_close(flash_handle);

    return ret;
}

static UINT32 manual_cal_search_txpwr_tab(UINT32 type, UINT32 start_addr)
{
    UINT32 status, addr, end_addr;
    DD_HANDLE flash_handle;
    TXPWR_ELEM_ST head;

    status = manual_cal_search_opt_tab(&addr);
    if(!status) {
        return 0;
    }

    flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);
    ddev_read(flash_handle, (char *)&head, sizeof(TXPWR_ELEM_ST), start_addr);
    addr = start_addr + sizeof(TXPWR_ELEM_ST);
    end_addr = addr + head.len;
    while(addr < end_addr) {
        ddev_read(flash_handle, (char *)&head, sizeof(TXPWR_ELEM_ST), addr);
        if(type == head.type){
            break;
        } else {
            addr += sizeof(TXPWR_ELEM_ST);
            addr += head.len;
        }
    }

    if(addr >= end_addr) {
        addr = 0;
    }
    ddev_close(flash_handle);

    return addr;
}

static UINT8 manual_cal_update_flash_area(UINT32 addr_offset, char *buf, UINT32 len)
{
    DD_HANDLE flash_handle = DD_HANDLE_UNVALID;
    UINT32 param, status, write_len, write_addr, flash_len;
    UINT8 ret = 0;
    UINT8 *read_buf = NULL;
    UINT8 check_times = BK_FLASH_WRITE_CHECK_TIMES;
	#if CFG_SUPPORT_ALIOS
	hal_logic_partition_t *pt = hal_flash_get_info(HAL_PARTITION_RF_FIRMWARE);
	#else
	bk_logic_partition_t *pt = bk_flash_get_info(BK_PARTITION_RF_FIRMWARE);
	#endif
	
    if(addr_offset > (BK_FLASH_SECTOR_SIZE)){
        MCAL_FATAL("write flash addroffset error:%08x\r\n", addr_offset);
        return 1;
    }
    if(len == 0)
        return 0;

    status = manual_cal_search_opt_tab(&flash_len);
    if(status && (flash_len >= (addr_offset + len))) {
        // read all flash otp
        write_len = flash_len;
    } else {
        write_len = addr_offset + len;
    }
    
    write_addr = pt->partition_start_addr;//TXPWR_TAB_FLASH_ADDR;

    read_buf = (UINT8*)os_malloc(write_len); 
    if(!read_buf){
        MCAL_FATAL("cann't malloc buf for flash write\r\n");
        ret = 1;
        goto updata_exit;
    }

    flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);

write_again:
    if(addr_offset) {   
        status = ddev_read(flash_handle, read_buf, write_len, write_addr);
        if(status != FLASH_SUCCESS) {
            MCAL_FATAL("cann't read flash before write\r\n");
            ret = 1;
            goto updata_exit;
        }
    }

    os_memcpy(read_buf+addr_offset, buf, len);

	#if CFG_SUPPORT_ALIOS
	hal_flash_dis_secure(0, 0, 0);
	#else
	bk_flash_enable_security(FLASH_PROTECT_NONE);
	#endif

    param = write_addr;
    ddev_control(flash_handle, CMD_FLASH_ERASE_SECTOR, &param);
    
    status = ddev_write(flash_handle, (char *)read_buf, write_len, write_addr);

    if(status != FLASH_SUCCESS) {
        MCAL_FATAL("save txpwr tab to flash failed\r\n");
        ret = 1;
        goto updata_exit;
    }
	
	#if CFG_SUPPORT_ALIOS
	hal_flash_enable_secure(0, 0, 0);
	#else
	bk_flash_enable_security(FLASH_UNPROTECT_LAST_BLOCK);
	#endif

    {
        UINT8 *check_buf = read_buf, *org_buf = (UINT8 *)buf;
        UINT32 check_addr = write_addr + addr_offset;
        UINT8 check_result;

        os_memset(check_buf, 0, len);   
        
        status = ddev_read(flash_handle, check_buf, len, check_addr);
        if(status != FLASH_SUCCESS) {
            MCAL_FATAL("cann't read flash in check\r\n");
            ret = 1;
            goto updata_exit;
        }

        check_result = 1;
        for(int i=0; i<len; i++) {
            if(check_buf[i] != org_buf[i]) {
                check_result = 0;
                break;
            }
        }

        if(check_result) 
        {
            //MCAL_PRT("\r\nshow txpwr tags before write flash:\r\n");
            #if 1
            for(int i=0; i<len; i++) 
            {
                MCAL_PRT("%02x,", org_buf[i]);
                if((i+1)%16 == 0)
                    MCAL_PRT("\r\n");
            }
            MCAL_PRT("\r\n");
            #endif
            MCAL_PRT("manual_cal_write_flash ok\r\n");
        
        }
        else 
        {
            check_times--;
            if(check_times)
               goto write_again;
            else
               MCAL_PRT("manual_cal_write_flash failed\r\n"); 
        }  
    }
    
updata_exit:
    if(flash_handle != DD_HANDLE_UNVALID)
        ddev_close(flash_handle);

    if(read_buf)
        os_free(read_buf);
    
    return ret;    
}

static UINT8 manual_cal_read_flash(UINT32 addr_offset, char *buf, UINT32 len)
{
    UINT32 status, addr;
    UINT8 ret = 0;
    DD_HANDLE flash_handle;
	#if CFG_SUPPORT_ALIOS
    hal_logic_partition_t *pt = hal_flash_get_info(HAL_PARTITION_RF_FIRMWARE);
	#else
    bk_logic_partition_t *pt = bk_flash_get_info(BK_PARTITION_RF_FIRMWARE);
	#endif
	
    if(addr_offset > (BK_FLASH_SECTOR_SIZE)){
        MCAL_FATAL("read flash addr error:%08x\r\n", addr_offset);
        return 0;
    }

    addr = pt->partition_start_addr+addr_offset;//TXPWR_TAB_FLASH_ADDR + addr_offset;
    
    flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);
    status = ddev_read(flash_handle, buf, len, addr);
    ddev_close(flash_handle);

    if(status != FLASH_SUCCESS)
        ret = 0;
    else 
        ret = len;
    
    return ret;
}

UINT32 manual_cal_load_txpwr_tab_flash(void)
{
    UINT32 status, addr, addr_start;
    DD_HANDLE flash_handle;
    TXPWR_ELEM_ST head;
    TXPWR_IS_RD is_ready_flash = 0; 
	#if CFG_SUPPORT_ALIOS
	hal_logic_partition_t *pt = hal_flash_get_info(HAL_PARTITION_RF_FIRMWARE);
	#else
	bk_logic_partition_t *pt = bk_flash_get_info(BK_PARTITION_RF_FIRMWARE);
	#endif

    addr_start = manual_cal_search_txpwr_tab(TXPWR_TAB_TAB, pt->partition_start_addr);//TXPWR_TAB_FLASH_ADDR); 
    if(!addr_start) {
        MCAL_WARN("NO TXPWR_TAB_TAB found in flash\r\n");
        return TXPWR_NONE_RD;
    }
    
    addr = manual_cal_search_txpwr_tab(TXPWR_ENABLE_ID, addr_start);  
    if(!addr)
    {
        MCAL_WARN("NO TXPWR_ENABLE_ID found in flash\r\n");
        return TXPWR_NONE_RD;
    }

    flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);
    ddev_read(flash_handle, (char *)&head, sizeof(TXPWR_ELEM_ST), addr);
    ddev_read(flash_handle, (char *)&status, head.len, addr + sizeof(TXPWR_ELEM_ST));
    is_ready_flash = status;
    MCAL_PRT("flash txpwr table:0x%x\r\n", is_ready_flash);

    // If txpwr tab in flash is unused, we should use auto calibration result    
    if(is_ready_flash == TXPWR_NONE_RD){
        MCAL_WARN("txpwr tabe in flash is unused\r\n");
        return TXPWR_NONE_RD;
    }

    // for txpwr b
    if(is_ready_flash & TXPWR_TAB_B_RD) {
        addr = manual_cal_search_txpwr_tab(TXPWR_TAB_B_ID, addr_start);
        if(addr) {
            ddev_read(flash_handle, (char *)&head, sizeof(TXPWR_ELEM_ST), addr);
            ddev_read(flash_handle, (char *)gtxpwr_tab_b, head.len, addr + sizeof(TXPWR_ELEM_ST));
        }else {
            MCAL_WARN("txpwr tabe b in flash no found\r\n");
        }
    }

    // for txpwr g and n20?  
    if(is_ready_flash & TXPWR_TAB_G_RD) {
        //for g first
        addr = manual_cal_search_txpwr_tab(TXPWR_TAB_G_ID, addr_start);
        if(addr) {
            ddev_read(flash_handle, (char *)&head, sizeof(TXPWR_ELEM_ST), addr);
            ddev_read(flash_handle, (char *)gtxpwr_tab_g, head.len, addr + sizeof(TXPWR_ELEM_ST));            
        } else {
            MCAL_WARN("txpwr tabe g in flash no found\r\n");
        }

        // for n20    
        addr = manual_cal_search_txpwr_tab(TXPWR_TAB_DIF_GN20_ID, addr_start);
        if(addr) {
            ddev_read(flash_handle, (char *)&head, sizeof(TXPWR_ELEM_ST), addr);
            ddev_read(flash_handle, (char *)&g_dif_g_n20, head.len, addr + sizeof(TXPWR_ELEM_ST));      
            MCAL_PRT("dif g and n20 ID in flash:%d\r\n", g_dif_g_n20);
        } else {
            MCAL_WARN("dif g and n20 ID in flash no found, use def:%d\r\n", g_dif_g_n20);
        }
        manual_cal_fit_txpwr_tab_n_20(g_dif_g_n20); 
    }

    // for txpwr N40   
    if(is_ready_flash & TXPWR_TAB_N_RD) {
        addr = manual_cal_search_txpwr_tab(TXPWR_TAB_N_ID, addr_start);
        if(addr) {
            ddev_read(flash_handle, (char *)&head, sizeof(TXPWR_ELEM_ST), addr);
            ddev_read(flash_handle, (char *)gtxpwr_tab_n_40, head.len, addr + sizeof(TXPWR_ELEM_ST));
        }else {
            MCAL_WARN("txpwr tabe n in flash no found\r\n");
        }
    }
    // only need load dist40
    addr = manual_cal_search_txpwr_tab(TXPWR_TAB_DIF_GN40_ID, addr_start);
    if(addr) {
        ddev_read(flash_handle, (char *)&head, sizeof(TXPWR_ELEM_ST), addr);
        ddev_read(flash_handle, (char *)&g_dif_g_n40, head.len, addr + sizeof(TXPWR_ELEM_ST));      
        MCAL_PRT("dif g and n40 ID in flash:%d\r\n", g_dif_g_n40);
    } else {
        MCAL_WARN("dif g and n40 ID in flash no found, use def:%d\r\n", g_dif_g_n40);
    }
    
    ddev_close(flash_handle);
    MCAL_PRT("read txpwr tab from flash success\r\n");
    
    return is_ready_flash;
}

UINT32 manual_cal_load_default_txpwr_tab(UINT32 is_ready_flash)
{
    #if TXPWR_DEFAULT_TAB
    if(!(is_ready_flash & TXPWR_TAB_B_RD)) {
        os_memcpy(gtxpwr_tab_b, gtxpwr_tab_def_b, sizeof(gtxpwr_tab_def_b));
        MCAL_WARN("Load default txpwr for b:%p\r\n", gtxpwr_tab_def_b);
    }

    if(!(is_ready_flash & TXPWR_TAB_G_RD)) {
        os_memcpy(gtxpwr_tab_g, gtxpwr_tab_def_g, sizeof(gtxpwr_tab_def_g));
        MCAL_WARN("Load default txpwr for g:%p\r\n", gtxpwr_tab_def_g);

        manual_cal_fit_txpwr_tab_n_20(g_dif_g_n20); 
        MCAL_WARN("fit n20 table with dist:%d\r\n", g_dif_g_n20);
    }
    
    if(!(is_ready_flash & TXPWR_TAB_N_RD)) {
        os_memcpy(gtxpwr_tab_n_40, gtxpwr_tab_def_n_40, sizeof(gtxpwr_tab_def_n_40));
        MCAL_WARN("Load default txpwr for n40:%p\r\n", gtxpwr_tab_def_n_40);
    }
    #endif
    return 0;
}

int manual_cal_save_txpwr_tab_to_flash(void)
{
    TXPWR_IS_RD is_ready, is_ready_flash;
    UINT32 len = 0, txpwr_len = 0, flash_len = 0;
    UINT8 *buf = NULL, *txpwr_buf = NULL;;
    TAG_TXPWR_ST tag_txpwr;
    TAG_TXPWR_PTR tag_txpwr_ptr = NULL;
    TAG_ENABLE_ST tag_enable;    
    TAG_ENABLE_PTR tag_enable_ptr = NULL;
    TAG_TXPWR_TAB_ST tag_tab;
    TAG_TXPWR_TAB_PTR tag_tab_ptr = NULL;
    TAG_DIF_ST tag_dif;
    TAG_DIF_PTR tag_dif_ptr = NULL;

    UINT32 status, addr, addr_start;
    DD_HANDLE flash_handle;
    TXPWR_ELEM_ST head;
	#if CFG_SUPPORT_ALIOS
	hal_logic_partition_t *pt = hal_flash_get_info(HAL_PARTITION_RF_FIRMWARE);
	#else
	bk_logic_partition_t *pt = bk_flash_get_info(BK_PARTITION_RF_FIRMWARE);
	#endif

    is_ready = manual_cal_txpwr_tab_is_fitted();
    MCAL_PRT("current txpwr table:0x%x\r\n", is_ready);
    if(is_ready == TXPWR_NONE_RD) {
        MCAL_WARN("TXPWR_NONE_RD, Cann't save txpwr tabe in flash\r\n");
        return 0;
    }

    // alloc all memery at onece, so we no need to change the size of buf in combin function
    txpwr_len = sizeof(TAG_TXPWR_ST) + sizeof(TAG_ENABLE_ST) 
            + 3*sizeof(TAG_TXPWR_TAB_ST) + 2*sizeof(TAG_DIF_ST);

    
    // read flash, then combin the table in flash
    flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);
    status = manual_cal_search_opt_tab(&flash_len);
    // Has TLV Header?
    if(status) {
        addr_start = manual_cal_search_txpwr_tab(TXPWR_TAB_TAB, pt->partition_start_addr);//TXPWR_TAB_FLASH_ADDR); 
        if(!addr_start) {
            // not TXPWR_TAB_TAB, but has any other id, so attch TXPWR_TAB_TAB after the table
            len = flash_len + txpwr_len;
            buf = (UINT8*)os_malloc(len); 
            if(!buf) {
                MCAL_FATAL("no memory for chipinfo save to flash\r\n");
                return 0;
            }
            // copy flash
            ddev_read(flash_handle, (char *)buf, flash_len, pt->partition_start_addr);//TXPWR_TAB_FLASH_ADDR);
            ddev_close(flash_handle);

            // updata new TLV header
            tag_txpwr_ptr = (TAG_TXPWR_PTR)buf;
            tag_txpwr.head.type= BK_FLASH_OPT_TLV_HEADER;
            tag_txpwr.head.len = len - sizeof(TAG_TXPWR_ST);;
            os_memcpy(tag_txpwr_ptr, &tag_txpwr, sizeof(TAG_TXPWR_ST));

            txpwr_buf = (UINT8 *)(buf + flash_len);
            addr_start = pt->partition_start_addr;//TXPWR_TAB_FLASH_ADDR;
        }else {
            // txpwr is exist, and found start addr.
            len = txpwr_len;
            txpwr_buf = buf = (UINT8*)os_malloc(txpwr_len);
            if(!buf) {
                MCAL_FATAL("no memory for txpwr tab save to flash\r\n");
                return 0;
            }
        }        
    } else {
        // nothing in flash, write TLV with chipid
        MCAL_WARN("NO BK_FLASH_OPT_TLV_HEADER found, save_txpwr_tab_to_flash failed\r\n");
        return 0;
    }

    is_ready_flash = TXPWR_NONE_RD;
    addr = manual_cal_search_txpwr_tab(TXPWR_ENABLE_ID, addr_start); 
    if(addr) {
        ddev_read(flash_handle, (char *)&head, sizeof(TXPWR_ELEM_ST), addr);
        ddev_read(flash_handle, (char *)&is_ready_flash, head.len, addr + sizeof(TXPWR_ELEM_ST));
        MCAL_PRT("flash txpwr table:0x%x\r\n", is_ready_flash);
    }
    
    // for tag TXPW
    tag_txpwr_ptr = (TAG_TXPWR_PTR)txpwr_buf;
    tag_txpwr.head.type = TXPWR_TAB_TAB;
    tag_txpwr.head.len = txpwr_len - sizeof(TAG_TXPWR_ST);
    os_memcpy(tag_txpwr_ptr, &tag_txpwr, sizeof(TAG_TXPWR_ST));
    txpwr_buf = (UINT8*)(txpwr_buf + sizeof(TAG_TXPWR_ST));
    
    // for tag TXPWR_ENABLE_ID
    tag_enable_ptr = (TAG_ENABLE_PTR)(txpwr_buf);
    tag_enable.head.type = TXPWR_ENABLE_ID;
    tag_enable.head.len = sizeof(tag_enable.flag);
    tag_enable.flag = (UINT32)is_ready;
    os_memcpy(tag_enable_ptr, &tag_enable, sizeof(TAG_ENABLE_ST));
    txpwr_buf = (UINT8*)(txpwr_buf + sizeof(TAG_ENABLE_ST));    
    
    // for tag TXPWR_TAB_B_ID
    tag_tab_ptr = (TAG_TXPWR_TAB_PTR)(txpwr_buf);
    tag_tab.head.type = TXPWR_TAB_B_ID;
    tag_tab.head.len = sizeof(TXPWR_ST)*WLAN_2_4_G_CHANNEL_NUM; 
    if(is_ready & TXPWR_TAB_B_RD) {
        os_memcpy(&tag_tab.tab[0], gtxpwr_tab_b, tag_tab.head.len);
    } else if(is_ready_flash & TXPWR_TAB_B_RD) {
        addr = manual_cal_search_txpwr_tab(TXPWR_TAB_B_ID, addr_start);
        if(addr) {
            ddev_read(flash_handle, (char *)&head, sizeof(TXPWR_ELEM_ST), addr);
            ddev_read(flash_handle, (char *)&tag_tab.tab[0], head.len, addr + sizeof(TXPWR_ELEM_ST)); 
        } else {
            MCAL_PRT("txpwr tabe b in flash no found\r\n");
        }
    }
    os_memcpy(tag_tab_ptr, &tag_tab, sizeof(TAG_TXPWR_TAB_ST));
    txpwr_buf = (UINT8*)(txpwr_buf + sizeof(TAG_TXPWR_TAB_ST));      
    
    
    // for tag TXPWR_TAB_G_ID
    tag_tab_ptr = (TAG_TXPWR_TAB_PTR)(txpwr_buf);
    tag_tab.head.type = TXPWR_TAB_G_ID;
    tag_tab.head.len = sizeof(TXPWR_ST)*WLAN_2_4_G_CHANNEL_NUM;
    if(is_ready & TXPWR_TAB_G_RD) {
        os_memcpy(&tag_tab.tab[0], gtxpwr_tab_g, tag_tab.head.len);
    } else if(is_ready_flash & TXPWR_TAB_G_RD) {
        addr = manual_cal_search_txpwr_tab(TXPWR_TAB_G_ID, addr_start);
        if(addr) {
            ddev_read(flash_handle, (char *)&head, sizeof(TXPWR_ELEM_ST), addr);
            ddev_read(flash_handle, (char *)&tag_tab.tab[0], head.len, addr + sizeof(TXPWR_ELEM_ST)); 
        } else {
            MCAL_PRT("txpwr tabe g in flash no found\r\n");
        }
    }
    os_memcpy(tag_tab_ptr, &tag_tab, sizeof(TAG_TXPWR_TAB_ST));
    txpwr_buf = (UINT8*)(txpwr_buf + sizeof(TAG_TXPWR_TAB_ST)); 

    // for tag TXPWR_TAB_N_ID
    tag_tab_ptr = (TAG_TXPWR_TAB_PTR)(txpwr_buf);  
    tag_tab.head.type = TXPWR_TAB_N_ID;
    tag_tab.head.len = sizeof(TXPWR_ST)*WLAN_2_4_G_CHANNEL_NUM;
    if(is_ready & TXPWR_TAB_N_RD) {
        os_memcpy(&tag_tab.tab[0], gtxpwr_tab_n_40, tag_tab.head.len);
    } else if(is_ready_flash & TXPWR_TAB_N_RD) {
        addr = manual_cal_search_txpwr_tab(TXPWR_TAB_N_ID, addr_start);
        if(addr) {
            ddev_read(flash_handle, (char *)&head, sizeof(TXPWR_ELEM_ST), addr);
            ddev_read(flash_handle, (char *)&tag_tab.tab[0], head.len, addr + sizeof(TXPWR_ELEM_ST)); 
        } else {
            MCAL_PRT("txpwr tabe N in flash no found\r\n");
        }
    }
    os_memcpy(tag_tab_ptr, &tag_tab, sizeof(TAG_TXPWR_TAB_ST));
    txpwr_buf = (UINT8*)(txpwr_buf + sizeof(TAG_TXPWR_TAB_ST)); 

    // for tag TXPWR_TAB_DIF_GN20_ID
    tag_dif_ptr = (TAG_DIF_PTR)(txpwr_buf);  
    tag_dif.head.type = TXPWR_TAB_DIF_GN20_ID;
    tag_dif.head.len = sizeof(UINT32);
    tag_dif.differ = g_dif_g_n20;
    os_memcpy(tag_dif_ptr, &tag_dif, sizeof(TAG_DIF_ST)); 
    txpwr_buf = (UINT8*)(txpwr_buf + sizeof(TAG_DIF_ST)); 

    // for tag TXPWR_TAB_DIF_GN40_ID
    tag_dif_ptr = (TAG_DIF_PTR)(txpwr_buf);  
    tag_dif.head.type = TXPWR_TAB_DIF_GN40_ID;
    tag_dif.head.len = sizeof(UINT32);
    tag_dif.differ = g_dif_g_n40;
    os_memcpy(tag_dif_ptr, &tag_dif, sizeof(TAG_DIF_ST)); 
    txpwr_buf = (UINT8*)(txpwr_buf + sizeof(TAG_DIF_ST));    
    
    ddev_close(flash_handle);

    addr_start -= pt->partition_start_addr;//TXPWR_TAB_FLASH_ADDR;
    manual_cal_update_flash_area(addr_start, (char *)buf, len);
    os_free(buf);
    
    return 1;
}

int manual_cal_save_chipinfo_tab_to_flash(void)
{
    UINT32 len = 0, chipinfo_len = 0, flash_len = 0;
    UINT8 *buf = NULL, *info_buf = NULL;
    TAG_TXPWR_ST tag_txpwr;
    TAG_TXPWR_PTR tag_txpwr_ptr = NULL;
    TAG_COMM_ST tag_com;    
    TAG_COMM_PTR tag_com_ptr = NULL;
    TAG_CHIPMAC_ST tag_mac;    
    TAG_CHIPMAC_PTR tag_mac_ptr = NULL;
    UINT32 status, addr, addr_start;
    DD_HANDLE flash_handle;
    TXPWR_ELEM_ST head;
	#if CFG_SUPPORT_ALIOS
	hal_logic_partition_t *pt = hal_flash_get_info(HAL_PARTITION_RF_FIRMWARE);
	#else
	bk_logic_partition_t *pt = bk_flash_get_info(BK_PARTITION_RF_FIRMWARE);
	#endif

    chipinfo_len = sizeof(TAG_TXPWR_ST) + sizeof(TAG_COMM_ST)
        #if MCAL_SAVE_MAC_ADDR
        + sizeof(TAG_CHIPMAC_ST)
        #endif
        + sizeof(TAG_COMM_ST) + sizeof(TAG_COMM_ST) + sizeof(TAG_COMM_ST)
        + sizeof(TAG_COMM_ST);

    flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);
    status = manual_cal_search_opt_tab(&flash_len);
    // Has TLV Header?
    if(status) {
        addr_start = manual_cal_search_txpwr_tab(TXID, pt->partition_start_addr);//TXPWR_TAB_FLASH_ADDR); 
        if(!addr_start) {
            // not txid, but has any other id, so attch txid after the table
            len = flash_len + chipinfo_len;
            buf = (UINT8*)os_malloc(len); 
            if(!buf) {
                MCAL_FATAL("no memory for chipinfo save to flash\r\n");
                return 0;
            }
            // copy flash
            ddev_read(flash_handle, (char *)buf, flash_len, pt->partition_start_addr);//TXPWR_TAB_FLASH_ADDR);
            ddev_close(flash_handle);

            // updata new TLV header
            tag_txpwr_ptr = (TAG_TXPWR_PTR)buf;
            tag_txpwr.head.type= BK_FLASH_OPT_TLV_HEADER;
            tag_txpwr.head.len = len - sizeof(TAG_TXPWR_ST);
            os_memcpy(tag_txpwr_ptr, &tag_txpwr, sizeof(TAG_TXPWR_ST));

            info_buf = (UINT8 *)(buf + flash_len);
        } else {
        #if 0
            // not need to write, return;
            ddev_close(flash_handle);
            return 0;
        #else
            // updata txid
            len = flash_len;
            buf = (UINT8*)os_malloc(len); 
            if(!buf) {
                MCAL_FATAL("no memory for chipinfo save to flash\r\n");
                return 0;
            }
            // copy flash
            ddev_read(flash_handle, (char *)buf, flash_len, pt->partition_start_addr);//TXPWR_TAB_FLASH_ADDR);
            ddev_close(flash_handle);

            // point to txid
            info_buf = (UINT8 *)(buf + sizeof(TAG_TXPWR_ST));            
        #endif
        }        
    } else {
        // nothing in flash, write TLV with chipid
        len = chipinfo_len + sizeof(TAG_TXPWR_ST);

        buf = (UINT8*)os_malloc(len); 
        if(!buf) {
            MCAL_FATAL("no memory for chipinfo save to flash\r\n");
            return 0;
        }
        
        // updata new TLV header
        tag_txpwr_ptr = (TAG_TXPWR_PTR)buf;
        tag_txpwr.head.type= BK_FLASH_OPT_TLV_HEADER;
        tag_txpwr.head.len = len - sizeof(TAG_TXPWR_ST);
        os_memcpy(tag_txpwr_ptr, &tag_txpwr, sizeof(TAG_TXPWR_ST));
        info_buf = (UINT8 *)(buf + sizeof(TAG_TXPWR_ST));
    }

    //////////////////////////////////////////////
    // for tag TXID
    tag_txpwr_ptr = (TAG_TXPWR_PTR)(info_buf);
    tag_txpwr.head.type = TXID;
    tag_txpwr.head.len = chipinfo_len - sizeof(TAG_TXPWR_ST);
    os_memcpy(tag_txpwr_ptr, &tag_txpwr, sizeof(TAG_TXPWR_ST));
    
    // for tag TXID_ID
    info_buf = (UINT8 *)(info_buf + sizeof(TAG_TXPWR_ST));
    tag_com_ptr = (TAG_COMM_PTR)(info_buf);
    tag_com.head.type = TXID_ID;
    tag_com.head.len = sizeof(tag_com.value);
    tag_com.value = (UINT32)DEFAULT_TXID_ID;
    os_memcpy(tag_com_ptr, &tag_com, sizeof(TAG_COMM_ST));

#if MCAL_SAVE_MAC_ADDR
    // for tag TXID_MAC
    info_buf = (UINT8 *)(info_buf + sizeof(TAG_COMM_ST));
    tag_mac_ptr = (TAG_CHIPMAC_PTR)(info_buf);
    tag_mac.head.type = TXID_MAC;
    tag_mac.head.len = 6;
    os_memcpy(&tag_mac.arry[0], system_mac, 6);
    os_memcpy(tag_mac_ptr, &tag_mac, sizeof(TAG_CHIPMAC_ST));
    info_buf = (UINT8 *)(info_buf + sizeof(TAG_CHIPMAC_ST)); 
#else
    info_buf = (UINT8 *)(info_buf + sizeof(TAG_COMM_ST)); 
#endif  
    // for tag TXID_THERMAL
    //manual_cal_get_current_temperature();
    os_printf("save sys temper:%d\r\n", g_cur_temp); 
    tag_com_ptr = (TAG_COMM_PTR)(info_buf  );
    tag_com.head.type = TXID_THERMAL;
    tag_com.head.len = sizeof(tag_com.value);
    tag_com.value = (UINT32)g_cur_temp;
    os_memcpy(tag_com_ptr, &tag_com, sizeof(TAG_COMM_ST));

    // for tag TXID_CHANNEL
    info_buf = (UINT8 *)(info_buf + sizeof(TAG_COMM_ST));      
    tag_com_ptr = (TAG_COMM_PTR)(info_buf);
    tag_com.head.type = TXID_CHANNEL;
    tag_com.head.len = sizeof(tag_com.value);
    tag_com.value = (UINT32)DEFAULT_TXID_CHANNEL;
    os_memcpy(tag_com_ptr, &tag_com, sizeof(TAG_COMM_ST));

    // for tag TXID_XTAL
    info_buf = (UINT8 *)(info_buf + sizeof(TAG_COMM_ST));      
    tag_com_ptr = (TAG_COMM_PTR)(info_buf);
    tag_com.head.type = TXID_XTAL;
    tag_com.head.len = sizeof(tag_com.value);
    tag_com.value = (UINT32)DEFAULT_TXID_XTAL;
    os_memcpy(tag_com_ptr, &tag_com, sizeof(TAG_COMM_ST));

    // for tag TXID_ADC
    info_buf = (UINT8 *)(info_buf + sizeof(TAG_COMM_ST));      
    tag_com_ptr = (TAG_COMM_PTR)(info_buf);
    tag_com.head.type = TXID_ADC;
    tag_com.head.len = sizeof(tag_com.value);
    tag_com.value = *((UINT32 *)&saradc_val);
    os_memcpy(tag_com_ptr, &tag_com, sizeof(TAG_COMM_ST));
    manual_cal_update_flash_area(0, (char *)buf, len);
    os_free(buf);
    
    return 1;
}

int manual_cal_get_macaddr_from_flash(UINT8 *mac_ptr)
{
    #if MCAL_SAVE_MAC_ADDR
    UINT32 flash_len = 0;
    UINT32 status, addr, addr_start;
    DD_HANDLE flash_handle;
    TXPWR_ELEM_ST head;
	#if CFG_SUPPORT_ALIOS
	hal_logic_partition_t *pt = hal_flash_get_info(HAL_PARTITION_RF_FIRMWARE);
	#else
	bk_logic_partition_t *pt = bk_flash_get_info(BK_PARTITION_RF_FIRMWARE);
	#endif
    flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);
    status = manual_cal_search_opt_tab(&flash_len);
    // Has TLV Header?
    if(status) {
        addr_start = manual_cal_search_txpwr_tab(TXID, pt->partition_start_addr);//TXPWR_TAB_FLASH_ADDR); 
        if(!addr_start) {
            // not txid, but has any other id, so attch txid after the table
            MCAL_FATAL("No txid header found in flash\r\n");
            return 0;
        }      
    } 
    else {
        MCAL_FATAL("No TLV header found in flash\r\n");
        return 0;
    }

    addr = manual_cal_search_txpwr_tab(TXID_MAC, addr_start); 
    if(addr) {
        ddev_read(flash_handle, (char *)&head, sizeof(TXPWR_ELEM_ST), addr);
        ddev_read(flash_handle, (char *)mac_ptr, head.len, addr + sizeof(TXPWR_ELEM_ST));
        //MCAL_PRT("read MAC ADDR from flash\r\n");
    }else {
        MCAL_FATAL("No MAC id found in txid header \r\n");
        return 0;
    }
    
    return 1;
    #else
    return 0;
    #endif
}

int manual_cal_write_macaddr_to_flash(UINT8 *mac_ptr)
{
    #if MCAL_SAVE_MAC_ADDR
    UINT32 flash_len = 0;
    UINT32 status, addr, addr_start;
    DD_HANDLE flash_handle;
    TXPWR_ELEM_ST head;
	#if CFG_SUPPORT_ALIOS
	hal_logic_partition_t *pt = hal_flash_get_info(HAL_PARTITION_RF_FIRMWARE);
	#else
	bk_logic_partition_t *pt = bk_flash_get_info(BK_PARTITION_RF_FIRMWARE);
	#endif

    flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);
    status = manual_cal_search_opt_tab(&flash_len);
    // Has TLV Header?
    if(status) {
        addr_start = manual_cal_search_txpwr_tab(TXID, pt->partition_start_addr);//TXPWR_TAB_FLASH_ADDR); 
        if(!addr_start) {
            // not txid, but has any other id, so attch txid after the table
            MCAL_FATAL("No txid header found in flash\r\n");
            return 0;
        }      
    } 
    else {
        MCAL_FATAL("No TLV header found in flash\r\n");
        return 0;
    }

    addr = manual_cal_search_txpwr_tab(TXID_MAC, addr_start); 
    if(addr) {
        ddev_read(flash_handle, (char *)&head, sizeof(TXPWR_ELEM_ST), addr);
        addr += sizeof(TXPWR_ELEM_ST);
        addr -= pt->partition_start_addr;//TXPWR_TAB_FLASH_ADDR;
        manual_cal_update_flash_area(addr, (char *)mac_ptr, 6); //0: sucess, 1 failed
    }else {
        MCAL_FATAL("No MAC id found in txid header \r\n");
        return 0;
    }
    
    return 1;
    #else
    return 0;
    #endif
}

UINT8 manual_cal_wirte_otp_flash(UINT32 addr, UINT32 len, UINT8 *buf)
{
    UINT8 ret = 0; 
	#if CFG_SUPPORT_ALIOS
	hal_logic_partition_t *pt = hal_flash_get_info(HAL_PARTITION_RF_FIRMWARE);
	#else
	bk_logic_partition_t *pt = bk_flash_get_info(BK_PARTITION_RF_FIRMWARE);
	#endif
    //MCAL_PRT("wirte_otp_flash:addr:%08x, len:%d, buf:%p\r\n", addr, len, buf);
    addr -= pt->partition_start_addr;//TXPWR_TAB_FLASH_ADDR; 
    ret = manual_cal_update_flash_area(addr, (char *)buf, len); //0: sucess, 1 failed
    return ret;
}

UINT8 manual_cal_read_otp_flash(UINT32 addr, UINT32 len, UINT8 *buf)
{
    UINT8 ret = 0; 
    //MCAL_PRT("read_otp_flash:addr:%08x, len:%d, buf:%p\r\n",addr, len, buf);
    ret = manual_cal_read_flash(addr, (char *)buf, len);  //0:failed, others: len
    return ret;
}

void manual_cal_get_current_temperature(void)
{   
    UINT32 temp_value;
    
    if(!temp_single_get_current_temperature(&temp_value))
        g_cur_temp = temp_value;

    MCAL_WARN("system temperature:%0d\r\n", g_cur_temp);
}

void manual_cal_show_otp_flash(void)
{   
    UINT32 status, flash_len, addr_start;
    DD_HANDLE flash_handle;
    TXPWR_ELEM_ST head;
	#if CFG_SUPPORT_ALIOS
    hal_logic_partition_t *pt = hal_flash_get_info(HAL_PARTITION_RF_FIRMWARE);
	#else
    bk_logic_partition_t *pt = bk_flash_get_info(BK_PARTITION_RF_FIRMWARE);
	#endif
	
    flash_len = 0;
    flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);
    status = manual_cal_search_opt_tab(&flash_len);

    if(status) 
    {
        UINT8 *buf = (UINT8*)os_malloc(flash_len); 
        if(!buf) 
        {
            MCAL_FATAL("no memory for show_otp_flash\r\n");
            ddev_close(flash_handle);
            return;
        }

        os_memset(buf, 0, sizeof(flash_len));
        addr_start = pt->partition_start_addr;
        // copy flash
        ddev_read(flash_handle, (char *)buf, flash_len, addr_start);//TXPWR_TAB_FLASH_ADDR);
        for(int i=0; i<flash_len; i++) 
        {
            MCAL_PRT("%02x,", buf[i]);
            if((i+1)%16 == 0)
                MCAL_PRT("\r\n");
        }
        MCAL_PRT("\r\n");  

        os_free(buf);
    }
    else
    {
        MCAL_FATAL("No TLV header found in flash\r\n");
    }

    ddev_close(flash_handle);
}

void manual_cal_clear_otp_flash(void)
{   
    UINT32 flash_len = 1024;
    UINT8 *buf = (UINT8*)os_malloc(flash_len); 

    os_memset(buf, 0xff, sizeof(flash_len));
    manual_cal_update_flash_area(0, buf, flash_len);
    os_free(buf);
}

int manual_cal_load_temp_tag_from_flash(void)
{
    UINT32 status, addr, addr_start;
    DD_HANDLE flash_handle;
    TXPWR_ELEM_ST head;
	#if CFG_SUPPORT_ALIOS
	hal_logic_partition_t *pt = hal_flash_get_info(HAL_PARTITION_RF_FIRMWARE);
	#else
	bk_logic_partition_t *pt = bk_flash_get_info(BK_PARTITION_RF_FIRMWARE);
	#endif
    UINT32 tem_in_flash = DEFAULT_TXID_THERMAL;

    addr_start = manual_cal_search_txpwr_tab(TXID, pt->partition_start_addr);//TXPWR_TAB_FLASH_ADDR); 
    if(!addr_start) {
        MCAL_FATAL("NO TXID found in flash, use def temp:%d\r\n", DEFAULT_TXID_THERMAL); 
        goto init_temp;
    }
    
    addr = manual_cal_search_txpwr_tab(TXID_THERMAL, addr_start);  
    if(!addr)
    {
        MCAL_FATAL("NO TXID_THERMAL found in flash, use def temp:%d\r\n", DEFAULT_TXID_THERMAL);
        goto init_temp;
    }

    flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);
    ddev_read(flash_handle, (char *)&head, sizeof(TXPWR_ELEM_ST), addr);
    ddev_read(flash_handle, (char *)&status, head.len, addr + sizeof(TXPWR_ELEM_ST));
    tem_in_flash = status;

init_temp:
    MCAL_FATAL("temp in flash is:%d\r\n", tem_in_flash);
    return tem_in_flash;
}

void manual_cal_load_differ_tag_from_flash(void)
{
    UINT32 status, addr, addr_start;
    DD_HANDLE flash_handle;
    TXPWR_ELEM_ST head;
	#if CFG_SUPPORT_ALIOS
	hal_logic_partition_t *pt = hal_flash_get_info(HAL_PARTITION_RF_FIRMWARE);
	#else
	bk_logic_partition_t *pt = bk_flash_get_info(BK_PARTITION_RF_FIRMWARE);
	#endif
    UINT32 dif_n20 = MOD_DIST_G_BW_N20;
    UINT32 dif_n40 = MOD_DIST_G_BW_N40;

    addr_start = manual_cal_search_txpwr_tab(TXPWR_TAB_TAB, pt->partition_start_addr);//TXPWR_TAB_FLASH_ADDR); 
    if(!addr_start) {
        MCAL_FATAL("NO TXPWR_TAB_TAB found in flash, use def:%d, %d\r\n", 
            MOD_DIST_G_BW_N20, MOD_DIST_G_BW_N40); 
        goto load_diff;
    }
    
    addr = manual_cal_search_txpwr_tab(TXPWR_TAB_DIF_GN20_ID, addr_start);  
    if(!addr) {
        MCAL_FATAL("NO TXPWR_TAB_DIF_GN20_ID found in flash, use def:%d\r\n", MOD_DIST_G_BW_N20);
    } else {
        flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);
        ddev_read(flash_handle, (char *)&head, sizeof(TXPWR_ELEM_ST), addr);
        ddev_read(flash_handle, (char *)&status, head.len, addr + sizeof(TXPWR_ELEM_ST));
        dif_n20 = status;
    }

    addr = manual_cal_search_txpwr_tab(TXPWR_TAB_DIF_GN40_ID, addr_start);  
    if(!addr) {
        MCAL_FATAL("NO TXPWR_TAB_DIF_GN40_ID found in flash, use def:%d\r\n", TXPWR_TAB_DIF_GN40_ID);
    } else {
        flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);
        ddev_read(flash_handle, (char *)&head, sizeof(TXPWR_ELEM_ST), addr);
        ddev_read(flash_handle, (char *)&status, head.len, addr + sizeof(TXPWR_ELEM_ST));
        dif_n40 = status;
    }

load_diff:
    MCAL_FATAL("diff in flash:n20-%d, n40-%d\r\n", dif_n20, dif_n40);
    return;
}

#endif  // CFG_SUPPORT_MANUAL_CALI


#if CFG_USE_TEMPERATURE_DETECT
#define TMP_PWR_TAB_LEN             13

typedef struct tmp_set_pwr_st {
    UINT16 indx;
    UINT16 temp_tab[TMP_PWR_TAB_LEN];
    TMP_PWR_PTR pwr_ptr;
    UINT16 init_mod;
    UINT16 init_pa;
} TMP_SET_PWR_ST, *TMP_SET_PWR_PTR;

const TMP_PWR_ST tmp_pwr_tab[TMP_PWR_TAB_LEN] = {
    { 0,  6},   // pwr index 0   5dbm
    { 0,  8},   // pwr index 1   6dbm
    { 1,  8},   // pwr index 2   7dbm
    { 2,  8},   // pwr index 3   8dbm
    { 3,  8},   // pwr index 4   9dbm
    { 4,  8},   // pwr index 5   10dbm
    { 6,  8},   // pwr index 6   11bm
    { 8,  8},   // pwr index 7   12dbm
    {11,  8},   // pwr index 8   13dbm
    {15,  8},   // pwr index 9   14dbm
    {15, 10},   // pwr index 10  15dbm
    {15, 12},   // pwr index 11  16dbm
    {15, 15},   // pwr index 12  17dbm
};

TMP_SET_PWR_ST g_tmp_pwr;

extern void sctrl_cali_dpll(UINT8 flag);
extern void sctrl_dpll_int_open(void);

void manual_cal_tmp_pwr_init_reg(UINT16 reg_mod, UINT16 reg_pa)
{
    TMP_DETECT_WARN("manual_cal_tmp_pwr_init_reg: mod:%d, pa:%d\r\n", reg_mod, reg_pa);
    g_tmp_pwr.init_mod = reg_mod;
    g_tmp_pwr.init_pa = reg_pa;       
}

void manual_cal_temp_pwr_unint(void)
{
    os_printf("manual_cal_temp_pwr_unint: mod:%d, pa:%d\r\n", g_tmp_pwr.init_mod,
        g_tmp_pwr.init_pa);
    rwnx_cal_set_reg_mod_pa(g_tmp_pwr.init_mod, g_tmp_pwr.init_pa);    
}

void manual_cal_tmp_pwr_init(UINT16 init_temp, UINT16 init_thre, UINT16 init_dist)
{
    INT32 idx = TMP_PWR_TAB_LEN-1, i;
    TMP_PWR_PTR pwr_ptr_0, pwr_ptr_1;
    UINT8 reg_mod, reg_pa;
    INT16 temp = 0;

    reg_mod = g_tmp_pwr.init_mod;
    reg_pa = g_tmp_pwr.init_pa;
    
    for(i=0; i<TMP_PWR_TAB_LEN-1; i++)
    {
        pwr_ptr_0 = (TMP_PWR_PTR)&tmp_pwr_tab[i];
        pwr_ptr_1 = (TMP_PWR_PTR)&tmp_pwr_tab[i+1];
        if(((pwr_ptr_0->mod <= reg_mod) && (pwr_ptr_1->mod >= reg_mod))
          && ((pwr_ptr_0->pa <= reg_pa) && (pwr_ptr_1->pa >= reg_pa))) 
        {
            if((pwr_ptr_1->pa == reg_pa) && (pwr_ptr_1->mod >= reg_mod))
                idx = i+1;
            else
                idx = i;
            
            break;
        }
    }

    os_printf("init temp pwr table: mod:%d, pa:%d, tmp:%d, idx:%d, dist:%d\r\n",
        reg_mod, reg_pa, init_temp, idx, init_dist);

    if(init_temp < init_dist) {
        init_temp = ADC_TEMP_VAL_MIN;
    } else {
        init_temp -= init_dist;
    }

    os_memset(&g_tmp_pwr.temp_tab[0], 0, sizeof(UINT16)*TMP_PWR_TAB_LEN);
    g_tmp_pwr.indx = idx;
    g_tmp_pwr.pwr_ptr = (TMP_PWR_PTR)&tmp_pwr_tab[idx];
    g_tmp_pwr.temp_tab[idx] = init_temp;

    for(i=idx+1; i<TMP_PWR_TAB_LEN; i++) {
        //temp = g_tmp_pwr.temp_tab[i-1] + ADC_TMEP_LSB_PER_10DEGREE;
        temp = g_tmp_pwr.temp_tab[i-1] + init_thre;
        temp = (temp > ADC_TEMP_VAL_MAX)? ADC_TEMP_VAL_MAX : temp;
        g_tmp_pwr.temp_tab[i] = temp; 
    }

    for(i=idx-1; i>=0; i--) {
        //temp = g_tmp_pwr.temp_tab[i+1] - ADC_TMEP_LSB_PER_10DEGREE;
        temp = g_tmp_pwr.temp_tab[i+1] - init_thre;
        temp = (temp < ADC_TEMP_VAL_MIN)? ADC_TEMP_VAL_MIN : temp;
        g_tmp_pwr.temp_tab[i] = temp;
    }

    TMP_DETECT_WARN("\r\n");
    for(i=0; i<TMP_PWR_TAB_LEN; i++) {
        TMP_DETECT_WARN("%02d: %03d\r\n", i, g_tmp_pwr.temp_tab[i]);
    }
    TMP_DETECT_WARN("\r\n");

    //
    temp_detect_init(init_temp);
}

TMP_PWR_PTR manual_cal_set_tmp_pwr(UINT16 cur_val, UINT16 thre, UINT16 *last)
{    
    UINT32 dist = 0, thre_1 = (thre>>1) + 2, thre_2 = thre_1 + thre ;
    UINT8 indx = g_tmp_pwr.indx, need_cal_dpll = 0;
    UINT16 last_val = g_tmp_pwr.temp_tab[g_tmp_pwr.indx];
   
    if(cur_val > last_val) 
    {
        dist = cur_val - last_val;
        if(dist >= thre_2) 
        {
            indx = (indx+2<TMP_PWR_TAB_LEN)? indx+2 : TMP_PWR_TAB_LEN-1; 
            need_cal_dpll = 1;
        }
        else if(dist >= thre_1)
            indx = (indx+1<TMP_PWR_TAB_LEN)? indx+1 : TMP_PWR_TAB_LEN-1;   
    }
    else if(cur_val < last_val) 
    {
        dist = last_val - cur_val;
        if(dist >= thre_2) 
        {
            indx = (indx>2)? indx-2:0; 
            need_cal_dpll = 1;
        }
        else if(dist >= thre_1) 
            indx = (indx>1)? indx-1:0; 
    }
    
    if(g_tmp_pwr.indx == indx)
        return NULL;
    
    g_tmp_pwr.indx = indx;
    g_tmp_pwr.pwr_ptr = (TMP_PWR_PTR)&tmp_pwr_tab[indx];
    *last = g_tmp_pwr.temp_tab[g_tmp_pwr.indx];

    if(need_cal_dpll) 
    {
        os_printf("cal dpll!\r\n");
        CHECK_OPERATE_RF_REG_IF_IN_SLEEP();      
        sctrl_cali_dpll(0);
        sctrl_dpll_int_open();
        CHECK_OPERATE_RF_REG_IF_IN_SLEEP_END();
    }

    bk7011_cal_bias();

    os_printf("set_tmp_pwr: indx:%d, mod:%d, pa:%d, tmp:%d\r\n", indx,
        g_tmp_pwr.pwr_ptr->mod, g_tmp_pwr.pwr_ptr->pa, *last); 

    return g_tmp_pwr.pwr_ptr;
}

UINT32 manual_cal_load_temp_tag_flash(void)
{
    g_cur_temp = g_cur_temp_flash= manual_cal_load_temp_tag_from_flash();
    // start temp dectect
    manual_cal_tmp_pwr_init(g_cur_temp, ADC_TMEP_LSB_PER_10DEGREE * ADC_TMEP_10DEGREE_PER_DBPWR, 
            ADC_TMEP_DIST_INTIAL_VAL);
    
    return g_cur_temp;
}

#endif // CFG_USE_TEMPERATURE_DETECT

#if CFG_SARADC_CALIBRATE
UINT32 manual_cal_load_adc_cali_flash(void)
{
    UINT32 status, addr, addr_start;
    DD_HANDLE flash_handle;
    TXPWR_ELEM_ST head;
	#if CFG_SUPPORT_ALIOS
	hal_logic_partition_t *pt = hal_flash_get_info(HAL_PARTITION_RF_FIRMWARE);
	#else
	bk_logic_partition_t *pt = bk_flash_get_info(BK_PARTITION_RF_FIRMWARE);
	#endif

    addr_start = manual_cal_search_txpwr_tab(TXID, pt->partition_start_addr);//TXPWR_TAB_FLASH_ADDR); 
    if(!addr_start) {
        goto FAILURE;
    }
    
    addr = manual_cal_search_txpwr_tab(TXID_ADC, addr_start);  
    if(!addr)
    {
        goto FAILURE;
    }

    flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);
    ddev_read(flash_handle, (char *)&head, sizeof(TXPWR_ELEM_ST), addr);
    ddev_read(flash_handle, (char *)&saradc_val, head.len, addr + sizeof(TXPWR_ELEM_ST));

    os_printf("calibrate low value:[%x]\r\n", saradc_val.low);
    os_printf("calibrate high value:[%x]\r\n", saradc_val.high);

    return SARADC_SUCCESS;

FAILURE:
    return SARADC_FAILURE;
}
#endif

#endif // (CFG_SOC_NAME != SOC_BK7231) 

