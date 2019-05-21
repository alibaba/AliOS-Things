/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2006              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/

/*
 * MUSB-MicroSW Bulk-Only Transport (BOT) definitions.
 * $Revision: 1.7 $
 */

#ifndef __MUSB_MSD_BOT_H__
#define __MUSB_MSD_BOT_H__

#include "mu_tools.h"

/* BOT Protocol */
#define MGC_MSD_BOT_CBW_SIGNATURE      0x43425355
#define MGC_MSD_BOT_CSW_SIGNATURE      0x53425355
#define MGC_MSD_BOT_CBW_LENGTH          31
#define MGC_MSD_BOT_CSW_LENGTH          13
#define MGC_MSD_BOT_LUN_MASK            0xf0

/* Mass Storage Device BOT Command Execution State */
#define MGC_MSD_BOT_COMMAND_PASSED      0x00
#define MGC_MSD_BOT_COMMAND_FAILED      0x01
#define MGC_MSD_BOT_PHASE_ERROR         0x02

/* Mass Storage Device BOT States */
#define MGC_MSD_BOT_STATE_IDLE              0x00
#define MGC_MSD_BOT_STATE_DATA_IN           0x01
#define MGC_MSD_BOT_STATE_DATA_OUT          0x02
#define MGC_MSD_BOT_STATE_CSW               0x03
#define MGC_MSD_BOT_STATE_CBW               0x04
#define MGC_MSD_BOT_STATE_INIT              0x05

/* MSD BOT Functional Characteristics */
#define MGC_MSD_BOT_RESET               0xFF
#define MGC_MSD_BOT_GET_MAXLUN          0xFE

/* MSD BOT Host-Device Error Condtions */
#define MGC_MSD_BOT_CASEOK                       0
#define MGC_MSD_BOT_CASE_Hn_eq_Dn                1
#define MGC_MSD_BOT_CASE_Hn_lt_Di                2
#define MGC_MSD_BOT_CASE_Hn_lt_Do                3
#define MGC_MSD_BOT_CASE_Hi_gt_Dn                4
#define MGC_MSD_BOT_CASE_Hi_gt_Di                5
#define MGC_MSD_BOT_CASE_Hi_eq_Di                6
#define MGC_MSD_BOT_CASE_Hi_lt_Di                7
#define MGC_MSD_BOT_CASE_Hi_ne_Do                8
#define MGC_MSD_BOT_CASE_Ho_gt_Dn                9
#define MGC_MSD_BOT_CASE_Ho_ne_Di               10
#define MGC_MSD_BOT_CASE_Ho_gt_Do               11
#define MGC_MSD_BOT_CASE_Ho_eq_Do               12
#define MGC_MSD_BOT_CASE_Ho_lt_Do               13
#define MGC_MSD_BOT_CASECBW                     14
#define MGC_MSD_BOT_CASE_ERROR                  15

/* Transfer Buffer Length for the Client : This value should not be
 * greater than (64*1024-1) i.e. max value should fit in 16 Bits
 */
#define MGC_MSD_BOT_XFER_BUF_LEN                (16 * 1024)

/* Data Transfer Length for different commands */
#define MGC_MSD_BOT_FMT_CACP_LEN        12 /* Format Capacity */
#define MGC_MSD_BOT_MODE_SEN_LEN        4  /* Mode Sense */
#define MGC_MSD_BOT_INQ_DATA_LEN        36 /* Inquiry */
#define MGC_MSD_BOT_REQ_SENS_LEN        18 /* Request Sense */
#define MGC_MSD_BOT_RD_CAPCY_LEN        8  /* Read Capacity */

/* Total Number of Commands Supported */
#define MGC_MSD_BOT_SUPPORTED_COMMANDS 15

#include "mu_pkon.h"

/**
 * MGC_MsdCbw.
 * BOT Command Block Wrapper.
 * @field dCbwSignature must be MGC_MSD_BOT_CBW_SIGNATURE
 * @field dCbwTag tag created by host, echoed in CSW
 * @field dCbwDataTransferLength data transfer length (0 is valid)
 * @field bmCbwFlags bit 7 is USB direction
 * @field bCbwLun Logical Unit Number
 * @field bCbwCbLength length of embedded command block
 * @field aCbwCb embedded command block
 */
typedef struct
{
    uint32_t dCbwSignature;
    uint32_t dCbwTag;
    uint32_t dCbwDataTransferLength;
    uint8_t bmCbwFlags;
    uint8_t bCbwLun;
    uint8_t bCbwCbLength;
    uint8_t aCbwCb[16];
} MGC_MsdCbw;
#include "mu_pkoff.h"

/* Flags: */
#define MGC_MSD_BOT_DIR_OUT 0
#define MGC_MSD_BOT_DIR_IN  0x80

#include "mu_pkon.h"

/**
 * MGC_MsdCsw.
 * BOT Command Status Wrapper.
 * @field dCswSignature must be MGC_MSD_BOT_CSW_SIGNATURE
 * @field dCswTag echo of that from CBW
 * @field dCswDataResidue transfer length minus amount processed/sent
 * @field bCswStatus status code
 */
typedef struct
{
    uint32_t dCswSignature;
    uint32_t dCswTag;
    uint32_t dCswDataResidue;
    uint8_t bCswStatus;
} MGC_MsdCsw;
#include "mu_pkoff.h"

#endif	/* multiple inclusion protection */
