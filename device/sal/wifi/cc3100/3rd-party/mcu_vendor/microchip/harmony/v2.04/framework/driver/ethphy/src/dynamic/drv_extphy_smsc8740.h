/*******************************************************************************
  SMSC LAN8740 definitions

  Company:
    Microchip Technology Inc.
    
  File Name:
    eth_pic32_ext_phy_smsc8740.h

  Summary:
    SMSC LAN8740 definitions

  Description:
    This file provides the SMSC LAN8740 definitions.
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2012 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// DOM-IGNORE-END

#ifndef _SMSC_8700_H_

#define _SMSC_8700_H_

typedef enum
{
	/*
	// basic registers, accross all registers: 0-1
	PHY_REG_BMCON		= 0,
	PHY_REG_BMSTAT		= 1,
	// extended registers: 2-15
	PHY_REG_PHYID1		= 2,
	PHY_REG_PHYID2		= 3,
	PHY_REG_ANAD		= 4,
	PHY_REG_ANLPAD		= 5,
	PHY_REG_ANLPADNP	= 5,
	PHY_REG_ANEXP		= 6,
	PHY_REG_ANNPTR		= 7,
	PHY_REG_ANLPRNP		= 8,
	*/
	// specific vendor registers: 16-31
	PHY_REG_SILICON_REV	= 16,
	PHY_REG_MODE_CTRL	= 17,
	PHY_REG_SPECIAL_MODE	= 18,
	PHY_REG_SYMBOL_ERR_CNT	= 26,
	PHY_REG_SPECIAL_CTRL	= 27,
	PHY_REG_INT_SOURCE	= 29,
	PHY_REG_INT_MASK	= 30,
	PHY_REG_PHY_CTRL	= 31,
	PHY_MMD_ACCESS_CONTROL = 13,
	PHY_MMD_ACCESS_DATA_ADDR = 14,
	//
	//PHY_REGISTERS		= 32	// total number of registers
}ePHY_VENDOR_REG;
// updated version of ePHY_REG


// vendor registers
//
typedef union {
  struct {    
    unsigned :6;
    unsigned SILICON_REV:4;
    unsigned :6;
  };
  struct {
    unsigned short w:16;
  };
} __SILICONREVbits_t;	// reg 16: PHY_REG_SILICON_REV 
#define	_SILICONREV_SILICON_REV_MASK		0x03c0


typedef union {
  struct {    
    unsigned :1;
    unsigned ENERGYON:1;
    unsigned FORCE_GOOD_LINK:1;
    unsigned PHYADPB:1;
    unsigned :2;
    unsigned ALTINT:1;
    unsigned :2;
    unsigned FAR_LOOPBACK:1;
    unsigned MDPREPB:1;
    unsigned LOWSQEN:1;
    unsigned :1;
    unsigned EDPWRDOWN:1;
    unsigned :2;
  };
  struct {
    unsigned short w:16;
  };
} __MODECTRLbits_t;	// reg 17: PHY_REG_MODE_CTRL
#define	_MODECTRL_ENERGYON_MASK		0x0002
#define	_MODECTRL_FORCE_GOOD_LINK_MASK	0x0004
#define	_MODECTRL_PHYADPB_MASK		0x0008
#define	_MODECTRL_ALTINT_MASK		0x0040
#define	_MODECTRL_FAR_LOOPBACK_MASK	0x0200
#define	_MODECTRL_MDPREPB_MASK		0x0400
#define	_MODECTRL_LOWSQEN_MASK		0x0800
#define	_MODECTRL_EDPWRDOWN_MASK	0x2000


typedef union {
  struct {    
    unsigned PHYAD:5;
    unsigned MODE:3;
    unsigned :6;
    unsigned MIIMODE:1;
    unsigned :1;
  };
  struct {
    unsigned short w:16;
  };
} __SPECIALMODEbits_t;	// reg 18: PHY_REG_SPECIAL_MODE
#define	_SPECIALMODE_PHYAD_MASK		0x001f
#define	_SPECIALMODE_MODE_MASK		0x00e0
#define	_SPECIALMODE_MIIMODE_MASK	0x4000




typedef union {
  struct {
    unsigned Sym_Err_Cnt:16;
  };
  struct {
    unsigned short w:16;
  };
} __SYMBOLERRCNTbits_t;	// reg 26: PHY_REG_SYMBOL_ERR_CNT


typedef union {
  struct {    
    unsigned :4;
    unsigned XPOL:1;
    unsigned :6;
    unsigned SQEOFF:1;
    unsigned :1;
    unsigned CH_SELECT:1;
    unsigned :1;
    unsigned AMDIXCTRL:1;
  };
  struct {
    unsigned short w:16;
  };
} __SPECIALCTRLbits_t;	// reg 27: PHY_REG_SPECIAL_CTRL
#define	_SPECIALCTRL_XPOL_MASK		0x0010
#define	_SPECIALCTRL_SQEOFF_MASK	0x0800
#define	_SPECIALCTRL_CH_SELECT_MASK	0x2000    // Manual MDIX
#define	_SPECIALCTRL_AMDIXCTRL_MASK	0x8000




typedef union {
  struct {    
    unsigned :1;
    unsigned INT1:1;
    unsigned INT2:1;
    unsigned INT3:1;
    unsigned INT4:1;
    unsigned INT5:1;
    unsigned INT6:1;
    unsigned INT7:1;
    unsigned INT8:1;
    unsigned :7;
  };
  struct {
    unsigned short w:16;
  };
} __INTSOURCEbits_t;	// reg 29: PHY_REG_INT_SOURCE

typedef union {
  struct {
    unsigned :1;
    unsigned INT1:1;
    unsigned INT2:1;
    unsigned INT3:1;
    unsigned INT4:1;
    unsigned INT5:1;
    unsigned INT6:1;
    unsigned INT7:1;
    unsigned INT8:1;
    unsigned :7;
  };
  struct {
    unsigned short w:16;
  };
} __INTMASKbits_t;	// reg 30: PHY_REG_INT_MASK

#define WOL_INT8_EN 0x0100

typedef union {
  struct {	      
    unsigned SCRMBL_DISBL:1;
    unsigned :1;
    unsigned SPEED:3;	// 1: 10MbpsHD; 5:10MbpsFD; 2: 100MbpsHD; 6: 100MbpsFD; 
    unsigned :1;
    unsigned ENABLE_4B5B:1;
    unsigned GPO:3;
    unsigned :2;
    unsigned AUTODONE:1;
    unsigned :3;
  };
  struct {
    unsigned short w:16;
  };
} __PHYCTRLbits_t;	// reg 31: PHY_REG_PHY_CTRL
#define	_PHYCTRL_SCRMBL_DISBL_MASK	0x0001
#define	_PHYCTRL_SPEED_MASK		0x001c
#define	_PHYCTRL_SPEED_FDUPLX_MASK	0x0010
#define	_PHYCTRL_SPEED_100_10_MASK	0x000c
#define	_PHYCTRL_SPEED_100_MASK		0x0008
#define	_PHYCTRL_ENABLE_4B5B_MASK	0x0040
#define	_PHYCTRL_GPO_MASK		0x0380
#define	_PHYCTRL_AUTODONE_MASK		0x1000

// MMD ( MDIO Managable Devices )
typedef union {
  struct {	      
    unsigned MMD_DEV_ADDR:5;
    unsigned :9;
    unsigned MMD_FUNCTION:2;
  };
  struct {
    unsigned short w:16;
  };
} __MMDACCESSCONTROLbits_t;	// Index 13: 


#define _PHY_MMD_CNTL_ACCESS_ADDRESS_MASK  0x0000 // 15 and 14th is 00
#define _PHY_MMD_CNTL_ACCESS_DATA_MASK  0x4000 // 15 and 14 th is 01
#define _PHY_MMD_RESERVED_MASK 0x3FE0

typedef enum 
{
    MMD_PCS =3,
    MMD_AUTONEG=7,
    MMD_VENDOR_SPECIFIC=30,
}eMMDDEVADDRES;

// Wake up control and Status Register
typedef union {
  struct {
    unsigned BROADCAST_WKAEUP_ENBL:1;
    unsigned MAGIC_PKT_ENBL:1;
    unsigned WAKEUP_FRAME_ENBL:1;
    unsigned PERFECT_DA_WAKEUP_ENBL:1;
    unsigned BROADCAST_PKT_RCVD:1;
    unsigned MAGIC_PKT_RCVD:1;
    unsigned RMT_WAKEUP_RCVD:1;
    unsigned WOL_DA_FRAME_RCVD:1;
    unsigned WOL_CONFIG:1;
    unsigned NPME_SLEF_CLEAR:1;
    unsigned RMII_SELECT:1;
    unsigned LED2_SELECT:1;
    unsigned LED1_SELECT:1;
    unsigned INTF_DISABLE:1;
  };
  struct {
    unsigned short w:16;
  };
} __WOL_CNTL_STATUS_bits_t;	// Index 3.32784: 

#define _MII_RMII_INTF_MASK                     0x8000
#define _LED1_FUNCTION_LINKACTIVITY_MASK        0x0000
#define _LED1_FUNCTION_NINT_MASK                0x2000
#define _LED1_FUNCTION_NPME_MASK                0x4000
#define _LED1_FUNCTION_LINK_SPEED_MASK          0x6000
#define _LED2_FUNCTION_LINKSPEED_MASK           0x0000
#define _LED2_FUNCTION_NINT_MASK                0x0800
#define _LED2_FUNCTION_NPME_MASK                0x1000
#define _LED2_FUNCTION_LINKACTIVITY_MASK        0x1800

#define _RXD2_FUNCTION_SELC_MASK                0x0400
#define _NPME_FUNCTION_SELCTION_MASK            0x0200
#define _WOL_CONFIGURED_MASK                    0x0100
#define _WOL_DA_FRAME_RECEIVED_MASK             0x0080
#define _WOL_REMOTE_WAKEUP_FRAME_RECEIVED_MASK  0x0040
#define _WOL_MAGIC_PACKET_RECEIVED_MASK         0x0020
#define _WOL_BROADCAST_FRAME_RECEIVED_MASK      0x0010
#define _WOL_DA_WAKEUP_FRAME_ENABLE_MASK        0x0008
#define _WOL_WAKEUP_FRAME_ENABLE_MASK           0x0004
#define _WOL_MAGIC_PKT_ENABLE_MASK              0x0002
#define _WOL_BROADCAST_WAKEU_ENABLE_MASK        0x0001


// Wake up Filter Configuration Register WUF_CFGA
typedef union {
  struct {
    unsigned filter_offset:8;
    unsigned filter_broadcast_en:1;
    unsigned filter_multicast_en:1;
    unsigned filter_match_en:1;
    unsigned :3;
    unsigned filter_triggered:1;
    unsigned filter_filter_en:1;
  };
  struct {
    unsigned short w:16;
  };
} __WOL_FILTER_CONFIG_bits_t;	// Index 3.32785: 

#define _WOL_WAKEUP_FILTER_ENABLE           0x8000
#define _WOL_WAKEUP_FILTER_TRIGGERED        0x4000
#define _WOL_WAKEUP_ADDRESS_MATCH_ENA       0x0400
#define _WOL_WAKEUP_MULTICAST_MATCH_ENA     0x0200
#define _WOL_WAKEUP_BROADCAST_ENA           0x0100
#define _WOL_WAKEUP_PATTERN_OFFSET_MASK     0x00FF

typedef enum
{
    UNICAST_ADDRESS_MATCH=_WOL_WAKEUP_ADDRESS_MATCH_ENA,
    MULTICAST_ADDRESS_MATCH=_WOL_WAKEUP_MULTICAST_MATCH_ENA,
    BROADCAST_ADDRESS_MATCH=_WOL_WAKEUP_BROADCAST_ENA,
    WAKEUP_FILTER_NONE=0,
}eWOL_WAKEUP_FILTER;

typedef enum
{
    WOL_WAKEUP_CONTROL_STATUS=32784,
    WOL_WAKEUP_FILTER_REGISTER_A=32785,
    WOL_WAKEUP_FILTER_CRC_REGISTER_B=32786,
    WOL_WAKEUP_FILTER_BYTEMASK_8=32801,
    WOL_WAKEUP_FILTER_BYTEMASK_7=32802,
    WOL_WAKEUP_FILTER_BYTEMASK_6=32803,
    WOL_WAKEUP_FILTER_BYTEMASK_5=32804,
    WOL_WAKEUP_FILTER_BYTEMASK_4=32805,
    WOL_WAKEUP_FILTER_BYTEMASK_3=32806,
    WOL_WAKEUP_FILTER_BYTEMASK_2=32807,
    WOL_WAKEUP_FILTER_BYTEMASK_1=32808,
    WOL_MAC_ADDRESS_A=32865,
    WOL_MAC_ADDRESS_B=32866,
    WOL_MAC_ADDRESS_C=32867,

}eWOL_MMD_INDEXNO;

typedef enum
{
    PERFECT_DA_FRAME=0,
    REMOTE_WAKEUP_FRAME,
    MAGIC_PKT_RECEIVED,
    BROADCAST_FRAME_RCVD,
}eWOL_WAKEUP_FRAME;

typedef enum
{
    WOL_INT_ENABLED=1,
    WOL_INT_DISABLED,
}eWOLIntSatus;

typedef enum
{
    WOL_WAKEUP_FILTER_ENA=1,
    WOL_WAKEUP_FILTER_DIS,
}eWOLWakeupFilterSatus;


#define WOl_CRC_POLY 0x8005 /* s/b international standard for CRC-16 */

#define WOL_WAKEUP_FRAME_BYTEMASK_SIZE 8
// User configuration for WOL Wake up frame Part
#define WOL_WAKEUP_FRAME_START_OFFSET   0x40

// WOL_WAKEUP_FRAME_PATTERN_LEN is the size of  WOL_WAKEUP_FRAME_PATTERN . 
// So both WOL_WAKEUP_FRAME_PATTERN_LEN and WOL_WAKEUP_FRAME_PATTERN are 
// related to each other
#define WOL_WAKEUP_FRAME_PATTERN_LEN 4
#define WOL_WAKEUP_FRAME_PATTERN  {0x01,0x01,0x01,0x01}

// WOL wake up frame byte mask values need to be provided here
//The byte mask is a 128-bit field that specifies whether or not each of the 128
//contiguous bytes within the frame, beginning with the pattern offset, should be checked.
//If bit j in the byte mask is set, the detection logic checks the byte (pattern offset + j) in the
//frame, otherwise byte (pattern offset + j) is ignored.

#define WOL_WAKEUP_BYTEMASK_REG0 0x000F
#define WOL_WAKEUP_BYTEMASK_REG1 0x0000
#define WOL_WAKEUP_BYTEMASK_REG2 0x0000
#define WOL_WAKEUP_BYTEMASK_REG3 0x0000
#define WOL_WAKEUP_BYTEMASK_REG4 0x0000
#define WOL_WAKEUP_BYTEMASK_REG5 0x0000
#define WOL_WAKEUP_BYTEMASK_REG6 0x0000
#define WOL_WAKEUP_BYTEMASK_REG7 0x0000

#endif	// _SMSC_8700_H_

