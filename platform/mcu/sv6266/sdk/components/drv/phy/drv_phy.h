#ifndef _DRV_PHY_H_
#define _DRV_PHY_H_

// abbreviation:
//		idx_tbl  -> phy info idx table
//		llen_tbl -> L-Length table
//
#define		PHY_IDX_TBL_CNT		39		// phy info idx table count
#define		PHY_IDX_TBL_LEN		156		// phy info idx table length = 39 * sizeof(u32), in bytes
#define		PHY_LLEN_TBL_CNT	8		// phy l-length table count
#define		PHY_LLEN_TBL_LEN	32		// phy l-length table length = 8  * sizeof(u32), in bytes
#define		PHY_PBUF_LEN		(PHY_IDX_TBL_LEN + PHY_LLEN_TBL_LEN)

#define _RF_MODE_STANDBY_   1
#define _RF_MODE_TRX_       2

void drv_phy_backup(void);
void drv_phy_installtbl(void);
void drv_phy_cali(void);
s32 drv_phy_channel_change(u32 channel_id, u32 ch_type);
void drv_phy_init(void);
void drv_phy_off(void);
void drv_phy_on(void);
void drv_phy_b_only(void);
void drv_phy_bgn(void);
s32 drv_phy_clean_dpd_cali(void);



// for power-on cali. begin
// #define __LOG_DRV_PHY_CALI_RCAL__
// #define __LOG_DRV_PHY_CALI_TXLO__
// #define __LOG_DRV_PHY_CALI_TXIQ__
// #define __LOG_DRV_PHY_CALI_RXIQ__
// #define __LOG_DRV_PHY_CALI_PAPD__
// #define __LOG_DRV_PHY_CALI_RXDC__
// #define __LOG_DRV_PHY_CALI_TSSI__
// #define __LOG_DRV_PHY_CALI_RXRC__
// #define __LOG_DRV_PHY_TEMPERATURE__
// #define __LOG_DRV_PHY_CHANNEL_CHANGE__
// #define __LOG_DRV_PHY_MCU_TIME__
// #define __LOG_DRV_PHY_INIT_CALI__
// #define __LOG_DRV_PHY_INIT_CALI_TSTAMP__

// #define _STATISTIC_DRV_PHY_CALI_RXDC_
// #define _STATISTIC_DRV_PHY_CALI_RXRC_
// #define _STATISTIC_DRV_PHY_CALI_TXLO_
// #define _STATISTIC_DRV_PHY_CALI_TXIQ_
// #define _STATISTIC_DRV_PHY_CALI_RXIQ_
// #define _STATISTIC_DRV_PHY_CALI_TSSI_
// #define _STATISTIC_DRV_PHY_CALI_PAPD_

#define GET_RG_IQCAL_DIV_DIVIDEND   GET_RG_IQCAL_MULT_OP0
#define GET_RG_IQCAL_DIV_DIVIDER    GET_RG_IQCAL_MULT_OP1

#define SET_RG_IQCAL_DIV_DIVIDEND   SET_RG_IQCAL_MULT_OP0
#define SET_RG_IQCAL_DIV_DIVIDER    SET_RG_IQCAL_MULT_OP1

#define RCAL_RDY_ADDR   ADR_READ_ONLY_FLAGS_1
#define RCAL_RDY_BIT    0

#define RO_IQCAL_SPRM_RDY_ADDR  ADR_RX_OBSERVATION_CIRCUIT_2
#define RO_IQCAL_SPRM_RDY_BIT   20

#define RO_IQCAL_IQCOL_RDY_ADDR ADR_RX_OBSERVATION_CIRCUIT_2
#define RO_IQCAL_IQCOL_RDY_BIT  21

#define RO_IQCAL_ALPHA_ESTM_RDY_ADDR    ADR_RX_OBSERVATION_CIRCUIT_2
#define RO_IQCAL_ALPHA_ESTM_RDY_BIT     22

#define RO_IQCAL_DC_RDY_ADDR    ADR_RX_OBSERVATION_CIRCUIT_2
#define RO_IQCAL_DC_RDY_BIT     23

#define RO_IQCAL_MULT_RDY_ADDR  ADR_RX_OBSERVATION_CIRCUIT_2
#define RO_IQCAL_MULT_RDY_BIT   24

#define RO_FFT_ENRG_RDY_ADDR    ADR_RX_OBSERVATION_CIRCUIT_2
#define RO_FFT_ENRG_RDY_BIT     25

#define RO_PHEST_RDY_ADDR       ADR_RX_OBSERVATION_CIRCUIT_2
#define RO_PHEST_RDY_BIT        26

#define RO_GP_DIV_RDY_ADDR      ADR_RX_OBSERVATION_CIRCUIT_2
#define RO_GP_DIV_RDY_BIT       27

#define RO_GAIN_EST_RDY_ADDR    ADR_RX_OBSERVATION_CIRCUIT_2
#define RO_GAIN_EST_RDY_BIT     28

#define SAR_ADC_FSM_RDY_ADDR    ADR_READ_ONLY_FLAGS_1
#define SAR_ADC_FSM_RDY_BIT     22

#define LCK_BIN_RDY_ADDR        ADR_READ_ONLY_FLAGS_1
#define LCK_BIN_RDY_BIT         1

#define _CALI_SEQ_RCAL_         (0x0001 << 0)
#define _CALI_SEQ_TEMPERATURE_  (0x0001 << 2)
#define _CALI_SEQ_RXDC_         (0x0001 << 3)
#define _CALI_SEQ_RXRC_         (0x0001 << 4)
#define _CALI_SEQ_TXDC_         (0x0001 << 5)
#define _CALI_SEQ_TXIQ_         (0x0001 << 6)
#define _CALI_SEQ_RXIQ_         (0x0001 << 7)
#define _CALI_SEQ_TSSI_         (0x0001 << 8)
#define _CALI_SEQ_PAPD_         (0x0001 << 9)

#define _CALI_SEQ_PAPD_LEARNING_    (0x0001 << 10)

#define _CALI_SEQ_N_            11
// for power-on cali. end

// for RF-bist in ft begin
#ifdef _BIST_RF_
#define _MEASURE_SEQ_TXGAIN_    (0x0001 << 0)
#define _MEASURE_SEQ_RFG_HG2MG_     (0x0001 << 1)
#define _MEASURE_SEQ_RFG_MG2LG_     (0x0001 << 2)
#define _MEASURE_SEQ_RFG_LG2ULG_    (0x0001 << 3)
#define _MEASURE_SEQ_PGAG_      (0x0001 << 4)
#define _MEASURE_SEQ_RSSI_      (0x0001 << 5)
#define _MEASURE_SEQ_TXPSAT_    (0x0001 << 6)

#define _MEASURE_SEQ_N_         7

// #define __LOG_DRV_PHY_BIST_RF__
#endif
// for RF-bist in ft end


void drv_phy_reg_init (void);
void drv_phy_cali_rtbl_load     (u32 rtbl_sel);
void drv_phy_cali_rtbl_load_def (u32 rtbl_sel);
void drv_phy_cali_rtbl_reset    (u32 rtbl_sel);
void drv_phy_cali_rtbl_set      (u32 rtbl_sel);
void drv_phy_cali_rtbl_export   (u32 rtbl_sel);
void drv_phy_evm_tk             (u32 enable  );
void drv_phy_tone_tk            (u32 tx_freq );
void drv_phy_rxcnt_tk           (u32 mode    );

s32 drv_phy_init_cali(u32 fx_sel);

typedef struct rtbl_cali_entry_st {    // cali. result table entry
    u32 addr_offset:20;
    u32 sht:5;      // lsb
    u32 hi:5;       // msb
    u32 rsv:2;      // reserved
    u16 def;
    u16 result;
} rtbl_cali_entry;

typedef struct rtbl_cali_st {
    u32 ever_cali:1;
    u32 rtbl_size:7;
    u32 rsv:24;
    //
    rtbl_cali_entry rtbl[];   // address base 0xce000000
} rtbl_cali;

typedef struct rtbl_measure_st {
    u32 valid:1;
    u32 rtbl_size:7;
    u32 rsv:24;
    //
    u32 rtbl[];
} rtbl_measure;

struct temperature_st {
    u16     valid;
    u16     temperature;
};

typedef struct phy_cali_config_st {
    u32 cfg_scale_tx:8;
    u32 cfg_scale_rx:8;
    u32 cfg_freq_tx :8;
    u32 cfg_freq_rx :8;
    u32 cfg_rtbl_sel:16;
    u32 cfg_ch      :4;
    u32 cfg_intg_prd:3;
    u32 cfg_selq    :1;
    u32 rsv         :8;
    u32 cfg_scale_fft:10+22;    // reserve 22-bit.
    //
    const ssv_cabrio_reg *rfreg_tbl;
    const ssv_cabrio_reg *phyreg_tbl;

    u32 rfreg_tbl_size:  8;
    u32 phyreg_tbl_size:24;

} phy_cali_config;

//phy_cali_config phy_cali_config_papd;

typedef s32 (*FX_CALI) (const phy_cali_config *);
typedef s32 (*FX_BIST) (const phy_cali_config *);

typedef void (*RG_RXDC_SET) (u32 rg_rxdc);

typedef struct cali_info_st {
    FX_CALI     fx_cali;
    rtbl_cali   *rtbl;
    const u16   seq;
    const s8    *name;
    //
    const phy_cali_config *phy_cfg;
} cali_info;

typedef struct bist_info_st {
    FX_BIST         fx_bist;
    rtbl_measure    *rtbl;
    const u16       seq;
    const s8        *name;
    //
    const phy_cali_config *phy_cfg;
} bist_info;

typedef struct rf_config_st {
    u32 cfg_xtal:8; // 0: 26M-xtal,     1: 40M-xtal
    u32 cfg_pa  :8; // 0: low-current,  1: high-power
    u32 cfg_pabias_ctrl:8;
    u32 cfg_pacascode_ctrl:8;
    u32 cfg_tssi_trgt:8;
    u32 cfg_tssi_div :3;
    u32 rsv     :5;
    u32 cfg_def_tx_scale_11b:8;
    u32 cfg_def_tx_scale_11b_p0d5:8;
    u32 cfg_def_tx_scale_11g:8;
    u32 cfg_def_tx_scale_11g_p0d5:8;
} rf_config;

//rf_config g_rf_config;

#define _RF_CONFIG_CH_N_    14

typedef struct rfreg_bych_st {
    u32 rg_sx_rfctrl_f;         // 24-bit
    u32 rg_sx_rfctrl_ch:11;     // 11-bit
    u32 rg_sx_target_cnt:13;    // 13-bit
    u32 rg_sx_refbytwo:8;       //  1-bit
} rfreg_bych;

typedef struct rfreg_byxtal_st {
    u32         rg_dp_fodiv;
    u32         rg_dp_refdiv;
    u32         rg_dpl_rfctrl_ch;
    u32         rg_dpl_rfctrl_f;
    u32         rg_en_dpl_mod;
    rfreg_bych  tbl_bych[_RF_CONFIG_CH_N_];
} rfreg_byxtal;

s32 drv_phy_cali_rcal  (const phy_cali_config *phy_cfg);
s32 drv_phy_temperature(const phy_cali_config *phy_cfg);
s32 drv_phy_cali_rxdc  (const phy_cali_config *phy_cfg);
s32 drv_phy_cali_rxrc  (const phy_cali_config *phy_cfg);
s32 drv_phy_cali_txlo  (const phy_cali_config *phy_cfg);
s32 drv_phy_cali_txiq  (const phy_cali_config *phy_cfg);
// s32 drv_phy_cali_rxiq  (const phy_cali_config *phy_cfg);
s32 drv_phy_cali_tssi  (const phy_cali_config *phy_cfg);
s32 drv_phy_cali_papd  (const phy_cali_config *phy_cfg);

s32 drv_phy_cali_papd_learning  (const phy_cali_config *phy_cfg);
s32 drv_phy_cali_papd_am_comp   ();
s32 drv_phy_cali_papd_pm_comp   ();

s32 drv_phy_cali_rxiq_fft  (const phy_cali_config *phy_cfg);

#ifdef _BIST_RF_
s32 drv_phy_bist_rf        (u32   fx_sel);
s32 drv_phy_bist_rf_txgain (const phy_cali_config *phy_cfg);
s32 drv_phy_bist_rf_pgag   (const phy_cali_config *phy_cfg);
s32 drv_phy_bist_rf_rssi   (const phy_cali_config *phy_cfg);
s32 drv_phy_bist_rf_txpsat (const phy_cali_config *phy_cfg);

s32 drv_phy_bist_rf_rfg_hg2mg  (const phy_cali_config *phy_cfg);
s32 drv_phy_bist_rf_rfg_mg2lg  (const phy_cali_config *phy_cfg);
s32 drv_phy_bist_rf_rfg_lg2ulg (const phy_cali_config *phy_cfg);
#endif
// Add by Bernie end

#endif /* _DRV_PHY_H_ */

