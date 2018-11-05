/*******************************************************************************
  External Phy register definition file

  Company:
    Microchip Technology Inc.
    
  FIle Name:
    drv_extphy_regs.h

  Summary:
    External Phy register definitions

  Description:
    This file provides the External Phy register definition.
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

#ifndef _DRV_EXTPHY_REGS_H_
#define _DRV_EXTPHY_REGS_H_

// This file contains common definitions (accross all PHY's)
//


// MIIM registers access
//

typedef enum
{
    // basic registers, accross all registers: 0-1
    PHY_REG_BMCON       = 0,  // Basic:    Control Register
    PHY_REG_BMSTAT      = 1,  // Basic:    Status Register
    PHY_REG_PHYID1      = 2,  // Extended: PHY Identifier 1
    PHY_REG_PHYID2      = 3,  // Extended: PHY Identifier 2
    PHY_REG_ANAD        = 4,  // Extended: Auto-Negotiation Advertisement Register
    PHY_REG_ANLPAD      = 5,  // Extended: Auto-Negotiation Link Partner Advertisement
    PHY_REG_ANLPADNP    = 5,  // Extended: Auto-Negotiation Link Partner Advertisement, Next Page (?)
    PHY_REG_ANEXP       = 6,  // Extended: Auto-Negotiation Expansion
    PHY_REG_ANNPTR      = 7,  // Extended: Auto-Negotiation Next Page Transmit
    PHY_REG_ANLPRNP     = 8,  // Extended: Auto-Negotiation Link Partner Received Next Page

    // specific vendor registers: 16-31
    PHY_REG_VENDOR  = 16,   // this is updated by each specific PHY
    //
    //
    //
    PHY_REGISTERS       = 32    // total number of registers
} ePHY_BASIC_REG;

// MIIM registers definitions
//

// basic registers
//

typedef union {
  struct {
    unsigned             :7;
    unsigned COLTEST     :1;
    unsigned DUPLEX      :1;
    unsigned AN_RESTART  :1;
    unsigned ISOLATE     :1;
    unsigned PDWN        :1;
    unsigned AN_ENABLE   :1;
    unsigned SPEED       :1;
    unsigned LOOPBACK    :1;
    unsigned RESET       :1;
  };
  struct {
    unsigned short w:16;
  };
} __BMCONbits_t;    // reg 0: PHY_REG_BMCON

#define _BMCON_COLTEST_MASK     0x0080
#define _BMCON_DUPLEX_MASK      0x0100
#define _BMCON_AN_RESTART_MASK  0x0200
#define _BMCON_ISOLATE_MASK     0x0400
#define _BMCON_PDWN_MASK        0x0800
#define _BMCON_AN_ENABLE_MASK   0x1000
#define _BMCON_SPEED_MASK       0x2000
#define _BMCON_LOOPBACK_MASK    0x4000
#define _BMCON_RESET_MASK       0x8000



typedef union {
  struct {
    unsigned EXTEND_ABLE        :1;
    unsigned JABBER_DET         :1;
    unsigned LINK_STAT          :1;
    unsigned AN_ABLE            :1;
    unsigned REM_FAULT          :1;
    unsigned AN_COMPLETE        :1;
    unsigned PREAMBLE_SUPPRESS  :1;
    unsigned                    :4;
    unsigned BASE10T_HDX        :1;
    unsigned BASE10T_FDX        :1;
    unsigned BASE100TX_HDX      :1;
    unsigned BASE100TX_FDX      :1;
    unsigned BASE100T4          :1;
  };
  struct {
    unsigned short w:16;
  };
} __BMSTATbits_t;   // reg 1: PHY_REG_BMSTAT

#define _BMSTAT_EXTEND_ABLE_MASK    0x0001
#define _BMSTAT_JABBER_DET_MASK     0x0002
#define _BMSTAT_LINK_STAT_MASK      0x0004
#define _BMSTAT_AN_ABLE_MASK        0x0008
#define _BMSTAT_REM_FAULT_MASK      0x0010
#define _BMSTAT_AN_COMPLETE_MASK    0x0020
#define _BMSTAT_PREAMBLE_SUPPRESS_MASK \
                                    0x0040
#define _BMSTAT_BASE10T_HDX_MASK    0x0800
#define _BMSTAT_BASE10T_FDX_MASK    0x1000
#define _BMSTAT_BASE100TX_HDX_MASK  0x2000
#define _BMSTAT_BASE100TX_FDX_MASK  0x4000
#define _BMSTAT_BASE100T4_MASK      0x8000


#define _BMSTAT_NEGOTIATION_MASK    (_BMSTAT_BASE10T_HDX_MASK|_BMSTAT_BASE10T_FDX_MASK| \
                    _BMSTAT_BASE100TX_HDX_MASK|_BMSTAT_BASE100TX_FDX_MASK|_BMSTAT_BASE100T4_MASK)       // negotiation field mask
#define _BMSTAT_NEGOTIATION_POS     11      // negotiation field position
#define _BMSTAT_NEGOTIATION_LENGTH  5       // negotiation field length


// extended registers
//

typedef union {
  struct {
    unsigned OUI_MSB  :16;
  };
  struct {
    unsigned short w  :16;
  };
} __PHYID1bits_t;   // reg 2: PHY_REG_PHYID1


typedef union {
  struct {
    unsigned MNF_REV    :4;
    unsigned MNF_MODEL  :6;
    unsigned OUI_LSB    :6;
  };
  struct {
    unsigned short w   :16;
  };
} __PHYID2bits_t;   // reg 3: PHY_REG_PHYID2

#define _PHYID2_MNF_REV_MASK    0x000F
#define _PHYID2_MNF_MODEL_MASK  0x03F0
#define _PHYID2_OUI_LSB_MASK    0xFC00


typedef union {
  struct {
    unsigned PROT_SEL       :5;
    unsigned BASE10T        :1;
    unsigned BASE10T_FDX    :1;
    unsigned BASE100TX      :1;
    unsigned BASE100TX_FDX  :1;
    unsigned BASE100T4      :1;
    unsigned PAUSE          :1; // NOTE: the PAUSE fields coding for SMSC is reversed!
    unsigned ASM_DIR        :1; // typo in the data sheet?
    unsigned                :1;
    unsigned REM_FAULT      :1;
    unsigned                :1;
    unsigned NP_ABLE        :1;
  };
  struct {
    unsigned short w       :16;
  };
} __ANADbits_t;     // reg 4: PHY_REG_ANAD

#define _ANAD_PROT_SEL_MASK       0x001F
#define _ANAD_BASE10T_MASK        0x0020
#define _ANAD_BASE10T_FDX_MASK    0x0040
#define _ANAD_BASE100TX_MASK      0x0080
#define _ANAD_BASE100TX_FDX_MASK  0x0100
#define _ANAD_BASE100T4_MASK      0x0200
#define _ANAD_PAUSE_MASK          0x0400
#define _ANAD_ASM_DIR_MASK        0x0800
#define _ANAD_REM_FAULT_MASK      0x2000
#define _ANAD_NP_ABLE_MASK        0x8000

#define _ANAD_NEGOTIATION_MASK      (_ANAD_BASE10T_MASK|_ANAD_BASE10T_FDX_MASK|_ANAD_BASE100TX_MASK|_ANAD_BASE100TX_FDX_MASK| \
                    _ANAD_BASE100T4_MASK)   // negotiation field mask
#define _ANAD_NEGOTIATION_POS       5       // negotiation field position
#define _ANAD_NEGOTIATION_LENGTH    5       // negotiation field length


typedef union {
  struct {
    unsigned PROT_SEL       :5;
    unsigned BASE10T        :1;
    unsigned BASE10T_FDX    :1;
    unsigned BASE100TX      :1;
    unsigned BASE100TX_FDX  :1;
    unsigned BASE100T4      :1;
    unsigned PAUSE          :1;
    unsigned ASM_DIR        :1;
    unsigned                :1;
    unsigned REM_FAULT      :1;
    unsigned ACK            :1;
    unsigned NP_ABLE        :1;
  };
  struct {
    unsigned short w       :16;
  };
} __ANLPADbits_t;   // reg 5: PHY_REG_ANLPAD

#define _ANLPAD_PROT_SEL_MASK       0x001F
#define _ANLPAD_BASE10T_MASK        0x0020
#define _ANLPAD_BASE10T_FDX_MASK    0x0040
#define _ANLPAD_BASE100TX_MASK      0x0080
#define _ANLPAD_BASE100TX_FDX_MASK  0x0100
#define _ANLPAD_BASE100T4_MASK      0x0200
#define _ANLPAD_PAUSE_MASK          0x0400
#define _ANLPAD_ASM_DIR_MASK        0x0800
#define _ANLPAD_REM_FAULT_MASK      0x2000
#define _ANLPAD_ACK_MASK            0x4000
#define _ANLPAD_NP_ABLE_MASK        0x8000

typedef union {
  struct {    
    unsigned MESSAGE:11;
    unsigned TOGGLE:1;		
    unsigned ACK2:1;
    unsigned MSGP:1;
    unsigned ACK:1;
    unsigned NP:1;
  };
  struct {
    unsigned short w:16;
  };
} __ANLPADNPbits_t;	// reg 5: PHY_REG_ANLPADNP: next page

#define _ANLPADNP_MESSAGE_MASK  0x07FF
#define _ANLPADNP_TOGGLE_MASK   0x0800
#define _ANLPADNP_ACK2_MASK     0x1000
#define _ANLPADNP_MSGP_MASK     0x2000
#define _ANLPADNP_ACK_MASK      0x4000
#define _ANLPADNP_NP_MASK       0x8000


typedef union {
  struct {
    unsigned LP_AN_ABLE  :1;
    unsigned PAGE_RX     :1;
    unsigned NP_ABLE     :1;
    unsigned LP_NP_ABLE  :1;
    unsigned PDF         :1;
    unsigned             :11;
  };
  struct {
    unsigned short w    :16;
  };
} __ANEXPbits_t;    // reg 6: PHY_REG_ANEXP

#define _ANEXP_LP_AN_ABLE_MASK  0x0001
#define _ANEXP_PAGE_RX_MASK     0x0002
#define _ANEXP_NP_ABLE_MASK     0x0004
#define _ANEXP_LP_NP_ABLE_MASK  0x0008
#define _ANEXP_PDF_MASK         0x0010




typedef union {
  struct {
    unsigned MESSAGE  :11;
    unsigned TOGGLE   :1;
    unsigned ACK2     :1;
    unsigned MSGP     :1;
    unsigned          :1;
    unsigned NP       :1;
  };
  struct {
    unsigned short w:16;
  };
} __ANNPTRbits_t;   // reg 7: PHY_REG_ANNPTR

#define _ANNPTR_MESSAGE_MASK  0x07FF
#define _ANNPTR_TOGGLE_MASK   0x0800
#define _ANNPTR_ACK2_MASK     0x1000
#define _ANNPTR_MSGP_MASK     0x2000
#define _ANNPTR_NP_MASK       0x8000

typedef union {
  struct {
    unsigned MESSAGE  :11;
    unsigned TOGGLE   :1;
    unsigned ACK2     :1;
    unsigned MSGP     :1;
    unsigned ACK      :1;
    unsigned NP       :1;
  };
  struct {
    unsigned short w:16;
  };
} __ANLPRNPbits_t;  // reg 8: PHY_REG_ANLPRNP

#define _ANLPRNP_MESSAGE_MASK  0x07FF
#define _ANLPRNP_TOGGLE_MASK   0x0800
#define _ANLPRNP_ACK2_MASK     0x1000
#define _ANLPRNP_MSGP_MASK     0x2000
#define _ANLPRNP_ACK_MASK      0x4000
#define _ANLPRNP_NP_MASK       0x8000


#endif  // _DRV_EXTPHY_REGS_H_
