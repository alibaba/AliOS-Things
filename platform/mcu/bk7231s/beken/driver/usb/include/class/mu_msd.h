/*****************************************************************************
 *                                                                           *
 *      Copyright Mentor Graphics Corporation 2006                           *
 *                                                                           *
 *                All Rights Reserved.                                       *
 *                                                                           *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION            *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS              *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                               *
 *                                                                           *
 ****************************************************************************/

/*
 * MUSB-MicroSW mass-storage definitions.
 * $Revision: 1.5 $
 */

#ifndef __MUSB_MSD_H__
#define __MUSB_MSD_H__

/* bInterfaceProtocol */
#define MGC_MSD_CBI_PROTOCOL           0x00
#define MGC_MSD_CB_PROTOCOL            0x01
#define MGC_MSD_BOT_PROTOCOL           0x50

/* bInterfaceSubClass */
#define MGC_MSD_RBC_SUBCLASS           0x01
#define MGC_MSD_ATAPI_SUBCLASS         0x02
#define MGC_MSD_QIC_157_SUBCLASS       0x03
#define MGC_MSD_UFI_SUBCLASS           0x04
#define MGC_MSD_SFF_8070i_SUBCLASS     0x05
#define MGC_MSD_SCSI_SUBCLASS          0x06

#endif	/* multiple inclusion protection */
