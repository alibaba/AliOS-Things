#ifndef _MAC_PHY_BYPASS_PUB_H_
#define _MAC_PHY_BYPASS_PUB_H_

#define MPB_DEV_NAME                 "mac_phy_bypass"


#define MPB_CMD_MAGIC               (0xcbe0000)
enum
{
    MCMD_RX_MODE_BYPASS_MAC                 = MPB_CMD_MAGIC + 0,
    MCMD_TX_MODE_BYPASS_MAC                 = MPB_CMD_MAGIC + 1, 
    MCMD_TX_LEGACY_SET_LEN,
    MCMD_TX_HT_VHT_SET_LEN,
    MCMD_STOP_BYPASS_MAC,
    MCMD_START_BYPASS_MAC,
    MCMD_SET_BANDWIDTH, 
    MCMD_SET_GI,
    MCMD_BYPASS_TX_SET_RATE_MFORMAT,
};

/*MCMD_TX_BYPASS_MAC_RATE*/
#define PPDU_RATE_POSI                           (4)
#define PPDU_RATE_MASK                           (0xF)

#define PPDU_BANDWIDTH_POSI                      (6)
#define PPDU_BANDWIDTH_MASK                      (0x1)

#define PPDU_LEG_RATE_1MBPS                      (0x00)
#define PPDU_LEG_RATE_2MBPS                      (0x01)
#define PPDU_LEG_RATE_5_5MBPS                    (0x02)
#define PPDU_LEG_RATE_11MBPS                     (0x03)
#define PPDU_LEG_RATE_6MBPS                      (0x0B)
#define PPDU_LEG_RATE_9MBPS                      (0x0F)
#define PPDU_LEG_RATE_12MBPS                     (0x0A)
#define PPDU_LEG_RATE_18MBPS                     (0x0E)
#define PPDU_LEG_RATE_24MBPS                     (0x09)
#define PPDU_LEG_RATE_36MBPS                     (0x0D)
#define PPDU_LEG_RATE_48MBPS                     (0x08)
#define PPDU_LEG_RATE_54MBPS                     (0x0C)

typedef struct mbps_txs_mfr_st {
    UINT32 mod_format;
    UINT32 rate;
}MBPS_TXS_MFR_ST, *MBPS_TXS_MFR_PTR;

extern void mpb_init(void);
extern void mpb_exit(void);
#endif // _MAC_PHY_BYPASS_PUB_H_
