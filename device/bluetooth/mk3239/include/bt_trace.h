
#ifndef BT_TRACE_H
#define BT_TRACE_H

#include "data_types.h"

#ifndef BTTRC_INCLUDED
#define BTTRC_INCLUDED  FALSE
#endif
#ifndef BTTRC_PARSER_INCLUDED
#define BTTRC_PARSER_INCLUDED FALSE
#endif
#ifndef MAX_TRACE_RAM_SIZE
#define MAX_TRACE_RAM_SIZE 10000
#endif

/* BTE tracing IDs for debug purposes */
/* LayerIDs for stack */
#define BTTRC_ID_STK_GKI                   1
#define BTTRC_ID_STK_BTU                   2
#define BTTRC_ID_STK_HCI                   3
#define BTTRC_ID_STK_L2CAP                 4
#define BTTRC_ID_STK_RFCM_MX               5
#define BTTRC_ID_STK_RFCM_PRT              6
#define BTTRC_ID_STK_OBEX_C                7
#define BTTRC_ID_STK_OBEX_S                8
#define BTTRC_ID_STK_AVCT                  9
#define BTTRC_ID_STK_AVDT                  10
#define BTTRC_ID_STK_AVRC                  11
#define BTTRC_ID_STK_BIC                   12
#define BTTRC_ID_STK_BIS                   13
#define BTTRC_ID_STK_BNEP                  14
#define BTTRC_ID_STK_BPP                   15
#define BTTRC_ID_STK_BTM_ACL               16
#define BTTRC_ID_STK_BTM_PM                17
#define BTTRC_ID_STK_BTM_DEV_CTRL          18
#define BTTRC_ID_STK_BTM_SVC_DSC           19
#define BTTRC_ID_STK_BTM_INQ               20
#define BTTRC_ID_STK_BTM_SCO               21
#define BTTRC_ID_STK_BTM_SEC               22
#define BTTRC_ID_STK_DUN                   23
#define BTTRC_ID_STK_HID                   24
#define BTTRC_ID_STK_HSP2                  25
#define BTTRC_ID_STK_FTC                   26
#define BTTRC_ID_STK_FTS                   27
#define BTTRC_ID_STK_GAP                   28
#define BTTRC_ID_STK_GOEP                  29
#define BTTRC_ID_STK_OPC                   30
#define BTTRC_ID_STK_OPS                   31
#define BTTRC_ID_STK_PAN                   32
#define BTTRC_ID_STK_SAP                   33
#define BTTRC_ID_STK_SDP                   34
#define BTTRC_ID_STK_SLIP                  35
#define BTTRC_ID_STK_SPP                   36
#define BTTRC_ID_STK_VDP                   37
#define BTTRC_ID_STK_MCAP                  38
#define BTTRC_ID_STK_GATT                  39
#define BTTRC_ID_STK_SMP                   40
#define BTTRC_ID_STK_NFC                   41
#define BTTRC_ID_STK_NCI                   42
#define BTTRC_ID_STK_IDEP                  43
#define BTTRC_ID_STK_NDEP                  44
#define BTTRC_ID_STK_LLCP                  45
#define BTTRC_ID_STK_RW                    46
#define BTTRC_ID_STK_CE                    47
#define BTTRC_ID_STK_SNEP                  48
#define BTTRC_ID_STK_NDEF                  49


/* LayerIDs for BTA */
#define BTTRC_ID_BTA_ACC                   50         /* Advanced Camera Client */
#define BTTRC_ID_BTA_AG                    51         /* audio gateway */
#define BTTRC_ID_BTA_AV                    52         /* Advanced audio */
#define BTTRC_ID_BTA_BIC                   53         /* Basic Imaging Client */
#define BTTRC_ID_BTA_BIS                   54         /* Basic Imaging Server */
#define BTTRC_ID_BTA_BP                    55         /* Basic Printing Client */
#define BTTRC_ID_BTA_DG                    56         /* data gateway */
#define BTTRC_ID_BTA_DM                    57         /* device manager */
#define BTTRC_ID_BTA_DM_SRCH               58         /* device manager search */
#define BTTRC_ID_BTA_DM_SEC                59         /* device manager security */
#define BTTRC_ID_BTA_FM                    60
#define BTTRC_ID_BTA_FTC                   61         /* file transfer client */
#define BTTRC_ID_BTA_FTS                   62         /* file transfer server */
#define BTTRC_ID_BTA_HIDH                  63
#define BTTRC_ID_BTA_HIDD                  64
#define BTTRC_ID_BTA_JV                    65
#define BTTRC_ID_BTA_OPC                   66         /* object push client */
#define BTTRC_ID_BTA_OPS                   67         /* object push server */
#define BTTRC_ID_BTA_PAN                   68         /* Personal Area Networking */
#define BTTRC_ID_BTA_PR                    69         /* Printer client */
#define BTTRC_ID_BTA_SC                    70         /* SIM Card Access server */
#define BTTRC_ID_BTA_SS                    71         /* synchronization server */
#define BTTRC_ID_BTA_SYS                   72         /* system manager */
#define BTTRC_ID_AVDT_SCB                  73         /* avdt scb */
#define BTTRC_ID_AVDT_CCB                  74         /* avdt ccb */

/* LayerIDs for BT APP */
#define BTTRC_ID_BTAPP                     75
#define BTTRC_ID_MAX_ID                    BTTRC_ID_BTAPP
#define BTTRC_ID_ALL_LAYERS                0xFF       /* all trace layers */
typedef UINT8 tBTTRC_LAYER_ID;

/* Trace type definitions. Note that these are mutually exclusive in a trace. This
means that any trace can be either error,warning,api,event or dbg */
#if (BTU_STACK_LITE_ENABLED == TRUE)
#define BTTRC_TYPE_ERROR                   0x81
#define BTTRC_TYPE_WARNING                 0x82
#define BTTRC_TYPE_API                     0x84
#define BTTRC_TYPE_EVENT                   0x88
#define BTTRC_TYPE_ACTION                  0x90
#define BTTRC_TYPE_DBG                     0xA0
#else
#define BTTRC_TYPE_ERROR                   0x01       /* Traces for error situation */
#define BTTRC_TYPE_WARNING                 0x02       /* Traces for warning situation */
#define BTTRC_TYPE_API                     0x04       /* Traces for API */
#define BTTRC_TYPE_EVENT                   0x08       /* Traces for EVENT */
#define BTTRC_TYPE_ACTION                  0x10       /* Traces for Action functions */
#define BTTRC_TYPE_DBG                     0x20       /* Traces for debugging purpose */
#endif
typedef UINT8 tBTTRC_TYPE;

/* Masks to identify the stack that originated the trace */
#define BTTRC_TRACE_LITE                   0x80       /* MM Lite stack */
#define BTTRC_TRACE_EMBD                   0x40       /* Embedded host stack */

/* Parameter datatypes used in Trace APIs */
#define BTTRC_PARAM_UINT8                  1
#define BTTRC_PARAM_UINT16                 2
#define BTTRC_PARAM_UINT32                 3
typedef UINT8 tBTTRC_PARAM_TYPE;

/* Special token definitions */
#define BTTRC_TOKEN_SM_STATE               0xFFFF     /* Token indicating the State of a State m/c */


#ifdef __cplusplus
extern "C" {
#endif

/* External declaration for appl_trace_level here to avoid to add the declaration in all the files using APPL_TRACExxx macros */
extern UINT8 appl_trace_level ;

/* Prototype for message logging function. */
EXPORT_API extern void LogMsg (UINT32 trace_set_mask, const char *fmt_str, ...);
extern void LogMsg_0 (UINT32 trace_set_mask, const char *p_str);
extern void LogMsg_1 (UINT32 trace_set_mask, const char *fmt_str, UINTPTR p1);
extern void LogMsg_2 (UINT32 trace_set_mask, const char *fmt_str, UINTPTR p1, UINTPTR p2);
extern void LogMsg_3 (UINT32 trace_set_mask, const char *fmt_str, UINTPTR p1, UINTPTR p2,
                      UINTPTR p3);
extern void LogMsg_4 (UINT32 trace_set_mask, const char *fmt_str, UINTPTR p1, UINTPTR p2,
                      UINTPTR p3, UINTPTR p4);
extern void LogMsg_5 (UINT32 trace_set_mask, const char *fmt_str, UINTPTR p1, UINTPTR p2,
                      UINTPTR p3, UINTPTR p4, UINTPTR p5);
extern void LogMsg_6 (UINT32 trace_set_mask, const char *fmt_str, UINTPTR p1, UINTPTR p2,
                      UINTPTR p3, UINTPTR p4, UINTPTR p5, UINTPTR p6);

/* Prototype for stack tracing function. */
EXPORT_API extern void BTTRC_StackTrace0(tBTTRC_LAYER_ID layer_id,
                                         tBTTRC_TYPE type,
                                         UINT16 token);
EXPORT_API extern void BTTRC_StackTrace1(tBTTRC_LAYER_ID layer_id,
                                         tBTTRC_TYPE type,
                                         UINT16 token,
                                         tBTTRC_PARAM_TYPE p1_type, UINTPTR p1_val);
EXPORT_API extern void BTTRC_StackTrace2(tBTTRC_LAYER_ID layer_id,
                                         tBTTRC_TYPE type,
                                         UINT16 token,
                                         tBTTRC_PARAM_TYPE p1_type, UINTPTR p1_val,
                                         tBTTRC_PARAM_TYPE p2_type, UINTPTR p2_val);
EXPORT_API extern void BTTRC_StackTrace3(tBTTRC_LAYER_ID layer_id,
                                         tBTTRC_TYPE type,
                                         UINT16 token,
                                         tBTTRC_PARAM_TYPE p1_type, UINTPTR p1_val,
                                         tBTTRC_PARAM_TYPE p2_type, UINTPTR p2_val,
                                         tBTTRC_PARAM_TYPE p3_type, UINTPTR p3_val);
EXPORT_API extern void BTTRC_StackTrace4(tBTTRC_LAYER_ID layer_id,
                                         tBTTRC_TYPE type,
                                         UINT16 token,
                                         tBTTRC_PARAM_TYPE p1_type, UINTPTR p1_val,
                                         tBTTRC_PARAM_TYPE p2_type, UINTPTR p2_val,
                                         tBTTRC_PARAM_TYPE p3_type, UINTPTR p3_val,
                                         tBTTRC_PARAM_TYPE p4_type, UINTPTR p4_val);
EXPORT_API extern void BTTRC_StackTrace5(tBTTRC_LAYER_ID layer_id,
                                         tBTTRC_TYPE type,
                                         UINT16 token,
                                         tBTTRC_PARAM_TYPE p1_type, UINTPTR p1_val,
                                         tBTTRC_PARAM_TYPE p2_type, UINTPTR p2_val,
                                         tBTTRC_PARAM_TYPE p3_type, UINTPTR p3_val,
                                         tBTTRC_PARAM_TYPE p4_type, UINTPTR p4_val,
                                         tBTTRC_PARAM_TYPE p5_type, UINTPTR p5_val);
EXPORT_API extern void BTTRC_StackTrace6(tBTTRC_LAYER_ID layer_id,
                                         tBTTRC_TYPE type,
                                         UINT16 token,
                                         tBTTRC_PARAM_TYPE p1_type, UINTPTR p1_val,
                                         tBTTRC_PARAM_TYPE p2_type, UINTPTR p2_val,
                                         tBTTRC_PARAM_TYPE p3_type, UINTPTR p3_val,
                                         tBTTRC_PARAM_TYPE p4_type, UINTPTR p4_val,
                                         tBTTRC_PARAM_TYPE p5_type, UINTPTR p5_val,
                                         tBTTRC_PARAM_TYPE p6_type, UINTPTR p6_val);

#ifdef __cplusplus
}
#endif

/******************************************************************************
**
** Trace configurable parameters
**
******************************************************************************/

/* Enables or disables verbose trace information. */
#ifndef BT_TRACE_VERBOSE
#define BT_TRACE_VERBOSE    FALSE
#endif

/* Enables or disables all trace messages. */
#ifndef BT_USE_TRACES
#define BT_USE_TRACES       TRUE
#endif

/* Enables or disables protocol trace information. */
#ifndef BT_TRACE_PROTOCOL
#define BT_TRACE_PROTOCOL   FALSE
#endif

/******************************************************************************
**
** Trace Levels
**
** The following values may be used for different levels:
**      BT_TRACE_LEVEL_NONE    0        * No trace messages to be generated
**      BT_TRACE_LEVEL_ERROR   1        * Error condition trace messages
**      BT_TRACE_LEVEL_WARNING 2        * Warning condition trace messages
**      BT_TRACE_LEVEL_API     3        * API traces
**      BT_TRACE_LEVEL_EVENT   4        * Debug messages for events
**      BT_TRACE_LEVEL_DEBUG   5        * Debug messages (general)
******************************************************************************/

/* Core Stack default trace levels */
#ifndef HCI_INITIAL_TRACE_LEVEL
#define HCI_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_DEBUG
#endif

#ifndef BTM_INITIAL_TRACE_LEVEL
#define BTM_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_DEBUG
#endif

#ifndef L2CAP_INITIAL_TRACE_LEVEL
#define L2CAP_INITIAL_TRACE_LEVEL           BT_TRACE_LEVEL_DEBUG
#endif

#ifndef LLCP_INITIAL_TRACE_LEVEL
#define LLCP_INITIAL_TRACE_LEVEL            BT_TRACE_LEVEL_DEBUG
#endif

#ifndef RFCOMM_INITIAL_TRACE_LEVEL
#define RFCOMM_INITIAL_TRACE_LEVEL          BT_TRACE_LEVEL_DEBUG
#endif

#ifndef OBX_INITIAL_TRACE_LEVEL
#define OBX_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_DEBUG
#endif

#ifndef SDP_INITIAL_TRACE_LEVEL
#define SDP_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_DEBUG
#endif

/* Profile default trace levels */
#ifndef DUN_INITIAL_TRACE_LEVEL
#define DUN_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_DEBUG
#endif

#ifndef GAP_INITIAL_TRACE_LEVEL
#define GAP_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_DEBUG
#endif

#ifndef GOEP_INITIAL_TRACE_LEVEL
#define GOEP_INITIAL_TRACE_LEVEL            BT_TRACE_LEVEL_DEBUG
#endif

#ifndef HSP2_INITIAL_TRACE_LEVEL
#define HSP2_INITIAL_TRACE_LEVEL            BT_TRACE_LEVEL_DEBUG
#endif

#ifndef SPP_INITIAL_TRACE_LEVEL
#define SPP_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_DEBUG
#endif

#ifndef BPP_INITIAL_TRACE_LEVEL
#define BPP_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_DEBUG
#endif

#ifndef BIP_INITIAL_TRACE_LEVEL
#define BIP_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_DEBUG
#endif

#ifndef BNEP_INITIAL_TRACE_LEVEL
#define BNEP_INITIAL_TRACE_LEVEL            BT_TRACE_LEVEL_DEBUG
#endif

#ifndef PAN_INITIAL_TRACE_LEVEL
#define PAN_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_DEBUG
#endif

#ifndef SAP_INITIAL_TRACE_LEVEL
#define SAP_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_DEBUG
#endif

#ifndef FTP_INITIAL_TRACE_LEVEL
#define FTP_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_DEBUG
#endif

#ifndef OPP_INITIAL_TRACE_LEVEL
#define OPP_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_DEBUG
#endif

#ifndef HFP_INITIAL_TRACE_LEVEL
#define HFP_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_DEBUG
#endif

#ifndef A2D_INITIAL_TRACE_LEVEL
#define A2D_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_DEBUG
#endif

#ifndef VDP_INITIAL_TRACE_LEVEL
#define VDP_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_DEBUG
#endif

#ifndef AVDT_INITIAL_TRACE_LEVEL
#define AVDT_INITIAL_TRACE_LEVEL            BT_TRACE_LEVEL_DEBUG
#endif

#ifndef AVCT_INITIAL_TRACE_LEVEL
#define AVCT_INITIAL_TRACE_LEVEL            BT_TRACE_LEVEL_DEBUG
#endif

#ifndef AVRC_INITIAL_TRACE_LEVEL
#define AVRC_INITIAL_TRACE_LEVEL            BT_TRACE_LEVEL_DEBUG
#endif

#ifndef MCA_INITIAL_TRACE_LEVEL
#define MCA_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_DEBUG
#endif

#ifndef HID_INITIAL_TRACE_LEVEL
#define HID_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_DEBUG
#endif

/* Application and other default trace levels */
#ifndef RPC_INITIAL_TRACE_LEVEL
#define RPC_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_DEBUG
#endif

#ifndef APPL_INITIAL_TRACE_LEVEL
#define APPL_INITIAL_TRACE_LEVEL            BT_TRACE_LEVEL_DEBUG
#endif

#ifndef BT_TRACE_APPL
#define BT_TRACE_APPL                       BT_USE_TRACES
#endif

#ifndef NFC_INITIAL_TRACE_LEVEL
#define NFC_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_DEBUG
#endif

#ifndef GATT_INITIAL_TRACE_LEVEL
#define GATT_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_DEBUG
#endif

#ifndef SMP_INITIAL_TRACE_LEVEL
#define SMP_INITIAL_TRACE_LEVEL             BT_TRACE_LEVEL_DEBUG
#endif


#if (BTTRC_INCLUDED == TRUE)
/***************************************************************************************/
/* BTTRC MACROS */

#define BTTRC_EVENT(lid, event, state)   \
            {BTTRC_StackTrace1(lid, BTTRC_TYPE_EVENT, event, BTTRC_PARAM_UINT8, state);}
#define BTTRC_ACTION(lid, action)  \
            {BTTRC_StackTrace0(lid, BTTRC_TYPE_ACTION, action);}
#define BTTRC_STATE(lid, state)   \
            {BTTRC_StackTrace1(lid, BTTRC_TYPE_EVENT, BTTRC_TOKEN_SM_STATE, BTTRC_PARAM_UINT8, state);}

#define BTTRC_API0(lid, api)  \
            {BTTRC_StackTrace0(lid, BTTRC_TYPE_API, api);}
#define BTTRC_API1(lid, api, p1_t,p1_v)  \
            {BTTRC_StackTrace1(lid, BTTRC_TYPE_API, api, p1_t,p1_v);}
#define BTTRC_API2(lid, api, p1_t,p1_v,p2_t,p2_v)  \
            {BTTRC_StackTrace2(lid, BTTRC_TYPE_API, api, p1_t,p1_v,p2_t,p2_v);}
#define BTTRC_API3(lid, api, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v)  \
            {BTTRC_StackTrace3(lid, BTTRC_TYPE_API, api, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v);}
#define BTTRC_API4(lid, api, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v)  \
            {BTTRC_StackTrace4(lid, BTTRC_TYPE_API, api, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v);}
#define BTTRC_API5(lid, api, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v)  \
            {BTTRC_StackTrace5(lid, BTTRC_TYPE_API, api, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v);}
#define BTTRC_API6(lid, api, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v,p6_t,p6_v)  \
            {BTTRC_StackTrace6(lid, BTTRC_TYPE_API, api, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v,p6_t,p6_v);}


#define BTTRC_DBG0(lid, dbg)  \
            {BTTRC_StackTrace0(lid, BTTRC_TYPE_DBG, dbg);}
#define BTTRC_DBG1(lid, dbg, p1_t,p1_v)  \
            {BTTRC_StackTrace1(lid, BTTRC_TYPE_DBG, dbg, p1_t,p1_v);}
#define BTTRC_DBG2(lid, dbg, p1_t,p1_v,p2_t,p2_v)  \
            {BTTRC_StackTrace2(lid, BTTRC_TYPE_DBG, dbg, p1_t,p1_v,p2_t,p2_v);}
#define BTTRC_DBG3(lid, dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v)  \
            {BTTRC_StackTrace3(lid, BTTRC_TYPE_DBG, dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v);}
#define BTTRC_DBG4(lid, dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v)  \
            {BTTRC_StackTrace4(lid, BTTRC_TYPE_DBG, dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v);}
#define BTTRC_DBG5(lid, dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v)  \
            {BTTRC_StackTrace5(lid, BTTRC_TYPE_DBG, dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v);}
#define BTTRC_DBG6(lid, dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v,p6_t,p6_v)  \
            {BTTRC_StackTrace6(lid, BTTRC_TYPE_DBG, dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v,p6_t,p6_v);}

/***************************************************************************************/
/*AVDT MACROS */

#define BTTRC_AVDT_API0(api)   \
        BTTRC_API0(BTTRC_ID_STK_AVDT, api)
#define BTTRC_AVDT_API1(api, p1_t, p1_v) \
        BTTRC_API1(BTTRC_ID_STK_AVDT, api, p1_t, p1_v)
#define BTTRC_AVDT_API2(api, p1_t, p1_v, p2_t, p2_v) \
        BTTRC_API2(BTTRC_ID_STK_AVDT, api, p1_t, p1_v, p2_t, p2_v)
/***************************************************************************************/
/*AVDT_SCB MACROS */

#define BTTRC_AVDT_SCB_EVENT(event, state)   \
            BTTRC_EVENT(BTTRC_ID_AVDT_SCB, event, state)
#define BTTRC_AVDT_SCB_ACTION(action)  \
            BTTRC_ACTION(BTTRC_ID_AVDT_SCB, action)
#define BTTRC_AVDT_SCB_STATE(next_state)   \
            BTTRC_STATE(BTTRC_ID_AVDT_SCB, next_state)

#define BTTRC_AVDT_SCB_DBG0(dbg)  \
            BTTRC_DBG0(BTTRC_ID_AVDT_SCB, dbg)
#define BTTRC_AVDT_SCB_DBG1(dbg, p1_t,p1_v)  \
            BTTRC_DBG1(BTTRC_ID_AVDT_SCB, dbg, p1_t,p1_v)
#define BTTRC_AVDT_SCB_DBG2(dbg, p1_t,p1_v,p2_t,p2_v)  \
            BTTRC_DBG2(BTTRC_ID_AVDT_SCB, dbg, p1_t,p1_v,p2_t,p2_v)
#define BTTRC_AVDT_SCB_DBG3(dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v)  \
            BTTRC_DBG3(BTTRC_ID_AVDT_SCB, dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v)
#define BTTRC_AVDT_SCB_DBG4(dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v)  \
            BTTRC_DBG4(BTTRC_ID_AVDT_SCB, dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v)
#define BTTRC_AVDT_SCB_DBG5(dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v)  \
            BTTRC_DBG5(BTTRC_ID_AVDT_SCB, dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v)
#define BTTRC_AVDT_SCB_DBG6(dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v,p6_t,p6_v)  \
            BTTRC_DBG6(BTTRC_ID_AVDT_SCB, dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v,p6_t,p6_v)
/***************************************************************************************/
/*AVDT_CCB MACROS */

#define BTTRC_AVDT_CCB_EVENT(event, state)   \
            BTTRC_EVENT(BTTRC_ID_AVDT_CCB, event, state)
#define BTTRC_AVDT_CCB_ACTION(action)  \
            BTTRC_ACTION(BTTRC_ID_AVDT_CCB, action)
#define BTTRC_AVDT_CCB_STATE(next_state)   \
            BTTRC_STATE(BTTRC_ID_AVDT_CCB, next_state)

#define BTTRC_AVDT_CCB_DBG0(dbg)  \
            BTTRC_DBG0(BTTRC_ID_AVDT_CCB, dbg)
#define BTTRC_AVDT_CCB_DBG1(dbg, p1_t,p1_v)  \
            BTTRC_DBG1(BTTRC_ID_AVDT_CCB, dbg, p1_t,p1_v)
#define BTTRC_AVDT_CCB_DBG2(dbg, p1_t,p1_v,p2_t,p2_v)  \
            BTTRC_DBG2(BTTRC_ID_AVDT_CCB, dbg, p1_t,p1_v,p2_t,p2_v)
#define BTTRC_AVDT_CCB_DBG3(dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v)  \
            BTTRC_DBG3(BTTRC_ID_AVDT_CCB, dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v)
#define BTTRC_AVDT_CCB_DBG4(dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v)  \
            BTTRC_DBG4(BTTRC_ID_AVDT_CCB, dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v)
#define BTTRC_AVDT_CCB_DBG5(dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v)  \
            BTTRC_DBG5(BTTRC_ID_AVDT_CCB, dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v)
#define BTTRC_AVDT_CCB_DBG6(dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v,p6_t,p6_v)  \
            BTTRC_DBG6(BTTRC_ID_AVDT_CCB, dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v,p6_t,p6_v)
/***************************************************************************************/

#else /*BTTRC_INCLUDED*/

/***************************************************************************************/
/* BTTRC MACROS */

#define BTTRC_EVENT(lid, event, state)
#define BTTRC_ACTION(lid, action)
#define BTTRC_STATE(lid, state)

#define BTTRC_API0(lid, api)
#define BTTRC_API1(lid, api, p1_t, p1_v)
#define BTTRC_API2(lid, api, p1_t, p1_v, p2_t, p2_v)
#define BTTRC_API3(lid, api, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v)
#define BTTRC_API4(lid, api, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v)
#define BTTRC_API5(lid, api, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v)
#define BTTRC_API6(lid, api, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v,p6_t,p6_v)


#define BTTRC_DBG0(lid, dbg)
#define BTTRC_DBG1(lid, dbg, p1_t,p1_v)
#define BTTRC_DBG2(lid, dbg, p1_t,p1_v,p2_t,p2_v)
#define BTTRC_DBG3(lid, dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v)
#define BTTRC_DBG4(lid, dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v)
#define BTTRC_DBG5(lid, dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v)
#define BTTRC_DBG6(lid, dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v,p6_t,p6_v)

/***************************************************************************************/
/*AVDT MACROS */
#define BTTRC_AVDT_API0(api)
#define BTTRC_AVDT_API1(api, p1_t,p1_v)
#define BTTRC_AVDT_API2(api, p1_t,p1_v,p2_t,p2_v)
/***************************************************************************************/
/*AVDT_SCB MACROS */

#define BTTRC_AVDT_SCB_EVENT(event, state)
#define BTTRC_AVDT_SCB_ACTION(action)
#define BTTRC_AVDT_SCB_STATE(next_state)

#define BTTRC_AVDT_SCB_DBG0(dbg)
#define BTTRC_AVDT_SCB_DBG1(dbg, p1_t,p1_v)
#define BTTRC_AVDT_SCB_DBG2(dbg, p1_t,p1_v,p2_t,p2_v)
#define BTTRC_AVDT_SCB_DBG3(dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v)
#define BTTRC_AVDT_SCB_DBG4(dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v)
#define BTTRC_AVDT_SCB_DBG5(dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v)
#define BTTRC_AVDT_SCB_DBG6(dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v,p6_t,p6_v)

/***************************************************************************************/
/*AVDT_CCB MACROS */

#define BTTRC_AVDT_CCB_EVENT(event, state)
#define BTTRC_AVDT_CCB_ACTION(action)
#define BTTRC_AVDT_CCB_STATE(next_state)

#define BTTRC_AVDT_CCB_DBG0(dbg)
#define BTTRC_AVDT_CCB_DBG1(dbg, p1_t,p1_v)
#define BTTRC_AVDT_CCB_DBG2(dbg, p1_t,p1_v,p2_t,p2_v)
#define BTTRC_AVDT_CCB_DBG3(dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v)
#define BTTRC_AVDT_CCB_DBG4(dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v)
#define BTTRC_AVDT_CCB_DBG5(dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v)
#define BTTRC_AVDT_CCB_DBG6(dbg, p1_t,p1_v,p2_t,p2_v,p3_t,p3_v,p4_t,p4_v,p5_t,p5_v,p6_t,p6_v)

/***************************************************************************************/

#endif /*BTTRC_INCLUDED*/


#if (BT_USE_TRACES == TRUE)

#define BT_TRACE_0(l,t,m)                           LogMsg_0((TRACE_CTRL_GENERAL | (l) | TRACE_ORG_STACK | (t)),(m))
#define BT_TRACE_1(l,t,m,p1)                        LogMsg_1(TRACE_CTRL_GENERAL | (l) | TRACE_ORG_STACK | (t),(m),(UINTPTR)(p1))
#define BT_TRACE_2(l,t,m,p1,p2)                     LogMsg_2(TRACE_CTRL_GENERAL | (l) | TRACE_ORG_STACK | (t),(m),(UINTPTR)(p1),   \
                                                        (UINTPTR)(p2))
#define BT_TRACE_3(l,t,m,p1,p2,p3)                  LogMsg_3(TRACE_CTRL_GENERAL | (l) | TRACE_ORG_STACK | (t),(m),(UINTPTR)(p1),   \
                                                        (UINTPTR)(p2),(UINTPTR)(p3))
#define BT_TRACE_4(l,t,m,p1,p2,p3,p4)               LogMsg_4(TRACE_CTRL_GENERAL | (l) | TRACE_ORG_STACK | (t),(m),(UINTPTR)(p1),   \
                                                        (UINTPTR)(p2),(UINTPTR)(p3),(UINTPTR)(p4))
#define BT_TRACE_5(l,t,m,p1,p2,p3,p4,p5)            LogMsg_5(TRACE_CTRL_GENERAL | (l) | TRACE_ORG_STACK | (t),(m),(UINTPTR)(p1),   \
                                                        (UINTPTR)(p2),(UINTPTR)(p3),(UINTPTR)(p4), \
                                                        (UINTPTR)(p5))
#define BT_TRACE_6(l,t,m,p1,p2,p3,p4,p5,p6)         LogMsg_6(TRACE_CTRL_GENERAL | (l) | TRACE_ORG_STACK | (t),(m),(UINTPTR)(p1),   \
                                                        (UINTPTR)(p2),(UINTPTR)(p3),(UINTPTR)(p4), \
                                                        (UINTPTR)(p5),(UINTPTR)(p6))

#define BT_ERROR_TRACE_0(l,m)                     LogMsg_0(TRACE_CTRL_GENERAL | (l) | TRACE_ORG_STACK | TRACE_TYPE_ERROR,(m))
#define BT_ERROR_TRACE_1(l,m,p1)                  LogMsg_1(TRACE_CTRL_GENERAL | (l) | TRACE_ORG_STACK | TRACE_TYPE_ERROR,(m),(UINTPTR)(p1))
#define BT_ERROR_TRACE_2(l,m,p1,p2)               LogMsg_2(TRACE_CTRL_GENERAL | (l) | TRACE_ORG_STACK | TRACE_TYPE_ERROR,(m),(UINTPTR)(p1),(UINTPTR)(p2))
#define BT_ERROR_TRACE_3(l,m,p1,p2,p3)            LogMsg_3(TRACE_CTRL_GENERAL | (l) | TRACE_ORG_STACK | TRACE_TYPE_ERROR,(m),(UINTPTR)(p1),(UINTPTR)(p2),(UINTPTR)(p3))

/* Define tracing for the HCI unit
*/
#define HCI_TRACE_ERROR0(m)                     {if (btu_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_HCI, TRACE_TYPE_ERROR, m);}
#define HCI_TRACE_ERROR1(m,p1)                  {if (btu_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_HCI, TRACE_TYPE_ERROR, m,p1);}
#define HCI_TRACE_ERROR2(m,p1,p2)               {if (btu_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_HCI, TRACE_TYPE_ERROR, m,p1,p2);}
#define HCI_TRACE_ERROR3(m,p1,p2,p3)            {if (btu_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_HCI, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define HCI_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (btu_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_HCI, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define HCI_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (btu_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_HCI, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define HCI_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (btu_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_HCI, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define HCI_TRACE_WARNING0(m)                   {if (btu_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_HCI, TRACE_TYPE_WARNING, m);}
#define HCI_TRACE_WARNING1(m,p1)                {if (btu_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_HCI, TRACE_TYPE_WARNING, m,p1);}
#define HCI_TRACE_WARNING2(m,p1,p2)             {if (btu_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_HCI, TRACE_TYPE_WARNING, m,p1,p2);}
#define HCI_TRACE_WARNING3(m,p1,p2,p3)          {if (btu_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_HCI, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define HCI_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (btu_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_HCI, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define HCI_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (btu_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_HCI, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define HCI_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (btu_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_HCI, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define HCI_TRACE_EVENT0(m)                     {if (btu_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_HCI, TRACE_TYPE_EVENT, m);}
#define HCI_TRACE_EVENT1(m,p1)                  {if (btu_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_HCI, TRACE_TYPE_EVENT, m, p1);}
#define HCI_TRACE_EVENT2(m,p1,p2)               {if (btu_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_HCI, TRACE_TYPE_EVENT, m,p1,p2);}
#define HCI_TRACE_EVENT3(m,p1,p2,p3)            {if (btu_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_HCI, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define HCI_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (btu_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_HCI, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define HCI_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (btu_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_HCI, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define HCI_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (btu_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_HCI, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define HCI_TRACE_DEBUG0(m)                     {if (btu_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_HCI, TRACE_TYPE_DEBUG, m);}
#define HCI_TRACE_DEBUG1(m,p1)                  {if (btu_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_HCI, TRACE_TYPE_DEBUG, m,p1);}
#define HCI_TRACE_DEBUG2(m,p1,p2)               {if (btu_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_HCI, TRACE_TYPE_DEBUG, m,p1,p2);}
#define HCI_TRACE_DEBUG3(m,p1,p2,p3)            {if (btu_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_HCI, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define HCI_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (btu_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_HCI, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define HCI_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (btu_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_HCI, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define HCI_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (btu_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_HCI, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}


/* Define tracing for BTM
*/
#define BTM_TRACE_ERROR0(m)                     {if (btm_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_BTM, TRACE_TYPE_ERROR, m);}
#define BTM_TRACE_ERROR1(m,p1)                  {if (btm_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_BTM, TRACE_TYPE_ERROR, m,p1);}
#define BTM_TRACE_ERROR2(m,p1,p2)               {if (btm_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_BTM, TRACE_TYPE_ERROR, m,p1,p2);}
#define BTM_TRACE_ERROR3(m,p1,p2,p3)            {if (btm_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_BTM, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define BTM_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (btm_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_BTM, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define BTM_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (btm_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_BTM, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define BTM_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (btm_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_BTM, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define BTM_TRACE_WARNING0(m)                   {if (btm_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_BTM, TRACE_TYPE_WARNING, m);}
#define BTM_TRACE_WARNING1(m,p1)                {if (btm_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_BTM, TRACE_TYPE_WARNING, m,p1);}
#define BTM_TRACE_WARNING2(m,p1,p2)             {if (btm_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_BTM, TRACE_TYPE_WARNING, m,p1,p2);}
#define BTM_TRACE_WARNING3(m,p1,p2,p3)          {if (btm_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_BTM, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define BTM_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (btm_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_BTM, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define BTM_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (btm_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_BTM, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define BTM_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (btm_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_BTM, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define BTM_TRACE_API0(m)                       {if (btm_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_BTM, TRACE_TYPE_API, m);}
#define BTM_TRACE_API1(m,p1)                    {if (btm_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_BTM, TRACE_TYPE_API, m, p1);}
#define BTM_TRACE_API2(m,p1,p2)                 {if (btm_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_BTM, TRACE_TYPE_API, m,p1,p2);}
#define BTM_TRACE_API3(m,p1,p2,p3)              {if (btm_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_BTM, TRACE_TYPE_API, m,p1,p2,p3);}
#define BTM_TRACE_API4(m,p1,p2,p3,p4)           {if (btm_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_BTM, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define BTM_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (btm_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_BTM, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define BTM_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (btm_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_BTM, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define BTM_TRACE_EVENT0(m)                     {if (btm_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_BTM, TRACE_TYPE_EVENT, m);}
#define BTM_TRACE_EVENT1(m,p1)                  {if (btm_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_BTM, TRACE_TYPE_EVENT, m, p1);}
#define BTM_TRACE_EVENT2(m,p1,p2)               {if (btm_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_BTM, TRACE_TYPE_EVENT, m,p1,p2);}
#define BTM_TRACE_EVENT3(m,p1,p2,p3)            {if (btm_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_BTM, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define BTM_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (btm_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_BTM, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define BTM_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (btm_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_BTM, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define BTM_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (btm_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_BTM, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define BTM_TRACE_DEBUG0(m)                     {if (btm_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_BTM, TRACE_TYPE_DEBUG, m);}
#define BTM_TRACE_DEBUG1(m,p1)                  {if (btm_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_BTM, TRACE_TYPE_DEBUG, m,p1);}
#define BTM_TRACE_DEBUG2(m,p1,p2)               {if (btm_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_BTM, TRACE_TYPE_DEBUG, m,p1,p2);}
#define BTM_TRACE_DEBUG3(m,p1,p2,p3)            {if (btm_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_BTM, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define BTM_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (btm_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_BTM, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define BTM_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (btm_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_BTM, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define BTM_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (btm_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_BTM, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}


/* Define tracing for the L2CAP unit
*/
#define L2CAP_TRACE_ERROR0(m)                     {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_L2CAP, TRACE_TYPE_ERROR, m);}
#define L2CAP_TRACE_ERROR1(m,p1)                  {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_L2CAP, TRACE_TYPE_ERROR, m,p1);}
#define L2CAP_TRACE_ERROR2(m,p1,p2)               {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_L2CAP, TRACE_TYPE_ERROR, m,p1,p2);}
#define L2CAP_TRACE_ERROR3(m,p1,p2,p3)            {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_L2CAP, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define L2CAP_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_L2CAP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define L2CAP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_L2CAP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define L2CAP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_L2CAP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define L2CAP_TRACE_WARNING0(m)                   {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_L2CAP, TRACE_TYPE_WARNING, m);}
#define L2CAP_TRACE_WARNING1(m,p1)                {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_L2CAP, TRACE_TYPE_WARNING, m,p1);}
#define L2CAP_TRACE_WARNING2(m,p1,p2)             {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_L2CAP, TRACE_TYPE_WARNING, m,p1,p2);}
#define L2CAP_TRACE_WARNING3(m,p1,p2,p3)          {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_L2CAP, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define L2CAP_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_L2CAP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define L2CAP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_L2CAP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define L2CAP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_L2CAP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define L2CAP_TRACE_API0(m)                       {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_L2CAP, TRACE_TYPE_API, m);}
#define L2CAP_TRACE_API1(m,p1)                    {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_L2CAP, TRACE_TYPE_API, m,p1);}
#define L2CAP_TRACE_API2(m,p1,p2)                 {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_L2CAP, TRACE_TYPE_API, m,p1,p2);}
#define L2CAP_TRACE_API3(m,p1,p2,p3)              {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_L2CAP, TRACE_TYPE_API, m,p1,p2,p3);}
#define L2CAP_TRACE_API4(m,p1,p2,p3,p4)           {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_L2CAP, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define L2CAP_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_L2CAP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define L2CAP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_L2CAP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define L2CAP_TRACE_EVENT0(m)                     {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_L2CAP, TRACE_TYPE_EVENT, m);}
#define L2CAP_TRACE_EVENT1(m,p1)                  {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_L2CAP, TRACE_TYPE_EVENT, m, p1);}
#define L2CAP_TRACE_EVENT2(m,p1,p2)               {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_L2CAP, TRACE_TYPE_EVENT, m,p1,p2);}
#define L2CAP_TRACE_EVENT3(m,p1,p2,p3)            {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_L2CAP, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define L2CAP_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_L2CAP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define L2CAP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_L2CAP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define L2CAP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_L2CAP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define L2CAP_TRACE_DEBUG0(m)                     {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_L2CAP, TRACE_TYPE_DEBUG, m);}
#define L2CAP_TRACE_DEBUG1(m,p1)                  {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_L2CAP, TRACE_TYPE_DEBUG, m,p1);}
#define L2CAP_TRACE_DEBUG2(m,p1,p2)               {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_L2CAP, TRACE_TYPE_DEBUG, m,p1,p2);}
#define L2CAP_TRACE_DEBUG3(m,p1,p2,p3)            {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_L2CAP, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define L2CAP_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_L2CAP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define L2CAP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_L2CAP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define L2CAP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (l2cb.l2cap_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_L2CAP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

/* Define tracing for the LLCP unit
*/
#define LLCP_TRACE_ERROR0(m)                     {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_LLCP, TRACE_TYPE_ERROR, m);}
#define LLCP_TRACE_ERROR1(m,p1)                  {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_LLCP, TRACE_TYPE_ERROR, m,p1);}
#define LLCP_TRACE_ERROR2(m,p1,p2)               {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_LLCP, TRACE_TYPE_ERROR, m,p1,p2);}
#define LLCP_TRACE_ERROR3(m,p1,p2,p3)            {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_LLCP, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define LLCP_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_LLCP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define LLCP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_LLCP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define LLCP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_LLCP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define LLCP_TRACE_WARNING0(m)                   {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_LLCP, TRACE_TYPE_WARNING, m);}
#define LLCP_TRACE_WARNING1(m,p1)                {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_LLCP, TRACE_TYPE_WARNING, m,p1);}
#define LLCP_TRACE_WARNING2(m,p1,p2)             {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_LLCP, TRACE_TYPE_WARNING, m,p1,p2);}
#define LLCP_TRACE_WARNING3(m,p1,p2,p3)          {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_LLCP, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define LLCP_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_LLCP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define LLCP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_LLCP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define LLCP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_LLCP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define LLCP_TRACE_API0(m)                       {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_LLCP, TRACE_TYPE_API, m);}
#define LLCP_TRACE_API1(m,p1)                    {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_LLCP, TRACE_TYPE_API, m,p1);}
#define LLCP_TRACE_API2(m,p1,p2)                 {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_LLCP, TRACE_TYPE_API, m,p1,p2);}
#define LLCP_TRACE_API3(m,p1,p2,p3)              {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_LLCP, TRACE_TYPE_API, m,p1,p2,p3);}
#define LLCP_TRACE_API4(m,p1,p2,p3,p4)           {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_LLCP, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define LLCP_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_LLCP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define LLCP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_LLCP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define LLCP_TRACE_EVENT0(m)                     {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_LLCP, TRACE_TYPE_EVENT, m);}
#define LLCP_TRACE_EVENT1(m,p1)                  {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_LLCP, TRACE_TYPE_EVENT, m, p1);}
#define LLCP_TRACE_EVENT2(m,p1,p2)               {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_LLCP, TRACE_TYPE_EVENT, m,p1,p2);}
#define LLCP_TRACE_EVENT3(m,p1,p2,p3)            {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_LLCP, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define LLCP_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_LLCP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define LLCP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_LLCP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define LLCP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_LLCP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define LLCP_TRACE_DEBUG0(m)                     {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_LLCP, TRACE_TYPE_DEBUG, m);}
#define LLCP_TRACE_DEBUG1(m,p1)                  {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_LLCP, TRACE_TYPE_DEBUG, m,p1);}
#define LLCP_TRACE_DEBUG2(m,p1,p2)               {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_LLCP, TRACE_TYPE_DEBUG, m,p1,p2);}
#define LLCP_TRACE_DEBUG3(m,p1,p2,p3)            {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_LLCP, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define LLCP_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_LLCP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define LLCP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_LLCP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define LLCP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (llcp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_LLCP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

/* Define tracing for the SDP unit
*/
#define SDP_TRACE_ERROR0(m)                     {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_SDP, TRACE_TYPE_ERROR, m);}
#define SDP_TRACE_ERROR1(m,p1)                  {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_SDP, TRACE_TYPE_ERROR, m,p1);}
#define SDP_TRACE_ERROR2(m,p1,p2)               {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_SDP, TRACE_TYPE_ERROR, m,p1,p2);}
#define SDP_TRACE_ERROR3(m,p1,p2,p3)            {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_SDP, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define SDP_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_SDP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define SDP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_SDP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define SDP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_SDP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define SDP_TRACE_WARNING0(m)                   {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_SDP, TRACE_TYPE_WARNING, m);}
#define SDP_TRACE_WARNING1(m,p1)                {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_SDP, TRACE_TYPE_WARNING, m,p1);}
#define SDP_TRACE_WARNING2(m,p1,p2)             {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_SDP, TRACE_TYPE_WARNING, m,p1,p2);}
#define SDP_TRACE_WARNING3(m,p1,p2,p3)          {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_SDP, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define SDP_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_SDP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define SDP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_SDP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define SDP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_SDP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define SDP_TRACE_API0(m)                       {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_SDP, TRACE_TYPE_API, m);}
#define SDP_TRACE_API1(m,p1)                    {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_SDP, TRACE_TYPE_API, m,p1);}
#define SDP_TRACE_API2(m,p1,p2)                 {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_SDP, TRACE_TYPE_API, m,p1,p2);}
#define SDP_TRACE_API3(m,p1,p2,p3)              {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_SDP, TRACE_TYPE_API, m,p1,p2,p3);}
#define SDP_TRACE_API4(m,p1,p2,p3,p4)           {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_SDP, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define SDP_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_SDP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define SDP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_SDP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define SDP_TRACE_EVENT0(m)                     {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_SDP, TRACE_TYPE_EVENT, m);}
#define SDP_TRACE_EVENT1(m,p1)                  {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_SDP, TRACE_TYPE_EVENT, m, p1);}
#define SDP_TRACE_EVENT2(m,p1,p2)               {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_SDP, TRACE_TYPE_EVENT, m,p1,p2);}
#define SDP_TRACE_EVENT3(m,p1,p2,p3)            {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_SDP, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define SDP_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_SDP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define SDP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_SDP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define SDP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_SDP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define SDP_TRACE_DEBUG0(m)                     {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_SDP, TRACE_TYPE_DEBUG, m);}
#define SDP_TRACE_DEBUG1(m,p1)                  {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_SDP, TRACE_TYPE_DEBUG, m,p1);}
#define SDP_TRACE_DEBUG2(m,p1,p2)               {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_SDP, TRACE_TYPE_DEBUG, m,p1,p2);}
#define SDP_TRACE_DEBUG3(m,p1,p2,p3)            {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_SDP, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define SDP_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_SDP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define SDP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_SDP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define SDP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (sdp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_SDP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

/* Define tracing for the RFCOMM unit
*/
#define RFCOMM_TRACE_ERROR0(m)                     {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_RFCOMM, TRACE_TYPE_ERROR, m);}
#define RFCOMM_TRACE_ERROR1(m,p1)                  {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_RFCOMM, TRACE_TYPE_ERROR, m,p1);}
#define RFCOMM_TRACE_ERROR2(m,p1,p2)               {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_RFCOMM, TRACE_TYPE_ERROR, m,p1,p2);}
#define RFCOMM_TRACE_ERROR3(m,p1,p2,p3)            {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_RFCOMM, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define RFCOMM_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_RFCOMM, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define RFCOMM_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_RFCOMM, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define RFCOMM_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_RFCOMM, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define RFCOMM_TRACE_WARNING0(m)                   {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_RFCOMM, TRACE_TYPE_WARNING, m);}
#define RFCOMM_TRACE_WARNING1(m,p1)                {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_RFCOMM, TRACE_TYPE_WARNING, m,p1);}
#define RFCOMM_TRACE_WARNING2(m,p1,p2)             {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_RFCOMM, TRACE_TYPE_WARNING, m,p1,p2);}
#define RFCOMM_TRACE_WARNING3(m,p1,p2,p3)          {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_RFCOMM, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define RFCOMM_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_RFCOMM, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define RFCOMM_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_RFCOMM, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define RFCOMM_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_RFCOMM, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define RFCOMM_TRACE_API0(m)                       {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_RFCOMM, TRACE_TYPE_API, m);}
#define RFCOMM_TRACE_API1(m,p1)                    {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_RFCOMM, TRACE_TYPE_API, m,p1);}
#define RFCOMM_TRACE_API2(m,p1,p2)                 {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_RFCOMM, TRACE_TYPE_API, m,p1,p2);}
#define RFCOMM_TRACE_API3(m,p1,p2,p3)              {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_RFCOMM, TRACE_TYPE_API, m,p1,p2,p3);}
#define RFCOMM_TRACE_API4(m,p1,p2,p3,p4)           {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_RFCOMM, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define RFCOMM_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_RFCOMM, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define RFCOMM_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_RFCOMM, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define RFCOMM_TRACE_EVENT0(m)                     {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_RFCOMM, TRACE_TYPE_EVENT, m);}
#define RFCOMM_TRACE_EVENT1(m,p1)                  {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_RFCOMM, TRACE_TYPE_EVENT, m, p1);}
#define RFCOMM_TRACE_EVENT2(m,p1,p2)               {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_RFCOMM, TRACE_TYPE_EVENT, m,p1,p2);}
#define RFCOMM_TRACE_EVENT3(m,p1,p2,p3)            {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_RFCOMM, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define RFCOMM_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_RFCOMM, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define RFCOMM_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_RFCOMM, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define RFCOMM_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_RFCOMM, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define RFCOMM_TRACE_DEBUG0(m)                     {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_RFCOMM, TRACE_TYPE_DEBUG, m);}
#define RFCOMM_TRACE_DEBUG1(m,p1)                  {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_RFCOMM, TRACE_TYPE_DEBUG, m,p1);}
#define RFCOMM_TRACE_DEBUG2(m,p1,p2)               {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_RFCOMM, TRACE_TYPE_DEBUG, m,p1,p2);}
#define RFCOMM_TRACE_DEBUG3(m,p1,p2,p3)            {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_RFCOMM, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define RFCOMM_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_RFCOMM, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define RFCOMM_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_RFCOMM, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define RFCOMM_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (rfc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_RFCOMM, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

/* Define tracing for Serial Port Profile
*/
#define SPP_TRACE_ERROR0(m)                     {if (spp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_SPP, TRACE_TYPE_ERROR, m);}
#define SPP_TRACE_ERROR1(m,p1)                  {if (spp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_SPP, TRACE_TYPE_ERROR, m,p1);}
#define SPP_TRACE_ERROR2(m,p1,p2)               {if (spp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_SPP, TRACE_TYPE_ERROR, m,p1,p2);}
#define SPP_TRACE_ERROR3(m,p1,p2,p3)            {if (spp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_SPP, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define SPP_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (spp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_SPP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define SPP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (spp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_SPP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define SPP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (spp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_SPP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define SPP_TRACE_WARNING0(m)                   {if (spp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_SPP, TRACE_TYPE_WARNING, m);}
#define SPP_TRACE_WARNING1(m,p1)                {if (spp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_SPP, TRACE_TYPE_WARNING, m,p1);}
#define SPP_TRACE_WARNING2(m,p1,p2)             {if (spp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_SPP, TRACE_TYPE_WARNING, m,p1,p2);}
#define SPP_TRACE_WARNING3(m,p1,p2,p3)          {if (spp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_SPP, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define SPP_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (spp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_SPP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define SPP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (spp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_SPP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define SPP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (spp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_SPP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define SPP_TRACE_EVENT0(m)                     {if (spp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_SPP, TRACE_TYPE_EVENT, m);}
#define SPP_TRACE_EVENT1(m,p1)                  {if (spp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_SPP, TRACE_TYPE_EVENT, m, p1);}
#define SPP_TRACE_EVENT2(m,p1,p2)               {if (spp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_SPP, TRACE_TYPE_EVENT, m,p1,p2);}
#define SPP_TRACE_EVENT3(m,p1,p2,p3)            {if (spp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_SPP, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define SPP_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (spp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_SPP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define SPP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (spp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_SPP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define SPP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (spp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_SPP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define SPP_TRACE_API0(m)                       {if (spp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_SPP, TRACE_TYPE_API, m);}
#define SPP_TRACE_API1(m,p1)                    {if (spp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_SPP, TRACE_TYPE_API, m, p1);}
#define SPP_TRACE_API2(m,p1,p2)                 {if (spp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_SPP, TRACE_TYPE_API, m,p1,p2);}
#define SPP_TRACE_API3(m,p1,p2,p3)              {if (spp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_SPP, TRACE_TYPE_API, m,p1,p2,p3);}
#define SPP_TRACE_API4(m,p1,p2,p3,p4)           {if (spp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_SPP, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define SPP_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (spp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_SPP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define SPP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (spp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_SPP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define SPP_TRACE_DEBUG0(m)                     {if (spp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_SPP, TRACE_TYPE_DEBUG, m);}
#define SPP_TRACE_DEBUG1(m,p1)                  {if (spp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_SPP, TRACE_TYPE_DEBUG, m,p1);}
#define SPP_TRACE_DEBUG2(m,p1,p2)               {if (spp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_SPP, TRACE_TYPE_DEBUG, m,p1,p2);}
#define SPP_TRACE_DEBUG3(m,p1,p2,p3)            {if (spp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_SPP, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define SPP_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (spp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_SPP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define SPP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (spp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_SPP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define SPP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (spp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_SPP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

/* Generic Access Profile traces */
#define GAP_TRACE_ERROR0(m)                     {if (gap_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_GAP, TRACE_TYPE_ERROR, m);}
#define GAP_TRACE_ERROR1(m,p1)                  {if (gap_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_GAP, TRACE_TYPE_ERROR, m, p1);}
#define GAP_TRACE_ERROR2(m,p1,p2)               {if (gap_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_GAP, TRACE_TYPE_ERROR, m,p1,p2);}
#define GAP_TRACE_ERROR3(m,p1,p2,p3)            {if (gap_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_GAP, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define GAP_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (gap_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_GAP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define GAP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (gap_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_GAP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define GAP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (gap_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_GAP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define GAP_TRACE_EVENT0(m)                     {if (gap_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_GAP, TRACE_TYPE_EVENT, m);}
#define GAP_TRACE_EVENT1(m,p1)                  {if (gap_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_GAP, TRACE_TYPE_EVENT, m, p1);}
#define GAP_TRACE_EVENT2(m,p1,p2)               {if (gap_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_GAP, TRACE_TYPE_EVENT, m,p1,p2);}
#define GAP_TRACE_EVENT3(m,p1,p2,p3)            {if (gap_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_GAP, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define GAP_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (gap_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_GAP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define GAP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (gap_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_GAP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define GAP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (gap_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_GAP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define GAP_TRACE_API0(m)                       {if (gap_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_GAP, TRACE_TYPE_API, m);}
#define GAP_TRACE_API1(m,p1)                    {if (gap_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_GAP, TRACE_TYPE_API, m, p1);}
#define GAP_TRACE_API2(m,p1,p2)                 {if (gap_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_GAP, TRACE_TYPE_API, m,p1,p2);}
#define GAP_TRACE_API3(m,p1,p2,p3)              {if (gap_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_GAP, TRACE_TYPE_API, m,p1,p2,p3);}
#define GAP_TRACE_API4(m,p1,p2,p3,p4)           {if (gap_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_GAP, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define GAP_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (gap_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_GAP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define GAP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (gap_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_GAP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define GAP_TRACE_WARNING0(m)                   {if (gap_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_GAP, TRACE_TYPE_WARNING, m);}
#define GAP_TRACE_WARNING1(m,p1)                {if (gap_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_GAP, TRACE_TYPE_WARNING, m, p1);}
#define GAP_TRACE_WARNING2(m,p1,p2)             {if (gap_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_GAP, TRACE_TYPE_WARNING, m,p1,p2);}
#define GAP_TRACE_WARNING3(m,p1,p2,p3)          {if (gap_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_GAP, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define GAP_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (gap_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_GAP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define GAP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (gap_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_GAP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define GAP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (gap_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_GAP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}


/* Define tracing for OBX
*/
#define OBX_TRACE_ERROR0(m)                      {if (obx_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_OBEX, TRACE_TYPE_ERROR, m);}
#define OBX_TRACE_ERROR1(m,p1)                   {if (obx_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_OBEX, TRACE_TYPE_ERROR, m,p1);}
#define OBX_TRACE_ERROR2(m,p1,p2)                {if (obx_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_OBEX, TRACE_TYPE_ERROR, m,p1,p2);}
#define OBX_TRACE_ERROR3(m,p1,p2,p3)             {if (obx_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_OBEX, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define OBX_TRACE_ERROR4(m,p1,p2,p3,p4)          {if (obx_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_OBEX, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define OBX_TRACE_ERROR5(m,p1,p2,p3,p4,p5)       {if (obx_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_OBEX, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define OBX_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)    {if (obx_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_OBEX, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define OBX_TRACE_WARNING0(m)                    {if (obx_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_OBEX, TRACE_TYPE_WARNING, m);}
#define OBX_TRACE_WARNING1(m,p1)                 {if (obx_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_OBEX, TRACE_TYPE_WARNING, m,p1);}
#define OBX_TRACE_WARNING2(m,p1,p2)              {if (obx_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_OBEX, TRACE_TYPE_WARNING, m,p1,p2);}
#define OBX_TRACE_WARNING3(m,p1,p2,p3)           {if (obx_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_OBEX, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define OBX_TRACE_WARNING4(m,p1,p2,p3,p4)        {if (obx_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_OBEX, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define OBX_TRACE_WARNING5(m,p1,p2,p3,p4,p5)     {if (obx_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_OBEX, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define OBX_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)  {if (obx_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_OBEX, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define OBX_TRACE_EVENT0(m)                      {if (obx_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_OBEX, TRACE_TYPE_EVENT, m);}
#define OBX_TRACE_EVENT1(m,p1)                   {if (obx_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_OBEX, TRACE_TYPE_EVENT, m, p1);}
#define OBX_TRACE_EVENT2(m,p1,p2)                {if (obx_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_OBEX, TRACE_TYPE_EVENT, m,p1,p2);}
#define OBX_TRACE_EVENT3(m,p1,p2,p3)             {if (obx_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_OBEX, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define OBX_TRACE_EVENT4(m,p1,p2,p3,p4)          {if (obx_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_OBEX, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define OBX_TRACE_EVENT5(m,p1,p2,p3,p4,p5)       {if (obx_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_OBEX, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define OBX_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)    {if (obx_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_OBEX, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define OBX_TRACE_DEBUG0(m)                      {if (obx_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_OBEX, TRACE_TYPE_DEBUG, m);}
#define OBX_TRACE_DEBUG1(m,p1)                   {if (obx_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_OBEX, TRACE_TYPE_DEBUG, m,p1);}
#define OBX_TRACE_DEBUG2(m,p1,p2)                {if (obx_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_OBEX, TRACE_TYPE_DEBUG, m,p1,p2);}
#define OBX_TRACE_DEBUG3(m,p1,p2,p3)             {if (obx_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_OBEX, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define OBX_TRACE_DEBUG4(m,p1,p2,p3,p4)          {if (obx_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_OBEX, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define OBX_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)       {if (obx_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_OBEX, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define OBX_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)    {if (obx_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_OBEX, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

#define OBX_TRACE_API0(m)                        {if (obx_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_OBEX, TRACE_TYPE_API, m);}
#define OBX_TRACE_API1(m,p1)                     {if (obx_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_OBEX, TRACE_TYPE_API, m, p1);}
#define OBX_TRACE_API2(m,p1,p2)                  {if (obx_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_OBEX, TRACE_TYPE_API, m,p1,p2);}
#define OBX_TRACE_API3(m,p1,p2,p3)               {if (obx_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_OBEX, TRACE_TYPE_API, m,p1,p2,p3);}
#define OBX_TRACE_API4(m,p1,p2,p3,p4)            {if (obx_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_OBEX, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define OBX_TRACE_API5(m,p1,p2,p3,p4,p5)         {if (obx_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_OBEX, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define OBX_TRACE_API6(m,p1,p2,p3,p4,p5,p6)      {if (obx_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_OBEX, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

/* Define tracing for OBEX application profiles
*/
#define GOEP_TRACE_ERROR0(m)                     {if (goep_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_GOEP, TRACE_TYPE_ERROR, m);}
#define GOEP_TRACE_ERROR1(m,p1)                  {if (goep_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_GOEP, TRACE_TYPE_ERROR, m,p1);}
#define GOEP_TRACE_ERROR2(m,p1,p2)               {if (goep_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_GOEP, TRACE_TYPE_ERROR, m,p1,p2);}
#define GOEP_TRACE_ERROR3(m,p1,p2,p3)            {if (goep_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_GOEP, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define GOEP_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (goep_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_GOEP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define GOEP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (goep_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_GOEP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define GOEP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (goep_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_GOEP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define GOEP_TRACE_WARNING0(m)                   {if (goep_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_GOEP, TRACE_TYPE_WARNING, m);}
#define GOEP_TRACE_WARNING1(m,p1)                {if (goep_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_GOEP, TRACE_TYPE_WARNING, m,p1);}
#define GOEP_TRACE_WARNING2(m,p1,p2)             {if (goep_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_GOEP, TRACE_TYPE_WARNING, m,p1,p2);}
#define GOEP_TRACE_WARNING3(m,p1,p2,p3)          {if (goep_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_GOEP, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define GOEP_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (goep_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_GOEP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define GOEP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (goep_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_GOEP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define GOEP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (goep_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_GOEP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define GOEP_TRACE_EVENT0(m)                     {if (goep_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_GOEP, TRACE_TYPE_EVENT, m);}
#define GOEP_TRACE_EVENT1(m,p1)                  {if (goep_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_GOEP, TRACE_TYPE_EVENT, m, p1);}
#define GOEP_TRACE_EVENT2(m,p1,p2)               {if (goep_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_GOEP, TRACE_TYPE_EVENT, m,p1,p2);}
#define GOEP_TRACE_EVENT3(m,p1,p2,p3)            {if (goep_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_GOEP, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define GOEP_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (goep_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_GOEP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define GOEP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (goep_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_GOEP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define GOEP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (goep_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_GOEP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define GOEP_TRACE_DEBUG0(m)                     {if (goep_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_GOEP, TRACE_TYPE_DEBUG, m);}
#define GOEP_TRACE_DEBUG1(m,p1)                  {if (goep_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_GOEP, TRACE_TYPE_DEBUG, m,p1);}
#define GOEP_TRACE_DEBUG2(m,p1,p2)               {if (goep_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_GOEP, TRACE_TYPE_DEBUG, m,p1,p2);}
#define GOEP_TRACE_DEBUG3(m,p1,p2,p3)            {if (goep_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_GOEP, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define GOEP_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (goep_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_GOEP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define GOEP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (goep_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_GOEP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define GOEP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (goep_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_GOEP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

#define GOEP_TRACE_API0(m)                       {if (goep_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_GOEP, TRACE_TYPE_API, m);}
#define GOEP_TRACE_API1(m,p1)                    {if (goep_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_GOEP, TRACE_TYPE_API, m, p1);}
#define GOEP_TRACE_API2(m,p1,p2)                 {if (goep_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_GOEP, TRACE_TYPE_API, m,p1,p2);}
#define GOEP_TRACE_API3(m,p1,p2,p3)              {if (goep_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_GOEP, TRACE_TYPE_API, m,p1,p2,p3);}
#define GOEP_TRACE_API4(m,p1,p2,p3,p4)           {if (goep_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_GOEP, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define GOEP_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (goep_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_GOEP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define GOEP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (goep_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_GOEP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

/* Define tracing for the BPP profile
*/
#define BPP_TRACE_ERROR0(m)                      {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_BPP, TRACE_TYPE_ERROR, m);}
#define BPP_TRACE_ERROR1(m,p1)                   {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_BPP, TRACE_TYPE_ERROR, m,p1);}
#define BPP_TRACE_ERROR2(m,p1,p2)                {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_BPP, TRACE_TYPE_ERROR, m,p1,p2);}
#define BPP_TRACE_ERROR3(m,p1,p2,p3)             {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_BPP, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define BPP_TRACE_ERROR4(m,p1,p2,p3,p4)          {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_BPP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define BPP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)       {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_BPP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define BPP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)    {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_BPP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define BPP_TRACE_WARNING0(m)                    {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_BPP, TRACE_TYPE_WARNING, m);}
#define BPP_TRACE_WARNING1(m,p1)                 {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_BPP, TRACE_TYPE_WARNING, m,p1);}
#define BPP_TRACE_WARNING2(m,p1,p2)              {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_BPP, TRACE_TYPE_WARNING, m,p1,p2);}
#define BPP_TRACE_WARNING3(m,p1,p2,p3)           {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_BPP, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define BPP_TRACE_WARNING4(m,p1,p2,p3,p4)        {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_BPP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define BPP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)     {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_BPP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define BPP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)  {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_BPP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define BPP_TRACE_EVENT0(m)                      {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_BPP, TRACE_TYPE_EVENT, m);}
#define BPP_TRACE_EVENT1(m,p1)                   {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_BPP, TRACE_TYPE_EVENT, m, p1);}
#define BPP_TRACE_EVENT2(m,p1,p2)                {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_BPP, TRACE_TYPE_EVENT, m,p1,p2);}
#define BPP_TRACE_EVENT3(m,p1,p2,p3)             {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_BPP, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define BPP_TRACE_EVENT4(m,p1,p2,p3,p4)          {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_BPP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define BPP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)       {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_BPP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define BPP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)    {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_BPP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define BPP_TRACE_DEBUG0(m)                      {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_BPP, TRACE_TYPE_DEBUG, m);}
#define BPP_TRACE_DEBUG1(m,p1)                   {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_BPP, TRACE_TYPE_DEBUG, m,p1);}
#define BPP_TRACE_DEBUG2(m,p1,p2)                {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_BPP, TRACE_TYPE_DEBUG, m,p1,p2);}
#define BPP_TRACE_DEBUG3(m,p1,p2,p3)             {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_BPP, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define BPP_TRACE_DEBUG4(m,p1,p2,p3,p4)          {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_BPP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define BPP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)       {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_BPP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define BPP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)    {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_BPP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

#define BPP_TRACE_API0(m)                        {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_BPP, TRACE_TYPE_API, m);}
#define BPP_TRACE_API1(m,p1)                     {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_BPP, TRACE_TYPE_API, m, p1);}
#define BPP_TRACE_API2(m,p1,p2)                  {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_BPP, TRACE_TYPE_API, m,p1,p2);}
#define BPP_TRACE_API3(m,p1,p2,p3)               {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_BPP, TRACE_TYPE_API, m,p1,p2,p3);}
#define BPP_TRACE_API4(m,p1,p2,p3,p4)            {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_BPP, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define BPP_TRACE_API5(m,p1,p2,p3,p4,p5)         {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_BPP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define BPP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)      {if (bpp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_BPP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

/* Define tracing for the BIP profile
*/
#define BIP_TRACE_ERROR0(m)                      {if (bip_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_BIP, TRACE_TYPE_ERROR, m);}
#define BIP_TRACE_ERROR1(m,p1)                   {if (bip_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_BIP, TRACE_TYPE_ERROR, m,p1);}
#define BIP_TRACE_ERROR2(m,p1,p2)                {if (bip_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_BIP, TRACE_TYPE_ERROR, m,p1,p2);}
#define BIP_TRACE_ERROR3(m,p1,p2,p3)             {if (bip_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_BIP, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define BIP_TRACE_ERROR4(m,p1,p2,p3,p4)          {if (bip_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_BIP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define BIP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)       {if (bip_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_BIP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define BIP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)    {if (bip_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_BIP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define BIP_TRACE_WARNING0(m)                    {if (bip_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_BIP, TRACE_TYPE_WARNING, m);}
#define BIP_TRACE_WARNING1(m,p1)                 {if (bip_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_BIP, TRACE_TYPE_WARNING, m,p1);}
#define BIP_TRACE_WARNING2(m,p1,p2)              {if (bip_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_BIP, TRACE_TYPE_WARNING, m,p1,p2);}
#define BIP_TRACE_WARNING3(m,p1,p2,p3)           {if (bip_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_BIP, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define BIP_TRACE_WARNING4(m,p1,p2,p3,p4)        {if (bip_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_BIP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define BIP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)     {if (bip_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_BIP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define BIP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)  {if (bip_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_BIP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define BIP_TRACE_EVENT0(m)                      {if (bip_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_BIP, TRACE_TYPE_EVENT, m);}
#define BIP_TRACE_EVENT1(m,p1)                   {if (bip_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_BIP, TRACE_TYPE_EVENT, m, p1);}
#define BIP_TRACE_EVENT2(m,p1,p2)                {if (bip_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_BIP, TRACE_TYPE_EVENT, m,p1,p2);}
#define BIP_TRACE_EVENT3(m,p1,p2,p3)             {if (bip_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_BIP, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define BIP_TRACE_EVENT4(m,p1,p2,p3,p4)          {if (bip_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_BIP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define BIP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)       {if (bip_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_BIP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define BIP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)    {if (bip_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_BIP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define BIP_TRACE_DEBUG0(m)                      {if (bip_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_BIP, TRACE_TYPE_DEBUG, m);}
#define BIP_TRACE_DEBUG1(m,p1)                   {if (bip_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_BIP, TRACE_TYPE_DEBUG, m,p1);}
#define BIP_TRACE_DEBUG2(m,p1,p2)                {if (bip_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_BIP, TRACE_TYPE_DEBUG, m,p1,p2);}
#define BIP_TRACE_DEBUG3(m,p1,p2,p3)             {if (bip_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_BIP, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define BIP_TRACE_DEBUG4(m,p1,p2,p3,p4)          {if (bip_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_BIP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define BIP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)       {if (bip_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_BIP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define BIP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)    {if (bip_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_BIP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

#define BIP_TRACE_API0(m)                        {if (bip_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_BIP, TRACE_TYPE_API, m);}
#define BIP_TRACE_API1(m,p1)                     {if (bip_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_BIP, TRACE_TYPE_API, m, p1);}
#define BIP_TRACE_API2(m,p1,p2)                  {if (bip_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_BIP, TRACE_TYPE_API, m,p1,p2);}
#define BIP_TRACE_API3(m,p1,p2,p3)               {if (bip_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_BIP, TRACE_TYPE_API, m,p1,p2,p3);}
#define BIP_TRACE_API4(m,p1,p2,p3,p4)            {if (bip_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_BIP, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define BIP_TRACE_API5(m,p1,p2,p3,p4,p5)         {if (bip_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_BIP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define BIP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)      {if (bip_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_BIP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

/* define traces for HID Host */
#define HIDH_TRACE_ERROR0(m)                     {if (hh_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_HID, TRACE_TYPE_ERROR, m);}
#define HIDH_TRACE_ERROR1(m,p1)                  {if (hh_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_HID, TRACE_TYPE_ERROR, m, p1);}
#define HIDH_TRACE_ERROR2(m,p1,p2)               {if (hh_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_HID, TRACE_TYPE_ERROR, m,p1,p2);}
#define HIDH_TRACE_ERROR3(m,p1,p2,p3)            {if (hh_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_HID, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define HIDH_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (hh_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_HID, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define HIDH_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (hh_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_HID, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define HIDH_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (hh_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_HID, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define HIDH_TRACE_WARNING0(m)                   {if (hh_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_HID, TRACE_TYPE_WARNING, m);}
#define HIDH_TRACE_WARNING1(m,p1)                {if (hh_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_HID, TRACE_TYPE_WARNING, m,p1);}
#define HIDH_TRACE_WARNING2(m,p1,p2)             {if (hh_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_HID, TRACE_TYPE_WARNING, m,p1,p2);}
#define HIDH_TRACE_WARNING3(m,p1,p2,p3)          {if (hh_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_HID, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define HIDH_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (hh_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_HID, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define HIDH_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (hh_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_HID, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define HIDH_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (hh_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_HID, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define HIDH_TRACE_API0(m)                       {if (hh_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_HID, TRACE_TYPE_API, m);}
#define HIDH_TRACE_API1(m,p1)                    {if (hh_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_HID, TRACE_TYPE_API, m, p1);}
#define HIDH_TRACE_API2(m,p1,p2)                 {if (hh_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_HID, TRACE_TYPE_API, m,p1,p2);}
#define HIDH_TRACE_API3(m,p1,p2,p3)              {if (hh_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_HID, TRACE_TYPE_API, m,p1,p2,p3);}
#define HIDH_TRACE_API4(m,p1,p2,p3,p4)           {if (hh_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_HID, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define HIDH_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (hh_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_HID, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define HIDH_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (hh_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_HID, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define HIDH_TRACE_EVENT0(m)                     {if (hh_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_HID, TRACE_TYPE_EVENT, m);}
#define HIDH_TRACE_EVENT1(m,p1)                  {if (hh_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_HID, TRACE_TYPE_EVENT, m, p1);}
#define HIDH_TRACE_EVENT2(m,p1,p2)               {if (hh_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_HID, TRACE_TYPE_EVENT, m,p1,p2);}
#define HIDH_TRACE_EVENT3(m,p1,p2,p3)            {if (hh_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_HID, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define HIDH_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (hh_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_HID, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define HIDH_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (hh_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_HID, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define HIDH_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (hh_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_HID, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define HIDH_TRACE_DEBUG0(m)                     {if (hh_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_HID, TRACE_TYPE_DEBUG, m);}
#define HIDH_TRACE_DEBUG1(m,p1)                  {if (hh_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_HID, TRACE_TYPE_DEBUG, m,p1);}
#define HIDH_TRACE_DEBUG2(m,p1,p2)               {if (hh_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_HID, TRACE_TYPE_DEBUG, m,p1,p2);}
#define HIDH_TRACE_DEBUG3(m,p1,p2,p3)            {if (hh_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_HID, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define HIDH_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (hh_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_HID, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define HIDH_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (hh_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_HID, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define HIDH_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (hh_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_HID, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

/* define traces for HID Device */
#define HIDD_TRACE_ERROR0(m)                     {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_HID, TRACE_TYPE_ERROR, m);}
#define HIDD_TRACE_ERROR1(m,p1)                  {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_HID, TRACE_TYPE_ERROR, m, p1);}
#define HIDD_TRACE_ERROR2(m,p1,p2)               {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_HID, TRACE_TYPE_ERROR, m,p1,p2);}
#define HIDD_TRACE_ERROR3(m,p1,p2,p3)            {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_HID, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define HIDD_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_HID, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define HIDD_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_HID, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define HIDD_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_HID, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define HIDD_TRACE_WARNING0(m)                   {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_HID, TRACE_TYPE_WARNING, m);}
#define HIDD_TRACE_WARNING1(m,p1)                {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_HID, TRACE_TYPE_WARNING, m,p1);}
#define HIDD_TRACE_WARNING2(m,p1,p2)             {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_HID, TRACE_TYPE_WARNING, m,p1,p2);}
#define HIDD_TRACE_WARNING3(m,p1,p2,p3)          {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_HID, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define HIDD_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_HID, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define HIDD_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_HID, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define HIDD_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_HID, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define HIDD_TRACE_API0(m)                       {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_HID, TRACE_TYPE_API, m);}
#define HIDD_TRACE_API1(m,p1)                    {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_HID, TRACE_TYPE_API, m, p1);}
#define HIDD_TRACE_API2(m,p1,p2)                 {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_HID, TRACE_TYPE_API, m,p1,p2);}
#define HIDD_TRACE_API3(m,p1,p2,p3)              {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_HID, TRACE_TYPE_API, m,p1,p2,p3);}
#define HIDD_TRACE_API4(m,p1,p2,p3,p4)           {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_HID, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define HIDD_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_HID, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define HIDD_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_HID, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define HIDD_TRACE_EVENT0(m)                     {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_HID, TRACE_TYPE_EVENT, m);}
#define HIDD_TRACE_EVENT1(m,p1)                  {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_HID, TRACE_TYPE_EVENT, m, p1);}
#define HIDD_TRACE_EVENT2(m,p1,p2)               {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_HID, TRACE_TYPE_EVENT, m,p1,p2);}
#define HIDD_TRACE_EVENT3(m,p1,p2,p3)            {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_HID, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define HIDD_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_HID, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define HIDD_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_HID, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define HIDD_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_HID, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define HIDD_TRACE_DEBUG0(m)                     {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_HID, TRACE_TYPE_DEBUG, m);}
#define HIDD_TRACE_DEBUG1(m,p1)                  {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_HID, TRACE_TYPE_DEBUG, m,p1);}
#define HIDD_TRACE_DEBUG2(m,p1,p2)               {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_HID, TRACE_TYPE_DEBUG, m,p1,p2);}
#define HIDD_TRACE_DEBUG3(m,p1,p2,p3)            {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_HID, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define HIDD_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_HID, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define HIDD_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_HID, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define HIDD_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (hidd_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_HID, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

/* define traces for headset profile */
#define HSP2_TRACE_ERROR0(pcb,m)                     {if (pcb->trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_HSP2, TRACE_TYPE_ERROR, m);}
#define HSP2_TRACE_ERROR1(pcb,m,p1)                  {if (pcb->trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_HSP2, TRACE_TYPE_ERROR, m, p1);}
#define HSP2_TRACE_ERROR2(pcb,m,p1,p2)               {if (pcb->trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_HSP2, TRACE_TYPE_ERROR, m,p1,p2);}
#define HSP2_TRACE_ERROR3(pcb,m,p1,p2,p3)            {if (pcb->trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_HSP2, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define HSP2_TRACE_ERROR4(pcb,m,p1,p2,p3,p4)         {if (pcb->trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_HSP2, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define HSP2_TRACE_ERROR5(pcb,m,p1,p2,p3,p4,p5)      {if (pcb->trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_HSP2, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define HSP2_TRACE_ERROR6(pcb,m,p1,p2,p3,p4,p5,p6)   {if (pcb->trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_HSP2, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define HSP2_TRACE_WARNING0(pcb,m)                   {if (pcb->trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_HSP2, TRACE_TYPE_WARNING, m);}
#define HSP2_TRACE_WARNING1(pcb,m,p1)                {if (pcb->trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_HSP2, TRACE_TYPE_WARNING, m,p1);}
#define HSP2_TRACE_WARNING2(pcb,m,p1,p2)             {if (pcb->trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_HSP2, TRACE_TYPE_WARNING, m,p1,p2);}
#define HSP2_TRACE_WARNING3(pcb,m,p1,p2,p3)          {if (pcb->trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_HSP2, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define HSP2_TRACE_WARNING4(pcb,m,p1,p2,p3,p4)       {if (pcb->trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_HSP2, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define HSP2_TRACE_WARNING5(pcb,m,p1,p2,p3,p4,p5)    {if (pcb->trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_HSP2, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define HSP2_TRACE_WARNING6(pcb,m,p1,p2,p3,p4,p5,p6) {if (pcb->trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_HSP2, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define HSP2_TRACE_API0(pcb,m)                       {if (pcb->trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_HSP2, TRACE_TYPE_API, m);}
#define HSP2_TRACE_API1(pcb,m,p1)                    {if (pcb->trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_HSP2, TRACE_TYPE_API, m, p1);}
#define HSP2_TRACE_API2(pcb,m,p1,p2)                 {if (pcb->trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_HSP2, TRACE_TYPE_API, m,p1,p2);}
#define HSP2_TRACE_API3(pcb,m,p1,p2,p3)              {if (pcb->trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_HSP2, TRACE_TYPE_API, m,p1,p2,p3);}
#define HSP2_TRACE_API4(pcb,m,p1,p2,p3,p4)           {if (pcb->trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_HSP2, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define HSP2_TRACE_API5(pcb,m,p1,p2,p3,p4,p5)        {if (pcb->trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_HSP2, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define HSP2_TRACE_API6(pcb,m,p1,p2,p3,p4,p5,p6)     {if (pcb->trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_HSP2, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define HSP2_TRACE_EVENT0(pcb,m)                     {if (pcb->trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_HSP2, TRACE_TYPE_EVENT, m);}
#define HSP2_TRACE_EVENT1(pcb,m,p1)                  {if (pcb->trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_HSP2, TRACE_TYPE_EVENT, m, p1);}
#define HSP2_TRACE_EVENT2(pcb,m,p1,p2)               {if (pcb->trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_HSP2, TRACE_TYPE_EVENT, m,p1,p2);}
#define HSP2_TRACE_EVENT3(pcb,m,p1,p2,p3)            {if (pcb->trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_HSP2, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define HSP2_TRACE_EVENT4(pcb,m,p1,p2,p3,p4)         {if (pcb->trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_HSP2, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define HSP2_TRACE_EVENT5(pcb,m,p1,p2,p3,p4,p5)      {if (pcb->trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_HSP2, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define HSP2_TRACE_EVENT6(pcb,m,p1,p2,p3,p4,p5,p6)   {if (pcb->trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_HSP2, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define HSP2_TRACE_DEBUG0(pcb,m)                     {if (pcb->trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_HSP2, TRACE_TYPE_DEBUG, m);}
#define HSP2_TRACE_DEBUG1(pcb,m,p1)                  {if (pcb->trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_HSP2, TRACE_TYPE_DEBUG, m,p1);}
#define HSP2_TRACE_DEBUG2(pcb,m,p1,p2)               {if (pcb->trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_HSP2, TRACE_TYPE_DEBUG, m,p1,p2);}
#define HSP2_TRACE_DEBUG3(pcb,m,p1,p2,p3)            {if (pcb->trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_HSP2, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define HSP2_TRACE_DEBUG4(pcb,m,p1,p2,p3,p4)         {if (pcb->trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_HSP2, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define HSP2_TRACE_DEBUG5(pcb,m,p1,p2,p3,p4,p5)      {if (pcb->trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_HSP2, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define HSP2_TRACE_DEBUG6(pcb,m,p1,p2,p3,p4,p5,p6)   {if (pcb->trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_HSP2, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

/* Define tracing for the NFC unit
*/
#define NFC_TRACE_ERROR0(m)                     {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_NFC, TRACE_TYPE_ERROR, m);}
#define NFC_TRACE_ERROR1(m,p1)                  {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_NFC, TRACE_TYPE_ERROR, m,p1);}
#define NFC_TRACE_ERROR2(m,p1,p2)               {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_NFC, TRACE_TYPE_ERROR, m,p1,p2);}
#define NFC_TRACE_ERROR3(m,p1,p2,p3)            {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_NFC, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define NFC_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_NFC, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define NFC_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_NFC, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define NFC_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_NFC, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define NFC_TRACE_WARNING0(m)                   {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_NFC, TRACE_TYPE_WARNING, m);}
#define NFC_TRACE_WARNING1(m,p1)                {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_NFC, TRACE_TYPE_WARNING, m,p1);}
#define NFC_TRACE_WARNING2(m,p1,p2)             {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_NFC, TRACE_TYPE_WARNING, m,p1,p2);}
#define NFC_TRACE_WARNING3(m,p1,p2,p3)          {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_NFC, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define NFC_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_NFC, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define NFC_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_NFC, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define NFC_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_NFC, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define NFC_TRACE_API0(m)                       {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_NFC, TRACE_TYPE_API, m);}
#define NFC_TRACE_API1(m,p1)                    {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_NFC, TRACE_TYPE_API, m,p1);}
#define NFC_TRACE_API2(m,p1,p2)                 {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_NFC, TRACE_TYPE_API, m,p1,p2);}
#define NFC_TRACE_API3(m,p1,p2,p3)              {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_NFC, TRACE_TYPE_API, m,p1,p2,p3);}
#define NFC_TRACE_API4(m,p1,p2,p3,p4)           {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_NFC, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define NFC_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_NFC, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define NFC_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_NFC, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define NFC_TRACE_EVENT0(m)                     {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_NFC, TRACE_TYPE_EVENT, m);}
#define NFC_TRACE_EVENT1(m,p1)                  {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_NFC, TRACE_TYPE_EVENT, m, p1);}
#define NFC_TRACE_EVENT2(m,p1,p2)               {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_NFC, TRACE_TYPE_EVENT, m,p1,p2);}
#define NFC_TRACE_EVENT3(m,p1,p2,p3)            {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_NFC, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define NFC_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_NFC, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define NFC_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_NFC, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define NFC_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_NFC, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define NFC_TRACE_DEBUG0(m)                     {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_NFC, TRACE_TYPE_DEBUG, m);}
#define NFC_TRACE_DEBUG1(m,p1)                  {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_NFC, TRACE_TYPE_DEBUG, m,p1);}
#define NFC_TRACE_DEBUG2(m,p1,p2)               {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_NFC, TRACE_TYPE_DEBUG, m,p1,p2);}
#define NFC_TRACE_DEBUG3(m,p1,p2,p3)            {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_NFC, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define NFC_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_NFC, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define NFC_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_NFC, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define NFC_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (nfc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_NFC, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

#define RW_TRACE_ERROR0(m)                     {if (rw_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_RW, TRACE_TYPE_ERROR, m);}
#define RW_TRACE_ERROR1(m,p1)                  {if (rw_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_RW, TRACE_TYPE_ERROR, m,p1);}
#define RW_TRACE_ERROR2(m,p1,p2)               {if (rw_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_RW, TRACE_TYPE_ERROR, m,p1,p2);}
#define RW_TRACE_ERROR3(m,p1,p2,p3)            {if (rw_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_RW, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define RW_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (rw_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_RW, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define RW_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (rw_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_RW, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define RW_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (rw_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_RW, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define RW_TRACE_WARNING0(m)                   {if (rw_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_RW, TRACE_TYPE_WARNING, m);}
#define RW_TRACE_WARNING1(m,p1)                {if (rw_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_RW, TRACE_TYPE_WARNING, m,p1);}
#define RW_TRACE_WARNING2(m,p1,p2)             {if (rw_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_RW, TRACE_TYPE_WARNING, m,p1,p2);}
#define RW_TRACE_WARNING3(m,p1,p2,p3)          {if (rw_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_RW, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define RW_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (rw_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_RW, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define RW_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (rw_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_RW, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define RW_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (rw_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_RW, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define RW_TRACE_API0(m)                       {if (rw_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_RW, TRACE_TYPE_API, m);}
#define RW_TRACE_API1(m,p1)                    {if (rw_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_RW, TRACE_TYPE_API, m,p1);}
#define RW_TRACE_API2(m,p1,p2)                 {if (rw_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_RW, TRACE_TYPE_API, m,p1,p2);}
#define RW_TRACE_API3(m,p1,p2,p3)              {if (rw_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_RW, TRACE_TYPE_API, m,p1,p2,p3);}
#define RW_TRACE_API4(m,p1,p2,p3,p4)           {if (rw_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_RW, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define RW_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (rw_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_RW, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define RW_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (rw_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_RW, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define RW_TRACE_EVENT0(m)                     {if (rw_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_RW, TRACE_TYPE_EVENT, m);}
#define RW_TRACE_EVENT1(m,p1)                  {if (rw_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_RW, TRACE_TYPE_EVENT, m, p1);}
#define RW_TRACE_EVENT2(m,p1,p2)               {if (rw_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_RW, TRACE_TYPE_EVENT, m,p1,p2);}
#define RW_TRACE_EVENT3(m,p1,p2,p3)            {if (rw_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_RW, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define RW_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (rw_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_RW, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define RW_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (rw_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_RW, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define RW_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (rw_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_RW, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define RW_TRACE_DEBUG0(m)                     {if (rw_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_RW, TRACE_TYPE_DEBUG, m);}
#define RW_TRACE_DEBUG1(m,p1)                  {if (rw_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_RW, TRACE_TYPE_DEBUG, m,p1);}
#define RW_TRACE_DEBUG2(m,p1,p2)               {if (rw_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_RW, TRACE_TYPE_DEBUG, m,p1,p2);}
#define RW_TRACE_DEBUG3(m,p1,p2,p3)            {if (rw_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_RW, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define RW_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (rw_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_RW, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define RW_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (rw_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_RW, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define RW_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (rw_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_RW, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

#define CE_TRACE_ERROR0(m)                     {if (ce_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_CE, TRACE_TYPE_ERROR, m);}
#define CE_TRACE_ERROR1(m,p1)                  {if (ce_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_CE, TRACE_TYPE_ERROR, m,p1);}
#define CE_TRACE_ERROR2(m,p1,p2)               {if (ce_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_CE, TRACE_TYPE_ERROR, m,p1,p2);}
#define CE_TRACE_ERROR3(m,p1,p2,p3)            {if (ce_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_CE, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define CE_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (ce_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_CE, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define CE_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (ce_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_CE, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define CE_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (ce_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_CE, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define CE_TRACE_WARNING0(m)                   {if (ce_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_CE, TRACE_TYPE_WARNING, m);}
#define CE_TRACE_WARNING1(m,p1)                {if (ce_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_CE, TRACE_TYPE_WARNING, m,p1);}
#define CE_TRACE_WARNING2(m,p1,p2)             {if (ce_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_CE, TRACE_TYPE_WARNING, m,p1,p2);}
#define CE_TRACE_WARNING3(m,p1,p2,p3)          {if (ce_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_CE, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define CE_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (ce_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_CE, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define CE_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (ce_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_CE, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define CE_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (ce_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_CE, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define CE_TRACE_API0(m)                       {if (ce_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_CE, TRACE_TYPE_API, m);}
#define CE_TRACE_API1(m,p1)                    {if (ce_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_CE, TRACE_TYPE_API, m,p1);}
#define CE_TRACE_API2(m,p1,p2)                 {if (ce_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_CE, TRACE_TYPE_API, m,p1,p2);}
#define CE_TRACE_API3(m,p1,p2,p3)              {if (ce_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_CE, TRACE_TYPE_API, m,p1,p2,p3);}
#define CE_TRACE_API4(m,p1,p2,p3,p4)           {if (ce_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_CE, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define CE_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (ce_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_CE, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define CE_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (ce_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_CE, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define CE_TRACE_EVENT0(m)                     {if (ce_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_CE, TRACE_TYPE_EVENT, m);}
#define CE_TRACE_EVENT1(m,p1)                  {if (ce_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_CE, TRACE_TYPE_EVENT, m, p1);}
#define CE_TRACE_EVENT2(m,p1,p2)               {if (ce_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_CE, TRACE_TYPE_EVENT, m,p1,p2);}
#define CE_TRACE_EVENT3(m,p1,p2,p3)            {if (ce_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_CE, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define CE_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (ce_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_CE, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define CE_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (ce_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_CE, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define CE_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (ce_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_CE, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define CE_TRACE_DEBUG0(m)                     {if (ce_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_CE, TRACE_TYPE_DEBUG, m);}
#define CE_TRACE_DEBUG1(m,p1)                  {if (ce_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_CE, TRACE_TYPE_DEBUG, m,p1);}
#define CE_TRACE_DEBUG2(m,p1,p2)               {if (ce_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_CE, TRACE_TYPE_DEBUG, m,p1,p2);}
#define CE_TRACE_DEBUG3(m,p1,p2,p3)            {if (ce_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_CE, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define CE_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (ce_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_CE, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define CE_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (ce_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_CE, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define CE_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (ce_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_CE, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

#define NDEF_TRACE_ERROR0(m)                     {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_NDEF, TRACE_TYPE_ERROR, m);}
#define NDEF_TRACE_ERROR1(m,p1)                  {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_NDEF, TRACE_TYPE_ERROR, m,p1);}
#define NDEF_TRACE_ERROR2(m,p1,p2)               {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_NDEF, TRACE_TYPE_ERROR, m,p1,p2);}
#define NDEF_TRACE_ERROR3(m,p1,p2,p3)            {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_NDEF, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define NDEF_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_NDEF, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define NDEF_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_NDEF, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define NDEF_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_NDEF, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define NDEF_TRACE_WARNING0(m)                   {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_NDEF, TRACE_TYPE_WARNING, m);}
#define NDEF_TRACE_WARNING1(m,p1)                {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_NDEF, TRACE_TYPE_WARNING, m,p1);}
#define NDEF_TRACE_WARNING2(m,p1,p2)             {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_NDEF, TRACE_TYPE_WARNING, m,p1,p2);}
#define NDEF_TRACE_WARNING3(m,p1,p2,p3)          {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_NDEF, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define NDEF_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_NDEF, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define NDEF_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_NDEF, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define NDEF_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_NDEF, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define NDEF_TRACE_API0(m)                       {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_NDEF, TRACE_TYPE_API, m);}
#define NDEF_TRACE_API1(m,p1)                    {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_NDEF, TRACE_TYPE_API, m,p1);}
#define NDEF_TRACE_API2(m,p1,p2)                 {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_NDEF, TRACE_TYPE_API, m,p1,p2);}
#define NDEF_TRACE_API3(m,p1,p2,p3)              {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_NDEF, TRACE_TYPE_API, m,p1,p2,p3);}
#define NDEF_TRACE_API4(m,p1,p2,p3,p4)           {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_NDEF, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define NDEF_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_NDEF, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define NDEF_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_NDEF, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define NDEF_TRACE_EVENT0(m)                     {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_NDEF, TRACE_TYPE_EVENT, m);}
#define NDEF_TRACE_EVENT1(m,p1)                  {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_NDEF, TRACE_TYPE_EVENT, m, p1);}
#define NDEF_TRACE_EVENT2(m,p1,p2)               {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_NDEF, TRACE_TYPE_EVENT, m,p1,p2);}
#define NDEF_TRACE_EVENT3(m,p1,p2,p3)            {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_NDEF, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define NDEF_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_NDEF, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define NDEF_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_NDEF, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define NDEF_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_NDEF, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define NDEF_TRACE_DEBUG0(m)                     {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_NDEF, TRACE_TYPE_DEBUG, m);}
#define NDEF_TRACE_DEBUG1(m,p1)                  {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_NDEF, TRACE_TYPE_DEBUG, m,p1);}
#define NDEF_TRACE_DEBUG2(m,p1,p2)               {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_NDEF, TRACE_TYPE_DEBUG, m,p1,p2);}
#define NDEF_TRACE_DEBUG3(m,p1,p2,p3)            {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_NDEF, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define NDEF_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_NDEF, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define NDEF_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_NDEF, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define NDEF_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (ndef_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_NDEF, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

/* Define tracing for the NFA unit
*/
#define NFA_TRACE_ERROR0(m)                     {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_NFA, TRACE_TYPE_ERROR, m);}
#define NFA_TRACE_ERROR1(m,p1)                  {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_NFA, TRACE_TYPE_ERROR, m,p1);}
#define NFA_TRACE_ERROR2(m,p1,p2)               {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_NFA, TRACE_TYPE_ERROR, m,p1,p2);}
#define NFA_TRACE_ERROR3(m,p1,p2,p3)            {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_NFA, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define NFA_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_NFA, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define NFA_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_NFA, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define NFA_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_NFA, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define NFA_TRACE_WARNING0(m)                   {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_NFA, TRACE_TYPE_WARNING, m);}
#define NFA_TRACE_WARNING1(m,p1)                {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_NFA, TRACE_TYPE_WARNING, m,p1);}
#define NFA_TRACE_WARNING2(m,p1,p2)             {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_NFA, TRACE_TYPE_WARNING, m,p1,p2);}
#define NFA_TRACE_WARNING3(m,p1,p2,p3)          {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_NFA, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define NFA_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_NFA, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define NFA_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_NFA, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define NFA_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_NFA, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define NFA_TRACE_API0(m)                       {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_NFA, TRACE_TYPE_API, m);}
#define NFA_TRACE_API1(m,p1)                    {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_NFA, TRACE_TYPE_API, m,p1);}
#define NFA_TRACE_API2(m,p1,p2)                 {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_NFA, TRACE_TYPE_API, m,p1,p2);}
#define NFA_TRACE_API3(m,p1,p2,p3)              {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_NFA, TRACE_TYPE_API, m,p1,p2,p3);}
#define NFA_TRACE_API4(m,p1,p2,p3,p4)           {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_NFA, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define NFA_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_NFA, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define NFA_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_NFA, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define NFA_TRACE_EVENT0(m)                     {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_NFA, TRACE_TYPE_EVENT, m);}
#define NFA_TRACE_EVENT1(m,p1)                  {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_NFA, TRACE_TYPE_EVENT, m, p1);}
#define NFA_TRACE_EVENT2(m,p1,p2)               {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_NFA, TRACE_TYPE_EVENT, m,p1,p2);}
#define NFA_TRACE_EVENT3(m,p1,p2,p3)            {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_NFA, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define NFA_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_NFA, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define NFA_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_NFA, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define NFA_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_NFA, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define NFA_TRACE_DEBUG0(m)                     {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_NFA, TRACE_TYPE_DEBUG, m);}
#define NFA_TRACE_DEBUG1(m,p1)                  {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_NFA, TRACE_TYPE_DEBUG, m,p1);}
#define NFA_TRACE_DEBUG2(m,p1,p2)               {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_NFA, TRACE_TYPE_DEBUG, m,p1,p2);}
#define NFA_TRACE_DEBUG3(m,p1,p2,p3)            {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_NFA, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define NFA_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_NFA, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define NFA_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_NFA, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define NFA_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (nfa_sys_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_NFA, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

/* Define tracing for the NFA P2P unit
*/
#define P2P_TRACE_ERROR0(m)                     {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_P2P, TRACE_TYPE_ERROR, m);}
#define P2P_TRACE_ERROR1(m,p1)                  {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_P2P, TRACE_TYPE_ERROR, m,p1);}
#define P2P_TRACE_ERROR2(m,p1,p2)               {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_P2P, TRACE_TYPE_ERROR, m,p1,p2);}
#define P2P_TRACE_ERROR3(m,p1,p2,p3)            {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_P2P, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define P2P_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_P2P, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define P2P_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_P2P, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define P2P_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_P2P, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define P2P_TRACE_WARNING0(m)                   {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_P2P, TRACE_TYPE_WARNING, m);}
#define P2P_TRACE_WARNING1(m,p1)                {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_P2P, TRACE_TYPE_WARNING, m,p1);}
#define P2P_TRACE_WARNING2(m,p1,p2)             {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_P2P, TRACE_TYPE_WARNING, m,p1,p2);}
#define P2P_TRACE_WARNING3(m,p1,p2,p3)          {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_P2P, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define P2P_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_P2P, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define P2P_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_P2P, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define P2P_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_P2P, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define P2P_TRACE_API0(m)                       {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_P2P, TRACE_TYPE_API, m);}
#define P2P_TRACE_API1(m,p1)                    {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_P2P, TRACE_TYPE_API, m,p1);}
#define P2P_TRACE_API2(m,p1,p2)                 {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_P2P, TRACE_TYPE_API, m,p1,p2);}
#define P2P_TRACE_API3(m,p1,p2,p3)              {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_P2P, TRACE_TYPE_API, m,p1,p2,p3);}
#define P2P_TRACE_API4(m,p1,p2,p3,p4)           {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_P2P, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define P2P_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_P2P, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define P2P_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_P2P, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define P2P_TRACE_EVENT0(m)                     {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_P2P, TRACE_TYPE_EVENT, m);}
#define P2P_TRACE_EVENT1(m,p1)                  {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_P2P, TRACE_TYPE_EVENT, m, p1);}
#define P2P_TRACE_EVENT2(m,p1,p2)               {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_P2P, TRACE_TYPE_EVENT, m,p1,p2);}
#define P2P_TRACE_EVENT3(m,p1,p2,p3)            {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_P2P, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define P2P_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_P2P, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define P2P_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_P2P, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define P2P_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_P2P, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define P2P_TRACE_DEBUG0(m)                     {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_P2P, TRACE_TYPE_DEBUG, m);}
#define P2P_TRACE_DEBUG1(m,p1)                  {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_P2P, TRACE_TYPE_DEBUG, m,p1);}
#define P2P_TRACE_DEBUG2(m,p1,p2)               {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_P2P, TRACE_TYPE_DEBUG, m,p1,p2);}
#define P2P_TRACE_DEBUG3(m,p1,p2,p3)            {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_P2P, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define P2P_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_P2P, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define P2P_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_P2P, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define P2P_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (nfa_p2p_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_P2P, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

/* Define tracing for the NFA CHO unit
*/
#define CHO_TRACE_ERROR0(m)                     {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_CHO, TRACE_TYPE_ERROR, m);}
#define CHO_TRACE_ERROR1(m,p1)                  {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_CHO, TRACE_TYPE_ERROR, m,p1);}
#define CHO_TRACE_ERROR2(m,p1,p2)               {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_CHO, TRACE_TYPE_ERROR, m,p1,p2);}
#define CHO_TRACE_ERROR3(m,p1,p2,p3)            {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_CHO, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define CHO_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_CHO, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define CHO_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_CHO, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define CHO_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_CHO, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define CHO_TRACE_WARNING0(m)                   {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_CHO, TRACE_TYPE_WARNING, m);}
#define CHO_TRACE_WARNING1(m,p1)                {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_CHO, TRACE_TYPE_WARNING, m,p1);}
#define CHO_TRACE_WARNING2(m,p1,p2)             {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_CHO, TRACE_TYPE_WARNING, m,p1,p2);}
#define CHO_TRACE_WARNING3(m,p1,p2,p3)          {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_CHO, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define CHO_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_CHO, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define CHO_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_CHO, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define CHO_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_CHO, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define CHO_TRACE_API0(m)                       {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_CHO, TRACE_TYPE_API, m);}
#define CHO_TRACE_API1(m,p1)                    {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_CHO, TRACE_TYPE_API, m,p1);}
#define CHO_TRACE_API2(m,p1,p2)                 {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_CHO, TRACE_TYPE_API, m,p1,p2);}
#define CHO_TRACE_API3(m,p1,p2,p3)              {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_CHO, TRACE_TYPE_API, m,p1,p2,p3);}
#define CHO_TRACE_API4(m,p1,p2,p3,p4)           {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_CHO, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define CHO_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_CHO, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define CHO_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_CHO, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define CHO_TRACE_EVENT0(m)                     {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_CHO, TRACE_TYPE_EVENT, m);}
#define CHO_TRACE_EVENT1(m,p1)                  {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_CHO, TRACE_TYPE_EVENT, m, p1);}
#define CHO_TRACE_EVENT2(m,p1,p2)               {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_CHO, TRACE_TYPE_EVENT, m,p1,p2);}
#define CHO_TRACE_EVENT3(m,p1,p2,p3)            {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_CHO, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define CHO_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_CHO, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define CHO_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_CHO, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define CHO_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_CHO, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define CHO_TRACE_DEBUG0(m)                     {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_CHO, TRACE_TYPE_DEBUG, m);}
#define CHO_TRACE_DEBUG1(m,p1)                  {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_CHO, TRACE_TYPE_DEBUG, m,p1);}
#define CHO_TRACE_DEBUG2(m,p1,p2)               {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_CHO, TRACE_TYPE_DEBUG, m,p1,p2);}
#define CHO_TRACE_DEBUG3(m,p1,p2,p3)            {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_CHO, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define CHO_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_CHO, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define CHO_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_CHO, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define CHO_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (nfa_cho_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_CHO, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

/* Define tracing for the NFA SNEP unit
*/
#define SNEP_TRACE_ERROR0(m)                     {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_SNEP, TRACE_TYPE_ERROR, m);}
#define SNEP_TRACE_ERROR1(m,p1)                  {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_SNEP, TRACE_TYPE_ERROR, m,p1);}
#define SNEP_TRACE_ERROR2(m,p1,p2)               {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_SNEP, TRACE_TYPE_ERROR, m,p1,p2);}
#define SNEP_TRACE_ERROR3(m,p1,p2,p3)            {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_SNEP, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define SNEP_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_SNEP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define SNEP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_SNEP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define SNEP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_SNEP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define SNEP_TRACE_WARNING0(m)                   {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_SNEP, TRACE_TYPE_WARNING, m);}
#define SNEP_TRACE_WARNING1(m,p1)                {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_SNEP, TRACE_TYPE_WARNING, m,p1);}
#define SNEP_TRACE_WARNING2(m,p1,p2)             {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_SNEP, TRACE_TYPE_WARNING, m,p1,p2);}
#define SNEP_TRACE_WARNING3(m,p1,p2,p3)          {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_SNEP, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define SNEP_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_SNEP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define SNEP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_SNEP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define SNEP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_SNEP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define SNEP_TRACE_API0(m)                       {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_SNEP, TRACE_TYPE_API, m);}
#define SNEP_TRACE_API1(m,p1)                    {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_SNEP, TRACE_TYPE_API, m,p1);}
#define SNEP_TRACE_API2(m,p1,p2)                 {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_SNEP, TRACE_TYPE_API, m,p1,p2);}
#define SNEP_TRACE_API3(m,p1,p2,p3)              {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_SNEP, TRACE_TYPE_API, m,p1,p2,p3);}
#define SNEP_TRACE_API4(m,p1,p2,p3,p4)           {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_SNEP, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define SNEP_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_SNEP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define SNEP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_SNEP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define SNEP_TRACE_EVENT0(m)                     {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_SNEP, TRACE_TYPE_EVENT, m);}
#define SNEP_TRACE_EVENT1(m,p1)                  {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_SNEP, TRACE_TYPE_EVENT, m, p1);}
#define SNEP_TRACE_EVENT2(m,p1,p2)               {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_SNEP, TRACE_TYPE_EVENT, m,p1,p2);}
#define SNEP_TRACE_EVENT3(m,p1,p2,p3)            {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_SNEP, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define SNEP_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_SNEP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define SNEP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_SNEP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define SNEP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_SNEP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define SNEP_TRACE_DEBUG0(m)                     {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_SNEP, TRACE_TYPE_DEBUG, m);}
#define SNEP_TRACE_DEBUG1(m,p1)                  {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_SNEP, TRACE_TYPE_DEBUG, m,p1);}
#define SNEP_TRACE_DEBUG2(m,p1,p2)               {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_SNEP, TRACE_TYPE_DEBUG, m,p1,p2);}
#define SNEP_TRACE_DEBUG3(m,p1,p2,p3)            {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_SNEP, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define SNEP_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_SNEP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define SNEP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_SNEP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define SNEP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (nfa_snep_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_SNEP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

#define MMI_TRACE_0(m)
#define MMI_TRACE_1(m,p1)
#define MMI_TRACE_2(m,p1,p2)
#define MMI_TRACE_3(m,p1,p2,p3)
#define MMI_TRACE_4(m,p1,p2,p3,p4)
#define MMI_TRACE_5(m,p1,p2,p3,p4,p5)
#define MMI_TRACE_6(m,p1,p2,p3,p4,p5,p6)

#define MMI_DEBUG_0(m)                           BT_TRACE_0(TRACE_LAYER_HID, TRACE_TYPE_DEBUG, m)
#define MMI_DEBUG_1(m,p1)                        BT_TRACE_1(TRACE_LAYER_HID, TRACE_TYPE_DEBUG, m,p1)
#define MMI_DEBUG_2(m,p1,p2)                     BT_TRACE_2(TRACE_LAYER_HID, TRACE_TYPE_DEBUG, m,p1,p2)
#define MMI_DEBUG_3(m,p1,p2,p3)                  BT_TRACE_3(TRACE_LAYER_HID, TRACE_TYPE_DEBUG, m,p1,p2,p3)
#define MMI_DEBUG_4(m,p1,p2,p3,p4)               BT_TRACE_4(TRACE_LAYER_HID, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4)
#define MMI_DEBUG_5(m,p1,p2,p3,p4,p5)            BT_TRACE_5(TRACE_LAYER_HID, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5)
#define MMI_DEBUG_6(m,p1,p2,p3,p4,p5,p6)         BT_TRACE_6(TRACE_LAYER_HID, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6)

#define MMI_WARNING_0(m)                         BT_TRACE_0(TRACE_LAYER_HID, TRACE_TYPE_WARNING, m)
#define MMI_WARNING_1(m,p1)                      BT_TRACE_1(TRACE_LAYER_HID, TRACE_TYPE_WARNING, m,p1)
#define MMI_WARNING_2(m,p1,p2)                   BT_TRACE_2(TRACE_LAYER_HID, TRACE_TYPE_WARNING, m,p1,p2)
#define MMI_WARNING_3(m,p1,p2,p3)                BT_TRACE_3(TRACE_LAYER_HID, TRACE_TYPE_WARNING, m,p1,p2,p3)
#define MMI_WARNING_4(m,p1,p2,p3,p4)             BT_TRACE_4(TRACE_LAYER_HID, TRACE_TYPE_WARNING, m,p1,p2,p3,p4)
#define MMI_WARNING_5(m,p1,p2,p3,p4,p5)          BT_TRACE_5(TRACE_LAYER_HID, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5)
#define MMI_WARNING_6(m,p1,p2,p3,p4,p5,p6)       BT_TRACE_6(TRACE_LAYER_HID, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6)

#define MMI_ERROR_0(m)                           BT_TRACE_0(TRACE_LAYER_HID, TRACE_TYPE_ERROR, m)
#define MMI_ERROR_1(m,p1)                        BT_TRACE_1(TRACE_LAYER_HID, TRACE_TYPE_ERROR, m,p1)
#define MMI_ERROR_2(m,p1,p2)                     BT_TRACE_2(TRACE_LAYER_HID, TRACE_TYPE_ERROR, m,p1,p2)
#define MMI_ERROR_3(m,p1,p2,p3)                  BT_TRACE_3(TRACE_LAYER_HID, TRACE_TYPE_ERROR, m,p1,p2,p3)
#define MMI_ERROR_4(m,p1,p2,p3,p4)               BT_TRACE_4(TRACE_LAYER_HID, TRACE_TYPE_ERROR, m,p1,p2,p3,p4)
#define MMI_ERROR_5(m,p1,p2,p3,p4,p5)            BT_TRACE_5(TRACE_LAYER_HID, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5)
#define MMI_ERROR_6(m,p1,p2,p3,p4,p5,p6)         BT_TRACE_6(TRACE_LAYER_HID, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6)

/* hid mouse module traces */

#define MSKB_TRACE_0(m)                         MMI_Echo(m)
#define MSKB_TRACE_1(m,p1)                      MMI_Echo(m,p1)
#define MSKB_TRACE_2(m,p1,p2)                   MMI_Echo(m,p1,p2)
#define MSKB_TRACE_3(m,p1,p2,p3)                MMI_Echo(m,p1,p2,p3)
#define MSKB_TRACE_4(m,p1,p2,p3,p4)             MMI_Echo(m,p1,p2,p3,p4)
#define MSKB_TRACE_5(m,p1,p2,p3,p4,p5)          MMI_Echo(m,p1,p2,p3,p4,p5)
#define MSKB_TRACE_6(m,p1,p2,p3,p4,p5,p6)       MMI_Echo(m,p1,p2,p3,p4,p5,p6)

#define MSKB_DEBUG_0(m)                         MMI_Echo(m)
#define MSKB_DEBUG_1(m,p1)                      MMI_Echo(m,p1)
#define MSKB_DEBUG_2(m,p1,p2)                   MMI_Echo(m,p1,p2)
#define MSKB_DEBUG_3(m,p1,p2,p3)                MMI_Echo(m,p1,p2,p3)
#define MSKB_DEBUG_4(m,p1,p2,p3,p4)             MMI_Echo(m,p1,p2,p3,p4)
#define MSKB_DEBUG_5(m,p1,p2,p3,p4,p5)          MMI_Echo(m,p1,p2,p3,p4,p5)
#define MSKB_DEBUG_6(m,p1,p2,p3,p4,p5,p6)       MMI_Echo(m,p1,p2,p3,p4,p5,p6)

#define MSKB_ERROR_0(m)                         MMI_Echo(m)
#define MSKB_ERROR_1(m,p1)                      MMI_Echo(m,p1)
#define MSKB_ERROR_2(m,p1,p2)                   MMI_Echo(m,p1,p2)
#define MSKB_ERROR_3(m,p1,p2,p3)                MMI_Echo(m,p1,p2,p3)
#define MSKB_ERROR_4(m,p1,p2,p3,p4)             MMI_Echo(m,p1,p2,p3,p4)
#define MSKB_ERROR_5(m,p1,p2,p3,p4,p5)          MMI_Echo(m,p1,p2,p3,p4,p5)
#define MSKB_ERROR_6(m,p1,p2,p3,p4,p5,p6)       MMI_Echo(m,p1,p2,p3,p4,p5,p6)

/* define traces for DUN */

#define DUN_TRACE_ERROR0(m)                     {if (dun_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_DUN, TRACE_TYPE_ERROR, m);}
#define DUN_TRACE_ERROR1(m,p1)                  {if (dun_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_DUN, TRACE_TYPE_ERROR, m, p1);}
#define DUN_TRACE_ERROR2(m,p1,p2)               {if (dun_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_DUN, TRACE_TYPE_ERROR, m,p1,p2);}
#define DUN_TRACE_ERROR3(m,p1,p2,p3)            {if (dun_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_DUN, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define DUN_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (dun_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_DUN, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define DUN_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (dun_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_DUN, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define DUN_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (dun_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_DUN, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define DUN_TRACE_WARNING0(m)                   {if (dun_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_DUN, TRACE_TYPE_WARNING, m);}
#define DUN_TRACE_WARNING1(m,p1)                {if (dun_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_DUN, TRACE_TYPE_WARNING, m,p1);}
#define DUN_TRACE_WARNING2(m,p1,p2)             {if (dun_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_DUN, TRACE_TYPE_WARNING, m,p1,p2);}
#define DUN_TRACE_WARNING3(m,p1,p2,p3)          {if (dun_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_DUN, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define DUN_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (dun_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_DUN, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define DUN_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (dun_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_DUN, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define DUN_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (dun_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_DUN, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define DUN_TRACE_API0(m)                       {if (dun_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_DUN, TRACE_TYPE_API, m);}
#define DUN_TRACE_API1(m,p1)                    {if (dun_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_DUN, TRACE_TYPE_API, m, p1);}
#define DUN_TRACE_API2(m,p1,p2)                 {if (dun_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_DUN, TRACE_TYPE_API, m,p1,p2);}
#define DUN_TRACE_API3(m,p1,p2,p3)              {if (dun_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_DUN, TRACE_TYPE_API, m,p1,p2,p3);}
#define DUN_TRACE_API4(m,p1,p2,p3,p4)           {if (dun_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_DUN, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define DUN_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (dun_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_DUN, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define DUN_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (dun_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_DUN, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define DUN_TRACE_EVENT0(m)                     {if (dun_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_DUN, TRACE_TYPE_EVENT, m);}
#define DUN_TRACE_EVENT1(m,p1)                  {if (dun_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_DUN, TRACE_TYPE_EVENT, m, p1);}
#define DUN_TRACE_EVENT2(m,p1,p2)               {if (dun_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_DUN, TRACE_TYPE_EVENT, m,p1,p2);}
#define DUN_TRACE_EVENT3(m,p1,p2,p3)            {if (dun_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_DUN, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define DUN_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (dun_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_DUN, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define DUN_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (dun_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_DUN, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define DUN_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (dun_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_DUN, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define DUN_TRACE_DEBUG0(m)                     {if (dun_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_DUN, TRACE_TYPE_DEBUG, m);}
#define DUN_TRACE_DEBUG1(m,p1)                  {if (dun_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_DUN, TRACE_TYPE_DEBUG, m,p1);}
#define DUN_TRACE_DEBUG2(m,p1,p2)               {if (dun_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_DUN, TRACE_TYPE_DEBUG, m,p1,p2);}
#define DUN_TRACE_DEBUG3(m,p1,p2,p3)            {if (dun_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_DUN, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define DUN_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (dun_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_DUN, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define DUN_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (dun_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_DUN, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define DUN_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (dun_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_DUN, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}


/* define traces for RPC */

#define RPC_TRACE_ERROR0(m)                      {if (rpc_trace_level >= BT_TRACE_LEVEL_ERROR) LogMsg_0(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_ERROR, (m));}
#define RPC_TRACE_ERROR1(m,p1)                   {if (rpc_trace_level >= BT_TRACE_LEVEL_ERROR) LogMsg_1(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_ERROR, \
                                                        (m), (UINTPTR)(p1));}
#define RPC_TRACE_ERROR2(m,p1,p2)                {if (rpc_trace_level >= BT_TRACE_LEVEL_ERROR) LogMsg_2(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_ERROR, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2));}
#define RPC_TRACE_ERROR3(m,p1,p2,p3)             {if (rpc_trace_level >= BT_TRACE_LEVEL_ERROR) LogMsg_3(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_ERROR, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3));}
#define RPC_TRACE_ERROR4(m,p1,p2,p3,p4)          {if (rpc_trace_level >= BT_TRACE_LEVEL_ERROR) LogMsg_4(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_ERROR, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4));}
#define RPC_TRACE_ERROR5(m,p1,p2,p3,p4,p5)       {if (rpc_trace_level >= BT_TRACE_LEVEL_ERROR) LogMsg_5(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_ERROR, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4), (UINTPTR)(p5));}
#define RPC_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)    {if (rpc_trace_level >= BT_TRACE_LEVEL_ERROR) LogMsg_6(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_ERROR, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4), (UINTPTR)(p5), (UINTPTR)(p6));}

#define RPC_TRACE_WARNING0(m)                    {if (rpc_trace_level >= BT_TRACE_LEVEL_WARNING) LogMsg_0(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_WARNING, (m));}
#define RPC_TRACE_WARNING1(m,p1)                 {if (rpc_trace_level >= BT_TRACE_LEVEL_WARNING) LogMsg_1(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_WARNING, \
                                                        (m), (UINTPTR)(p1));}
#define RPC_TRACE_WARNING2(m,p1,p2)              {if (rpc_trace_level >= BT_TRACE_LEVEL_WARNING) LogMsg_2(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_WARNING, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2));}
#define RPC_TRACE_WARNING3(m,p1,p2,p3)           {if (rpc_trace_level >= BT_TRACE_LEVEL_WARNING) LogMsg_3(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_WARNING, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3));}
#define RPC_TRACE_WARNING4(m,p1,p2,p3,p4)        {if (rpc_trace_level >= BT_TRACE_LEVEL_WARNING) LogMsg_4(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_WARNING, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4));}
#define RPC_TRACE_WARNING5(m,p1,p2,p3,p4,p5)     {if (rpc_trace_level >= BT_TRACE_LEVEL_WARNING) LogMsg_5(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_WARNING, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4), (UINTPTR)(p5));}
#define RPC_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)  {if (rpc_trace_level >= BT_TRACE_LEVEL_WARNING) LogMsg_6(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_WARNING, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4), (UINTPTR)(p5), (UINTPTR)(p6));}

#define RPC_TRACE_API0(m)                        {if (rpc_trace_level >= BT_TRACE_LEVEL_API) LogMsg_0(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_API, (m));}
#define RPC_TRACE_API1(m,p1)                     {if (rpc_trace_level >= BT_TRACE_LEVEL_API) LogMsg_1(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_API, \
                                                        (m), (UINTPTR)(p1));}
#define RPC_TRACE_API2(m,p1,p2)                  {if (rpc_trace_level >= BT_TRACE_LEVEL_API) LogMsg_2(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_API, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2));}
#define RPC_TRACE_API3(m,p1,p2,p3)               {if (rpc_trace_level >= BT_TRACE_LEVEL_API) LogMsg_3(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_API, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3));}
#define RPC_TRACE_API4(m,p1,p2,p3,p4)            {if (rpc_trace_level >= BT_TRACE_LEVEL_API) LogMsg_4(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_API, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4));}
#define RPC_TRACE_API5(m,p1,p2,p3,p4,p5)         {if (rpc_trace_level >= BT_TRACE_LEVEL_API) LogMsg_5(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_API, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4), (UINTPTR)(p5));}
#define RPC_TRACE_API6(m,p1,p2,p3,p4,p5,p6)      {if (rpc_trace_level >= BT_TRACE_LEVEL_API) LogMsg_6(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_API, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4), (UINTPTR)(p5), (UINTPTR)(p6));}

#define RPC_TRACE_EVENT0(m)                      {if (rpc_trace_level >= BT_TRACE_LEVEL_EVENT) LogMsg_0(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_EVENT, (m));}
#define RPC_TRACE_EVENT1(m,p1)                   {if (rpc_trace_level >= BT_TRACE_LEVEL_EVENT) LogMsg_1(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_EVENT, \
                                                        (m), (UINTPTR)(p1));}
#define RPC_TRACE_EVENT2(m,p1,p2)                {if (rpc_trace_level >= BT_TRACE_LEVEL_EVENT) LogMsg_2(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_EVENT, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2));}
#define RPC_TRACE_EVENT3(m,p1,p2,p3)             {if (rpc_trace_level >= BT_TRACE_LEVEL_EVENT) LogMsg_3(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_EVENT, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3));}
#define RPC_TRACE_EVENT4(m,p1,p2,p3,p4)          {if (rpc_trace_level >= BT_TRACE_LEVEL_EVENT) LogMsg_4(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_EVENT, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4));}
#define RPC_TRACE_EVENT5(m,p1,p2,p3,p4,p5)       {if (rpc_trace_level >= BT_TRACE_LEVEL_EVENT) LogMsg_5(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_EVENT, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4), (UINTPTR)(p5));}
#define RPC_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)    {if (rpc_trace_level >= BT_TRACE_LEVEL_EVENT) LogMsg_6(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_EVENT, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4), (UINTPTR)(p5), (UINTPTR)(p6));}

#define RPC_TRACE_DEBUG0(m)                      {if (rpc_trace_level >= BT_TRACE_LEVEL_DEBUG) LogMsg_0(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_DEBUG, (m));}
#define RPC_TRACE_DEBUG1(m,p1)                   {if (rpc_trace_level >= BT_TRACE_LEVEL_DEBUG) LogMsg_1(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_DEBUG, \
                                                        (m), (UINTPTR)(p1));}
#define RPC_TRACE_DEBUG2(m,p1,p2)                {if (rpc_trace_level >= BT_TRACE_LEVEL_DEBUG) LogMsg_2(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_DEBUG, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2));}
#define RPC_TRACE_DEBUG3(m,p1,p2,p3)             {if (rpc_trace_level >= BT_TRACE_LEVEL_DEBUG) LogMsg_3(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_DEBUG, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3));}
#define RPC_TRACE_DEBUG4(m,p1,p2,p3,p4)          {if (rpc_trace_level >= BT_TRACE_LEVEL_DEBUG) LogMsg_4(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_DEBUG, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4));}
#define RPC_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)       {if (rpc_trace_level >= BT_TRACE_LEVEL_DEBUG) LogMsg_5(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_DEBUG, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4), (UINTPTR)(p5));}
#define RPC_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)    {if (rpc_trace_level >= BT_TRACE_LEVEL_DEBUG) LogMsg_6(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_RPC | TRACE_TYPE_DEBUG, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4), (UINTPTR)(p5), (UINTPTR)(p6));}

/* define traces for BNEP */

#define BNEP_TRACE_ERROR0(m)                     {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_BNEP, TRACE_TYPE_ERROR, m);}
#define BNEP_TRACE_ERROR1(m,p1)                  {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_BNEP, TRACE_TYPE_ERROR, m, p1);}
#define BNEP_TRACE_ERROR2(m,p1,p2)               {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_BNEP, TRACE_TYPE_ERROR, m,p1,p2);}
#define BNEP_TRACE_ERROR3(m,p1,p2,p3)            {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_BNEP, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define BNEP_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_BNEP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define BNEP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_BNEP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define BNEP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_BNEP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define BNEP_TRACE_WARNING0(m)                   {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_BNEP, TRACE_TYPE_WARNING, m);}
#define BNEP_TRACE_WARNING1(m,p1)                {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_BNEP, TRACE_TYPE_WARNING, m,p1);}
#define BNEP_TRACE_WARNING2(m,p1,p2)             {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_BNEP, TRACE_TYPE_WARNING, m,p1,p2);}
#define BNEP_TRACE_WARNING3(m,p1,p2,p3)          {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_BNEP, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define BNEP_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_BNEP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define BNEP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_BNEP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define BNEP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_BNEP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define BNEP_TRACE_API0(m)                       {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_BNEP, TRACE_TYPE_API, m);}
#define BNEP_TRACE_API1(m,p1)                    {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_BNEP, TRACE_TYPE_API, m, p1);}
#define BNEP_TRACE_API2(m,p1,p2)                 {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_BNEP, TRACE_TYPE_API, m,p1,p2);}
#define BNEP_TRACE_API3(m,p1,p2,p3)              {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_BNEP, TRACE_TYPE_API, m,p1,p2,p3);}
#define BNEP_TRACE_API4(m,p1,p2,p3,p4)           {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_BNEP, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define BNEP_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_BNEP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define BNEP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_BNEP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define BNEP_TRACE_EVENT0(m)                     {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_BNEP, TRACE_TYPE_EVENT, m);}
#define BNEP_TRACE_EVENT1(m,p1)                  {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_BNEP, TRACE_TYPE_EVENT, m, p1);}
#define BNEP_TRACE_EVENT2(m,p1,p2)               {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_BNEP, TRACE_TYPE_EVENT, m,p1,p2);}
#define BNEP_TRACE_EVENT3(m,p1,p2,p3)            {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_BNEP, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define BNEP_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_BNEP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define BNEP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_BNEP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define BNEP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_BNEP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define BNEP_TRACE_DEBUG0(m)                     {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_BNEP, TRACE_TYPE_DEBUG, m);}
#define BNEP_TRACE_DEBUG1(m,p1)                  {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_BNEP, TRACE_TYPE_DEBUG, m,p1);}
#define BNEP_TRACE_DEBUG2(m,p1,p2)               {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_BNEP, TRACE_TYPE_DEBUG, m,p1,p2);}
#define BNEP_TRACE_DEBUG3(m,p1,p2,p3)            {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_BNEP, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define BNEP_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_BNEP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define BNEP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_BNEP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define BNEP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (bnep_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_BNEP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

/* define traces for PAN */

#define PAN_TRACE_ERROR0(m)                     {if (pan_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_PAN, TRACE_TYPE_ERROR, m);}
#define PAN_TRACE_ERROR1(m,p1)                  {if (pan_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_PAN, TRACE_TYPE_ERROR, m, p1);}
#define PAN_TRACE_ERROR2(m,p1,p2)               {if (pan_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_PAN, TRACE_TYPE_ERROR, m,p1,p2);}
#define PAN_TRACE_ERROR3(m,p1,p2,p3)            {if (pan_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_PAN, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define PAN_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (pan_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_PAN, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define PAN_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (pan_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_PAN, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define PAN_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (pan_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_PAN, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define PAN_TRACE_WARNING0(m)                   {if (pan_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_PAN, TRACE_TYPE_WARNING, m);}
#define PAN_TRACE_WARNING1(m,p1)                {if (pan_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_PAN, TRACE_TYPE_WARNING, m,p1);}
#define PAN_TRACE_WARNING2(m,p1,p2)             {if (pan_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_PAN, TRACE_TYPE_WARNING, m,p1,p2);}
#define PAN_TRACE_WARNING3(m,p1,p2,p3)          {if (pan_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_PAN, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define PAN_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (pan_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_PAN, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define PAN_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (pan_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_PAN, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define PAN_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (pan_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_PAN, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define PAN_TRACE_API0(m)                       {if (pan_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_PAN, TRACE_TYPE_API, m);}
#define PAN_TRACE_API1(m,p1)                    {if (pan_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_PAN, TRACE_TYPE_API, m, p1);}
#define PAN_TRACE_API2(m,p1,p2)                 {if (pan_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_PAN, TRACE_TYPE_API, m,p1,p2);}
#define PAN_TRACE_API3(m,p1,p2,p3)              {if (pan_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_PAN, TRACE_TYPE_API, m,p1,p2,p3);}
#define PAN_TRACE_API4(m,p1,p2,p3,p4)           {if (pan_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_PAN, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define PAN_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (pan_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_PAN, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define PAN_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (pan_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_PAN, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define PAN_TRACE_EVENT0(m)                     {if (pan_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_PAN, TRACE_TYPE_EVENT, m);}
#define PAN_TRACE_EVENT1(m,p1)                  {if (pan_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_PAN, TRACE_TYPE_EVENT, m, p1);}
#define PAN_TRACE_EVENT2(m,p1,p2)               {if (pan_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_PAN, TRACE_TYPE_EVENT, m,p1,p2);}
#define PAN_TRACE_EVENT3(m,p1,p2,p3)            {if (pan_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_PAN, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define PAN_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (pan_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_PAN, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define PAN_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (pan_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_PAN, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define PAN_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (pan_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_PAN, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define PAN_TRACE_DEBUG0(m)                     {if (pan_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_PAN, TRACE_TYPE_DEBUG, m);}
#define PAN_TRACE_DEBUG1(m,p1)                  {if (pan_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_PAN, TRACE_TYPE_DEBUG, m,p1);}
#define PAN_TRACE_DEBUG2(m,p1,p2)               {if (pan_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_PAN, TRACE_TYPE_DEBUG, m,p1,p2);}
#define PAN_TRACE_DEBUG3(m,p1,p2,p3)            {if (pan_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_PAN, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define PAN_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (pan_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_PAN, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define PAN_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (pan_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_PAN, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define PAN_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (pan_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_PAN, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

/* define traces for SIM */

#define SAP_TRACE_ERROR0(m)                     {if (sap_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_SAP, TRACE_TYPE_ERROR, m);}
#define SAP_TRACE_ERROR1(m,p1)                  {if (sap_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_SAP, TRACE_TYPE_ERROR, m, p1);}
#define SAP_TRACE_ERROR2(m,p1,p2)               {if (sap_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_SAP, TRACE_TYPE_ERROR, m,p1,p2);}
#define SAP_TRACE_ERROR3(m,p1,p2,p3)            {if (sap_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_SAP, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define SAP_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (sap_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_SAP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define SAP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (sap_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_SAP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define SAP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (sap_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_SAP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define SAP_TRACE_WARNING0(m)                   {if (sap_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_SAP, TRACE_TYPE_WARNING, m);}
#define SAP_TRACE_WARNING1(m,p1)                {if (sap_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_SAP, TRACE_TYPE_WARNING, m,p1);}
#define SAP_TRACE_WARNING2(m,p1,p2)             {if (sap_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_SAP, TRACE_TYPE_WARNING, m,p1,p2);}
#define SAP_TRACE_WARNING3(m,p1,p2,p3)          {if (sap_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_SAP, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define SAP_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (sap_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_SAP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define SAP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (sap_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_SAP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define SAP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (sap_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_SAP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define SAP_TRACE_API0(m)                       {if (sap_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_SAP, TRACE_TYPE_API, m);}
#define SAP_TRACE_API1(m,p1)                    {if (sap_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_SAP, TRACE_TYPE_API, m, p1);}
#define SAP_TRACE_API2(m,p1,p2)                 {if (sap_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_SAP, TRACE_TYPE_API, m,p1,p2);}
#define SAP_TRACE_API3(m,p1,p2,p3)              {if (sap_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_SAP, TRACE_TYPE_API, m,p1,p2,p3);}
#define SAP_TRACE_API4(m,p1,p2,p3,p4)           {if (sap_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_SAP, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define SAP_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (sap_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_SAP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define SAP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (sap_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_SAP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define SAP_TRACE_EVENT0(m)                     {if (sap_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_SAP, TRACE_TYPE_EVENT, m);}
#define SAP_TRACE_EVENT1(m,p1)                  {if (sap_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_SAP, TRACE_TYPE_EVENT, m, p1);}
#define SAP_TRACE_EVENT2(m,p1,p2)               {if (sap_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_SAP, TRACE_TYPE_EVENT, m,p1,p2);}
#define SAP_TRACE_EVENT3(m,p1,p2,p3)            {if (sap_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_SAP, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define SAP_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (sap_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_SAP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define SAP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (sap_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_SAP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define SAP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (sap_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_SAP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define SAP_TRACE_DEBUG0(m)                     {if (sap_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_SAP, TRACE_TYPE_DEBUG, m);}
#define SAP_TRACE_DEBUG1(m,p1)                  {if (sap_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_SAP, TRACE_TYPE_DEBUG, m,p1);}
#define SAP_TRACE_DEBUG2(m,p1,p2)               {if (sap_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_SAP, TRACE_TYPE_DEBUG, m,p1,p2);}
#define SAP_TRACE_DEBUG3(m,p1,p2,p3)            {if (sap_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_SAP, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define SAP_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (sap_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_SAP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define SAP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (sap_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_SAP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define SAP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (sap_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_SAP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

/* Define tracing for OPP profile
*/
#define OPP_TRACE_ERROR0(m)                      {if (opp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_OPP, TRACE_TYPE_ERROR, m);}
#define OPP_TRACE_ERROR1(m,p1)                   {if (opp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_OPP, TRACE_TYPE_ERROR, m,p1);}
#define OPP_TRACE_ERROR2(m,p1,p2)                {if (opp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_OPP, TRACE_TYPE_ERROR, m,p1,p2);}
#define OPP_TRACE_ERROR3(m,p1,p2,p3)             {if (opp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_OPP, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define OPP_TRACE_ERROR4(m,p1,p2,p3,p4)          {if (opp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_OPP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define OPP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)       {if (opp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_OPP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define OPP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)    {if (opp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_OPP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define OPP_TRACE_WARNING0(m)                    {if (opp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_OPP, TRACE_TYPE_WARNING, m);}
#define OPP_TRACE_WARNING1(m,p1)                 {if (opp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_OPP, TRACE_TYPE_WARNING, m,p1);}
#define OPP_TRACE_WARNING2(m,p1,p2)              {if (opp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_OPP, TRACE_TYPE_WARNING, m,p1,p2);}
#define OPP_TRACE_WARNING3(m,p1,p2,p3)           {if (opp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_OPP, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define OPP_TRACE_WARNING4(m,p1,p2,p3,p4)        {if (opp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_OPP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define OPP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)     {if (opp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_OPP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define OPP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)  {if (opp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_OPP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define OPP_TRACE_EVENT0(m)                      {if (opp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_OPP, TRACE_TYPE_EVENT, m);}
#define OPP_TRACE_EVENT1(m,p1)                   {if (opp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_OPP, TRACE_TYPE_EVENT, m, p1);}
#define OPP_TRACE_EVENT2(m,p1,p2)                {if (opp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_OPP, TRACE_TYPE_EVENT, m,p1,p2);}
#define OPP_TRACE_EVENT3(m,p1,p2,p3)             {if (opp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_OPP, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define OPP_TRACE_EVENT4(m,p1,p2,p3,p4)          {if (opp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_OPP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define OPP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)       {if (opp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_OPP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define OPP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)    {if (opp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_OPP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define OPP_TRACE_DEBUG0(m)                      {if (opp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_OPP, TRACE_TYPE_DEBUG, m);}
#define OPP_TRACE_DEBUG1(m,p1)                   {if (opp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_OPP, TRACE_TYPE_DEBUG, m,p1);}
#define OPP_TRACE_DEBUG2(m,p1,p2)                {if (opp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_OPP, TRACE_TYPE_DEBUG, m,p1,p2);}
#define OPP_TRACE_DEBUG3(m,p1,p2,p3)             {if (opp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_OPP, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define OPP_TRACE_DEBUG4(m,p1,p2,p3,p4)          {if (opp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_OPP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define OPP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)       {if (opp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_OPP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define OPP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)    {if (opp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_OPP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

/* Define tracing for FTP profile
*/
#define FTP_TRACE_ERROR0(m)                      {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_FTP, TRACE_TYPE_ERROR, m);}
#define FTP_TRACE_ERROR1(m,p1)                   {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_FTP, TRACE_TYPE_ERROR, m,p1);}
#define FTP_TRACE_ERROR2(m,p1,p2)                {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_FTP, TRACE_TYPE_ERROR, m,p1,p2);}
#define FTP_TRACE_ERROR3(m,p1,p2,p3)             {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_FTP, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define FTP_TRACE_ERROR4(m,p1,p2,p3,p4)          {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_FTP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define FTP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)       {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_FTP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define FTP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)    {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_FTP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define FTP_TRACE_WARNING0(m)                    {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_FTP, TRACE_TYPE_WARNING, m);}
#define FTP_TRACE_WARNING1(m,p1)                 {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_FTP, TRACE_TYPE_WARNING, m,p1);}
#define FTP_TRACE_WARNING2(m,p1,p2)              {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_FTP, TRACE_TYPE_WARNING, m,p1,p2);}
#define FTP_TRACE_WARNING3(m,p1,p2,p3)           {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_FTP, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define FTP_TRACE_WARNING4(m,p1,p2,p3,p4)        {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_FTP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define FTP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)     {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_FTP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define FTP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)  {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_FTP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define FTP_TRACE_EVENT0(m)                      {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_FTP, TRACE_TYPE_EVENT, m);}
#define FTP_TRACE_EVENT1(m,p1)                   {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_FTP, TRACE_TYPE_EVENT, m, p1);}
#define FTP_TRACE_EVENT2(m,p1,p2)                {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_FTP, TRACE_TYPE_EVENT, m,p1,p2);}
#define FTP_TRACE_EVENT3(m,p1,p2,p3)             {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_FTP, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define FTP_TRACE_EVENT4(m,p1,p2,p3,p4)          {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_FTP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define FTP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)       {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_FTP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define FTP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)    {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_FTP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define FTP_TRACE_DEBUG0(m)                      {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_FTP, TRACE_TYPE_DEBUG, m);}
#define FTP_TRACE_DEBUG1(m,p1)                   {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_FTP, TRACE_TYPE_DEBUG, m,p1);}
#define FTP_TRACE_DEBUG2(m,p1,p2)                {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_FTP, TRACE_TYPE_DEBUG, m,p1,p2);}
#define FTP_TRACE_DEBUG3(m,p1,p2,p3)             {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_FTP, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define FTP_TRACE_DEBUG4(m,p1,p2,p3,p4)          {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_FTP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define FTP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)       {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_FTP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define FTP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)    {if (ftp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_FTP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

/* Define tracing for the A2DP profile
*/
#define A2D_TRACE_ERROR0(m)                      {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_A2D, TRACE_TYPE_ERROR,m);}
#define A2D_TRACE_ERROR1(m,p1)                   {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_A2D, TRACE_TYPE_ERROR,m,p1);}
#define A2D_TRACE_ERROR2(m,p1,p2)                {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_A2D, TRACE_TYPE_ERROR,m,p1,p2);}
#define A2D_TRACE_ERROR3(m,p1,p2,p3)             {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_A2D, TRACE_TYPE_ERROR,m,p1,p2,p3);}
#define A2D_TRACE_ERROR4(m,p1,p2,p3,p4)          {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_A2D, TRACE_TYPE_ERROR,m,p1,p2,p3,p4);}
#define A2D_TRACE_ERROR5(m,p1,p2,p3,p4,p5)       {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_A2D, TRACE_TYPE_ERROR,m,p1,p2,p3,p4,p5);}
#define A2D_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)    {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_A2D, TRACE_TYPE_ERROR,m,p1,p2,p3,p4,p5,p6);}

#define A2D_TRACE_WARNING0(m)                    {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_A2D, TRACE_TYPE_WARNING,m);}
#define A2D_TRACE_WARNING1(m,p1)                 {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_A2D, TRACE_TYPE_WARNING,m,p1);}
#define A2D_TRACE_WARNING2(m,p1,p2)              {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_A2D, TRACE_TYPE_WARNING,m,p1,p2);}
#define A2D_TRACE_WARNING3(m,p1,p2,p3)           {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_A2D, TRACE_TYPE_WARNING,m,p1,p2,p3);}
#define A2D_TRACE_WARNING4(m,p1,p2,p3,p4)        {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_A2D, TRACE_TYPE_WARNING,m,p1,p2,p3,p4);}
#define A2D_TRACE_WARNING5(m,p1,p2,p3,p4,p5)     {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_A2D, TRACE_TYPE_WARNING,m,p1,p2,p3,p4,p5);}
#define A2D_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)  {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_A2D, TRACE_TYPE_WARNING,m,p1,p2,p3,p4,p5,p6);}

#define A2D_TRACE_EVENT0(m)                      {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_A2D, TRACE_TYPE_EVENT,m);}
#define A2D_TRACE_EVENT1(m,p1)                   {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_A2D, TRACE_TYPE_EVENT,m, p1);}
#define A2D_TRACE_EVENT2(m,p1,p2)                {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_A2D, TRACE_TYPE_EVENT,m,p1,p2);}
#define A2D_TRACE_EVENT3(m,p1,p2,p3)             {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_A2D, TRACE_TYPE_EVENT,m,p1,p2,p3);}
#define A2D_TRACE_EVENT4(m,p1,p2,p3,p4)          {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_A2D, TRACE_TYPE_EVENT,m,p1,p2,p3,p4);}
#define A2D_TRACE_EVENT5(m,p1,p2,p3,p4,p5)       {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_A2D, TRACE_TYPE_EVENT,m,p1,p2,p3,p4,p5);}
#define A2D_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)    {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_A2D, TRACE_TYPE_EVENT,m,p1,p2,p3,p4,p5,p6);}

#define A2D_TRACE_DEBUG0(m)                      {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_A2D, TRACE_TYPE_DEBUG,m);}
#define A2D_TRACE_DEBUG1(m,p1)                   {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_A2D, TRACE_TYPE_DEBUG,m,p1);}
#define A2D_TRACE_DEBUG2(m,p1,p2)                {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_A2D, TRACE_TYPE_DEBUG,m,p1,p2);}
#define A2D_TRACE_DEBUG3(m,p1,p2,p3)             {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_A2D, TRACE_TYPE_DEBUG,m,p1,p2,p3);}
#define A2D_TRACE_DEBUG4(m,p1,p2,p3,p4)          {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_A2D, TRACE_TYPE_DEBUG,m,p1,p2,p3,p4);}
#define A2D_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)       {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_A2D, TRACE_TYPE_DEBUG,m,p1,p2,p3,p4,p5);}
#define A2D_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)    {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_A2D, TRACE_TYPE_DEBUG,m,p1,p2,p3,p4,p5,p6);}

#define A2D_TRACE_API0(m)                        {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_A2D, TRACE_TYPE_API,m);}
#define A2D_TRACE_API1(m,p1)                     {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_A2D, TRACE_TYPE_API,m, p1);}
#define A2D_TRACE_API2(m,p1,p2)                  {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_A2D, TRACE_TYPE_API,m,p1,p2);}
#define A2D_TRACE_API3(m,p1,p2,p3)               {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_A2D, TRACE_TYPE_API,m,p1,p2,p3);}
#define A2D_TRACE_API4(m,p1,p2,p3,p4)            {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_A2D, TRACE_TYPE_API,m,p1,p2,p3,p4);}
#define A2D_TRACE_API5(m,p1,p2,p3,p4,p5)         {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_A2D, TRACE_TYPE_API,m,p1,p2,p3,p4,p5);}
#define A2D_TRACE_API6(m,p1,p2,p3,p4,p5,p6)      {if (a2d_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_A2D, TRACE_TYPE_API,m,p1,p2,p3,p4,p5,p6);}

/* Define tracing for the VDP profile
*/
#define VDP_TRACE_ERROR0(m)                      {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_AVP, TRACE_TYPE_ERROR,m);}
#define VDP_TRACE_ERROR1(m,p1)                   {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_AVP, TRACE_TYPE_ERROR,m,p1);}
#define VDP_TRACE_ERROR2(m,p1,p2)                {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_AVP, TRACE_TYPE_ERROR,m,p1,p2);}
#define VDP_TRACE_ERROR3(m,p1,p2,p3)             {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_AVP, TRACE_TYPE_ERROR,m,p1,p2,p3);}
#define VDP_TRACE_ERROR4(m,p1,p2,p3,p4)          {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_AVP, TRACE_TYPE_ERROR,m,p1,p2,p3,p4);}
#define VDP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)       {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_AVP, TRACE_TYPE_ERROR,m,p1,p2,p3,p4,p5);}
#define VDP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)    {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_AVP, TRACE_TYPE_ERROR,m,p1,p2,p3,p4,p5,p6);}

#define VDP_TRACE_WARNING0(m)                    {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_AVP, TRACE_TYPE_WARNING,m);}
#define VDP_TRACE_WARNING1(m,p1)                 {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_AVP, TRACE_TYPE_WARNING,m,p1);}
#define VDP_TRACE_WARNING2(m,p1,p2)              {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_AVP, TRACE_TYPE_WARNING,m,p1,p2);}
#define VDP_TRACE_WARNING3(m,p1,p2,p3)           {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_AVP, TRACE_TYPE_WARNING,m,p1,p2,p3);}
#define VDP_TRACE_WARNING4(m,p1,p2,p3,p4)        {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_AVP, TRACE_TYPE_WARNING,m,p1,p2,p3,p4);}
#define VDP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)     {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_AVP, TRACE_TYPE_WARNING,m,p1,p2,p3,p4,p5);}
#define VDP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)  {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_AVP, TRACE_TYPE_WARNING,m,p1,p2,p3,p4,p5,p6);}

#define VDP_TRACE_EVENT0(m)                      {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_AVP, TRACE_TYPE_EVENT,m);}
#define VDP_TRACE_EVENT1(m,p1)                   {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_AVP, TRACE_TYPE_EVENT,m, p1);}
#define VDP_TRACE_EVENT2(m,p1,p2)                {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_AVP, TRACE_TYPE_EVENT,m,p1,p2);}
#define VDP_TRACE_EVENT3(m,p1,p2,p3)             {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_AVP, TRACE_TYPE_EVENT,m,p1,p2,p3);}
#define VDP_TRACE_EVENT4(m,p1,p2,p3,p4)          {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_AVP, TRACE_TYPE_EVENT,m,p1,p2,p3,p4);}
#define VDP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)       {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_AVP, TRACE_TYPE_EVENT,m,p1,p2,p3,p4,p5);}
#define VDP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)    {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_AVP, TRACE_TYPE_EVENT,m,p1,p2,p3,p4,p5,p6);}

#define VDP_TRACE_DEBUG0(m)                      {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG,m);}
#define VDP_TRACE_DEBUG1(m,p1)                   {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG,m,p1);}
#define VDP_TRACE_DEBUG2(m,p1,p2)                {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG,m,p1,p2);}
#define VDP_TRACE_DEBUG3(m,p1,p2,p3)             {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG,m,p1,p2,p3);}
#define VDP_TRACE_DEBUG4(m,p1,p2,p3,p4)          {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG,m,p1,p2,p3,p4);}
#define VDP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)       {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG,m,p1,p2,p3,p4,p5);}
#define VDP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)    {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG,m,p1,p2,p3,p4,p5,p6);}

#define VDP_TRACE_API0(m)                        {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_AVP, TRACE_TYPE_API,m);}
#define VDP_TRACE_API1(m,p1)                     {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_AVP, TRACE_TYPE_API,m, p1);}
#define VDP_TRACE_API2(m,p1,p2)                  {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_AVP, TRACE_TYPE_API,m,p1,p2);}
#define VDP_TRACE_API3(m,p1,p2,p3)               {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_AVP, TRACE_TYPE_API,m,p1,p2,p3);}
#define VDP_TRACE_API4(m,p1,p2,p3,p4)            {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_AVP, TRACE_TYPE_API,m,p1,p2,p3,p4);}
#define VDP_TRACE_API5(m,p1,p2,p3,p4,p5)         {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_AVP, TRACE_TYPE_API,m,p1,p2,p3,p4,p5);}
#define VDP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)      {if (vdp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_AVP, TRACE_TYPE_API,m,p1,p2,p3,p4,p5,p6);}


/* Define tracing for the LM unit
*/
#define LMP_TRACE_ERROR0(m)                     {if (lmp_trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_LM, TRACE_TYPE_ERROR, m);}
#define LMP_TRACE_ERROR1(m,p1)                  {if (lmp_trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_LM, TRACE_TYPE_ERROR, m,p1);}
#define LMP_TRACE_ERROR2(m,p1,p2)               {if (lmp_trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_LM, TRACE_TYPE_ERROR, m,p1,p2);}
#define LMP_TRACE_ERROR3(m,p1,p2,p3)            {if (lmp_trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_LM, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define LMP_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (lmp_trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_LM, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define LMP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (lmp_trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_LM, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define LMP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (lmp_trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_LM, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define LMP_TRACE_WARNING0(m)                   {if (lmp_trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_LM, TRACE_TYPE_WARNING, m);}
#define LMP_TRACE_WARNING1(m,p1)                {if (lmp_trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_LM, TRACE_TYPE_WARNING, m,p1);}
#define LMP_TRACE_WARNING2(m,p1,p2)             {if (lmp_trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_LM, TRACE_TYPE_WARNING, m,p1,p2);}
#define LMP_TRACE_WARNING3(m,p1,p2,p3)          {if (lmp_trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_LM, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define LMP_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (lmp_trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_LM, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define LMP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (lmp_trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_LM, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define LMP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (lmp_trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_LM, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define LMP_TRACE_EVENT0(m)                     {if (lmp_trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_LM, TRACE_TYPE_EVENT, m);}
#define LMP_TRACE_EVENT1(m,p1)                  {if (lmp_trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_LM, TRACE_TYPE_EVENT, m, p1);}
#define LMP_TRACE_EVENT2(m,p1,p2)               {if (lmp_trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_LM, TRACE_TYPE_EVENT, m,p1,p2);}
#define LMP_TRACE_EVENT3(m,p1,p2,p3)            {if (lmp_trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_LM, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define LMP_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (lmp_trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_LM, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define LMP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (lmp_trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_LM, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define LMP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (lmp_trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_LM, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define LMP_TRACE_DEBUG0(m)                     {if (lmp_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_LM, TRACE_TYPE_DEBUG, m);}
#define LMP_TRACE_DEBUG1(m,p1)                  {if (lmp_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_LM, TRACE_TYPE_DEBUG, m,p1);}
#define LMP_TRACE_DEBUG2(m,p1,p2)               {if (lmp_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_LM, TRACE_TYPE_DEBUG, m,p1,p2);}
#define LMP_TRACE_DEBUG3(m,p1,p2,p3)            {if (lmp_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_LM, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define LMP_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (lmp_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_LM, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define LMP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (lmp_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_LM, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define LMP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (lmp_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_LM, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

/* Define tracing for the LC unit
*/
#define LC_TRACE_ERROR0(m)                     {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_ERROR]) BT_TRACE_0(TRACE_LAYER_LC, TRACE_TYPE_ERROR, m);}
#define LC_TRACE_ERROR1(m,p1)                  {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_ERROR]) BT_TRACE_1(TRACE_LAYER_LC, TRACE_TYPE_ERROR, m,p1);}
#define LC_TRACE_ERROR2(m,p1,p2)               {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_ERROR]) BT_TRACE_2(TRACE_LAYER_LC, TRACE_TYPE_ERROR, m,p1,p2);}
#define LC_TRACE_ERROR3(m,p1,p2,p3)            {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_ERROR]) BT_TRACE_3(TRACE_LAYER_LC, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define LC_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_ERROR]) BT_TRACE_4(TRACE_LAYER_LC, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define LC_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_ERROR]) BT_TRACE_5(TRACE_LAYER_LC, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define LC_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_ERROR]) BT_TRACE_6(TRACE_LAYER_LC, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define LC_TRACE_WARNING0(m)                   {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_WARNING]) BT_TRACE_0(TRACE_LAYER_LC, TRACE_TYPE_WARNING, m);}
#define LC_TRACE_WARNING1(m,p1)                {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_WARNING]) BT_TRACE_1(TRACE_LAYER_LC, TRACE_TYPE_WARNING, m,p1);}
#define LC_TRACE_WARNING2(m,p1,p2)             {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_WARNING]) BT_TRACE_2(TRACE_LAYER_LC, TRACE_TYPE_WARNING, m,p1,p2);}
#define LC_TRACE_WARNING3(m,p1,p2,p3)          {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_WARNING]) BT_TRACE_3(TRACE_LAYER_LC, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define LC_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_WARNING]) BT_TRACE_4(TRACE_LAYER_LC, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define LC_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_WARNING]) BT_TRACE_5(TRACE_LAYER_LC, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define LC_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_WARNING]) BT_TRACE_6(TRACE_LAYER_LC, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define LC_TRACE_EVENT0(m)                     {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_EVENT]) BT_TRACE_0(TRACE_LAYER_LC, TRACE_TYPE_EVENT, m);}
#define LC_TRACE_EVENT1(m,p1)                  {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_EVENT]) BT_TRACE_1(TRACE_LAYER_LC, TRACE_TYPE_EVENT, m, p1);}
#define LC_TRACE_EVENT2(m,p1,p2)               {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_EVENT]) BT_TRACE_2(TRACE_LAYER_LC, TRACE_TYPE_EVENT, m,p1,p2);}
#define LC_TRACE_EVENT3(m,p1,p2,p3)            {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_EVENT]) BT_TRACE_3(TRACE_LAYER_LC, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define LC_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_EVENT]) BT_TRACE_4(TRACE_LAYER_LC, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define LC_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_EVENT]) BT_TRACE_5(TRACE_LAYER_LC, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define LC_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_EVENT]) BT_TRACE_6(TRACE_LAYER_LC, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define LC_TRACE_DEBUG0(m)                     {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_DEBUG]) BT_TRACE_0(TRACE_LAYER_LC, TRACE_TYPE_DEBUG, m);}
#define LC_TRACE_DEBUG1(m,p1)                  {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_DEBUG]) BT_TRACE_1(TRACE_LAYER_LC, TRACE_TYPE_DEBUG, m,p1);}
#define LC_TRACE_DEBUG2(m,p1,p2)               {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_DEBUG]) BT_TRACE_2(TRACE_LAYER_LC, TRACE_TYPE_DEBUG, m,p1,p2);}
#define LC_TRACE_DEBUG3(m,p1,p2,p3)            {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_DEBUG]) BT_TRACE_3(TRACE_LAYER_LC, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define LC_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_DEBUG]) BT_TRACE_5(TRACE_LAYER_LC, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define LC_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (lc_trace_level & trace_map[BT_TRACE_LEVEL_DEBUG]) BT_TRACE_6(TRACE_LAYER_LC, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

/* Define tracing for the Serial Dongle Application SDA
*/
#define SDA_TRACE_ERROR0(m)                     {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(m);}
#define SDA_TRACE_ERROR1(m,p1)                  {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(m,p1);}
#define SDA_TRACE_ERROR2(m,p1,p2)               {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(m,p1,p2);}
#define SDA_TRACE_ERROR3(m,p1,p2,p3)            {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(m,p1,p2,p3);}
#define SDA_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(m,p1,p2,p3,p4);}
#define SDA_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(m,p1,p2,p3,p4,p5);}
#define SDA_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(m,p1,p2,p3,p4,p5,p6);}

#define SDA_TRACE_WARNING0(m)                   {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(m);}
#define SDA_TRACE_WARNING1(m,p1)                {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(m,p1);}
#define SDA_TRACE_WARNING2(m,p1,p2)             {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(m,p1,p2);}
#define SDA_TRACE_WARNING3(m,p1,p2,p3)          {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(m,p1,p2,p3);}
#define SDA_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(m,p1,p2,p3,p4);}
#define SDA_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(m,p1,p2,p3,p4,p5);}
#define SDA_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(m,p1,p2,p3,p4,p5,p6);}

#define SDA_TRACE_EVENT0(m)                     {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(m);}
#define SDA_TRACE_EVENT1(m,p1)                  {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(m, p1);}
#define SDA_TRACE_EVENT2(m,p1,p2)               {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(m,p1,p2);}
#define SDA_TRACE_EVENT3(m,p1,p2,p3)            {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(m,p1,p2,p3);}
#define SDA_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(m,p1,p2,p3,p4);}
#define SDA_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(m,p1,p2,p3,p4,p5);}
#define SDA_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(m,p1,p2,p3,p4,p5,p6);}

#define SDA_TRACE_DEBUG0(m)                     {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(m);}
#define SDA_TRACE_DEBUG1(m,p1)                  {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(m,p1);}
#define SDA_TRACE_DEBUG2(m,p1,p2)               {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(m,p1,p2);}
#define SDA_TRACE_DEBUG3(m,p1,p2,p3)            {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(m,p1,p2,p3);}
#define SDA_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(m,p1,p2,p3,p4);}
#define SDA_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(m,p1,p2,p3,p4,p5);}
#define SDA_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (sda_config_cb.sda_trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(m,p1,p2,p3,p4,p5,p6);}

/* AVDTP
*/
#define AVDT_TRACE_ERROR0(m)                     {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_AVP, TRACE_TYPE_ERROR, m);}
#define AVDT_TRACE_ERROR1(m,p1)                  {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_AVP, TRACE_TYPE_ERROR, m,p1);}
#define AVDT_TRACE_ERROR2(m,p1,p2)               {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_AVP, TRACE_TYPE_ERROR, m,p1,p2);}
#define AVDT_TRACE_ERROR3(m,p1,p2,p3)            {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_AVP, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define AVDT_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_AVP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define AVDT_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_AVP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define AVDT_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_AVP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define AVDT_TRACE_WARNING0(m)                   {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_AVP, TRACE_TYPE_WARNING, m);}
#define AVDT_TRACE_WARNING1(m,p1)                {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_AVP, TRACE_TYPE_WARNING, m,p1);}
#define AVDT_TRACE_WARNING2(m,p1,p2)             {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_AVP, TRACE_TYPE_WARNING, m,p1,p2);}
#define AVDT_TRACE_WARNING3(m,p1,p2,p3)          {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_AVP, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define AVDT_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_AVP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define AVDT_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_AVP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define AVDT_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_AVP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define AVDT_TRACE_EVENT0(m)                     {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_AVP, TRACE_TYPE_EVENT, m);}
#define AVDT_TRACE_EVENT1(m,p1)                  {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_AVP, TRACE_TYPE_EVENT, m, p1);}
#define AVDT_TRACE_EVENT2(m,p1,p2)               {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_AVP, TRACE_TYPE_EVENT, m,p1,p2);}
#define AVDT_TRACE_EVENT3(m,p1,p2,p3)            {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_AVP, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define AVDT_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_AVP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define AVDT_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_AVP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define AVDT_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_AVP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define AVDT_TRACE_DEBUG0(m)                     {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG, m);}
#define AVDT_TRACE_DEBUG1(m,p1)                  {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG, m,p1);}
#define AVDT_TRACE_DEBUG2(m,p1,p2)               {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG, m,p1,p2);}
#define AVDT_TRACE_DEBUG3(m,p1,p2,p3)            {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define AVDT_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define AVDT_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define AVDT_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

#define AVDT_TRACE_API0(m)                     {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_AVP, TRACE_TYPE_API, m);}
#define AVDT_TRACE_API1(m,p1)                  {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_AVP, TRACE_TYPE_API, m,p1);}
#define AVDT_TRACE_API2(m,p1,p2)               {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_AVP, TRACE_TYPE_API, m,p1,p2);}
#define AVDT_TRACE_API3(m,p1,p2,p3)            {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_AVP, TRACE_TYPE_API, m,p1,p2,p3);}
#define AVDT_TRACE_API4(m,p1,p2,p3,p4)         {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_AVP, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define AVDT_TRACE_API5(m,p1,p2,p3,p4,p5)      {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_AVP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define AVDT_TRACE_API6(m,p1,p2,p3,p4,p5,p6)   {if (avdt_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_AVP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

/* Define tracing for the AVCTP protocol
*/
#define AVCT_TRACE_ERROR0(m)                     {if (avct_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_AVP, TRACE_TYPE_ERROR, m);}
#define AVCT_TRACE_ERROR1(m,p1)                  {if (avct_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_AVP, TRACE_TYPE_ERROR, m,p1);}
#define AVCT_TRACE_ERROR2(m,p1,p2)               {if (avct_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_AVP, TRACE_TYPE_ERROR, m,p1,p2);}
#define AVCT_TRACE_ERROR3(m,p1,p2,p3)            {if (avct_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_AVP, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define AVCT_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (avct_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_AVP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define AVCT_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (avct_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_AVP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define AVCT_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (avct_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_AVP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define AVCT_TRACE_WARNING0(m)                   {if (avct_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_AVP, TRACE_TYPE_WARNING, m);}
#define AVCT_TRACE_WARNING1(m,p1)                {if (avct_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_AVP, TRACE_TYPE_WARNING, m,p1);}
#define AVCT_TRACE_WARNING2(m,p1,p2)             {if (avct_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_AVP, TRACE_TYPE_WARNING, m,p1,p2);}
#define AVCT_TRACE_WARNING3(m,p1,p2,p3)          {if (avct_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_AVP, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define AVCT_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (avct_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_AVP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define AVCT_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (avct_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_AVP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define AVCT_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (avct_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_AVP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define AVCT_TRACE_EVENT0(m)                     {if (avct_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_AVP, TRACE_TYPE_EVENT, m);}
#define AVCT_TRACE_EVENT1(m,p1)                  {if (avct_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_AVP, TRACE_TYPE_EVENT, m, p1);}
#define AVCT_TRACE_EVENT2(m,p1,p2)               {if (avct_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_AVP, TRACE_TYPE_EVENT, m,p1,p2);}
#define AVCT_TRACE_EVENT3(m,p1,p2,p3)            {if (avct_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_AVP, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define AVCT_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (avct_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_AVP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define AVCT_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (avct_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_AVP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define AVCT_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (avct_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_AVP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define AVCT_TRACE_DEBUG0(m)                     {if (avct_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG, m);}
#define AVCT_TRACE_DEBUG1(m,p1)                  {if (avct_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG, m,p1);}
#define AVCT_TRACE_DEBUG2(m,p1,p2)               {if (avct_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG, m,p1,p2);}
#define AVCT_TRACE_DEBUG3(m,p1,p2,p3)            {if (avct_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define AVCT_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (avct_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define AVCT_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (avct_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define AVCT_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (avct_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

#define AVCT_TRACE_API0(m)                     {if (avct_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_AVP, TRACE_TYPE_API, m);}
#define AVCT_TRACE_API1(m,p1)                  {if (avct_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_AVP, TRACE_TYPE_API, m,p1);}
#define AVCT_TRACE_API2(m,p1,p2)               {if (avct_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_AVP, TRACE_TYPE_API, m,p1,p2);}
#define AVCT_TRACE_API3(m,p1,p2,p3)            {if (avct_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_AVP, TRACE_TYPE_API, m,p1,p2,p3);}
#define AVCT_TRACE_API4(m,p1,p2,p3,p4)         {if (avct_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_AVP, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define AVCT_TRACE_API5(m,p1,p2,p3,p4,p5)      {if (avct_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_AVP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define AVCT_TRACE_API6(m,p1,p2,p3,p4,p5,p6)   {if (avct_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_AVP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}


/* Define tracing for the AVRCP profile
*/
#define AVRC_TRACE_ERROR0(m)                      {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_AVP, TRACE_TYPE_ERROR,m);}
#define AVRC_TRACE_ERROR1(m,p1)                   {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_AVP, TRACE_TYPE_ERROR,m,p1);}
#define AVRC_TRACE_ERROR2(m,p1,p2)                {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_AVP, TRACE_TYPE_ERROR,m,p1,p2);}
#define AVRC_TRACE_ERROR3(m,p1,p2,p3)             {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_AVP, TRACE_TYPE_ERROR,m,p1,p2,p3);}
#define AVRC_TRACE_ERROR4(m,p1,p2,p3,p4)          {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_AVP, TRACE_TYPE_ERROR,m,p1,p2,p3,p4);}
#define AVRC_TRACE_ERROR5(m,p1,p2,p3,p4,p5)       {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_AVP, TRACE_TYPE_ERROR,m,p1,p2,p3,p4,p5);}
#define AVRC_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)    {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_AVP, TRACE_TYPE_ERROR,m,p1,p2,p3,p4,p5,p6);}

#define AVRC_TRACE_WARNING0(m)                    {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_AVP, TRACE_TYPE_WARNING,m);}
#define AVRC_TRACE_WARNING1(m,p1)                 {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_AVP, TRACE_TYPE_WARNING,m,p1);}
#define AVRC_TRACE_WARNING2(m,p1,p2)              {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_AVP, TRACE_TYPE_WARNING,m,p1,p2);}
#define AVRC_TRACE_WARNING3(m,p1,p2,p3)           {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_AVP, TRACE_TYPE_WARNING,m,p1,p2,p3);}
#define AVRC_TRACE_WARNING4(m,p1,p2,p3,p4)        {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_AVP, TRACE_TYPE_WARNING,m,p1,p2,p3,p4);}
#define AVRC_TRACE_WARNING5(m,p1,p2,p3,p4,p5)     {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_AVP, TRACE_TYPE_WARNING,m,p1,p2,p3,p4,p5);}
#define AVRC_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)  {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_AVP, TRACE_TYPE_WARNING,m,p1,p2,p3,p4,p5,p6);}

#define AVRC_TRACE_EVENT0(m)                      {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_AVP, TRACE_TYPE_EVENT,m);}
#define AVRC_TRACE_EVENT1(m,p1)                   {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_AVP, TRACE_TYPE_EVENT,m, p1);}
#define AVRC_TRACE_EVENT2(m,p1,p2)                {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_AVP, TRACE_TYPE_EVENT,m,p1,p2);}
#define AVRC_TRACE_EVENT3(m,p1,p2,p3)             {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_AVP, TRACE_TYPE_EVENT,m,p1,p2,p3);}
#define AVRC_TRACE_EVENT4(m,p1,p2,p3,p4)          {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_AVP, TRACE_TYPE_EVENT,m,p1,p2,p3,p4);}
#define AVRC_TRACE_EVENT5(m,p1,p2,p3,p4,p5)       {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_AVP, TRACE_TYPE_EVENT,m,p1,p2,p3,p4,p5);}
#define AVRC_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)    {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_AVP, TRACE_TYPE_EVENT,m,p1,p2,p3,p4,p5,p6);}

#define AVRC_TRACE_DEBUG0(m)                      {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG,m);}
#define AVRC_TRACE_DEBUG1(m,p1)                   {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG,m,p1);}
#define AVRC_TRACE_DEBUG2(m,p1,p2)                {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG,m,p1,p2);}
#define AVRC_TRACE_DEBUG3(m,p1,p2,p3)             {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG,m,p1,p2,p3);}
#define AVRC_TRACE_DEBUG4(m,p1,p2,p3,p4)          {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG,m,p1,p2,p3,p4);}
#define AVRC_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)       {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG,m,p1,p2,p3,p4,p5);}
#define AVRC_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)    {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_AVP, TRACE_TYPE_DEBUG,m,p1,p2,p3,p4,p5,p6);}

#define AVRC_TRACE_API0(m)                        {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_AVP, TRACE_TYPE_API,m);}
#define AVRC_TRACE_API1(m,p1)                     {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_AVP, TRACE_TYPE_API,m, p1);}
#define AVRC_TRACE_API2(m,p1,p2)                  {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_AVP, TRACE_TYPE_API,m,p1,p2);}
#define AVRC_TRACE_API3(m,p1,p2,p3)               {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_AVP, TRACE_TYPE_API,m,p1,p2,p3);}
#define AVRC_TRACE_API4(m,p1,p2,p3,p4)            {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_AVP, TRACE_TYPE_API,m,p1,p2,p3,p4);}
#define AVRC_TRACE_API5(m,p1,p2,p3,p4,p5)         {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_AVP, TRACE_TYPE_API,m,p1,p2,p3,p4,p5);}
#define AVRC_TRACE_API6(m,p1,p2,p3,p4,p5,p6)      {if (avrc_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_AVP, TRACE_TYPE_API,m,p1,p2,p3,p4,p5,p6);}

/* MCAP
*/
#define MCA_TRACE_ERROR0(m)                     {if (mca_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_MCA, TRACE_TYPE_ERROR, m);}
#define MCA_TRACE_ERROR1(m,p1)                  {if (mca_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_MCA, TRACE_TYPE_ERROR, m,p1);}
#define MCA_TRACE_ERROR2(m,p1,p2)               {if (mca_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_MCA, TRACE_TYPE_ERROR, m,p1,p2);}
#define MCA_TRACE_ERROR3(m,p1,p2,p3)            {if (mca_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_MCA, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define MCA_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (mca_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_MCA, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define MCA_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (mca_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_MCA, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define MCA_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (mca_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_MCA, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define MCA_TRACE_WARNING0(m)                   {if (mca_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_MCA, TRACE_TYPE_WARNING, m);}
#define MCA_TRACE_WARNING1(m,p1)                {if (mca_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_MCA, TRACE_TYPE_WARNING, m,p1);}
#define MCA_TRACE_WARNING2(m,p1,p2)             {if (mca_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_MCA, TRACE_TYPE_WARNING, m,p1,p2);}
#define MCA_TRACE_WARNING3(m,p1,p2,p3)          {if (mca_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_MCA, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define MCA_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (mca_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_MCA, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define MCA_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (mca_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_MCA, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define MCA_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (mca_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_MCA, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define MCA_TRACE_EVENT0(m)                     {if (mca_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_MCA, TRACE_TYPE_EVENT, m);}
#define MCA_TRACE_EVENT1(m,p1)                  {if (mca_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_MCA, TRACE_TYPE_EVENT, m, p1);}
#define MCA_TRACE_EVENT2(m,p1,p2)               {if (mca_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_MCA, TRACE_TYPE_EVENT, m,p1,p2);}
#define MCA_TRACE_EVENT3(m,p1,p2,p3)            {if (mca_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_MCA, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define MCA_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (mca_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_MCA, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define MCA_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (mca_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_MCA, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define MCA_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (mca_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_MCA, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define MCA_TRACE_DEBUG0(m)                     {if (mca_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_MCA, TRACE_TYPE_DEBUG, m);}
#define MCA_TRACE_DEBUG1(m,p1)                  {if (mca_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_MCA, TRACE_TYPE_DEBUG, m,p1);}
#define MCA_TRACE_DEBUG2(m,p1,p2)               {if (mca_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_MCA, TRACE_TYPE_DEBUG, m,p1,p2);}
#define MCA_TRACE_DEBUG3(m,p1,p2,p3)            {if (mca_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_MCA, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define MCA_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (mca_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_MCA, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define MCA_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (mca_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_MCA, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define MCA_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (mca_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_MCA, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

#define MCA_TRACE_API0(m)                     {if (mca_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_MCA, TRACE_TYPE_API, m);}
#define MCA_TRACE_API1(m,p1)                  {if (mca_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_MCA, TRACE_TYPE_API, m,p1);}
#define MCA_TRACE_API2(m,p1,p2)               {if (mca_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_MCA, TRACE_TYPE_API, m,p1,p2);}
#define MCA_TRACE_API3(m,p1,p2,p3)            {if (mca_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_MCA, TRACE_TYPE_API, m,p1,p2,p3);}
#define MCA_TRACE_API4(m,p1,p2,p3,p4)         {if (mca_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_MCA, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define MCA_TRACE_API5(m,p1,p2,p3,p4,p5)      {if (mca_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_MCA, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define MCA_TRACE_API6(m,p1,p2,p3,p4,p5,p6)   {if (mca_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_MCA, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

/* Define tracing for the ATT/GATT unit
*/
#define GATT_TRACE_ERROR0(m)                     {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_ATT, TRACE_TYPE_ERROR, m);}
#define GATT_TRACE_ERROR1(m,p1)                  {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_ATT, TRACE_TYPE_ERROR, m,p1);}
#define GATT_TRACE_ERROR2(m,p1,p2)               {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_ATT, TRACE_TYPE_ERROR, m,p1,p2);}
#define GATT_TRACE_ERROR3(m,p1,p2,p3)            {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_ATT, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define GATT_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_ATT, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define GATT_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_ATT, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define GATT_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_ATT, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define GATT_TRACE_WARNING0(m)                   {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_ATT, TRACE_TYPE_WARNING, m);}
#define GATT_TRACE_WARNING1(m,p1)                {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_ATT, TRACE_TYPE_WARNING, m,p1);}
#define GATT_TRACE_WARNING2(m,p1,p2)             {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_ATT, TRACE_TYPE_WARNING, m,p1,p2);}
#define GATT_TRACE_WARNING3(m,p1,p2,p3)          {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_ATT, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define GATT_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_ATT, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define GATT_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_ATT, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define GATT_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_ATT, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define GATT_TRACE_API0(m)                       {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_ATT, TRACE_TYPE_API, m);}
#define GATT_TRACE_API1(m,p1)                    {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_ATT, TRACE_TYPE_API, m,p1);}
#define GATT_TRACE_API2(m,p1,p2)                 {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_ATT, TRACE_TYPE_API, m,p1,p2);}
#define GATT_TRACE_API3(m,p1,p2,p3)              {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_ATT, TRACE_TYPE_API, m,p1,p2,p3);}
#define GATT_TRACE_API4(m,p1,p2,p3,p4)           {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_ATT, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define GATT_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_ATT, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define GATT_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_ATT, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define GATT_TRACE_EVENT0(m)                     {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_ATT, TRACE_TYPE_EVENT, m);}
#define GATT_TRACE_EVENT1(m,p1)                  {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_ATT, TRACE_TYPE_EVENT, m, p1);}
#define GATT_TRACE_EVENT2(m,p1,p2)               {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_ATT, TRACE_TYPE_EVENT, m,p1,p2);}
#define GATT_TRACE_EVENT3(m,p1,p2,p3)            {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_ATT, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define GATT_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_ATT, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define GATT_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_ATT, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define GATT_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_ATT, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define GATT_TRACE_DEBUG0(m)                     {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_ATT, TRACE_TYPE_DEBUG, m);}
#define GATT_TRACE_DEBUG1(m,p1)                  {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_ATT, TRACE_TYPE_DEBUG, m,p1);}
#define GATT_TRACE_DEBUG2(m,p1,p2)               {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_ATT, TRACE_TYPE_DEBUG, m,p1,p2);}
#define GATT_TRACE_DEBUG3(m,p1,p2,p3)            {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_ATT, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define GATT_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_ATT, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define GATT_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_ATT, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define GATT_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (gatt_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_ATT, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

/* Define tracing for the SMP unit
*/
#define SMP_TRACE_ERROR0(m)                     {if (smp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_0(TRACE_LAYER_SMP, TRACE_TYPE_ERROR, m);}
#define SMP_TRACE_ERROR1(m,p1)                  {if (smp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_1(TRACE_LAYER_SMP, TRACE_TYPE_ERROR, m,p1);}
#define SMP_TRACE_ERROR2(m,p1,p2)               {if (smp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_2(TRACE_LAYER_SMP, TRACE_TYPE_ERROR, m,p1,p2);}
#define SMP_TRACE_ERROR3(m,p1,p2,p3)            {if (smp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_3(TRACE_LAYER_SMP, TRACE_TYPE_ERROR, m,p1,p2,p3);}
#define SMP_TRACE_ERROR4(m,p1,p2,p3,p4)         {if (smp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_4(TRACE_LAYER_SMP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4);}
#define SMP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)      {if (smp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_5(TRACE_LAYER_SMP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5);}
#define SMP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)   {if (smp_cb.trace_level >= BT_TRACE_LEVEL_ERROR) BT_TRACE_6(TRACE_LAYER_SMP, TRACE_TYPE_ERROR, m,p1,p2,p3,p4,p5,p6);}

#define SMP_TRACE_WARNING0(m)                   {if (smp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_0(TRACE_LAYER_SMP, TRACE_TYPE_WARNING, m);}
#define SMP_TRACE_WARNING1(m,p1)                {if (smp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_1(TRACE_LAYER_SMP, TRACE_TYPE_WARNING, m,p1);}
#define SMP_TRACE_WARNING2(m,p1,p2)             {if (smp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_2(TRACE_LAYER_SMP, TRACE_TYPE_WARNING, m,p1,p2);}
#define SMP_TRACE_WARNING3(m,p1,p2,p3)          {if (smp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_3(TRACE_LAYER_SMP, TRACE_TYPE_WARNING, m,p1,p2,p3);}
#define SMP_TRACE_WARNING4(m,p1,p2,p3,p4)       {if (smp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_4(TRACE_LAYER_SMP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4);}
#define SMP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)    {if (smp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_5(TRACE_LAYER_SMP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5);}
#define SMP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (smp_cb.trace_level >= BT_TRACE_LEVEL_WARNING) BT_TRACE_6(TRACE_LAYER_SMP, TRACE_TYPE_WARNING, m,p1,p2,p3,p4,p5,p6);}

#define SMP_TRACE_API0(m)                       {if (smp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_0(TRACE_LAYER_SMP, TRACE_TYPE_API, m);}
#define SMP_TRACE_API1(m,p1)                    {if (smp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_1(TRACE_LAYER_SMP, TRACE_TYPE_API, m,p1);}
#define SMP_TRACE_API2(m,p1,p2)                 {if (smp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_2(TRACE_LAYER_SMP, TRACE_TYPE_API, m,p1,p2);}
#define SMP_TRACE_API3(m,p1,p2,p3)              {if (smp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_3(TRACE_LAYER_SMP, TRACE_TYPE_API, m,p1,p2,p3);}
#define SMP_TRACE_API4(m,p1,p2,p3,p4)           {if (smp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_4(TRACE_LAYER_SMP, TRACE_TYPE_API, m,p1,p2,p3,p4);}
#define SMP_TRACE_API5(m,p1,p2,p3,p4,p5)        {if (smp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_5(TRACE_LAYER_SMP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5);}
#define SMP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)     {if (smp_cb.trace_level >= BT_TRACE_LEVEL_API) BT_TRACE_6(TRACE_LAYER_SMP, TRACE_TYPE_API, m,p1,p2,p3,p4,p5,p6);}

#define SMP_TRACE_EVENT0(m)                     {if (smp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_0(TRACE_LAYER_SMP, TRACE_TYPE_EVENT, m);}
#define SMP_TRACE_EVENT1(m,p1)                  {if (smp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_1(TRACE_LAYER_SMP, TRACE_TYPE_EVENT, m, p1);}
#define SMP_TRACE_EVENT2(m,p1,p2)               {if (smp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_2(TRACE_LAYER_SMP, TRACE_TYPE_EVENT, m,p1,p2);}
#define SMP_TRACE_EVENT3(m,p1,p2,p3)            {if (smp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_3(TRACE_LAYER_SMP, TRACE_TYPE_EVENT, m,p1,p2,p3);}
#define SMP_TRACE_EVENT4(m,p1,p2,p3,p4)         {if (smp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_4(TRACE_LAYER_SMP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4);}
#define SMP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)      {if (smp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_5(TRACE_LAYER_SMP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5);}
#define SMP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)   {if (smp_cb.trace_level >= BT_TRACE_LEVEL_EVENT) BT_TRACE_6(TRACE_LAYER_SMP, TRACE_TYPE_EVENT, m,p1,p2,p3,p4,p5,p6);}

#define SMP_TRACE_DEBUG0(m)                     {if (smp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_0(TRACE_LAYER_SMP, TRACE_TYPE_DEBUG, m);}
#define SMP_TRACE_DEBUG1(m,p1)                  {if (smp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_1(TRACE_LAYER_SMP, TRACE_TYPE_DEBUG, m,p1);}
#define SMP_TRACE_DEBUG2(m,p1,p2)               {if (smp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_2(TRACE_LAYER_SMP, TRACE_TYPE_DEBUG, m,p1,p2);}
#define SMP_TRACE_DEBUG3(m,p1,p2,p3)            {if (smp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_3(TRACE_LAYER_SMP, TRACE_TYPE_DEBUG, m,p1,p2,p3);}
#define SMP_TRACE_DEBUG4(m,p1,p2,p3,p4)         {if (smp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_4(TRACE_LAYER_SMP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4);}
#define SMP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)      {if (smp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_5(TRACE_LAYER_SMP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5);}
#define SMP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)   {if (smp_cb.trace_level >= BT_TRACE_LEVEL_DEBUG) BT_TRACE_6(TRACE_LAYER_SMP, TRACE_TYPE_DEBUG, m,p1,p2,p3,p4,p5,p6);}

/* END OF USE TRACES */
#else

#define BT_TRACE_0(l,t,m)
#define BT_TRACE_1(l,t,m,p1)
#define BT_TRACE_2(l,t,m,p1,p2)
#define BT_TRACE_3(l,t,m,p1,p2,p3)
#define BT_TRACE_4(l,t,m,p1,p2,p3,p4)
#define BT_TRACE_5(l,t,m,p1,p2,p3,p4,p5)
#define BT_TRACE_6(l,t,m,p1,p2,p3,p4,p5,p6)

#define BT_ERROR_TRACE_0(l,m)
#define BT_ERROR_TRACE_1(l,m,p1)
#define BT_ERROR_TRACE_2(l,m,p1,p2)
#define BT_ERROR_TRACE_3(l,m,p1,p2,p3)

/* Define tracing for the HCI unit
*/
#define HCI_TRACE_ERROR0(m)
#define HCI_TRACE_ERROR1(m,p1)
#define HCI_TRACE_ERROR2(m,p1,p2)
#define HCI_TRACE_ERROR3(m,p1,p2,p3)
#define HCI_TRACE_ERROR4(m,p1,p2,p3,p4)
#define HCI_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define HCI_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define HCI_TRACE_WARNING0(m)
#define HCI_TRACE_WARNING1(m,p1)
#define HCI_TRACE_WARNING2(m,p1,p2)
#define HCI_TRACE_WARNING3(m,p1,p2,p3)
#define HCI_TRACE_WARNING4(m,p1,p2,p3,p4)
#define HCI_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define HCI_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define HCI_TRACE_EVENT0(m)
#define HCI_TRACE_EVENT1(m,p1)
#define HCI_TRACE_EVENT2(m,p1,p2)
#define HCI_TRACE_EVENT3(m,p1,p2,p3)
#define HCI_TRACE_EVENT4(m,p1,p2,p3,p4)
#define HCI_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define HCI_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define HCI_TRACE_DEBUG0(m)
#define HCI_TRACE_DEBUG1(m,p1)
#define HCI_TRACE_DEBUG2(m,p1,p2)
#define HCI_TRACE_DEBUG3(m,p1,p2,p3)
#define HCI_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define HCI_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define HCI_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)


/* Define tracing for BTM
*/
#define BTM_TRACE_ERROR0(m)
#define BTM_TRACE_ERROR1(m,p1)
#define BTM_TRACE_ERROR2(m,p1,p2)
#define BTM_TRACE_ERROR3(m,p1,p2,p3)
#define BTM_TRACE_ERROR4(m,p1,p2,p3,p4)
#define BTM_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define BTM_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define BTM_TRACE_WARNING0(m)
#define BTM_TRACE_WARNING1(m,p1)
#define BTM_TRACE_WARNING2(m,p1,p2)
#define BTM_TRACE_WARNING3(m,p1,p2,p3)
#define BTM_TRACE_WARNING4(m,p1,p2,p3,p4)
#define BTM_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define BTM_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define BTM_TRACE_API0(m)
#define BTM_TRACE_API1(m,p1)
#define BTM_TRACE_API2(m,p1,p2)
#define BTM_TRACE_API3(m,p1,p2,p3)
#define BTM_TRACE_API4(m,p1,p2,p3,p4)
#define BTM_TRACE_API5(m,p1,p2,p3,p4,p5)
#define BTM_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define BTM_TRACE_EVENT0(m)
#define BTM_TRACE_EVENT1(m,p1)
#define BTM_TRACE_EVENT2(m,p1,p2)
#define BTM_TRACE_EVENT3(m,p1,p2,p3)
#define BTM_TRACE_EVENT4(m,p1,p2,p3,p4)
#define BTM_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define BTM_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define BTM_TRACE_DEBUG0(m)
#define BTM_TRACE_DEBUG1(m,p1)
#define BTM_TRACE_DEBUG2(m,p1,p2)
#define BTM_TRACE_DEBUG3(m,p1,p2,p3)
#define BTM_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define BTM_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define BTM_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)


/* Define tracing for the L2CAP unit
*/
#define L2CAP_TRACE_ERROR0(m)
#define L2CAP_TRACE_ERROR1(m,p1)
#define L2CAP_TRACE_ERROR2(m,p1,p2)
#define L2CAP_TRACE_ERROR3(m,p1,p2,p3)
#define L2CAP_TRACE_ERROR4(m,p1,p2,p3,p4)
#define L2CAP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define L2CAP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define L2CAP_TRACE_WARNING0(m)
#define L2CAP_TRACE_WARNING1(m,p1)
#define L2CAP_TRACE_WARNING2(m,p1,p2)
#define L2CAP_TRACE_WARNING3(m,p1,p2,p3)
#define L2CAP_TRACE_WARNING4(m,p1,p2,p3,p4)
#define L2CAP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define L2CAP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define L2CAP_TRACE_API0(m)
#define L2CAP_TRACE_API1(m,p1)
#define L2CAP_TRACE_API2(m,p1,p2)
#define L2CAP_TRACE_API3(m,p1,p2,p3)
#define L2CAP_TRACE_API4(m,p1,p2,p3,p4)
#define L2CAP_TRACE_API5(m,p1,p2,p3,p4,p5)
#define L2CAP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define L2CAP_TRACE_EVENT0(m)
#define L2CAP_TRACE_EVENT1(m,p1)
#define L2CAP_TRACE_EVENT2(m,p1,p2)
#define L2CAP_TRACE_EVENT3(m,p1,p2,p3)
#define L2CAP_TRACE_EVENT4(m,p1,p2,p3,p4)
#define L2CAP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define L2CAP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define L2CAP_TRACE_DEBUG0(m)
#define L2CAP_TRACE_DEBUG1(m,p1)
#define L2CAP_TRACE_DEBUG2(m,p1,p2)
#define L2CAP_TRACE_DEBUG3(m,p1,p2,p3)
#define L2CAP_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define L2CAP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define L2CAP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

/* Define tracing for the LLCP unit
*/
#define LLCP_TRACE_ERROR0(m)
#define LLCP_TRACE_ERROR1(m,p1)
#define LLCP_TRACE_ERROR2(m,p1,p2)
#define LLCP_TRACE_ERROR3(m,p1,p2,p3)
#define LLCP_TRACE_ERROR4(m,p1,p2,p3,p4)
#define LLCP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define LLCP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define LLCP_TRACE_WARNING0(m)
#define LLCP_TRACE_WARNING1(m,p1)
#define LLCP_TRACE_WARNING2(m,p1,p2)
#define LLCP_TRACE_WARNING3(m,p1,p2,p3)
#define LLCP_TRACE_WARNING4(m,p1,p2,p3,p4)
#define LLCP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define LLCP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define LLCP_TRACE_API0(m)
#define LLCP_TRACE_API1(m,p1)
#define LLCP_TRACE_API2(m,p1,p2)
#define LLCP_TRACE_API3(m,p1,p2,p3)
#define LLCP_TRACE_API4(m,p1,p2,p3,p4)
#define LLCP_TRACE_API5(m,p1,p2,p3,p4,p5)
#define LLCP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define LLCP_TRACE_EVENT0(m)
#define LLCP_TRACE_EVENT1(m,p1)
#define LLCP_TRACE_EVENT2(m,p1,p2)
#define LLCP_TRACE_EVENT3(m,p1,p2,p3)
#define LLCP_TRACE_EVENT4(m,p1,p2,p3,p4)
#define LLCP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define LLCP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define LLCP_TRACE_DEBUG0(m)
#define LLCP_TRACE_DEBUG1(m,p1)
#define LLCP_TRACE_DEBUG2(m,p1,p2)
#define LLCP_TRACE_DEBUG3(m,p1,p2,p3)
#define LLCP_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define LLCP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define LLCP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

/* Define tracing for the SDP unit
*/
#define SDP_TRACE_ERROR0(m)
#define SDP_TRACE_ERROR1(m,p1)
#define SDP_TRACE_ERROR2(m,p1,p2)
#define SDP_TRACE_ERROR3(m,p1,p2,p3)
#define SDP_TRACE_ERROR4(m,p1,p2,p3,p4)
#define SDP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define SDP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define SDP_TRACE_WARNING0(m)
#define SDP_TRACE_WARNING1(m,p1)
#define SDP_TRACE_WARNING2(m,p1,p2)
#define SDP_TRACE_WARNING3(m,p1,p2,p3)
#define SDP_TRACE_WARNING4(m,p1,p2,p3,p4)
#define SDP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define SDP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define SDP_TRACE_API0(m)
#define SDP_TRACE_API1(m,p1)
#define SDP_TRACE_API2(m,p1,p2)
#define SDP_TRACE_API3(m,p1,p2,p3)
#define SDP_TRACE_API4(m,p1,p2,p3,p4)
#define SDP_TRACE_API5(m,p1,p2,p3,p4,p5)
#define SDP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define SDP_TRACE_EVENT0(m)
#define SDP_TRACE_EVENT1(m,p1)
#define SDP_TRACE_EVENT2(m,p1,p2)
#define SDP_TRACE_EVENT3(m,p1,p2,p3)
#define SDP_TRACE_EVENT4(m,p1,p2,p3,p4)
#define SDP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define SDP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define SDP_TRACE_DEBUG0(m)
#define SDP_TRACE_DEBUG1(m,p1)
#define SDP_TRACE_DEBUG2(m,p1,p2)
#define SDP_TRACE_DEBUG3(m,p1,p2,p3)
#define SDP_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define SDP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define SDP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

/* Define tracing for the RFCOMM unit
*/
#define RFCOMM_TRACE_ERROR0(m)
#define RFCOMM_TRACE_ERROR1(m,p1)
#define RFCOMM_TRACE_ERROR2(m,p1,p2)
#define RFCOMM_TRACE_ERROR3(m,p1,p2,p3)
#define RFCOMM_TRACE_ERROR4(m,p1,p2,p3,p4)
#define RFCOMM_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define RFCOMM_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define RFCOMM_TRACE_WARNING0(m)
#define RFCOMM_TRACE_WARNING1(m,p1)
#define RFCOMM_TRACE_WARNING2(m,p1,p2)
#define RFCOMM_TRACE_WARNING3(m,p1,p2,p3)
#define RFCOMM_TRACE_WARNING4(m,p1,p2,p3,p4)
#define RFCOMM_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define RFCOMM_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define RFCOMM_TRACE_API0(m)
#define RFCOMM_TRACE_API1(m,p1)
#define RFCOMM_TRACE_API2(m,p1,p2)
#define RFCOMM_TRACE_API3(m,p1,p2,p3)
#define RFCOMM_TRACE_API4(m,p1,p2,p3,p4)
#define RFCOMM_TRACE_API5(m,p1,p2,p3,p4,p5)
#define RFCOMM_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define RFCOMM_TRACE_EVENT0(m)
#define RFCOMM_TRACE_EVENT1(m,p1)
#define RFCOMM_TRACE_EVENT2(m,p1,p2)
#define RFCOMM_TRACE_EVENT3(m,p1,p2,p3)
#define RFCOMM_TRACE_EVENT4(m,p1,p2,p3,p4)
#define RFCOMM_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define RFCOMM_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define RFCOMM_TRACE_DEBUG0(m)
#define RFCOMM_TRACE_DEBUG1(m,p1)
#define RFCOMM_TRACE_DEBUG2(m,p1,p2)
#define RFCOMM_TRACE_DEBUG3(m,p1,p2,p3)
#define RFCOMM_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define RFCOMM_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define RFCOMM_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

/* Define tracing for Serial Port Profile
*/
#define SPP_TRACE_ERROR0(m)
#define SPP_TRACE_ERROR1(m,p1)
#define SPP_TRACE_ERROR2(m,p1,p2)
#define SPP_TRACE_ERROR3(m,p1,p2,p3)
#define SPP_TRACE_ERROR4(m,p1,p2,p3,p4)
#define SPP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define SPP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define SPP_TRACE_WARNING0(m)
#define SPP_TRACE_WARNING1(m,p1)
#define SPP_TRACE_WARNING2(m,p1,p2)
#define SPP_TRACE_WARNING3(m,p1,p2,p3)
#define SPP_TRACE_WARNING4(m,p1,p2,p3,p4)
#define SPP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define SPP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define SPP_TRACE_EVENT0(m)
#define SPP_TRACE_EVENT1(m,p1)
#define SPP_TRACE_EVENT2(m,p1,p2)
#define SPP_TRACE_EVENT3(m,p1,p2,p3)
#define SPP_TRACE_EVENT4(m,p1,p2,p3,p4)
#define SPP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define SPP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define SPP_TRACE_API0(m)
#define SPP_TRACE_API1(m,p1)
#define SPP_TRACE_API2(m,p1,p2)
#define SPP_TRACE_API3(m,p1,p2,p3)
#define SPP_TRACE_API4(m,p1,p2,p3,p4)
#define SPP_TRACE_API5(m,p1,p2,p3,p4,p5)
#define SPP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define SPP_TRACE_DEBUG0(m)
#define SPP_TRACE_DEBUG1(m,p1)
#define SPP_TRACE_DEBUG2(m,p1,p2)
#define SPP_TRACE_DEBUG3(m,p1,p2,p3)
#define SPP_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define SPP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define SPP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)


/* Generic Access Profile traces */
#define GAP_TRACE_ERROR0(m)
#define GAP_TRACE_ERROR1(m,p1)
#define GAP_TRACE_ERROR2(m,p1,p2)
#define GAP_TRACE_ERROR3(m,p1,p2,p3)
#define GAP_TRACE_ERROR4(m,p1,p2,p3,p4)
#define GAP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define GAP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define GAP_TRACE_EVENT0(m)
#define GAP_TRACE_EVENT1(m,p1)
#define GAP_TRACE_EVENT2(m,p1,p2)
#define GAP_TRACE_EVENT3(m,p1,p2,p3)
#define GAP_TRACE_EVENT4(m,p1,p2,p3,p4)
#define GAP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define GAP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define GAP_TRACE_API0(m)
#define GAP_TRACE_API1(m,p1)
#define GAP_TRACE_API2(m,p1,p2)
#define GAP_TRACE_API3(m,p1,p2,p3)
#define GAP_TRACE_API4(m,p1,p2,p3,p4)
#define GAP_TRACE_API5(m,p1,p2,p3,p4,p5)
#define GAP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define GAP_TRACE_WARNING0(m)
#define GAP_TRACE_WARNING1(m,p1)
#define GAP_TRACE_WARNING2(m,p1,p2)
#define GAP_TRACE_WARNING3(m,p1,p2,p3)
#define GAP_TRACE_WARNING4(m,p1,p2,p3,p4)
#define GAP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define GAP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)


/* Define tracing for OBX
*/
#define OBX_TRACE_ERROR0(m)
#define OBX_TRACE_ERROR1(m,p1)
#define OBX_TRACE_ERROR2(m,p1,p2)
#define OBX_TRACE_ERROR3(m,p1,p2,p3)
#define OBX_TRACE_ERROR4(m,p1,p2,p3,p4)
#define OBX_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define OBX_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define OBX_TRACE_WARNING0(m)
#define OBX_TRACE_WARNING1(m,p1)
#define OBX_TRACE_WARNING2(m,p1,p2)
#define OBX_TRACE_WARNING3(m,p1,p2,p3)
#define OBX_TRACE_WARNING4(m,p1,p2,p3,p4)
#define OBX_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define OBX_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define OBX_TRACE_EVENT0(m)
#define OBX_TRACE_EVENT1(m,p1)
#define OBX_TRACE_EVENT2(m,p1,p2)
#define OBX_TRACE_EVENT3(m,p1,p2,p3)
#define OBX_TRACE_EVENT4(m,p1,p2,p3,p4)
#define OBX_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define OBX_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define OBX_TRACE_DEBUG0(m)
#define OBX_TRACE_DEBUG1(m,p1)
#define OBX_TRACE_DEBUG2(m,p1,p2)
#define OBX_TRACE_DEBUG3(m,p1,p2,p3)
#define OBX_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define OBX_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define OBX_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

#define OBX_TRACE_API0(m)
#define OBX_TRACE_API1(m,p1)
#define OBX_TRACE_API2(m,p1,p2)
#define OBX_TRACE_API3(m,p1,p2,p3)
#define OBX_TRACE_API4(m,p1,p2,p3,p4)
#define OBX_TRACE_API5(m,p1,p2,p3,p4,p5)
#define OBX_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

/* Define tracing for GOEP application profiles
*/
#define GOEP_TRACE_ERROR0(m)
#define GOEP_TRACE_ERROR1(m,p1)
#define GOEP_TRACE_ERROR2(m,p1,p2)
#define GOEP_TRACE_ERROR3(m,p1,p2,p3)
#define GOEP_TRACE_ERROR4(m,p1,p2,p3,p4)
#define GOEP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define GOEP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define GOEP_TRACE_WARNING0(m)
#define GOEP_TRACE_WARNING1(m,p1)
#define GOEP_TRACE_WARNING2(m,p1,p2)
#define GOEP_TRACE_WARNING3(m,p1,p2,p3)
#define GOEP_TRACE_WARNING4(m,p1,p2,p3,p4)
#define GOEP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define GOEP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define GOEP_TRACE_EVENT0(m)
#define GOEP_TRACE_EVENT1(m,p1)
#define GOEP_TRACE_EVENT2(m,p1,p2)
#define GOEP_TRACE_EVENT3(m,p1,p2,p3)
#define GOEP_TRACE_EVENT4(m,p1,p2,p3,p4)
#define GOEP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define GOEP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define GOEP_TRACE_DEBUG0(m)
#define GOEP_TRACE_DEBUG1(m,p1)
#define GOEP_TRACE_DEBUG2(m,p1,p2)
#define GOEP_TRACE_DEBUG3(m,p1,p2,p3)
#define GOEP_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define GOEP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

#define GOEP_TRACE_API0(m)
#define GOEP_TRACE_API1(m,p1)
#define GOEP_TRACE_API2(m,p1,p2)
#define GOEP_TRACE_API3(m,p1,p2,p3)
#define GOEP_TRACE_API4(m,p1,p2,p3,p4)
#define GOEP_TRACE_API5(m,p1,p2,p3,p4,p5)
#define GOEP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

/* Define tracing for the BPP profile
*/
#define BPP_TRACE_ERROR0(m)
#define BPP_TRACE_ERROR1(m,p1)
#define BPP_TRACE_ERROR2(m,p1,p2)
#define BPP_TRACE_ERROR3(m,p1,p2,p3)
#define BPP_TRACE_ERROR4(m,p1,p2,p3,p4)
#define BPP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define BPP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define BPP_TRACE_WARNING0(m)
#define BPP_TRACE_WARNING1(m,p1)
#define BPP_TRACE_WARNING2(m,p1,p2)
#define BPP_TRACE_WARNING3(m,p1,p2,p3)
#define BPP_TRACE_WARNING4(m,p1,p2,p3,p4)
#define BPP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define BPP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define BPP_TRACE_EVENT0(m)
#define BPP_TRACE_EVENT1(m,p1)
#define BPP_TRACE_EVENT2(m,p1,p2)
#define BPP_TRACE_EVENT3(m,p1,p2,p3)
#define BPP_TRACE_EVENT4(m,p1,p2,p3,p4)
#define BPP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define BPP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define BPP_TRACE_DEBUG0(m)
#define BPP_TRACE_DEBUG1(m,p1)
#define BPP_TRACE_DEBUG2(m,p1,p2)
#define BPP_TRACE_DEBUG3(m,p1,p2,p3)
#define BPP_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define BPP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define BPP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

#define BPP_TRACE_API0(m)
#define BPP_TRACE_API1(m,p1)
#define BPP_TRACE_API2(m,p1,p2)
#define BPP_TRACE_API3(m,p1,p2,p3)
#define BPP_TRACE_API4(m,p1,p2,p3,p4)
#define BPP_TRACE_API5(m,p1,p2,p3,p4,p5)
#define BPP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

/* Define tracing for the BIP profile
*/
#define BIP_TRACE_ERROR0(m)
#define BIP_TRACE_ERROR1(m,p1)
#define BIP_TRACE_ERROR2(m,p1,p2)
#define BIP_TRACE_ERROR3(m,p1,p2,p3)
#define BIP_TRACE_ERROR4(m,p1,p2,p3,p4)
#define BIP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define BIP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define BIP_TRACE_WARNING0(m)
#define BIP_TRACE_WARNING1(m,p1)
#define BIP_TRACE_WARNING2(m,p1,p2)
#define BIP_TRACE_WARNING3(m,p1,p2,p3)
#define BIP_TRACE_WARNING4(m,p1,p2,p3,p4)
#define BIP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define BIP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define BIP_TRACE_EVENT0(m)
#define BIP_TRACE_EVENT1(m,p1)
#define BIP_TRACE_EVENT2(m,p1,p2)
#define BIP_TRACE_EVENT3(m,p1,p2,p3)
#define BIP_TRACE_EVENT4(m,p1,p2,p3,p4)
#define BIP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define BIP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define BIP_TRACE_DEBUG0(m)
#define BIP_TRACE_DEBUG1(m,p1)
#define BIP_TRACE_DEBUG2(m,p1,p2)
#define BIP_TRACE_DEBUG3(m,p1,p2,p3)
#define BIP_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define BIP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define BIP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

#define BIP_TRACE_API0(m)
#define BIP_TRACE_API1(m,p1)
#define BIP_TRACE_API2(m,p1,p2)
#define BIP_TRACE_API3(m,p1,p2,p3)
#define BIP_TRACE_API4(m,p1,p2,p3,p4)
#define BIP_TRACE_API5(m,p1,p2,p3,p4,p5)
#define BIP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

/* define traces for headset profile */

#define HSP2_TRACE_ERROR0(pcb,m)
#define HSP2_TRACE_ERROR1(pcb,m,p1)
#define HSP2_TRACE_ERROR2(pcb,m,p1,p2)
#define HSP2_TRACE_ERROR3(pcb,m,p1,p2,p3)
#define HSP2_TRACE_ERROR4(pcb,m,p1,p2,p3,p4)
#define HSP2_TRACE_ERROR5(pcb,m,p1,p2,p3,p4,p5)
#define HSP2_TRACE_ERROR6(pcb,m,p1,p2,p3,p4,p5,p6)

#define HSP2_TRACE_WARNING0(pcb,m)
#define HSP2_TRACE_WARNING1(pcb,m,p1)
#define HSP2_TRACE_WARNING2(pcb,m,p1,p2)
#define HSP2_TRACE_WARNING3(pcb,m,p1,p2,p3)
#define HSP2_TRACE_WARNING4(pcb,m,p1,p2,p3,p4)
#define HSP2_TRACE_WARNING5(pcb,m,p1,p2,p3,p4,p5)
#define HSP2_TRACE_WARNING6(pcb,m,p1,p2,p3,p4,p5,p6)

#define HSP2_TRACE_API0(pcb,m)
#define HSP2_TRACE_API1(pcb,m,p1)
#define HSP2_TRACE_API2(pcb,m,p1,p2)
#define HSP2_TRACE_API3(pcb,m,p1,p2,p3)
#define HSP2_TRACE_API4(pcb,m,p1,p2,p3,p4)
#define HSP2_TRACE_API5(pcb,m,p1,p2,p3,p4,p5)
#define HSP2_TRACE_API6(pcb,m,p1,p2,p3,p4,p5,p6)

#define HSP2_TRACE_EVENT0(pcb,m)
#define HSP2_TRACE_EVENT1(pcb,m,p1)
#define HSP2_TRACE_EVENT2(pcb,m,p1,p2)
#define HSP2_TRACE_EVENT3(pcb,m,p1,p2,p3)
#define HSP2_TRACE_EVENT4(pcb,m,p1,p2,p3,p4)
#define HSP2_TRACE_EVENT5(pcb,m,p1,p2,p3,p4,p5)
#define HSP2_TRACE_EVENT6(pcb,m,p1,p2,p3,p4,p5,p6)

#define HSP2_TRACE_DEBUG0(pcb,m)
#define HSP2_TRACE_DEBUG1(pcb,m,p1)
#define HSP2_TRACE_DEBUG2(pcb,m,p1,p2)
#define HSP2_TRACE_DEBUG3(pcb,m,p1,p2,p3)
#define HSP2_TRACE_DEBUG4(pcb,m,p1,p2,p3,p4)
#define HSP2_TRACE_DEBUG5(pcb,m,p1,p2,p3,p4,p5)
#define HSP2_TRACE_DEBUG6(pcb,m,p1,p2,p3,p4,p5,p6)

/* Define tracing for the NFC unit
*/
#define NFC_TRACE_ERROR0(m)
#define NFC_TRACE_ERROR1(m,p1)
#define NFC_TRACE_ERROR2(m,p1,p2)
#define NFC_TRACE_ERROR3(m,p1,p2,p3)
#define NFC_TRACE_ERROR4(m,p1,p2,p3,p4)
#define NFC_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define NFC_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define NFC_TRACE_WARNING0(m)
#define NFC_TRACE_WARNING1(m,p1)
#define NFC_TRACE_WARNING2(m,p1,p2)
#define NFC_TRACE_WARNING3(m,p1,p2,p3)
#define NFC_TRACE_WARNING4(m,p1,p2,p3,p4)
#define NFC_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define NFC_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define NFC_TRACE_API0(m)
#define NFC_TRACE_API1(m,p1)
#define NFC_TRACE_API2(m,p1,p2)
#define NFC_TRACE_API3(m,p1,p2,p3)
#define NFC_TRACE_API4(m,p1,p2,p3,p4)
#define NFC_TRACE_API5(m,p1,p2,p3,p4,p5)
#define NFC_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define NFC_TRACE_EVENT0(m)
#define NFC_TRACE_EVENT1(m,p1)
#define NFC_TRACE_EVENT2(m,p1,p2)
#define NFC_TRACE_EVENT3(m,p1,p2,p3)
#define NFC_TRACE_EVENT4(m,p1,p2,p3,p4)
#define NFC_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define NFC_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define NFC_TRACE_DEBUG0(m)
#define NFC_TRACE_DEBUG1(m,p1)
#define NFC_TRACE_DEBUG2(m,p1,p2)
#define NFC_TRACE_DEBUG3(m,p1,p2,p3)
#define NFC_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define NFC_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define NFC_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

#define RW_TRACE_ERROR0(m)
#define RW_TRACE_ERROR1(m,p1)
#define RW_TRACE_ERROR2(m,p1,p2)
#define RW_TRACE_ERROR3(m,p1,p2,p3)
#define RW_TRACE_ERROR4(m,p1,p2,p3,p4)
#define RW_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define RW_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define RW_TRACE_WARNING0(m)
#define RW_TRACE_WARNING1(m,p1)
#define RW_TRACE_WARNING2(m,p1,p2)
#define RW_TRACE_WARNING3(m,p1,p2,p3)
#define RW_TRACE_WARNING4(m,p1,p2,p3,p4)
#define RW_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define RW_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) }

#define RW_TRACE_API0(m)
#define RW_TRACE_API1(m,p1)
#define RW_TRACE_API2(m,p1,p2)
#define RW_TRACE_API3(m,p1,p2,p3)
#define RW_TRACE_API4(m,p1,p2,p3,p4)
#define RW_TRACE_API5(m,p1,p2,p3,p4,p5)
#define RW_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define RW_TRACE_EVENT0(m)
#define RW_TRACE_EVENT1(m,p1)
#define RW_TRACE_EVENT2(m,p1,p2)
#define RW_TRACE_EVENT3(m,p1,p2,p3)
#define RW_TRACE_EVENT4(m,p1,p2,p3,p4)
#define RW_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define RW_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define RW_TRACE_DEBUG0(m)
#define RW_TRACE_DEBUG1(m,p1)
#define RW_TRACE_DEBUG2(m,p1,p2)
#define RW_TRACE_DEBUG3(m,p1,p2,p3)
#define RW_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define RW_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define RW_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

#define CE_TRACE_ERROR0(m)
#define CE_TRACE_ERROR1(m,p1)
#define CE_TRACE_ERROR2(m,p1,p2)
#define CE_TRACE_ERROR3(m,p1,p2,p3)
#define CE_TRACE_ERROR4(m,p1,p2,p3,p4)
#define CE_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define CE_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define CE_TRACE_WARNING0(m)
#define CE_TRACE_WARNING1(m,p1)
#define CE_TRACE_WARNING2(m,p1,p2)
#define CE_TRACE_WARNING3(m,p1,p2,p3)
#define CE_TRACE_WARNING4(m,p1,p2,p3,p4)
#define CE_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define CE_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define CE_TRACE_API0(m)
#define CE_TRACE_API1(m,p1)
#define CE_TRACE_API2(m,p1,p2)
#define CE_TRACE_API3(m,p1,p2,p3)
#define CE_TRACE_API4(m,p1,p2,p3,p4)
#define CE_TRACE_API5(m,p1,p2,p3,p4,p5)
#define CE_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define CE_TRACE_EVENT0(m)
#define CE_TRACE_EVENT1(m,p1)
#define CE_TRACE_EVENT2(m,p1,p2)
#define CE_TRACE_EVENT3(m,p1,p2,p3)
#define CE_TRACE_EVENT4(m,p1,p2,p3,p4)
#define CE_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define CE_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define CE_TRACE_DEBUG0(m)
#define CE_TRACE_DEBUG1(m,p1)
#define CE_TRACE_DEBUG2(m,p1,p2)
#define CE_TRACE_DEBUG3(m,p1,p2,p3)
#define CE_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define CE_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define CE_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

#define NDEF_TRACE_ERROR0(m)
#define NDEF_TRACE_ERROR1(m,p1)
#define NDEF_TRACE_ERROR2(m,p1,p2)
#define NDEF_TRACE_ERROR3(m,p1,p2,p3)
#define NDEF_TRACE_ERROR4(m,p1,p2,p3,p4)
#define NDEF_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define NDEF_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define NDEF_TRACE_WARNING0(m)
#define NDEF_TRACE_WARNING1(m,p1)
#define NDEF_TRACE_WARNING2(m,p1,p2)
#define NDEF_TRACE_WARNING3(m,p1,p2,p3)
#define NDEF_TRACE_WARNING4(m,p1,p2,p3,p4)
#define NDEF_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define NDEF_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define NDEF_TRACE_API0(m)
#define NDEF_TRACE_API1(m,p1)
#define NDEF_TRACE_API2(m,p1,p2)
#define NDEF_TRACE_API3(m,p1,p2,p3)
#define NDEF_TRACE_API4(m,p1,p2,p3,p4)
#define NDEF_TRACE_API5(m,p1,p2,p3,p4,p5)
#define NDEF_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define NDEF_TRACE_EVENT0(m)
#define NDEF_TRACE_EVENT1(m,p1)
#define NDEF_TRACE_EVENT2(m,p1,p2)
#define NDEF_TRACE_EVENT3(m,p1,p2,p3)
#define NDEF_TRACE_EVENT4(m,p1,p2,p3,p4)
#define NDEF_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define NDEF_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define NDEF_TRACE_DEBUG0(m)
#define NDEF_TRACE_DEBUG1(m,p1)
#define NDEF_TRACE_DEBUG2(m,p1,p2)
#define NDEF_TRACE_DEBUG3(m,p1,p2,p3)
#define NDEF_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define NDEF_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define NDEF_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

/* Define tracing for the NFA unit
*/
#define NFA_TRACE_ERROR0(m)
#define NFA_TRACE_ERROR1(m,p1)
#define NFA_TRACE_ERROR2(m,p1,p2)
#define NFA_TRACE_ERROR3(m,p1,p2,p3)
#define NFA_TRACE_ERROR4(m,p1,p2,p3,p4)
#define NFA_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define NFA_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define NFA_TRACE_WARNING0(m)
#define NFA_TRACE_WARNING1(m,p1)
#define NFA_TRACE_WARNING2(m,p1,p2)
#define NFA_TRACE_WARNING3(m,p1,p2,p3)
#define NFA_TRACE_WARNING4(m,p1,p2,p3,p4)
#define NFA_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define NFA_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define NFA_TRACE_API0(m)
#define NFA_TRACE_API1(m,p1)
#define NFA_TRACE_API2(m,p1,p2)
#define NFA_TRACE_API3(m,p1,p2,p3)
#define NFA_TRACE_API4(m,p1,p2,p3,p4)
#define NFA_TRACE_API5(m,p1,p2,p3,p4,p5)
#define NFA_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define NFA_TRACE_EVENT0(m)
#define NFA_TRACE_EVENT1(m,p1)
#define NFA_TRACE_EVENT2(m,p1,p2)
#define NFA_TRACE_EVENT3(m,p1,p2,p3)
#define NFA_TRACE_EVENT4(m,p1,p2,p3,p4)
#define NFA_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define NFA_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define NFA_TRACE_DEBUG0(m)
#define NFA_TRACE_DEBUG1(m,p1)
#define NFA_TRACE_DEBUG2(m,p1,p2)
#define NFA_TRACE_DEBUG3(m,p1,p2,p3)
#define NFA_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define NFA_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define NFA_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

/* Define tracing for the NFA P2P unit
*/
#define P2P_TRACE_ERROR0(m)
#define P2P_TRACE_ERROR1(m,p1)
#define P2P_TRACE_ERROR2(m,p1,p2)
#define P2P_TRACE_ERROR3(m,p1,p2,p3)
#define P2P_TRACE_ERROR4(m,p1,p2,p3,p4)
#define P2P_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define P2P_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define P2P_TRACE_WARNING0(m)
#define P2P_TRACE_WARNING1(m,p1)
#define P2P_TRACE_WARNING2(m,p1,p2)
#define P2P_TRACE_WARNING3(m,p1,p2,p3)
#define P2P_TRACE_WARNING4(m,p1,p2,p3,p4)
#define P2P_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define P2P_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define P2P_TRACE_API0(m)
#define P2P_TRACE_API1(m,p1)
#define P2P_TRACE_API2(m,p1,p2)
#define P2P_TRACE_API3(m,p1,p2,p3)
#define P2P_TRACE_API4(m,p1,p2,p3,p4)
#define P2P_TRACE_API5(m,p1,p2,p3,p4,p5)
#define P2P_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define P2P_TRACE_EVENT0(m)
#define P2P_TRACE_EVENT1(m,p1)
#define P2P_TRACE_EVENT2(m,p1,p2)
#define P2P_TRACE_EVENT3(m,p1,p2,p3)
#define P2P_TRACE_EVENT4(m,p1,p2,p3,p4)
#define P2P_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define P2P_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define P2P_TRACE_DEBUG0(m)
#define P2P_TRACE_DEBUG1(m,p1)
#define P2P_TRACE_DEBUG2(m,p1,p2)
#define P2P_TRACE_DEBUG3(m,p1,p2,p3)
#define P2P_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define P2P_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define P2P_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

/* Define tracing for the NFA CHO unit
*/
#define CHO_TRACE_ERROR0(m)
#define CHO_TRACE_ERROR1(m,p1)
#define CHO_TRACE_ERROR2(m,p1,p2)
#define CHO_TRACE_ERROR3(m,p1,p2,p3)
#define CHO_TRACE_ERROR4(m,p1,p2,p3,p4)
#define CHO_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define CHO_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define CHO_TRACE_WARNING0(m)
#define CHO_TRACE_WARNING1(m,p1)
#define CHO_TRACE_WARNING2(m,p1,p2)
#define CHO_TRACE_WARNING3(m,p1,p2,p3)
#define CHO_TRACE_WARNING4(m,p1,p2,p3,p4)
#define CHO_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define CHO_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define CHO_TRACE_API0(m)
#define CHO_TRACE_API1(m,p1)
#define CHO_TRACE_API2(m,p1,p2)
#define CHO_TRACE_API3(m,p1,p2,p3)
#define CHO_TRACE_API4(m,p1,p2,p3,p4)
#define CHO_TRACE_API5(m,p1,p2,p3,p4,p5)
#define CHO_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define CHO_TRACE_EVENT0(m)
#define CHO_TRACE_EVENT1(m,p1)
#define CHO_TRACE_EVENT2(m,p1,p2)
#define CHO_TRACE_EVENT3(m,p1,p2,p3)
#define CHO_TRACE_EVENT4(m,p1,p2,p3,p4)
#define CHO_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define CHO_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define CHO_TRACE_DEBUG0(m)
#define CHO_TRACE_DEBUG1(m,p1)
#define CHO_TRACE_DEBUG2(m,p1,p2)
#define CHO_TRACE_DEBUG3(m,p1,p2,p3)
#define CHO_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define CHO_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define CHO_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

/* Define tracing for the NFA SNEP unit
*/
#define SNEP_TRACE_ERROR0(m)
#define SNEP_TRACE_ERROR1(m,p1)
#define SNEP_TRACE_ERROR2(m,p1,p2)
#define SNEP_TRACE_ERROR3(m,p1,p2,p3)
#define SNEP_TRACE_ERROR4(m,p1,p2,p3,p4)
#define SNEP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define SNEP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define SNEP_TRACE_WARNING0(m)
#define SNEP_TRACE_WARNING1(m,p1)
#define SNEP_TRACE_WARNING2(m,p1,p2)
#define SNEP_TRACE_WARNING3(m,p1,p2,p3)
#define SNEP_TRACE_WARNING4(m,p1,p2,p3,p4)
#define SNEP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define SNEP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define SNEP_TRACE_API0(m)
#define SNEP_TRACE_API1(m,p1)
#define SNEP_TRACE_API2(m,p1,p2)
#define SNEP_TRACE_API3(m,p1,p2,p3)
#define SNEP_TRACE_API4(m,p1,p2,p3,p4)
#define SNEP_TRACE_API5(m,p1,p2,p3,p4,p5)
#define SNEP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define SNEP_TRACE_EVENT0(m)
#define SNEP_TRACE_EVENT1(m,p1)
#define SNEP_TRACE_EVENT2(m,p1,p2)
#define SNEP_TRACE_EVENT3(m,p1,p2,p3)
#define SNEP_TRACE_EVENT4(m,p1,p2,p3,p4)
#define SNEP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define SNEP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define SNEP_TRACE_DEBUG0(m)
#define SNEP_TRACE_DEBUG1(m,p1)
#define SNEP_TRACE_DEBUG2(m,p1,p2)
#define SNEP_TRACE_DEBUG3(m,p1,p2,p3)
#define SNEP_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define SNEP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define SNEP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)


/* define traces for HID Host */
#define HIDH_TRACE_ERROR0(m)
#define HIDH_TRACE_ERROR1(m,p1)
#define HIDH_TRACE_ERROR2(m,p1,p2)
#define HIDH_TRACE_ERROR3(m,p1,p2,p3)
#define HIDH_TRACE_ERROR4(m,p1,p2,p3,p4)
#define HIDH_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define HIDH_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define HIDH_TRACE_WARNING0(m)
#define HIDH_TRACE_WARNING1(m,p1)
#define HIDH_TRACE_WARNING2(m,p1,p2)
#define HIDH_TRACE_WARNING3(m,p1,p2,p3)
#define HIDH_TRACE_WARNING4(m,p1,p2,p3,p4)
#define HIDH_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define HIDH_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define HIDH_TRACE_API0(m)
#define HIDH_TRACE_API1(m,p1)
#define HIDH_TRACE_API2(m,p1,p2)
#define HIDH_TRACE_API3(m,p1,p2,p3)
#define HIDH_TRACE_API4(m,p1,p2,p3,p4)
#define HIDH_TRACE_API5(m,p1,p2,p3,p4,p5)
#define HIDH_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define HIDH_TRACE_EVENT0(m)
#define HIDH_TRACE_EVENT1(m,p1)
#define HIDH_TRACE_EVENT2(m,p1,p2)
#define HIDH_TRACE_EVENT3(m,p1,p2,p3)
#define HIDH_TRACE_EVENT4(m,p1,p2,p3,p4)
#define HIDH_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define HIDH_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define HIDH_TRACE_DEBUG0(m)
#define HIDH_TRACE_DEBUG1(m,p1)
#define HIDH_TRACE_DEBUG2(m,p1,p2)
#define HIDH_TRACE_DEBUG3(m,p1,p2,p3)
#define HIDH_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define HIDH_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define HIDH_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

/* define traces for HID Device */
#define HIDD_TRACE_ERROR0(m)
#define HIDD_TRACE_ERROR1(m,p1)
#define HIDD_TRACE_ERROR2(m,p1,p2)
#define HIDD_TRACE_ERROR3(m,p1,p2,p3)
#define HIDD_TRACE_ERROR4(m,p1,p2,p3,p4)
#define HIDD_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define HIDD_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define HIDD_TRACE_WARNING0(m)
#define HIDD_TRACE_WARNING1(m,p1)
#define HIDD_TRACE_WARNING2(m,p1,p2)
#define HIDD_TRACE_WARNING3(m,p1,p2,p3)
#define HIDD_TRACE_WARNING4(m,p1,p2,p3,p4)
#define HIDD_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define HIDD_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define HIDD_TRACE_API0(m)
#define HIDD_TRACE_API1(m,p1)
#define HIDD_TRACE_API2(m,p1,p2)
#define HIDD_TRACE_API3(m,p1,p2,p3)
#define HIDD_TRACE_API4(m,p1,p2,p3,p4)
#define HIDD_TRACE_API5(m,p1,p2,p3,p4,p5)
#define HIDD_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define HIDD_TRACE_EVENT0(m)
#define HIDD_TRACE_EVENT1(m,p1)
#define HIDD_TRACE_EVENT2(m,p1,p2)
#define HIDD_TRACE_EVENT3(m,p1,p2,p3)
#define HIDD_TRACE_EVENT4(m,p1,p2,p3,p4)
#define HIDD_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define HIDD_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define HIDD_TRACE_DEBUG0(m)
#define HIDD_TRACE_DEBUG1(m,p1)
#define HIDD_TRACE_DEBUG2(m,p1,p2)
#define HIDD_TRACE_DEBUG3(m,p1,p2,p3)
#define HIDD_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define HIDD_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define HIDD_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

/* define traces for DUN */

#define DUN_TRACE_ERROR0(m)
#define DUN_TRACE_ERROR1(m,p1)
#define DUN_TRACE_ERROR2(m,p1,p2)
#define DUN_TRACE_ERROR3(m,p1,p2,p3)
#define DUN_TRACE_ERROR4(m,p1,p2,p3,p4)
#define DUN_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define DUN_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define DUN_TRACE_WARNING0(m)
#define DUN_TRACE_WARNING1(m,p1)
#define DUN_TRACE_WARNING2(m,p1,p2)
#define DUN_TRACE_WARNING3(m,p1,p2,p3)
#define DUN_TRACE_WARNING4(m,p1,p2,p3,p4)
#define DUN_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define DUN_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define DUN_TRACE_API0(m)
#define DUN_TRACE_API1(m,p1)
#define DUN_TRACE_API2(m,p1,p2)
#define DUN_TRACE_API3(m,p1,p2,p3)
#define DUN_TRACE_API4(m,p1,p2,p3,p4)
#define DUN_TRACE_API5(m,p1,p2,p3,p4,p5)
#define DUN_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define DUN_TRACE_EVENT0(m)
#define DUN_TRACE_EVENT1(m,p1)
#define DUN_TRACE_EVENT2(m,p1,p2)
#define DUN_TRACE_EVENT3(m,p1,p2,p3)
#define DUN_TRACE_EVENT4(m,p1,p2,p3,p4)
#define DUN_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define DUN_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define DUN_TRACE_DEBUG0(m)
#define DUN_TRACE_DEBUG1(m,p1)
#define DUN_TRACE_DEBUG2(m,p1,p2)
#define DUN_TRACE_DEBUG3(m,p1,p2,p3)
#define DUN_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define DUN_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define DUN_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

/* define traces for RPC */

#define RPC_TRACE_ERROR0(m)
#define RPC_TRACE_ERROR1(m,p1)
#define RPC_TRACE_ERROR2(m,p1,p2)
#define RPC_TRACE_ERROR3(m,p1,p2,p3)
#define RPC_TRACE_ERROR4(m,p1,p2,p3,p4)
#define RPC_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define RPC_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define RPC_TRACE_WARNING0(m)
#define RPC_TRACE_WARNING1(m,p1)
#define RPC_TRACE_WARNING2(m,p1,p2)
#define RPC_TRACE_WARNING3(m,p1,p2,p3)
#define RPC_TRACE_WARNING4(m,p1,p2,p3,p4)
#define RPC_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define RPC_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define RPC_TRACE_API0(m)
#define RPC_TRACE_API1(m,p1)
#define RPC_TRACE_API2(m,p1,p2)
#define RPC_TRACE_API3(m,p1,p2,p3)
#define RPC_TRACE_API4(m,p1,p2,p3,p4)
#define RPC_TRACE_API5(m,p1,p2,p3,p4,p5)
#define RPC_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define RPC_TRACE_EVENT0(m)
#define RPC_TRACE_EVENT1(m,p1)
#define RPC_TRACE_EVENT2(m,p1,p2)
#define RPC_TRACE_EVENT3(m,p1,p2,p3)
#define RPC_TRACE_EVENT4(m,p1,p2,p3,p4)
#define RPC_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define RPC_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define RPC_TRACE_DEBUG0(m)
#define RPC_TRACE_DEBUG1(m,p1)
#define RPC_TRACE_DEBUG2(m,p1,p2)
#define RPC_TRACE_DEBUG3(m,p1,p2,p3)
#define RPC_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define RPC_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define RPC_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

/* define traces for BNEP */

#define BNEP_TRACE_ERROR0(m)
#define BNEP_TRACE_ERROR1(m,p1)
#define BNEP_TRACE_ERROR2(m,p1,p2)
#define BNEP_TRACE_ERROR3(m,p1,p2,p3)
#define BNEP_TRACE_ERROR4(m,p1,p2,p3,p4)
#define BNEP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define BNEP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define BNEP_TRACE_WARNING0(m)
#define BNEP_TRACE_WARNING1(m,p1)
#define BNEP_TRACE_WARNING2(m,p1,p2)
#define BNEP_TRACE_WARNING3(m,p1,p2,p3)
#define BNEP_TRACE_WARNING4(m,p1,p2,p3,p4)
#define BNEP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define BNEP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define BNEP_TRACE_API0(m)
#define BNEP_TRACE_API1(m,p1)
#define BNEP_TRACE_API2(m,p1,p2)
#define BNEP_TRACE_API3(m,p1,p2,p3)
#define BNEP_TRACE_API4(m,p1,p2,p3,p4)
#define BNEP_TRACE_API5(m,p1,p2,p3,p4,p5)
#define BNEP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define BNEP_TRACE_EVENT0(m)
#define BNEP_TRACE_EVENT1(m,p1)
#define BNEP_TRACE_EVENT2(m,p1,p2)
#define BNEP_TRACE_EVENT3(m,p1,p2,p3)
#define BNEP_TRACE_EVENT4(m,p1,p2,p3,p4)
#define BNEP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define BNEP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define BNEP_TRACE_DEBUG0(m)
#define BNEP_TRACE_DEBUG1(m,p1)
#define BNEP_TRACE_DEBUG2(m,p1,p2)
#define BNEP_TRACE_DEBUG3(m,p1,p2,p3)
#define BNEP_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define BNEP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define BNEP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

/* hid module traces */

#define MSKB_TRACE_0(m)
#define MSKB_TRACE_1(m,p1)
#define MSKB_TRACE_2(m,p1,p2)
#define MSKB_TRACE_3(m,p1,p2,p3)
#define MSKB_TRACE_4(m,p1,p2,p3,p4)
#define MSKB_TRACE_5(m,p1,p2,p3,p4,p5)
#define MSKB_TRACE_6(m,p1,p2,p3,p4,p5,p6)

#define MSKB_DEBUG_0(m)
#define MSKB_DEBUG_1(m,p1)
#define MSKB_DEBUG_2(m,p1,p2)
#define MSKB_DEBUG_3(m,p1,p2,p3)
#define MSKB_DEBUG_4(m,p1,p2,p3,p4)
#define MSKB_DEBUG_5(m,p1,p2,p3,p4,p5)
#define MSKB_DEBUG_6(m,p1,p2,p3,p4,p5,p6)

#define MSKB_ERROR_0(m)
#define MSKB_ERROR_1(m,p1)
#define MSKB_ERROR_2(m,p1,p2)
#define MSKB_ERROR_3(m,p1,p2,p3)
#define MSKB_ERROR_4(m,p1,p2,p3,p4)
#define MSKB_ERROR_5(m,p1,p2,p3,p4,p5)
#define MSKB_ERROR_6(m,p1,p2,p3,p4,p5,p6)

/* define traces for PAN */

#define PAN_TRACE_ERROR0(m)
#define PAN_TRACE_ERROR1(m,p1)
#define PAN_TRACE_ERROR2(m,p1,p2)
#define PAN_TRACE_ERROR3(m,p1,p2,p3)
#define PAN_TRACE_ERROR4(m,p1,p2,p3,p4)
#define PAN_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define PAN_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define PAN_TRACE_WARNING0(m)
#define PAN_TRACE_WARNING1(m,p1)
#define PAN_TRACE_WARNING2(m,p1,p2)
#define PAN_TRACE_WARNING3(m,p1,p2,p3)
#define PAN_TRACE_WARNING4(m,p1,p2,p3,p4)
#define PAN_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define PAN_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define PAN_TRACE_API0(m)
#define PAN_TRACE_API1(m,p1)
#define PAN_TRACE_API2(m,p1,p2)
#define PAN_TRACE_API3(m,p1,p2,p3)
#define PAN_TRACE_API4(m,p1,p2,p3,p4)
#define PAN_TRACE_API5(m,p1,p2,p3,p4,p5)
#define PAN_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define PAN_TRACE_EVENT0(m)
#define PAN_TRACE_EVENT1(m,p1)
#define PAN_TRACE_EVENT2(m,p1,p2)
#define PAN_TRACE_EVENT3(m,p1,p2,p3)
#define PAN_TRACE_EVENT4(m,p1,p2,p3,p4)
#define PAN_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define PAN_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define PAN_TRACE_DEBUG0(m)
#define PAN_TRACE_DEBUG1(m,p1)
#define PAN_TRACE_DEBUG2(m,p1,p2)
#define PAN_TRACE_DEBUG3(m,p1,p2,p3)
#define PAN_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define PAN_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define PAN_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

/* define traces for SIM */

#define SAP_TRACE_ERROR0(m)
#define SAP_TRACE_ERROR1(m,p1)
#define SAP_TRACE_ERROR2(m,p1,p2)
#define SAP_TRACE_ERROR3(m,p1,p2,p3)
#define SAP_TRACE_ERROR4(m,p1,p2,p3,p4)
#define SAP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define SAP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define SAP_TRACE_WARNING0(m)
#define SAP_TRACE_WARNING1(m,p1)
#define SAP_TRACE_WARNING2(m,p1,p2)
#define SAP_TRACE_WARNING3(m,p1,p2,p3)
#define SAP_TRACE_WARNING4(m,p1,p2,p3,p4)
#define SAP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define SAP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define SAP_TRACE_API0(m)
#define SAP_TRACE_API1(m,p1)
#define SAP_TRACE_API2(m,p1,p2)
#define SAP_TRACE_API3(m,p1,p2,p3)
#define SAP_TRACE_API4(m,p1,p2,p3,p4)
#define SAP_TRACE_API5(m,p1,p2,p3,p4,p5)
#define SAP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define SAP_TRACE_EVENT0(m)
#define SAP_TRACE_EVENT1(m,p1)
#define SAP_TRACE_EVENT2(m,p1,p2)
#define SAP_TRACE_EVENT3(m,p1,p2,p3)
#define SAP_TRACE_EVENT4(m,p1,p2,p3,p4)
#define SAP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define SAP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define SAP_TRACE_DEBUG0(m)
#define SAP_TRACE_DEBUG1(m,p1)
#define SAP_TRACE_DEBUG2(m,p1,p2)
#define SAP_TRACE_DEBUG3(m,p1,p2,p3)
#define SAP_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define SAP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define SAP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

/* Define tracing for the OPP profile
*/
#define OPP_TRACE_ERROR0(m)
#define OPP_TRACE_ERROR1(m,p1)
#define OPP_TRACE_ERROR2(m,p1,p2)
#define OPP_TRACE_ERROR3(m,p1,p2,p3)
#define OPP_TRACE_ERROR4(m,p1,p2,p3,p4)
#define OPP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define OPP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define OPP_TRACE_WARNING0(m)
#define OPP_TRACE_WARNING1(m,p1)
#define OPP_TRACE_WARNING2(m,p1,p2)
#define OPP_TRACE_WARNING3(m,p1,p2,p3)
#define OPP_TRACE_WARNING4(m,p1,p2,p3,p4)
#define OPP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define OPP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define OPP_TRACE_EVENT0(m)
#define OPP_TRACE_EVENT1(m,p1)
#define OPP_TRACE_EVENT2(m,p1,p2)
#define OPP_TRACE_EVENT3(m,p1,p2,p3)
#define OPP_TRACE_EVENT4(m,p1,p2,p3,p4)
#define OPP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define OPP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define OPP_TRACE_DEBUG0(m)
#define OPP_TRACE_DEBUG1(m,p1)
#define OPP_TRACE_DEBUG2(m,p1,p2)
#define OPP_TRACE_DEBUG3(m,p1,p2,p3)
#define OPP_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define OPP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define OPP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

#define OPP_TRACE_API0(m)
#define OPP_TRACE_API1(m,p1)
#define OPP_TRACE_API2(m,p1,p2)
#define OPP_TRACE_API3(m,p1,p2,p3)
#define OPP_TRACE_API4(m,p1,p2,p3,p4)
#define OPP_TRACE_API5(m,p1,p2,p3,p4,p5)
#define OPP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

/* Define tracing for the FTP profile
*/
#define FTP_TRACE_ERROR0(m)
#define FTP_TRACE_ERROR1(m,p1)
#define FTP_TRACE_ERROR2(m,p1,p2)
#define FTP_TRACE_ERROR3(m,p1,p2,p3)
#define FTP_TRACE_ERROR4(m,p1,p2,p3,p4)
#define FTP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define FTP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define FTP_TRACE_WARNING0(m)
#define FTP_TRACE_WARNING1(m,p1)
#define FTP_TRACE_WARNING2(m,p1,p2)
#define FTP_TRACE_WARNING3(m,p1,p2,p3)
#define FTP_TRACE_WARNING4(m,p1,p2,p3,p4)
#define FTP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define FTP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define FTP_TRACE_EVENT0(m)
#define FTP_TRACE_EVENT1(m,p1)
#define FTP_TRACE_EVENT2(m,p1,p2)
#define FTP_TRACE_EVENT3(m,p1,p2,p3)
#define FTP_TRACE_EVENT4(m,p1,p2,p3,p4)
#define FTP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define FTP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define FTP_TRACE_DEBUG0(m)
#define FTP_TRACE_DEBUG1(m,p1)
#define FTP_TRACE_DEBUG2(m,p1,p2)
#define FTP_TRACE_DEBUG3(m,p1,p2,p3)
#define FTP_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define FTP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define FTP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

#define FTP_TRACE_API0(m)
#define FTP_TRACE_API1(m,p1)
#define FTP_TRACE_API2(m,p1,p2)
#define FTP_TRACE_API3(m,p1,p2,p3)
#define FTP_TRACE_API4(m,p1,p2,p3,p4)
#define FTP_TRACE_API5(m,p1,p2,p3,p4,p5)
#define FTP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)


/* Define tracing for the A2DP profile
*/
#define A2D_TRACE_ERROR0(m)
#define A2D_TRACE_ERROR1(m,p1)
#define A2D_TRACE_ERROR2(m,p1,p2)
#define A2D_TRACE_ERROR3(m,p1,p2,p3)
#define A2D_TRACE_ERROR4(m,p1,p2,p3,p4)
#define A2D_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define A2D_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define A2D_TRACE_WARNING0(m)
#define A2D_TRACE_WARNING1(m,p1)
#define A2D_TRACE_WARNING2(m,p1,p2)
#define A2D_TRACE_WARNING3(m,p1,p2,p3)
#define A2D_TRACE_WARNING4(m,p1,p2,p3,p4)
#define A2D_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define A2D_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define A2D_TRACE_EVENT0(m)
#define A2D_TRACE_EVENT1(m,p1)
#define A2D_TRACE_EVENT2(m,p1,p2)
#define A2D_TRACE_EVENT3(m,p1,p2,p3)
#define A2D_TRACE_EVENT4(m,p1,p2,p3,p4)
#define A2D_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define A2D_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define A2D_TRACE_DEBUG0(m)
#define A2D_TRACE_DEBUG1(m,p1)
#define A2D_TRACE_DEBUG2(m,p1,p2)
#define A2D_TRACE_DEBUG3(m,p1,p2,p3)
#define A2D_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define A2D_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define A2D_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

#define A2D_TRACE_API0(m)
#define A2D_TRACE_API1(m,p1)
#define A2D_TRACE_API2(m,p1,p2)
#define A2D_TRACE_API3(m,p1,p2,p3)
#define A2D_TRACE_API4(m,p1,p2,p3,p4)
#define A2D_TRACE_API5(m,p1,p2,p3,p4,p5)
#define A2D_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

/* Define tracing for the VDP profile
*/
#define VDP_TRACE_ERROR0(m)
#define VDP_TRACE_ERROR1(m,p1)
#define VDP_TRACE_ERROR2(m,p1,p2)
#define VDP_TRACE_ERROR3(m,p1,p2,p3)
#define VDP_TRACE_ERROR4(m,p1,p2,p3,p4)
#define VDP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define VDP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define VDP_TRACE_WARNING0(m)
#define VDP_TRACE_WARNING1(m,p1)
#define VDP_TRACE_WARNING2(m,p1,p2)
#define VDP_TRACE_WARNING3(m,p1,p2,p3)
#define VDP_TRACE_WARNING4(m,p1,p2,p3,p4)
#define VDP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define VDP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define VDP_TRACE_EVENT0(m)
#define VDP_TRACE_EVENT1(m,p1)
#define VDP_TRACE_EVENT2(m,p1,p2)
#define VDP_TRACE_EVENT3(m,p1,p2,p3)
#define VDP_TRACE_EVENT4(m,p1,p2,p3,p4)
#define VDP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define VDP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define VDP_TRACE_DEBUG0(m)
#define VDP_TRACE_DEBUG1(m,p1)
#define VDP_TRACE_DEBUG2(m,p1,p2)
#define VDP_TRACE_DEBUG3(m,p1,p2,p3)
#define VDP_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define VDP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define VDP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

#define VDP_TRACE_API0(m)
#define VDP_TRACE_API1(m,p1)
#define VDP_TRACE_API2(m,p1,p2)
#define VDP_TRACE_API3(m,p1,p2,p3)
#define VDP_TRACE_API4(m,p1,p2,p3,p4)
#define VDP_TRACE_API5(m,p1,p2,p3,p4,p5)
#define VDP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)


/* Define tracing for the LM unit
*/
#define LMP_TRACE_ERROR0(m)
#define LMP_TRACE_ERROR1(m,p1)
#define LMP_TRACE_ERROR2(m,p1,p2)
#define LMP_TRACE_ERROR3(m,p1,p2,p3)
#define LMP_TRACE_ERROR4(m,p1,p2,p3,p4)
#define LMP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define LMP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define LMP_TRACE_WARNING0(m)
#define LMP_TRACE_WARNING1(m,p1)
#define LMP_TRACE_WARNING2(m,p1,p2)
#define LMP_TRACE_WARNING3(m,p1,p2,p3)
#define LMP_TRACE_WARNING4(m,p1,p2,p3,p4)
#define LMP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define LMP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define LMP_TRACE_EVENT0(m)
#define LMP_TRACE_EVENT1(m,p1)
#define LMP_TRACE_EVENT2(m,p1,p2)
#define LMP_TRACE_EVENT3(m,p1,p2,p3)
#define LMP_TRACE_EVENT4(m,p1,p2,p3,p4)
#define LMP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define LMP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define LMP_TRACE_DEBUG0(m)
#define LMP_TRACE_DEBUG1(m,p1)
#define LMP_TRACE_DEBUG2(m,p1,p2)
#define LMP_TRACE_DEBUG3(m,p1,p2,p3)
#define LMP_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define LMP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define LMP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

/* Define tracing for the LC unit
*/
#define LC_TRACE_ERROR0(m)
#define LC_TRACE_ERROR1(m,p1)
#define LC_TRACE_ERROR2(m,p1,p2)
#define LC_TRACE_ERROR3(m,p1,p2,p3)
#define LC_TRACE_ERROR4(m,p1,p2,p3,p4)
#define LC_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define LC_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define LC_TRACE_WARNING0(m)
#define LC_TRACE_WARNING1(m,p1)
#define LC_TRACE_WARNING2(m,p1,p2)
#define LC_TRACE_WARNING3(m,p1,p2,p3)
#define LC_TRACE_WARNING4(m,p1,p2,p3,p4)
#define LC_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define LC_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define LC_TRACE_EVENT0(m)
#define LC_TRACE_EVENT1(m,p1)
#define LC_TRACE_EVENT2(m,p1,p2)
#define LC_TRACE_EVENT3(m,p1,p2,p3)
#define LC_TRACE_EVENT4(m,p1,p2,p3,p4)
#define LC_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define LC_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define LC_TRACE_DEBUG0(m)
#define LC_TRACE_DEBUG1(m,p1)
#define LC_TRACE_DEBUG2(m,p1,p2)
#define LC_TRACE_DEBUG3(m,p1,p2,p3)
#define LC_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define LC_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define LC_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

#define SDA_TRACE_ERROR0(m)
#define SDA_TRACE_ERROR1(m,p1)
#define SDA_TRACE_ERROR2(m,p1,p2)
#define SDA_TRACE_ERROR3(m,p1,p2,p3)
#define SDA_TRACE_ERROR4(m,p1,p2,p3,p4)
#define SDA_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define SDA_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define SDA_TRACE_WARNING0(m)
#define SDA_TRACE_WARNING1(m,p1)
#define SDA_TRACE_WARNING2(m,p1,p2)
#define SDA_TRACE_WARNING3(m,p1,p2,p3)
#define SDA_TRACE_WARNING4(m,p1,p2,p3,p4)
#define SDA_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define SDA_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define SDA_TRACE_EVENT0(m)
#define SDA_TRACE_EVENT1(m,p1)
#define SDA_TRACE_EVENT2(m,p1,p2)
#define SDA_TRACE_EVENT3(m,p1,p2,p3)
#define SDA_TRACE_EVENT4(m,p1,p2,p3,p4)
#define SDA_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define SDA_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define SDA_TRACE_DEBUG0(m)
#define SDA_TRACE_DEBUG1(m,p1)
#define SDA_TRACE_DEBUG2(m,p1,p2)
#define SDA_TRACE_DEBUG3(m,p1,p2,p3)
#define SDA_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define SDA_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define SDA_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

/* AVDTP
*/
#define AVDT_TRACE_ERROR0(m)
#define AVDT_TRACE_ERROR1(m,p1)
#define AVDT_TRACE_ERROR2(m,p1,p2)
#define AVDT_TRACE_ERROR3(m,p1,p2,p3)
#define AVDT_TRACE_ERROR4(m,p1,p2,p3,p4)
#define AVDT_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define AVDT_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define AVDT_TRACE_WARNING0(m)
#define AVDT_TRACE_WARNING1(m,p1)
#define AVDT_TRACE_WARNING2(m,p1,p2)
#define AVDT_TRACE_WARNING3(m,p1,p2,p3)
#define AVDT_TRACE_WARNING4(m,p1,p2,p3,p4)
#define AVDT_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define AVDT_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define AVDT_TRACE_EVENT0(m)
#define AVDT_TRACE_EVENT1(m,p1)
#define AVDT_TRACE_EVENT2(m,p1,p2)
#define AVDT_TRACE_EVENT3(m,p1,p2,p3)
#define AVDT_TRACE_EVENT4(m,p1,p2,p3,p4)
#define AVDT_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define AVDT_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define AVDT_TRACE_DEBUG0(m)
#define AVDT_TRACE_DEBUG1(m,p1)
#define AVDT_TRACE_DEBUG2(m,p1,p2)
#define AVDT_TRACE_DEBUG3(m,p1,p2,p3)
#define AVDT_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define AVDT_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define AVDT_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

#define AVDT_TRACE_API0(m)
#define AVDT_TRACE_API1(m,p1)
#define AVDT_TRACE_API2(m,p1,p2)
#define AVDT_TRACE_API3(m,p1,p2,p3)
#define AVDT_TRACE_API4(m,p1,p2,p3,p4)
#define AVDT_TRACE_API5(m,p1,p2,p3,p4,p5)
#define AVDT_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

/* Define tracing for the AVCTP protocol
*/
#define AVCT_TRACE_ERROR0(m)
#define AVCT_TRACE_ERROR1(m,p1)
#define AVCT_TRACE_ERROR2(m,p1,p2)
#define AVCT_TRACE_ERROR3(m,p1,p2,p3)
#define AVCT_TRACE_ERROR4(m,p1,p2,p3,p4)
#define AVCT_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define AVCT_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define AVCT_TRACE_WARNING0(m)
#define AVCT_TRACE_WARNING1(m,p1)
#define AVCT_TRACE_WARNING2(m,p1,p2)
#define AVCT_TRACE_WARNING3(m,p1,p2,p3)
#define AVCT_TRACE_WARNING4(m,p1,p2,p3,p4)
#define AVCT_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define AVCT_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define AVCT_TRACE_EVENT0(m)
#define AVCT_TRACE_EVENT1(m,p1)
#define AVCT_TRACE_EVENT2(m,p1,p2)
#define AVCT_TRACE_EVENT3(m,p1,p2,p3)
#define AVCT_TRACE_EVENT4(m,p1,p2,p3,p4)
#define AVCT_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define AVCT_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define AVCT_TRACE_DEBUG0(m)
#define AVCT_TRACE_DEBUG1(m,p1)
#define AVCT_TRACE_DEBUG2(m,p1,p2)
#define AVCT_TRACE_DEBUG3(m,p1,p2,p3)
#define AVCT_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define AVCT_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define AVCT_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

#define AVCT_TRACE_API0(m)
#define AVCT_TRACE_API1(m,p1)
#define AVCT_TRACE_API2(m,p1,p2)
#define AVCT_TRACE_API3(m,p1,p2,p3)
#define AVCT_TRACE_API4(m,p1,p2,p3,p4)
#define AVCT_TRACE_API5(m,p1,p2,p3,p4,p5)
#define AVCT_TRACE_API6(m,p1,p2,p3,p4,p5,p6)


/* Define tracing for the AVRCP profile
*/
#define AVRC_TRACE_ERROR0(m)
#define AVRC_TRACE_ERROR1(m,p1)
#define AVRC_TRACE_ERROR2(m,p1,p2)
#define AVRC_TRACE_ERROR3(m,p1,p2,p3)
#define AVRC_TRACE_ERROR4(m,p1,p2,p3,p4)
#define AVRC_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define AVRC_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define AVRC_TRACE_WARNING0(m)
#define AVRC_TRACE_WARNING1(m,p1)
#define AVRC_TRACE_WARNING2(m,p1,p2)
#define AVRC_TRACE_WARNING3(m,p1,p2,p3)
#define AVRC_TRACE_WARNING4(m,p1,p2,p3,p4)
#define AVRC_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define AVRC_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define AVRC_TRACE_EVENT0(m)
#define AVRC_TRACE_EVENT1(m,p1)
#define AVRC_TRACE_EVENT2(m,p1,p2)
#define AVRC_TRACE_EVENT3(m,p1,p2,p3)
#define AVRC_TRACE_EVENT4(m,p1,p2,p3,p4)
#define AVRC_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define AVRC_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define AVRC_TRACE_DEBUG0(m)
#define AVRC_TRACE_DEBUG1(m,p1)
#define AVRC_TRACE_DEBUG2(m,p1,p2)
#define AVRC_TRACE_DEBUG3(m,p1,p2,p3)
#define AVRC_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define AVRC_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define AVRC_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

#define AVRC_TRACE_API0(m)
#define AVRC_TRACE_API1(m,p1)
#define AVRC_TRACE_API2(m,p1,p2)
#define AVRC_TRACE_API3(m,p1,p2,p3)
#define AVRC_TRACE_API4(m,p1,p2,p3,p4)
#define AVRC_TRACE_API5(m,p1,p2,p3,p4,p5)
#define AVRC_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

/* MCAP
*/
#define MCA_TRACE_ERROR0(m)
#define MCA_TRACE_ERROR1(m,p1)
#define MCA_TRACE_ERROR2(m,p1,p2)
#define MCA_TRACE_ERROR3(m,p1,p2,p3)
#define MCA_TRACE_ERROR4(m,p1,p2,p3,p4)
#define MCA_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define MCA_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define MCA_TRACE_WARNING0(m)
#define MCA_TRACE_WARNING1(m,p1)
#define MCA_TRACE_WARNING2(m,p1,p2)
#define MCA_TRACE_WARNING3(m,p1,p2,p3)
#define MCA_TRACE_WARNING4(m,p1,p2,p3,p4)
#define MCA_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define MCA_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define MCA_TRACE_EVENT0(m)
#define MCA_TRACE_EVENT1(m,p1)
#define MCA_TRACE_EVENT2(m,p1,p2)
#define MCA_TRACE_EVENT3(m,p1,p2,p3)
#define MCA_TRACE_EVENT4(m,p1,p2,p3,p4)
#define MCA_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define MCA_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define MCA_TRACE_DEBUG0(m)
#define MCA_TRACE_DEBUG1(m,p1)
#define MCA_TRACE_DEBUG2(m,p1,p2)
#define MCA_TRACE_DEBUG3(m,p1,p2,p3)
#define MCA_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define MCA_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define MCA_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

#define MCA_TRACE_API0(m)
#define MCA_TRACE_API1(m,p1)
#define MCA_TRACE_API2(m,p1,p2)
#define MCA_TRACE_API3(m,p1,p2,p3)
#define MCA_TRACE_API4(m,p1,p2,p3,p4)
#define MCA_TRACE_API5(m,p1,p2,p3,p4,p5)
#define MCA_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

/* Define tracing for the GATT
*/
#define GATT_TRACE_ERROR0(m)
#define GATT_TRACE_ERROR1(m,p1)
#define GATT_TRACE_ERROR2(m,p1,p2)
#define GATT_TRACE_ERROR3(m,p1,p2,p3)
#define GATT_TRACE_ERROR4(m,p1,p2,p3,p4)
#define GATT_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define GATT_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define GATT_TRACE_WARNING0(m)
#define GATT_TRACE_WARNING1(m,p1)
#define GATT_TRACE_WARNING2(m,p1,p2)
#define GATT_TRACE_WARNING3(m,p1,p2,p3)
#define GATT_TRACE_WARNING4(m,p1,p2,p3,p4)
#define GATT_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define GATT_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define GATT_TRACE_API0(m)
#define GATT_TRACE_API1(m,p1)
#define GATT_TRACE_API2(m,p1,p2)
#define GATT_TRACE_API3(m,p1,p2,p3)
#define GATT_TRACE_API4(m,p1,p2,p3,p4)
#define GATT_TRACE_API5(m,p1,p2,p3,p4,p5)
#define GATT_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define GATT_TRACE_EVENT0(m)
#define GATT_TRACE_EVENT1(m,p1)
#define GATT_TRACE_EVENT2(m,p1,p2)
#define GATT_TRACE_EVENT3(m,p1,p2,p3)
#define GATT_TRACE_EVENT4(m,p1,p2,p3,p4)
#define GATT_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define GATT_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define GATT_TRACE_DEBUG0(m)
#define GATT_TRACE_DEBUG1(m,p1)
#define GATT_TRACE_DEBUG2(m,p1,p2)
#define GATT_TRACE_DEBUG3(m,p1,p2,p3)
#define GATT_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define GATT_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define GATT_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

/* Define tracing for the SMP unit
*/
#define SMP_TRACE_ERROR0(m)
#define SMP_TRACE_ERROR1(m,p1)
#define SMP_TRACE_ERROR2(m,p1,p2)
#define SMP_TRACE_ERROR3(m,p1,p2,p3)
#define SMP_TRACE_ERROR4(m,p1,p2,p3,p4)
#define SMP_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define SMP_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define SMP_TRACE_WARNING0(m)
#define SMP_TRACE_WARNING1(m,p1)
#define SMP_TRACE_WARNING2(m,p1,p2)
#define SMP_TRACE_WARNING3(m,p1,p2,p3)
#define SMP_TRACE_WARNING4(m,p1,p2,p3,p4)
#define SMP_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define SMP_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define SMP_TRACE_API0(m)
#define SMP_TRACE_API1(m,p1)
#define SMP_TRACE_API2(m,p1,p2)
#define SMP_TRACE_API3(m,p1,p2,p3)
#define SMP_TRACE_API4(m,p1,p2,p3,p4)
#define SMP_TRACE_API5(m,p1,p2,p3,p4,p5)
#define SMP_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define SMP_TRACE_EVENT0(m)
#define SMP_TRACE_EVENT1(m,p1)
#define SMP_TRACE_EVENT2(m,p1,p2)
#define SMP_TRACE_EVENT3(m,p1,p2,p3)
#define SMP_TRACE_EVENT4(m,p1,p2,p3,p4)
#define SMP_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define SMP_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define SMP_TRACE_DEBUG0(m)
#define SMP_TRACE_DEBUG1(m,p1)
#define SMP_TRACE_DEBUG2(m,p1,p2)
#define SMP_TRACE_DEBUG3(m,p1,p2,p3)
#define SMP_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define SMP_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define SMP_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

#endif

#if (BT_USE_TRACES == TRUE || BT_TRACE_APPL == TRUE) && !RELEASE_VERSION

/* define traces for application */
#define APPL_TRACE_ERROR0(m)                    {if (appl_trace_level >= BT_TRACE_LEVEL_ERROR) LogMsg_0(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_ERROR, (m));}
#define APPL_TRACE_ERROR1(m,p1)                 {if (appl_trace_level >= BT_TRACE_LEVEL_ERROR) LogMsg_1(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_ERROR, \
                                                        (m), (UINTPTR)(p1));}
#define APPL_TRACE_ERROR2(m,p1,p2)              {if (appl_trace_level >= BT_TRACE_LEVEL_ERROR) LogMsg_2(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_ERROR, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2));}
#define APPL_TRACE_ERROR3(m,p1,p2,p3)           {if (appl_trace_level >= BT_TRACE_LEVEL_ERROR) LogMsg_3(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_ERROR, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3));}
#define APPL_TRACE_ERROR4(m,p1,p2,p3,p4)        {if (appl_trace_level >= BT_TRACE_LEVEL_ERROR) LogMsg_4(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_ERROR, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4));}
#define APPL_TRACE_ERROR5(m,p1,p2,p3,p4,p5)     {if (appl_trace_level >= BT_TRACE_LEVEL_ERROR) LogMsg_5(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_ERROR, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4), (UINTPTR)(p5));}
#define APPL_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)  {if (appl_trace_level >= BT_TRACE_LEVEL_ERROR) LogMsg_6(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_ERROR, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4), (UINTPTR)(p5), (UINTPTR)(p6));}

#define APPL_TRACE_WARNING0(m)                  {if (appl_trace_level >= BT_TRACE_LEVEL_WARNING) LogMsg_0(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_WARNING, (m));}
#define APPL_TRACE_WARNING1(m,p1)               {if (appl_trace_level >= BT_TRACE_LEVEL_WARNING) LogMsg_1(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_WARNING, \
                                                        (m), (UINTPTR)(p1));}
#define APPL_TRACE_WARNING2(m,p1,p2)            {if (appl_trace_level >= BT_TRACE_LEVEL_WARNING) LogMsg_2(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_WARNING, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2));}
#define APPL_TRACE_WARNING3(m,p1,p2,p3)         {if (appl_trace_level >= BT_TRACE_LEVEL_WARNING) LogMsg_3(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_WARNING, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3));}
#define APPL_TRACE_WARNING4(m,p1,p2,p3,p4)      {if (appl_trace_level >= BT_TRACE_LEVEL_WARNING) LogMsg_4(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_WARNING, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4));}
#define APPL_TRACE_WARNING5(m,p1,p2,p3,p4,p5)   {if (appl_trace_level >= BT_TRACE_LEVEL_WARNING) LogMsg_5(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_WARNING, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4), (UINTPTR)(p5));}
#define APPL_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6) {if (appl_trace_level >= BT_TRACE_LEVEL_WARNING) LogMsg_6(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_WARNING, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4), (UINTPTR)(p5), (UINTPTR)(p6));}

#define APPL_TRACE_API0(m)                      {if (appl_trace_level >= BT_TRACE_LEVEL_API) LogMsg_0(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_API, (m));}
#define APPL_TRACE_API1(m,p1)                   {if (appl_trace_level >= BT_TRACE_LEVEL_API) LogMsg_1(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_API, \
                                                        (m), (UINTPTR)(p1));}
#define APPL_TRACE_API2(m,p1,p2)                {if (appl_trace_level >= BT_TRACE_LEVEL_API) LogMsg_2(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_API, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2));}
#define APPL_TRACE_API3(m,p1,p2,p3)             {if (appl_trace_level >= BT_TRACE_LEVEL_API) LogMsg_3(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_API, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3));}
#define APPL_TRACE_API4(m,p1,p2,p3,p4)          {if (appl_trace_level >= BT_TRACE_LEVEL_API) LogMsg_4(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_API, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4));}
#define APPL_TRACE_API5(m,p1,p2,p3,p4,p5)       {if (appl_trace_level >= BT_TRACE_LEVEL_API) LogMsg_5(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_API, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4), (UINTPTR)(p5));}
#define APPL_TRACE_API6(m,p1,p2,p3,p4,p5,p6)    {if (appl_trace_level >= BT_TRACE_LEVEL_API) LogMsg_6(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_API, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4), (UINTPTR)(p5), (UINTPTR)(p6));}

#define APPL_TRACE_EVENT0(m)                    {if (appl_trace_level >= BT_TRACE_LEVEL_EVENT) LogMsg_0(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_EVENT, (m));}
#define APPL_TRACE_EVENT1(m,p1)                 {if (appl_trace_level >= BT_TRACE_LEVEL_EVENT) LogMsg_1(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_EVENT, \
                                                        (m), (UINTPTR)(p1));}
#define APPL_TRACE_EVENT2(m,p1,p2)              {if (appl_trace_level >= BT_TRACE_LEVEL_EVENT) LogMsg_2(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_EVENT, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2));}
#define APPL_TRACE_EVENT3(m,p1,p2,p3)           {if (appl_trace_level >= BT_TRACE_LEVEL_EVENT) LogMsg_3(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_EVENT, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3));}
#define APPL_TRACE_EVENT4(m,p1,p2,p3,p4)        {if (appl_trace_level >= BT_TRACE_LEVEL_EVENT) LogMsg_4(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_EVENT, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4));}
#define APPL_TRACE_EVENT5(m,p1,p2,p3,p4,p5)     {if (appl_trace_level >= BT_TRACE_LEVEL_EVENT) LogMsg_5(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_EVENT, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4), (UINTPTR)(p5));}
#define APPL_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)  {if (appl_trace_level >= BT_TRACE_LEVEL_EVENT) LogMsg_6(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_EVENT, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4), (UINTPTR)(p5), (UINTPTR)(p6));}

#define APPL_TRACE_DEBUG0(m)                    {if (appl_trace_level >= BT_TRACE_LEVEL_DEBUG) LogMsg_0(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_DEBUG, (m));}
#define APPL_TRACE_DEBUG1(m,p1)                 {if (appl_trace_level >= BT_TRACE_LEVEL_DEBUG) LogMsg_1(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_DEBUG, \
                                                        (m), (UINTPTR)(p1));}
#define APPL_TRACE_DEBUG2(m,p1,p2)              {if (appl_trace_level >= BT_TRACE_LEVEL_DEBUG) LogMsg_2(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_DEBUG, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2));}
#define APPL_TRACE_DEBUG3(m,p1,p2,p3)           {if (appl_trace_level >= BT_TRACE_LEVEL_DEBUG) LogMsg_3(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_DEBUG, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3));}
#define APPL_TRACE_DEBUG4(m,p1,p2,p3,p4)        {if (appl_trace_level >= BT_TRACE_LEVEL_DEBUG) LogMsg_4(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_DEBUG, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4));}
#define APPL_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)     {if (appl_trace_level >= BT_TRACE_LEVEL_DEBUG) LogMsg_5(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_DEBUG, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4), (UINTPTR)(p5));}
#define APPL_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)  {if (appl_trace_level >= BT_TRACE_LEVEL_DEBUG) LogMsg_6(TRACE_CTRL_GENERAL | TRACE_LAYER_NONE | TRACE_ORG_APPL | TRACE_TYPE_DEBUG, \
                                                        (m), (UINTPTR)(p1), (UINTPTR)(p2), (UINTPTR)(p3), (UINTPTR)(p4), (UINTPTR)(p5), (UINTPTR)(p6));}
#else
/* define traces for Application */

#define APPL_TRACE_ERROR0(m)
#define APPL_TRACE_ERROR1(m,p1)
#define APPL_TRACE_ERROR2(m,p1,p2)
#define APPL_TRACE_ERROR3(m,p1,p2,p3)
#define APPL_TRACE_ERROR4(m,p1,p2,p3,p4)
#define APPL_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define APPL_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

#define APPL_TRACE_WARNING0(m)
#define APPL_TRACE_WARNING1(m,p1)
#define APPL_TRACE_WARNING2(m,p1,p2)
#define APPL_TRACE_WARNING3(m,p1,p2,p3)
#define APPL_TRACE_WARNING4(m,p1,p2,p3,p4)
#define APPL_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define APPL_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)

#define APPL_TRACE_API0(m)
#define APPL_TRACE_API1(m,p1)
#define APPL_TRACE_API2(m,p1,p2)
#define APPL_TRACE_API3(m,p1,p2,p3)
#define APPL_TRACE_API4(m,p1,p2,p3,p4)
#define APPL_TRACE_API5(m,p1,p2,p3,p4,p5)
#define APPL_TRACE_API6(m,p1,p2,p3,p4,p5,p6)

#define APPL_TRACE_EVENT0(m)
#define APPL_TRACE_EVENT1(m,p1)
#define APPL_TRACE_EVENT2(m,p1,p2)
#define APPL_TRACE_EVENT3(m,p1,p2,p3)
#define APPL_TRACE_EVENT4(m,p1,p2,p3,p4)
#define APPL_TRACE_EVENT5(m,p1,p2,p3,p4,p5)
#define APPL_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)

#define APPL_TRACE_DEBUG0(m)
#define APPL_TRACE_DEBUG1(m,p1)
#define APPL_TRACE_DEBUG2(m,p1,p2)
#define APPL_TRACE_DEBUG3(m,p1,p2,p3)
#define APPL_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define APPL_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define APPL_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)

#endif

#if ((MMI_INCLUDED == TRUE) && (!defined(HID_MSKB_INCLUDED) || (HID_MSKB_INCLUDED == FALSE)))
/* UI for sample applications */
#define SAP_TRACE_0(m)                            MMI_Echo(m)
#define SAP_TRACE_1(m,p1)                         MMI_Echo(m,p1)
#define SAP_TRACE_2(m,p1,p2)                      MMI_Echo(m,p1,p2)
#define SAP_TRACE_3(m,p1,p2,p3)                   MMI_Echo(m,p1,p2,p3)
#define SAP_TRACE_4(m,p1,p2,p3,p4)                MMI_Echo(m,p1,p2,p3,p4)
#define SAP_TRACE_5(m,p1,p2,p3,p4,p5)             MMI_Echo(m,p1,p2,p3,p4,p5)
#define SAP_TRACE_6(m,p1,p2,p3,p4,p5,p6)          MMI_Echo(m,p1,p2,p3,p4,p5,p6)
#else
#define SAP_TRACE_0(m)
#define SAP_TRACE_1(m,p1)
#define SAP_TRACE_2(m,p1,p2)
#define SAP_TRACE_3(m,p1,p2,p3)
#define SAP_TRACE_4(m,p1,p2,p3,p4)
#define SAP_TRACE_5(m,p1,p2,p3,p4,p5)
#define SAP_TRACE_6(m,p1,p2,p3,p4,p5,p6)

#endif  /* End of MMI_INCLUDED */
#if defined(DRV_DEBUG_MSG) && (DRV_DEBUG_MSG == TRUE)
/* Driver Trace macros
*/
#define DRV_TRACE_WARNING0(m)                      APPL_TRACE_WARNING0(m)
#define DRV_TRACE_WARNING1(m,p1)                   APPL_TRACE_WARNING1(m,p1)
#define DRV_TRACE_WARNING2(m,p1,p2)                APPL_TRACE_WARNING2(m,p1,p2)
#define DRV_TRACE_WARNING3(m,p1,p2,p3)             APPL_TRACE_WARNING3(m,p1,p2,p3)
#define DRV_TRACE_WARNING4(m,p1,p2,p3,p4)          APPL_TRACE_WARNING4(m,p1,p2,p3,p4)
#define DRV_TRACE_WARNING5(m,p1,p2,p3,p4,p5)       APPL_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define DRV_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)    APPL_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)
#else
/* Driver Trace macros
*/
#define DRV_TRACE_WARNING0(m)
#define DRV_TRACE_WARNING1(m,p1)
#define DRV_TRACE_WARNING2(m,p1,p2)
#define DRV_TRACE_WARNING3(m,p1,p2,p3)
#define DRV_TRACE_WARNING4(m,p1,p2,p3,p4)
#define DRV_TRACE_WARNING5(m,p1,p2,p3,p4,p5)
#define DRV_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)
#endif

#define DRV_TRACE_ERROR0(m)                      APPL_TRACE_ERROR0(m)
#define DRV_TRACE_ERROR1(m,p1)                   APPL_TRACE_ERROR1(m,p1)
#define DRV_TRACE_ERROR2(m,p1,p2)                APPL_TRACE_ERROR2(m,p1,p2)
#define DRV_TRACE_ERROR3(m,p1,p2,p3)             APPL_TRACE_ERROR3(m,p1,p2,p3)
#define DRV_TRACE_ERROR4(m,p1,p2,p3,p4)          APPL_TRACE_ERROR4(m,p1,p2,p3,p4)
#define DRV_TRACE_ERROR5(m,p1,p2,p3,p4,p5)       APPL_TRACE_ERROR5(m,p1,p2,p3,p4,p5)
#define DRV_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)    APPL_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)

/* Driver Trace macros
*/
#define DRV_TRACE_DEBUG0(m)                    APPL_TRACE_DEBUG0(m)
#define DRV_TRACE_DEBUG1(m,p1)                 APPL_TRACE_DEBUG1(m,p1)
#define DRV_TRACE_DEBUG2(m,p1,p2)              APPL_TRACE_DEBUG2(m,p1,p2)
#define DRV_TRACE_DEBUG3(m,p1,p2,p3)           APPL_TRACE_DEBUG3(m,p1,p2,p3)
#define DRV_TRACE_DEBUG4(m,p1,p2,p3,p4)        APPL_TRACE_DEBUG4(m,p1,p2,p3,p4)
#define DRV_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)     APPL_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)
#define DRV_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)  APPL_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)


#endif /* BT_TRACE_H */


