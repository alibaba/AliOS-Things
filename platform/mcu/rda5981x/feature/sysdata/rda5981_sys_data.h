#ifndef _RDA5981_SYS_DATA_H_
#define _RDA5981_SYS_DATA_H_

#include "rda_def.h"

/** This struct contains all smartconfig mbed flash parameter. */
typedef struct
{
    r_u8 ssid[36];
    r_u8 key[64];
} wland_sta_data_t;

/** This struct contains ap data, include ssid key and channel. */
typedef struct
{
    r_u8 channel;
    r_u8 ssid[35];
    r_u8 key[64];
} wland_ap_data_t;

/** This struct contains ap net data. */
typedef struct
{
    r_u32 ip;
    r_u32 msk;
    r_u32 gw;
    r_u32 dhcps;
    r_u32 dhcpe;
} wland_ap_net_data_t;

typedef struct
{
    r_u32 fixip;
    r_u32 ip;
    r_u32 msk;
    r_u32 gw;
} wland_dhcp_t;

/** This struct contains tx power parameter. */
typedef struct
{
    r_u8 b[14];
    r_u8 g[14];
    r_u8 n[14];
    r_u8 sum;
    r_u8 padding1;//alignment
} wland_tx_power_t;

/*
 *function: store reg and corresponding value related to test mode
 *@valid: 1 means data is valid
 *@flag: if biti(i=0~7) == 1, it means reg_val[i] is in use
 *@reg_val: store reg and value, reg_val[i][0] is reg, reg_val[i][1] ~ reg_val[i][14] is value
 */
typedef struct {
    r_u32 valid;
    r_u32 flag;
    r_u16 reg_val[8][2];
} wland_rf_t;

typedef struct {
    r_u32 valid;
    r_u32 flag;
    r_u16 reg_val[8][15];
} wland_rf_channels_t;

typedef struct {
    r_u32 valid;
    r_u32 flag;
    r_u32 reg_val[8][2];
} wland_phy_t;

typedef struct {
    r_u32 valid;
    r_u32 flag;
    r_u32 reg_val[8][15];
} wland_phy_channels_t;

/* if you add or delete any macros below, modify RDA5981_SYS_DATA_FLAG_UNINITIALIZED at the same time */
#define RDA5981_SYS_DATA_FLAG_MAC BIT0
#define RDA5981_SYS_DATA_FLAG_STA BIT1
#define RDA5981_SYS_DATA_FLAG_PMK BIT2
#define RDA5981_SYS_DATA_FLAG_IP BIT3
#define RDA5981_SYS_DATA_FLAG_PARTER_DATA_LEN BIT4
#define RDA5981_SYS_DATA_FLAG_TX_POWER BIT5
#define RDA5981_SYS_DATA_FLAG_XTAL_CAL BIT6
#define RDA5981_SYS_DATA_FLAG_TX_POWER_RF BIT7
#define RDA5981_SYS_DATA_FLAG_TX_POWER_PHY_GN BIT8
#define RDA5981_SYS_DATA_FLAG_TX_POWER_PHY_B BIT9
#define RDA5981_SYS_DATA_FLAG_AP BIT10
#define RDA5981_SYS_DATA_FLAG_APNET BIT11
#define RDA5981_SYS_DATA_FLAG_DHCP BIT12
#define RDA5981_SYS_DATA_FLAG_UART BIT13
#define RDA5981_SYS_DATA_FLAG_RF BIT14
#define RDA5981_SYS_DATA_FLAG_RF_CHANNELS BIT15
#define RDA5981_SYS_DATA_FLAG_PHY BIT16
#define RDA5981_SYS_DATA_FLAG_PHY_CHANNELS BIT17
#define RDA5981_SYS_DATA_FLAG_TX_POWER_OFFSET BIT18

#define RDA5981_VBAT_CAL BIT0
#define RDA5981_GPADC0_CAL BIT1
#define RDA5981_GPADC1_CAL BIT2
#define RDA5981_PRODUCT_ID BIT3
#define RDA5981_POWER_CLASS BIT4

#define RDA5981_SYS_DATA_FLAG_UNINITIALIZED (0xFFFFFFFF & \
    (~(RDA5981_SYS_DATA_FLAG_MAC | \
    RDA5981_SYS_DATA_FLAG_STA | \
    RDA5981_SYS_DATA_FLAG_PMK | \
    RDA5981_SYS_DATA_FLAG_IP | \
    RDA5981_SYS_DATA_FLAG_PARTER_DATA_LEN | \
    RDA5981_SYS_DATA_FLAG_TX_POWER | \
    RDA5981_SYS_DATA_FLAG_XTAL_CAL | \
    RDA5981_SYS_DATA_FLAG_TX_POWER_RF | \
    RDA5981_SYS_DATA_FLAG_TX_POWER_PHY_GN | \
    RDA5981_SYS_DATA_FLAG_TX_POWER_PHY_B | \
    RDA5981_SYS_DATA_FLAG_AP | \
    RDA5981_SYS_DATA_FLAG_APNET | \
    RDA5981_SYS_DATA_FLAG_DHCP | \
    RDA5981_SYS_DATA_FLAG_UART | \
    RDA5981_SYS_DATA_FLAG_RF | \
    RDA5981_SYS_DATA_FLAG_RF_CHANNELS | \
    RDA5981_SYS_DATA_FLAG_PHY | \
    RDA5981_SYS_DATA_FLAG_PHY_CHANNELS | \
    RDA5981_SYS_DATA_FLAG_TX_POWER_OFFSET)))

#define RDA5981_SYS_DATA_IP_LENGTH 8

#define RDA5981_FLAG_FLAG "RDA5981"

typedef struct
{
    r_u32 flag;
    r_u8 rda5981_flag[8];
    r_u8 mac_addr[6];
    r_u8 tp_offset;
    r_u8 padding1;//alignment
    wland_sta_data_t sta_data;
    r_u8 pmk[32];
    r_u8 ip[RDA5981_SYS_DATA_IP_LENGTH];
    r_u32 parter_data_len;
    wland_tx_power_t tx_power;
    r_u16 xtal_cal;
    r_u8 padding2[2];//alignment
    r_u16 tx_power_rf[14];
    r_u8 tx_power_phy_gn;
    r_u8 tx_power_phy_b;
    r_u8 padding3[2];
    wland_ap_data_t ap_data;
    wland_ap_net_data_t ap_net_data;
    wland_dhcp_t dhcp;
    r_u32 uart;
    wland_rf_t rf;
    wland_rf_channels_t rf_channels;
    wland_phy_t phy;
    wland_phy_channels_t phy_channels;
}rda5981_sys_data_t;

extern r_s32 rda5981_write_sys_data(r_u8 *data, r_u16 len, r_u32 flag);
extern r_s32 rda5981_erase_sys_data(r_u32 flag);
extern r_s32 rda5981_read_sys_data(r_u8 *data, r_u16 len, r_u32 flag);
extern r_s32 rda5981_init_sys_data();
extern r_s32 rda5981_write_sys_data_regs(r_u8 *reg, r_u8 *value, r_u32 flag);
extern r_s32 rda5981_erase_sys_data_regs(r_u8 *reg, r_u32 flag);
extern r_s32 rda5981_read_sys_data_regs(r_u8 *reg, r_u8 *value, r_u32 flag);

#endif /*_RDA5981_SYS_DATA_H_*/


