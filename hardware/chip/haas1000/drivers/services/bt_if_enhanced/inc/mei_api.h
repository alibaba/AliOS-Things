/****************************************************************************
 *
 * File:
 *     $Id: mei_api.h 886 2009-02-24 14:40:12Z brentk $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 886 $
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     upper layer of the Management Entity.
 *
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of BES.
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __MEI_API__H__
#define __MEI_API__H__
 
/*---------------------------------------------------------------------------
 *
 * Authenticate State of a Remote Device (BtAuthState). The typedef is in 
 * mei_api.h
 */
#define BTIF_BAS_NOT_AUTHENTICATED  0x00
#define  BTIF_BAS_START_AUTHENTICATE 0x01
#define  BTIF_BAS_WAITING_KEY_REQ    0x02
#define  BTIF_BAS_SENDING_KEY        0x03
#define  BTIF_BAS_WAITING_FOR_IO     0x04
#define  BTIF_BAS_WAITING_FOR_IO_R   0x05
#define  BTIF_BAS_WAITING_FOR_KEY    0x06
#define  BTIF_BAS_WAITING_FOR_KEY_R  0x07
#define  BTIF_BAS_AUTHENTICATED      0x08
    
#define BTIF_DBG_SNIFFER_SCO_STOP  0
#define BTIF_DBG_SNIFFER_SCO_START  1
#define BTIF_DBG_SNIFFER_ACL_SWOTCH  2
#define BTIF_DBG_SEND_MAX_SLOT_REQ  3
    
#endif /* __ME_H */
