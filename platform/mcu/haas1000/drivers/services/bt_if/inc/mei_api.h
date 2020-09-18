/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

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
