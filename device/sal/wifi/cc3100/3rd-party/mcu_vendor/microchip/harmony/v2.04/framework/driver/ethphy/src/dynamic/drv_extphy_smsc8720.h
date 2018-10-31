/*******************************************************************************
  SMSC LAN8720 definitions

  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_extphy_smsc8720.h

  Summary:
    SMSC LAN8720 definitions

  Description:
    This file provides the SMSC LAN8720 definitions.

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

#ifndef _SMSC_8720_H_

#define _SMSC_8720_H_

typedef enum
{
    /*
    // basic registers, accross all registers: 0-1
    PHY_REG_BMCON       = 0,
    PHY_REG_BMSTAT      = 1,
    // extended registers: 2-15
    PHY_REG_PHYID1      = 2,
    PHY_REG_PHYID2      = 3,
    PHY_REG_ANAD        = 4,
    PHY_REG_ANLPAD      = 5,
    PHY_REG_ANLPADNP    = 5,
    PHY_REG_ANEXP       = 6,
    // PHY_REG_ANNPTR       = 7,    not defined for SMSC 8720
    // PHY_REG_ANLPRNP      = 8,    not defined for SMSC 8720
    */
    // specific vendor registers: 16-31
    PHY_REG_MODE_CTRL       = 17,
    PHY_REG_SPECIAL_MODE    = 18,
    PHY_REG_SYMBOL_ERR_CNT  = 26,
    PHY_REG_SPECIAL_CTRL    = 27,
    PHY_REG_INT_SOURCE      = 29,
    PHY_REG_INT_MASK        = 30,
    PHY_REG_PHY_CTRL        = 31,
    //
    //PHY_REGISTERS     = 32    // total number of registers
}ePHY_VENDOR_REG;
// updated version of ePHY_REG


// vendor registers
//

typedef union {
  struct {
    unsigned               :1;
    unsigned ENERGYON      :1;
    unsigned               :4;
    unsigned ALTINT        :1;
    unsigned               :2;
    unsigned FAR_LOOPBACK  :1;
    unsigned               :3;
    unsigned EDPWRDOWN     :1;
    unsigned               :2;
  };
  struct {
    unsigned short w       :16;
  };
} __MODECTRLbits_t; // reg 17: PHY_REG_MODE_CTRL
#define _MODECTRL_ENERGYON_MASK     0x0002
#define _MODECTRL_ALTINT_MASK       0x0040
#define _MODECTRL_FAR_LOOPBACK_MASK 0x0200
#define _MODECTRL_EDPWRDOWN_MASK    0x2000


typedef union {
  struct {
    unsigned PHYAD    :5;
    unsigned MODE     :3;
    unsigned          :6;
    unsigned          :1;
    unsigned          :1;
  };
  struct {
    unsigned short w  :16;
  };
} __SPECIALMODEbits_t;  // reg 18: PHY_REG_SPECIAL_MODE
#define _SPECIALMODE_PHYAD_MASK     0x001f
#define _SPECIALMODE_MODE_MASK      0x00e0




typedef union {
  struct {
    unsigned Sym_Err_Cnt:16;
  };
  struct {
    unsigned short w:16;
  };
} __SYMBOLERRCNTbits_t; // reg 26: PHY_REG_SYMBOL_ERR_CNT


typedef union {
  struct {
    unsigned            :4;
    unsigned XPOL       :1;
    unsigned            :6;
    unsigned            :2;
    unsigned CH_SELECT  :1;
    unsigned            :1;
    unsigned AMDIXCTRL  :1;
  };
  struct {
    unsigned short w    :16;
  };
} __SPECIALCTRLbits_t;  // reg 27: PHY_REG_SPECIAL_CTRL
#define _SPECIALCTRL_XPOL_MASK      0x0010
#define _SPECIALCTRL_CH_SELECT_MASK 0x2000
#define _SPECIALCTRL_AMDIXCTRL_MASK 0x8000




typedef union {
  struct {
    unsigned          :1;
    unsigned INT1     :1;
    unsigned INT2     :1;
    unsigned INT3     :1;
    unsigned INT4     :1;
    unsigned INT5     :1;
    unsigned INT6     :1;
    unsigned INT7     :1;
    unsigned          :8;
  };
  struct {
    unsigned short w  :16;
  };
} __INTSOURCEbits_t;    // reg 29: PHY_REG_INT_SOURCE

typedef union {
  struct {
    unsigned          :1;
    unsigned INT1     :1;
    unsigned INT2     :1;
    unsigned INT3     :1;
    unsigned INT4     :1;
    unsigned INT5     :1;
    unsigned INT6     :1;
    unsigned INT7     :1;
    unsigned          :8;
  };
  struct {
    unsigned short w  :16;
  };
} __INTMASKbits_t;  // reg 30: PHY_REG_INT_MASK


typedef union {
  struct {
    unsigned           :2;
    unsigned SPEED     :3;   // 1: 10MbpsHD; 5:10MbpsFD; 2: 100MbpsHD; 6: 100MbpsFD;
    unsigned           :1;
    unsigned           :1;  // ENABLE_4B5B: should be always set
    unsigned           :3;        // GPO: N/A
    unsigned           :2;
    unsigned AUTODONE  :1;
    unsigned           :3;
  };
  struct {
    unsigned short w   :16;
  };
} __PHYCTRLbits_t;  // reg 31: PHY_REG_PHY_CTRL
#define _PHYCTRL_SPEED_MASK         0x001c
#define _PHYCTRL_SPEED_FDUPLX_MASK  0x0010
#define _PHYCTRL_SPEED_100_10_MASK  0x000c
#define _PHYCTRL_SPEED_100_MASK     0x0008
#define _PHYCTRL_AUTODONE_MASK      0x1000



#endif  // _SMSC_8720_H_

