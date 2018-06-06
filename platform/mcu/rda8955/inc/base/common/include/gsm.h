/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/


#ifndef __GSM_H__
#define __GSM_H__


// GSM burst length (number of symbols)
#define GSM_BURST_LEN        157
// Burst length definitions in Qs
#define GSM_SLOT_QS_DURATION 625
#define GSM_ABURST_ACTIVE_QS_DURATION (88 * 4)
#define GSM_NBURST_ACTIVE_QS_DURATION (148 * 4)

// Windows type definition
typedef enum
{
    GSM_WIN_TYPE_MONIT = 1, ///< Type for Monitoring Window
    GSM_WIN_TYPE_FCCH = 2, ///< Type for FCCH Window
    GSM_WIN_TYPE_SCH = 3, ///< Type for SCH Window
    GSM_WIN_TYPE_NBURST = 4, ///< Type for Normal Burst Window
    GSM_WIN_TYPE_INTRF = 5, ///< Type for Interference Window
    GSM_WIN_TYPE_TXABURST = 6, ///< Type for Access Burst Tx Window
    GSM_WIN_TYPE_TXNBURST = 7, ///< Type for Normal Burst Tx Window
    GSM_WIN_TYPE_MONIT_EXT = 8 ///< Type for Monitor extra windows
} GSM_WIN_TYPE_T;

// Coding Schemes
typedef enum
{
    GSM_CS1=0,
    GSM_CS2=1,
    GSM_CS3=2,
    GSM_CS4=3,
    EGPRS_MCS1=4,
    EGPRS_MCS2=5,
    EGPRS_MCS3=6,
    EGPRS_MCS4=7,
    EGPRS_MCS5=8,
    EGPRS_MCS6=9,
    EGPRS_MCS7=10,
    EGPRS_MCS8=11,
    EGPRS_MCS9=12
} GSM_CS_T;

// Band definition
typedef enum
{
    GSM_BAND_GSM850 =  0,
    GSM_BAND_GSM900 =  1,
    GSM_BAND_DCS1800 = 2,
    GSM_BAND_PCS1900 = 3,
    GSM_BAND_QTY
} GSM_RFBAND_T;


typedef enum
{
    PSK8          = 0x00000000,
    GMSK          = 0x00000001
} GSM_MODULATION_T;


//Fch mode: according to the one in spc
/*typedef enum
{
/// Original Delay2 Correlation Algo
    FCCH_SEARCH_ALGO_DELAY2                     = 0x00000000,
/// New Delay4 Correlation Algo
    FCCH_SEARCH_ALGO_DELAY4                     = 0x00000001,
/// New Delay4 Correlation Algo
    FCCH_SEARCH_ALGO_FILTER                     = 0x00000002
} SPC_FCCH_SEARCH_MODE_T;
*/
typedef enum
{
/// Original Delay2 Correlation Algo
    GSM_FCCH_SEARCH_ALGO_DELAY2                     = 0x00000000,
/// New Delay4 Correlation Algo
    GSM_FCCH_SEARCH_ALGO_DELAY4                     = 0x00000001,
/// New Delay4 Correlation Algo
    GSM_FCCH_SEARCH_ALGO_FILTER                     = 0x00000002
} GSM_FCCH_SEARCH_MODE_T;

//--------------------------------------------------------------------//
//                                                                    //
//                       ARFCN limits                                 //
//                                                                    //
//--------------------------------------------------------------------//
// GSM
#define GSM_ARFCN_GSM850_MIN         128
#define GSM_ARFCN_GSM850_MAX         251
// EGSM RX
#define GSM_ARFCN_EGSM_RX_MIN1       0
#define GSM_ARFCN_EGSM_RX_MAX1       124
#define GSM_ARFCN_EGSM_RX_MIN2       975
#define GSM_ARFCN_EGSM_RX_MAX2       1023
// EGSM TX
#define GSM_ARFCN_EGSM_TX_MIN1_1     0
#define GSM_ARFCN_EGSM_TX_MAX1_1     24
#define GSM_ARFCN_EGSM_TX_MIN1_2     50
#define GSM_ARFCN_EGSM_TX_MAX1_2     124
#define GSM_ARFCN_EGSM_TX_MIN2       25
#define GSM_ARFCN_EGSM_TX_MAX2       49
#define GSM_ARFCN_EGSM_TX_MIN3       975
#define GSM_ARFCN_EGSM_TX_MAX3       1023
// DCS
#define GSM_ARFCN_DCS_MIN            512
#define GSM_ARFCN_DCS_MAX            885
// PCS
#define GSM_ARFCN_PCS_MIN            512
#define GSM_ARFCN_PCS_MAX            810


#endif //__GSM_H__
