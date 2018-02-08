/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _RTL8195A_MII_H_
#define _RTL8195A_MII_H_

#include "basic_types.h"
#include "hal_api.h"



#define MII_TX_DESC_NO					8
#define MII_RX_DESC_NO					8
#define MII_BUF_SIZE					1600
#define MAX_FRAME_SIZE					1514


#define HAL_MII_READ32(addr)				HAL_READ32(MII_REG_BASE, addr)
#define HAL_MII_WRITE32(addr, value)		HAL_WRITE32(MII_REG_BASE, addr, value)
#define HAL_MII_READ16(addr)				HAL_READ16(MII_REG_BASE, addr)
#define HAL_MII_WRITE16(addr, value)		HAL_WRITE16(MII_REG_BASE, addr, value)
#define HAL_MII_READ8(addr)					HAL_READ8(MII_REG_BASE, addr)
#define HAL_MII_WRITE8(addr, value)			HAL_WRITE8(MII_REG_BASE, addr, value)

/* =============== MAC Register Offset Definition =============== */
#define REG_MII_IDR0					0x0000
#define REG_MII_IDR4					0x0004
#define REG_MII_COM						0x0038
#define REG_MII_ISRIMR					0x003C
#define REG_MII_TC						0x0040
#define REG_MII_RC						0x0044
#define REG_MII_MS						0x0058
#define REG_MII_MIIA					0x005C

#define REG_MII_TXFDP1					0x1300
#define REG_MII_RXFDP1					0x13F0
#define REG_MII_ETNRXCPU1				0x1430
#define REG_MII_IOCMD					0x1434
#define REG_MII_IOCMD1					0x1438

/* =============== MAC Register BIT Definition =============== */
/* Command Register (0x38) */
#define COM_RST							BIT0
#define COM_RXCHKSUM					BIT1
#define COM_RXJUMBO						BIT3

/* Interrupt Status & Interrupt Mask Register (0x3C & 0x3E) */
#define ISR_RXOK						BIT0
#define ISR_RER_RUNT					BIT2
#define ISR_RER_OVF						BIT4
#define ISR_RDU							BIT5
#define ISR_TXOK						BIT6
#define ISR_TER							BIT7
#define ISR_LINKCHG						BIT8
#define ISR_TDU							BIT9
#define ISR_CLR_ALL						0x0000FFFF
#define IMR_RXOK						BIT16
#define IMR_RER_RUNT					BIT18
#define IMR_RER_OVF						BIT20
#define IMR_RDU							BIT21
#define IMR_TXOK						BIT22
#define IMR_TER							BIT23
#define IMR_LINKCHG						BIT24
#define IMR_TDU							BIT25

/* Transmit Configuration Register (0x40) */
#define TC_TX_NOPADDING					BIT0
#define TC_NORMAL_MODE					0
#define TC_LBK_R2T						1
#define TC_LBK_T2R						3
#define TC_LBK_MASK						0x00000300	// bit[9:8]
#define TC_IFG_TIME						3			// 9.6 us for 10Mbps, 960 ns for 100Mbps
#define TC_IFG_MASK						0x00001C00	// bit[12:10]

/* Receive Configuration Register (0x44) */
#define RC_AAP							BIT0
#define RC_APM							BIT1
#define RC_AM							BIT2
#define RC_AB							BIT3
#define RC_AR							BIT4
#define RC_AER							BIT5

/* Media Status Register (0x58) */
#define MS_LINKB						BIT26

/* MII Access Register (0x5C) */
#define MIIA_FLAG						BIT31  // 1: Write, 0: Read
#define MIIA_PHY_ADDR_MASK				0x7C000000  // bit[30:26]
#define MIIA_PHY_REG_ADDR_MASK			0x001F0000  // bit[20:16]

/* IO Command Register (0x1434) */
#define IOCMD_TXFN1ST					BIT0
#define IOCMD_TE						BIT4
#define IOCMD_RE						BIT5
#define IOCMD_RXFTH_1024				0
#define IOCMD_RXFTH_128					1
#define IOCMD_RXFTH_256					2
#define IOCMD_RXFTH_512					3
#define IOCMD_RXFTH_MASK				0x00001800  // bit[12:11]
#define IOCMD_TXFTH_128					0
#define IOCMD_TXFTH_256					1
#define IOCMD_TXFTH_512					2
#define IOCMD_TXFTH_1024				3
#define IOCMD_TXFTH_MASK				0x00180000  // bit[20:19]
#define IOCMD_SHORT_DES_FMT				BIT30

/* IO Command1 Register (0x1438) */
#define IOCMD1_RXRING1					BIT16
#define IOCMD1_EN_1GB					BIT24
#define IOCMD1_DSC_FMT_EXTRA			0x3  // 011
#define IOCMD1_DSCFMTEXTRA_MASK			0x70000000  // bit[30:28]

/* =============== PHY (RTL8201F) Register Bit Definition =============== */
#define PHY_ADDRESS						0x1  // 5 bits
#define PHY_REG0_ADDR					0x0  // 5 bits
#define PHY_REG1_ADDR					0x1  // 5 bits

/* Register 0 */
#define PHY_SPEED_MSB					BIT6
#define PHY_DUPLEX_MODE					BIT8
#define PHY_RESTART_NWAY				BIT9
#define PHY_NWAY_EN						BIT12
#define PHY_SPEED_LSB					BIT13
#define PHY_SW_RESET					BIT15

/* Register 1 */
#define PHY_LINK_STATUS					BIT2
#define PHY_NWAY_COMPLETE				BIT5

/* =============== Tx/Rx Descriptor Bit Definition =============== */
#define TX_DESC_OWN						BIT31
#define TX_DESC_EOR						BIT30
#define TX_DESC_FS						BIT29
#define TX_DESC_LS						BIT28
#define TX_DESC_CRC						BIT23
#define TX_DESC_DATA_LEN_MASK			0x1FFFF  // bit[16:0]
#define TX_DESC_VLAN_INTACT				0
#define TX_DESC_VLAN_INSERT				1
#define TX_DESC_VLAN_REMOVE				2
#define TX_DESC_VLAN_REMARKING			3
#define TX_DESC_VLAN_ACT_MASK			0x06000000
#define C_VLAN_HDR						0x8100279F
#define S_VLAN_HDR						0x88A8279F
#define TX_DESC_VLAN_TAG_MASK			0x0000FFFF

#define RX_DESC_OWN						BIT31
#define RX_DESC_EOR						BIT30
#define RX_DESC_PKT_TYPE_MASK			0x001E0000  // bit[20:17]
#define RX_DESC_DATA_LEN_MASK			0xFFF  // bit[11:0]


typedef struct _TX_DESC_FMT_
{
	u32 dw1;	// offset 0
	u32 addr;	// offset 4
	u32 dw2;	// offset 8
	u32 dw3;	// offset 12
	u32 dw4;	// offset 16
}TX_DESC_FMT, *PTX_DESC_FMT;

typedef struct _RX_DESC_FMT_
{
	u32 dw1;	// offset 0
	u32 addr;	// offset 4
	u32 dw2;	// offset 8
	u32 dw3;	// offset 12
}RX_DESC_FMT, *PRX_DESC_FMT;



VOID
HalMiiInitIrqRtl8195a(
	IN VOID *Data
);

s32
HalMiiInitRtl8195a(
	IN VOID
);

VOID
HalMiiDeInitRtl8195a(
	IN VOID
);

s32
HalMiiWriteDataRtl8195a(
	IN const char *Data,
	IN u32 Size
);

u32
HalMiiSendPacketRtl8195a(
	IN VOID
);

u32
HalMiiReceivePacketRtl8195a(
	IN VOID
);

u32
HalMiiReadDataRtl8195a(
	IN u8 *Data,
	IN u32 Size
);

VOID
HalMiiGetMacAddressRtl8195a(
	IN u8 *Addr
);

u32
HalMiiGetLinkStatusRtl8195a(
	IN VOID
);

VOID
HalMiiForceLinkRtl8195a(
	IN s32 Speed,
	IN s32 Duplex
);



#ifdef CONFIG_MII_VERIFY

/* Ethernet Module registers */
#define REG_RTL_MII_IDR0    0x0000  // Table 2 IDR0 (Offset 0000h-0003h, R/W)
#define REG_RTL_MII_IDR4    0x0004  // Table 3 IDR4 (Offset 0004h-0007h, R/W)
#define REG_RTL_MII_MAR0    0x0008  // Table 4 MAR0 (Offset 0008h-000bh, R/W)
#define REG_RTL_MII_MAR4    0x000C  // Table 5 MAR4 (Offset 000ch-000fh, R/W)
#define REG_RTL_MII_CR      0x0038  // Table 21 Command Register          (COM_REG, Offset 0038-003Bh, R/W)
#define REG_RTL_MII_IMRISR  0x003C  // Table 22 + Table 23
#define REG_RTL_MII_TCR     0x0040  // Table 24 Transmit Configuration Register (TC_REG, Offset 0040h-0043h, R/W)
#define REG_RTL_MII_RCR     0x0044  // Table 25 Receive Configuration Register  (RC_REG, Offset 0044h-0047h, R/W)
#define REG_RTL_MII_CTCR    0x0048  // Table 26 CPU Tag Control Register  (CPUTAG_REG, Offset 0048h-004bh, R/W)
#define REG_RTL_MII_CONFIG  0x004C  // Table 27 Configuration Register (CONFIG_REG, Offset 004ch-004fh, R/W)
#define REG_RTL_MII_CTCR1   0x0050  // Table 28 CPUTAG1 Register          (CPUTAG1_REG, Offset 0050h-0053h, R/W)
#define REG_RTL_MII_MSR     0x0058  // Table 29 Media Status Register     (MS_reg: Offset 0058h ??005bh, R/W)
#define REG_RTL_MII_MIIAR   0x005C  // Table 30 MII Access Register (MIIA_REG, Offset 005c-005fh, R/W)
#define REG_RTL_MII_VR      0x0064  // Table 32 VLAN Register (VLAN_REG, Offset 0064-0067h, R/W)
#define REG_RTL_MII_IMR0    0x00D0  // Table 50 IMR0_REG (IMR0_REG, Offset D0h-D3h)
#define REG_RTL_MII_IMR1    0x00D4  // Table 51 IMR1_REG (IMR1_REG, Offset d4h-d7h)
#define REG_RTL_MII_ISR1    0x00D8  // Table 52 ISR1 Register (ISR1_REG, Offset D8h-DBh)
#define REG_RTL_MII_INTR    0x00DC  // Table 53 Interrupt routing register (INTR_REG, Offset DCh-DFh)
#define REG_RTL_MII_CCR     0x00E4  // Table xx Clock Control Register (CLKCTL_REG, Offset E4h-E7h)

/* CPU Interface registers */
#define REG_RTL_MII_TXFDP1  0x1300  // Table 55 TxFDP1 register (TXFDP1_REG, offset 1300h-1303h)
#define REG_RTL_MII_TXCDO1  0x1304  // Table 56 TxCDO1 register (TXCDO1_REG, offset 1304h-1305h)
#define REG_RTL_MII_TXFDP2  0x1310  // Table 57 TxFDP2 register (TXFDP2_REG, offset 1310h-1313h)
#define REG_RTL_MII_TXCDO2  0x1314  // Table 58 TxCDO2 register (TXCDO2_REG, offset 1314h-1315h)
#define REG_RTL_MII_TXFDP3  0x1320  // Table 59 TxFDP3 register (TXFDP3_REG, offset 1320h-1323h)
#define REG_RTL_MII_TXCDO3  0x1324  // Table 60 TxCDO3 register (TXCDO3_REG, offset 1324h-1325h)
#define REG_RTL_MII_TXFDP4  0x1330  // Table 61 TxFDP4 register (TXFDP4_REG, offset 1330h-1333h)
#define REG_RTL_MII_TXCDO4  0x1334  // Table 62 TxCDO4 register (TXCDO4_REG, offset 1334h-1335h)
#define REG_RTL_MII_TXFDP5  0x1340  // Table 63 TxFDP5 register (TXFDP5_REG, offset 1340h-1343h)
#define REG_RTL_MII_TXCDO5  0x1344  // Table 64 TxCDO5 register (TXCDO5_REG, offset 1344h-1345h)
#define REG_RTL_MII_RXFDP2  0x1390  // Table 66 RxFDP2 register (RXFDP#_REG, offset 1390h-1393h)
#define REG_RTL_MII_RXFDP1  0x13F0  // Table 71 RxFDP1 register (RXFDP1_REG, offset 13F0h-13F3h)
#define REG_RTL_MII_RXRS1   0x13F6  // Table 73 Rx Ring Size1 register (RX_RS1_REG, offset 13F6h-13F7h)

#define REG_RTL_MII_RX_PSE1   0x142C  // Table 77 Rx_Pse_Des_Thres_1_h (RX_PSE1_REG, Offset 142ch)
#define REG_RTL_MII_ETNRXCPU1 0x1430  // Table 79 EhtrntRxCPU_Des_Num1 (ETNRXCPU1_REG, Offset 1430h-1433h)
#define REG_RTL_MII_IOCMD     0x1434  // Table 80 Ethernet_IO_CMD (ETN_IO_CMD_REG, Offset 1434h-1437h)
#define REG_RTL_MII_IOCMD1    0x1438  // Table 81 Ethernet_IO_CMD1 (IO_CMD1_REG: Offset 1438h-143bh)


#define  CMD_CONFIG  0x00081000

//2014-04-29 yclin (disable [27] r_en_precise_dma)
// #define CMD1_CONFIG  0x39000000
#define CMD1_CONFIG  0x31000000

// #define MAX_RX_DESC_SIZE  6
#define MAX_RX_DESC_SIZE  1
#define MAX_TX_DESC_SIZE  5

// 0058h
#define BIT_SHIFT_MSR_FORCE_SPEED_SELECT    16
#define BIT_MASK_MSR_FORCE_SPEED_SELECT     0x3
#define BIT_MSR_FORCE_SPEED_SELECT(x)(((x) & BIT_MASK_MSR_FORCE_SPEED_SELECT) << BIT_SHIFT_MSR_FORCE_SPEED_SELECT)
#define BIT_INVC_MSR_FORCE_SPEED_SELECT (~(BIT_MASK_MSR_FORCE_SPEED_SELECT << BIT_SHIFT_MSR_FORCE_SPEED_SELECT))

#define BIT_SHIFT_MSR_FORCE_SPEED_MODE_ENABLE    10
#define BIT_MASK_MSR_FORCE_SPEED_MODE_ENABLE     0x1
#define BIT_MSR_FORCE_SPEED_MODE_ENABLE(x)(((x) & BIT_MASK_MSR_FORCE_SPEED_MODE_ENABLE) << BIT_SHIFT_MSR_FORCE_SPEED_MODE_ENABLE)
#define BIT_INVC_MSR_FORCE_SPEED_MODE_ENABLE (~(BIT_MASK_MSR_FORCE_SPEED_MODE_ENABLE << BIT_SHIFT_MSR_FORCE_SPEED_MODE_ENABLE))

// 1434h
#define BIT_SHIFT_IOCMD_RXENABLE      5
#define BIT_MASK_IOCMD_RXENABLE       0x1
#define BIT_IOCMD_RXENABLE(x)(((x) & BIT_MASK_IOCMD_RXENABLE) << BIT_SHIFT_IOCMD_RXENABLE)
#define BIT_INVC_IOCMD_RXENABLE (~(BIT_MASK_IOCMD_RXENABLE << BIT_SHIFT_IOCMD_RXENABLE))

#define BIT_SHIFT_IOCMD_TXENABLE      4
#define BIT_MASK_IOCMD_TXENABLE       0x1
#define BIT_IOCMD_TXENABLE(x)(((x) & BIT_MASK_IOCMD_TXENABLE) << BIT_SHIFT_IOCMD_TXENABLE)
#define BIT_INVC_IOCMD_TXENABLE (~(BIT_MASK_IOCMD_TXENABLE << BIT_SHIFT_IOCMD_TXENABLE))

#define BIT_SHIFT_IOCMD_FIRST_DMATX_ENABLE      0
#define BIT_MASK_IOCMD_FIRST_DMATX_ENABLE       0x1
#define BIT_IOCMD_FIRST_DMATX_ENABLE(x)(((x) & BIT_MASK_IOCMD_FIRST_DMATX_ENABLE) << BIT_SHIFT_IOCMD_FIRST_DMATX_ENABLE)
#define BIT_INVC_IOCMD_FIRST_DMATX_ENABLE (~(BIT_MASK_IOCMD_FIRST_DMATX_ENABLE << BIT_SHIFT_IOCMD_FIRST_DMATX_ENABLE))

// 1438h
#define BIT_SHIFT_IOCMD1_FIRST_DMARX_ENABLE    16
#define BIT_MASK_IOCMD1_FIRST_DMARX_ENABLE     0x1
#define BIT_IOCMD1_FIRST_DMARX_ENABLE(x)(((x) & BIT_MASK_IOCMD1_FIRST_DMARX_ENABLE) << BIT_SHIFT_IOCMD1_FIRST_DMARX_ENABLE)
#define BIT_INVC_IOCMD1_FIRST_DMARX_ENABLE (~(BIT_MASK_IOCMD1_FIRST_DMARX_ENABLE << BIT_SHIFT_IOCMD1_FIRST_DMARX_ENABLE))


/**
 * 1.4.1.7 Tx command descriptor used in RL6266
 *   5 dobule words
 */
typedef struct _TX_INFO_ {
    union {
        struct {
            u32 own:1;          //31
            u32 eor:1;          //30
            u32 fs:1;           //29
            u32 ls:1;           //28
            u32 ipcs:1;         //27
            u32 l4cs:1;         //26
            u32 keep:1;         //25
            u32 blu:1;          //24
            u32 crc:1;          //23
            u32 vsel:1;         //22
            u32 dislrn:1;       //21
            u32 cputag_ipcs:1;  //20
            u32 cputag_l4cs:1;  //19
            u32 cputag_psel:1;  //18
            u32 rsvd:1;         //17
            u32 data_length:17; //0~16
        } bit;
        u32 dw; //double word
    } opts1;

    u32 addr;

    union {
        struct {
            u32 cputag:1;          //31
            u32 aspri:1;           //30
            u32 cputag_pri:3;      //27~29
            u32 tx_vlan_action:2;  //25~26
            u32 tx_pppoe_action:2; //23~24
            u32 tx_pppoe_idx:3;    //20~22
            u32 efid:1;            //19
            u32 enhance_fid:3;     //16~18
            u32 vidl:8;            // 8~15
            u32 prio:3;            // 5~7
            u32 cfi:1;             // 4
            u32 vidh:4;            // 0~3
        } bit;
        u32 dw; //double word
    } opts2;

    union {
        struct {
            u32 extspa:3;           //29~31
            u32 tx_portmask:6;      //23~28
            u32 tx_dst_stream_id:7; //16~22
            u32 rsvd:14;            // 2~15
            u32 l34keep:1;          // 1
            u32 ptp:1;              // 0
        } bit;
        u32 dw; //double word
    } opts3;

    union {
        struct {
            u32 lgsen:1;  //31
            u32 lgmss:11; //20~30
            u32 rsvd:20;  // 0~19
        } bit;
        u32 dw; //double word
    } opts4;

} TX_INFO, *PTX_INFO;

typedef struct _RX_INFO_ {
    union{
        struct{
            u32 own:1;          //31
            u32 eor:1;          //30
            u32 fs:1;           //29
            u32 ls:1;           //28
            u32 crcerr:1;       //27
            u32 ipv4csf:1;      //26
            u32 l4csf:1;        //25
            u32 rcdf:1;         //24
            u32 ipfrag:1;       //23
            u32 pppoetag:1;     //22
            u32 rwt:1;          //21
            u32 pkttype:4;      //20-17
            u32 l3routing:1;    //16
            u32 origformat:1;   //15
            u32 pctrl:1;        //14
#ifdef CONFIG_RG_JUMBO_FRAME
            u32 data_length:14; //13~0
#else
            u32 rsvd:2;         //13~12
            u32 data_length:12; //11~0
#endif
        }bit;
        u32 dw; //double word
    }opts1;

    u32 addr;

    union{
        struct{
            u32 cputag:1;               //31
            u32 ptp_in_cpu_tag_exist:1; //30
            u32 svlan_tag_exist:1;      //29
            u32 rsvd_2:2;               //27~28
            u32 pon_stream_id:7;        //20~26
            u32 rsvd_1:3;               //17~19
            u32 ctagva:1;               //16
            u32 cvlan_tag:16;           //15~0
        }bit;
        u32 dw; //double word
    }opts2;

    union{
        struct{
            u32 src_port_num:5;      //27~31
            u32 dst_port_mask:6;     //21~26
            u32 reason:8;            //13~20
            u32 internal_priority:3; //10~12
            u32 ext_port_ttl_1:5;    //5~9
            u32 rsvd:5;              //4~0
        }bit;
        u32 dw; //double word
    }opts3;
} RX_INFO, *PRX_INFO;

/**
 * GMAC_STATUS_REGS
 */
// TX/RX Descriptor Common
#define BIT_SHIFT_GMAC_DESCOWN        31
#define BIT_MASK_GMAC_DESCOWN        0x1
#define BIT_GMAC_DESCOWN(x)(((x) & BIT_MASK_GMAC_DESCOWN) << BIT_SHIFT_GMAC_DESCOWN)
#define BIT_INVC_GMAC_DESCOWN (~(BIT_MASK_GMAC_DESCOWN << BIT_SHIFT_GMAC_DESCOWN))

#define BIT_SHIFT_GMAC_RINGEND        30
#define BIT_MASK_GMAC_RINGEND        0x1
#define BIT_GMAC_RINGEND(x)(((x) & BIT_MASK_GMAC_RINGEND) << BIT_SHIFT_GMAC_RINGEND)
#define BIT_INVC_GMAC_RINGEND (~(BIT_MASK_GMAC_RINGEND << BIT_SHIFT_GMAC_RINGEND))

#define BIT_SHIFT_GMAC_FIRSTFRAG      29
#define BIT_MASK_GMAC_FIRSTFRAG      0x1
#define BIT_GMAC_FIRSTFRAG(x)(((x) & BIT_MASK_GMAC_FIRSTFRAG) << BIT_SHIFT_GMAC_FIRSTFRAG)
#define BIT_INVC_GMAC_FIRSTFRAG (~(BIT_MASK_GMAC_FIRSTFRAG << BIT_SHIFT_GMAC_FIRSTFRAG))

#define BIT_SHIFT_GMAC_LASTFRAG       28
#define BIT_MASK_GMAC_LASTFRAG       0x1
#define BIT_GMAC_LASTFRAG(x)(((x) & BIT_MASK_GMAC_LASTFRAG) << BIT_SHIFT_GMAC_LASTFRAG)
#define BIT_INVC_GMAC_LASTFRAG (~(BIT_MASK_GMAC_LASTFRAG << BIT_SHIFT_GMAC_LASTFRAG))

// TX Descriptor opts1
#define BIT_SHIFT_GMAC_IPCS           27
#define BIT_MASK_GMAC_IPCS           0x1
#define BIT_GMAC_IPCS(x)(((x) & BIT_MASK_GMAC_IPCS) << BIT_SHIFT_GMAC_IPCS)
#define BIT_INVC_GMAC_IPCS (~(BIT_MASK_GMAC_IPCS << BIT_SHIFT_GMAC_IPCS))

#define BIT_SHIFT_GMAC_L4CS           26
#define BIT_MASK_GMAC_L4CS           0x1
#define BIT_GMAC_L4CS(x)(((x) & BIT_MASK_GMAC_L4CS) << BIT_SHIFT_GMAC_L4CS)
#define BIT_INVC_GMAC_L4CS (~(BIT_MASK_GMAC_L4CS << BIT_SHIFT_GMAC_L4CS))

#define BIT_SHIFT_GMAC_KEEP           25
#define BIT_MASK_GMAC_KEEP           0x1
#define BIT_GMAC_KEEP(x)(((x) & BIT_MASK_GMAC_KEEP) << BIT_SHIFT_GMAC_KEEP)
#define BIT_INVC_GMAC_KEEP (~(BIT_MASK_GMAC_KEEP << BIT_SHIFT_GMAC_KEEP))

#define BIT_SHIFT_GMAC_BLU            24
#define BIT_MASK_GMAC_BLU            0x1
#define BIT_GMAC_BLU(x)(((x) & BIT_MASK_GMAC_BLU) << BIT_SHIFT_GMAC_BLU)
#define BIT_INVC_GMAC_BLU (~(BIT_MASK_GMAC_BLU << BIT_SHIFT_GMAC_BLU))

#define BIT_SHIFT_GMAC_TXCRC          23
#define BIT_MASK_GMAC_TXCRC          0x1
#define BIT_GMAC_TXCRC(x)(((x) & BIT_MASK_GMAC_TXCRC) << BIT_SHIFT_GMAC_TXCRC)
#define BIT_INVC_GMAC_TXCRC (~(BIT_MASK_GMAC_TXCRC << BIT_SHIFT_GMAC_TXCRC))

#define BIT_SHIFT_GMAC_VSEL           22
#define BIT_MASK_GMAC_VSEL           0x1
#define BIT_GMAC_VSEL(x)(((x) & BIT_MASK_GMAC_VSEL) << BIT_SHIFT_GMAC_VSEL)
#define BIT_INVC_GMAC_VSEL (~(BIT_MASK_GMAC_VSEL << BIT_SHIFT_GMAC_VSEL))

#define BIT_SHIFT_GMAC_DISLRN         21
#define BIT_MASK_GMAC_DISLRN         0x1
#define BIT_GMAC_DISLRN(x)(((x) & BIT_MASK_GMAC_DISLRN) << BIT_SHIFT_GMAC_DISLRN)
#define BIT_INVC_GMAC_DISLRN (~(BIT_MASK_GMAC_DISLRN << BIT_SHIFT_GMAC_DISLRN))

#define BIT_SHIFT_GMAC_CPUTAG_IPCS    20
#define BIT_MASK_GMAC_CPUTAG_IPCS    0x1
#define BIT_GMAC_CPUTAG_IPCS(x)(((x) & BIT_MASK_GMAC_CPUTAG_IPCS) << BIT_SHIFT_GMAC_CPUTAG_IPCS)
#define BIT_INVC_GMAC_CPUTAG_IPCS (~(BIT_MASK_GMAC_CPUTAG_IPCS << BIT_SHIFT_GMAC_CPUTAG_IPCS))

#define BIT_SHIFT_GMAC_CPUTAG_L4CS    19
#define BIT_MASK_GMAC_CPUTAG_L4CS    0x1
#define BIT_GMAC_CPUTAG_L4CS(x)(((x) & BIT_MASK_GMAC_CPUTAG_L4CS) << BIT_SHIFT_GMAC_CPUTAG_L4CS)
#define BIT_INVC_GMAC_CPUTAG_L4CS (~(BIT_MASK_GMAC_CPUTAG_L4CS << BIT_SHIFT_GMAC_CPUTAG_L4CS))

#define BIT_SHIFT_GMAC_CPUTAG_PSEL    18
#define BIT_MASK_GMAC_CPUTAG_PSEL    0x1
#define BIT_GMAC_CPUTAG_PSEL(x)(((x) & BIT_MASK_GMAC_CPUTAG_PSEL) << BIT_SHIFT_GMAC_CPUTAG_PSEL)
#define BIT_INVC_GMAC_CPUTAG_PSEL (~(BIT_MASK_GMAC_CPUTAG_PSEL << BIT_SHIFT_GMAC_CPUTAG_PSEL))


typedef struct _PHY_MODE_INFO_ {
    u8 PhyAddress;
    u8 PhyMode;
    u8 PhyInterface;
} PHY_MODE_INFO, *PPHY_MODE_INFO;

typedef enum _PHY_MODE_SWITCH_ {
    PHY_MODE_DISABLE = 0,
    PHY_MODE_ENABLE  = 1
} PHY_MODE_SWITCH, *PPHY_MODE_SWITCH;

typedef enum _PHY_INTERFACE_SELECT_ {
    PHY_INTERFACE_ONE_WORKS  = 0,
    PHY_INTERFACE_ZERO_WORKS = 1
} PHY_INTERFACE_SELECT, *PPHY_INTERFACE_SELECT;

typedef enum _GMAC_MSR_FORCE_SPEED_ {
    FORCE_SPD_100M = 0,
    FORCE_SPD_10M  = 1,
    FORCE_SPD_GIGA = 2,
    NO_FORCE_SPD   = 3
}GMAC_MSR_FORCE_SPEED, *PGMAC_MSR_FORCE_SPEED;

typedef enum _GMAC_INTERRUPT_MASK_ {
    GMAC_IMR_ROK      = BIT16,
    GMAC_IMR_CNT_WRAP = BIT17,
    GMAC_IMR_RER_RUNT = BIT18,
    // BIT19 Reserved
    GMAC_IMR_RER_OVF  = BIT20,
    GMAC_IMR_RDU      = BIT21,
    GMAC_IMR_TOK_TI   = BIT22,
    GMAC_IMR_TER      = BIT23,
    GMAC_IMR_LINKCHG  = BIT24,
    GMAC_IMR_TDU      = BIT25,
    GMAC_IMR_SWINT    = BIT26,
    GMAC_IMR_RDU2     = BIT27,
    GMAC_IMR_RDU3     = BIT28,
    GMAC_IMR_RDU4     = BIT29,
    GMAC_IMR_RDU5     = BIT30,
    GMAC_IMR_RDU6     = BIT31,
} GMAC_INTERRUPT_MASK, *PGMAC_INTERRUPT_MASK;

typedef enum _GMAC_INTERRUPT_STATUS_ {
    GMAC_ISR_ROK      = BIT0,
    GMAC_ISR_CNT_WRAP = BIT1,
    GMAC_ISR_RER_RUNT = BIT2,
    // BIT3 Reserved
    GMAC_ISR_RER_OVF  = BIT4,
    GMAC_ISR_RDU      = BIT5,
    GMAC_ISR_TOK_TI   = BIT6,
    GMAC_ISR_TER      = BIT7,
    GMAC_ISR_LINKCHG  = BIT8,
    GMAC_ISR_TDU      = BIT9,
    GMAC_ISR_SWINT    = BIT10,
    GMAC_ISR_RDU2     = BIT11,
    GMAC_ISR_RDU3     = BIT12,
    GMAC_ISR_RDU4     = BIT13,
    GMAC_ISR_RDU5     = BIT14,
    GMAC_ISR_RDU6     = BIT15,
} GMAC_INTERRUPT_STATUS, *PGMAC_INTERRUPT_STATUS;

typedef enum _GMAC_TX_VLAN_ACTION_ {
    INTACT			= 0,
    INSERT_VLAN_HDR = 1,
    REMOVE_VLAN_HDR = 2,
    REMARKING_VID   = 3
}GMAC_TX_VLAN_ACTION, *PGMAC_TX_VLAN_ACTION;

typedef enum _GMAC_RX_PACKET_TYPE_ {
    TYPE_ETHERNET	= 0,
    TYPE_IPV4		= 1,
    TYPE_IPV4_PPTP	= 2,
    TYPE_IPV4_ICMP	= 3,
	TYPE_IPV4_IGMP	= 4,
	TYPE_IPV4_TCP	= 5,
	TYPE_IPV4_UDP	= 6,
	TYPE_IPV6		= 7,
	TYPE_ICMPV6		= 8,
	TYPE_IPV6_TCP	= 9,
	TYPE_IPV6_UDP	= 10	
}GMAC_RX_PACKET_TYPE, *PGMAC_RX_PACKET_TYPE;



BOOL
HalMiiGmacInitRtl8195a(
        IN VOID *Data
        );

BOOL
HalMiiGmacResetRtl8195a(
        IN VOID *Data
        );

BOOL
HalMiiGmacEnablePhyModeRtl8195a(
        IN VOID *Data
        );

u32
HalMiiGmacXmitRtl8195a(
        IN VOID *Data
        );

VOID
HalMiiGmacCleanTxRingRtl8195a(
        IN VOID *Data
        );

VOID
HalMiiGmacFillTxInfoRtl8195a(
        IN VOID *Data
        );

VOID
HalMiiGmacFillRxInfoRtl8195a(
        IN VOID *Data
        );

VOID
HalMiiGmacTxRtl8195a(
        IN VOID *Data
        );

VOID
HalMiiGmacRxRtl8195a(
        IN VOID *Data
        );

VOID
HalMiiGmacSetDefaultEthIoCmdRtl8195a(
        IN VOID *Data
        );

VOID
HalMiiGmacInitIrqRtl8195a(
        IN VOID *Data
        );

u32
HalMiiGmacGetInterruptStatusRtl8195a(
		VOID
        );

VOID
HalMiiGmacClearInterruptStatusRtl8195a(
        u32 IsrStatus
        );
#endif  // #ifdef CONFIG_MII_VERIFY

#endif  // #ifndef _RTL8195A_MII_H_


