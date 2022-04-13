/*
 * @file uknx_def.h
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UKNX_DEF_H__
#define __UKNX_DEF_H__

/** @addtogroup uknx_stack uknx
 *  Optional policy of uknx.
 *
 *  @{
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef int UKNX_RET;
typedef unsigned char *UKNX_BUF;
typedef int UKNX_SIZE;

typedef unsigned char UKNX_U8;
typedef char UKNX_I8;
typedef unsigned short UKNX_U16;
typedef short UKNX_I16;
typedef unsigned int UKNX_U32;
typedef int UKNX_I32;

#define UKNX_PACK __attribute__((packed))

typedef struct {
    UKNX_U8 len;
    UKNX_U8 proto;
    UKNX_U8 ip[4];
    UKNX_U16 port;
} UKNX_PACK UNKX_HPAI_IP;

typedef struct {
    UKNX_U8   ctrl;
    UKNX_U8   source[2];
    UKNX_U8   dest[2];
    UKNX_U8   npci;
    UKNX_U8   tpci;
    UKNX_U8   apci;
    UKNX_U8   data[0];
} UKNX_PACK UKNX_LL_FRAME, *UKNX_LL_FRAME_REF;

typedef enum {
    KNX_ADDR_INDIVIDUAL = ( ( UKNX_U8 )0x00 ),
    KNX_ADDR_MULTICAST  = ( ( UKNX_U8 )0x80 )
} UKnx_DafType;

typedef enum {
    KNX_FRAME_EXTENDED  = ( ( UKNX_U8 )0x00 ),
    KNX_FRAME_STANDARD  = ( ( UKNX_U8 )0x80 ),
    KNX_FRAME_POLLING   = ( ( UKNX_U8 )0xC0 )
} UKnx_FrameTypeType;

#define KNX_OBJ_PRIO_SYSTEM     ((UKNX_U8)0)
#define KNX_OBJ_PRIO_URGENT     ((UKNX_U8)2)
#define KNX_OBJ_PRIO_NORMAL     ((UKNX_U8)1)
#define KNX_OBJ_PRIO_LOW        ((UKNX_U8)3)

#if !defined(UKNX_LOBYTE)
#define UKNX_LOBYTE(w)               ((UKNX_U8)((UKNX_U16)((UKNX_U16)(w) & 0x00ffU)))
#endif

#if !defined(UKNX_HIBYTE)
#define UKNX_HIBYTE(w)               ((UKNX_U8)((UKNX_U16)(((UKNX_U16)(w ) >> 8) & 0x00ffU)))
#endif

/*
**  Group services.
*/
#define A_GROUPVALUE_READ                       ((UKNX_U16)0x0000)
#define A_GROUPVALUE_RESPONSE                   ((UKNX_U16)0x0040)
#define A_GROUPVALUE_WRITE                      ((UKNX_U16)0x0080)

/*
**  Broadcast.
*/
#define A_PHYSICALADDRESS_WRITE                 ((UKNX_U16)0x00C0)    /* Phys.AddrSet                 */
#define A_PHYSICALADDRESS_READ                  ((UKNX_U16)0x0100)    /* Phys.AddrRead                */
#define A_PHYSICALADDRESS_RESPONSE              ((UKNX_U16)0x0140)    /* Phys.AddrResponse            */

/*
**  Point-to-Point.
*/
#define A_ADC_READ                              ((UKNX_U16)0x0180)    /* ADCRead                      */
#define A_ADC_RESPONSE                          ((UKNX_U16)0x01C0)    /* ADCResponse                  */
#define A_MEMORY_READ                           ((UKNX_U16)0x0200)    /* MemoryRead                   */
#define A_MEMORY_RESPONSE                       ((UKNX_U16)0x0240)    /* MemoryResponse               */
#define A_MEMORY_WRITE                          ((UKNX_U16)0x0280)    /* MemoryWrite                  */

#define A_USERMEMORY_READ                       ((UKNX_U16)0x02C0)    /* UserDataRead                 */
#define A_USERMEMORY_RESPONSE                   ((UKNX_U16)0x02C1)    /* UserDataResponse             */
#define A_USERMEMORY_WRITE                      ((UKNX_U16)0x02C2)    /* UserDataWrite                */
#define A_USERMEMORYBIT_WRITE                   ((UKNX_U16)0x02C4)    /* UserDataBitWrite             */
#define A_USERMANUFACTURERINFO_READ             ((UKNX_U16)0x02C5)    /* UserMgmtTypeRead             */
#define A_USERMANUFACTURERINFO_RESPONSE         ((UKNX_U16)0x02C6)    /* UserMgmtTypeResponse         */

/*
** Reserved for USERMSGs: 0x2C7 - 2F7 (48 Codes).
** Reserved for manufacturer specific msgs: 0x2F8 - 2FE (6 Codes).
*/

#define A_DEVICEDESCRIPTOR_READ                 ((UKNX_U16)0x0300)    /* MaskVersionRead              */
#define A_DEVICEDESCRIPTOR_RESPONSE             ((UKNX_U16)0x0340)    /* MaskVersionResponse          */
#define A_RESTART                               ((UKNX_U16)0x0380)    /* Restart                      */

/*
**  Coupler-/Router-specific.
*/
#define A_OPEN_ROUTING_TABLE_REQ                ((UKNX_U16)0x03C0)    /* LcTabMemEnable               */
#define A_READ_ROUTING_TABLE_REQ                ((UKNX_U16)0x03C1)    /* LcTabMemRead                 */
#define A_READ_ROUTING_TABLE_RES                ((UKNX_U16)0x03C2)    /* LcTabMemResponse             */
#define A_WRITE_ROUTING_TABLE_REQ               ((UKNX_U16)0x03C3)    /* LcTabMemWrite                */
#define A_READ_ROUTER_MEMORY_REQ                ((UKNX_U16)0x03C8)    /* LcSlaveRead                  */
#define A_READ_ROUTER_MEMORY_RES                ((UKNX_U16)0x03C9)    /* LcSlaveResponse              */
#define A_WRITE_ROUTER_MEMORY_REQ               ((UKNX_U16)0x03CA)    /* LcSlaveWrite                 */
#define A_READ_ROUTER_STATUS_REQ                ((UKNX_U16)0x03CD)    /* LcGroupRead                  */
#define A_READ_ROUTER_STATUS_RES                ((UKNX_U16)0x03CE)    /* LcGroupResponse              */
#define A_WRITE_ROUTER_STATUS_REQ               ((UKNX_U16)0x03CF)    /* LcGroupWrite                 */

#define A_MEMORYBIT_WRITE                       ((UKNX_U16)0x03D0)    /* BitWrite                     */
#define A_AUTHORIZE_REQUEST                     ((UKNX_U16)0x03D1)    /* AuthorizeRequest             */
#define A_AUTHORIZE_RESPONSE                    ((UKNX_U16)0x03D2)    /* AuthorizeResponse            */
#define A_KEY_WRITE                             ((UKNX_U16)0x03D3)    /* SetKeyRequest                */
#define A_KEY_RESPONSE                          ((UKNX_U16)0x03D4)    /* SetKeyResponse               */

#define A_PROPERTYVALUE_READ                    ((UKNX_U16)0x03D5)    /* PropertyRead                 */
#define A_PROPERTYVALUE_RESPONSE                ((UKNX_U16)0x03D6)    /* PropertyResponse             */
#define A_PROPERTYVALUE_WRITE                   ((UKNX_U16)0x03D7)    /* PropertyWrite                */
#define A_PROPERTYDESCRIPTION_READ              ((UKNX_U16)0x03D8)    /* PropertyDescriptionRead      */
#define A_PROPERTYDESCRIPTION_RESPONSE          ((UKNX_U16)0x03D9)    /* PropertyDescriptionResponse  */

/*
** Broadcast.
**
*/
#define A_PHYSICALADDRESSSERIALNUMBER_READ      ((UKNX_U16)0x03DC)    /* PhysAddrSerNoRead            */
#define A_PHYSICALADDRESSSERIALNUMBER_RESPONSE  ((UKNX_U16)0x03DD)    /* PhysAddrSerNoResponse        */
#define A_PHYSICALADDRESSSERIALNUMBER_WRITE     ((UKNX_U16)0x03DE)    /* PhysAddrSerNoWrite           */
#define A_SERVICEINFORMATION_INDICATION_WRITE   ((UKNX_U16)0x03DF)    /* ServiceInfo                  */

#define A_DOMAINADDRESS_WRITE                   ((UKNX_U16)0x03E0)    /* SysIdWrite                   */
#define A_DOMAINADDRESS_READ                    ((UKNX_U16)0x03E1)    /* SysIdRead                    */
#define A_DOMAINADDRESS_RESPONSE                ((UKNX_U16)0x03E2)    /* SysIdResponse                */
#define A_DOMAINADDRESSSELECTIVE_READ           ((UKNX_U16)0x03E3)    /* SysIdSelectiveRead           */

#define A_NETWORKPARAMETER_WRITE                ((UKNX_U16)0x03E4)    /* A_NetworkParameter_Write     */
#define A_NETWORKPARAMETER_READ                 ((UKNX_U16)0x03DA)    /* (s. Supplement S09, S08)     */
#define A_NETWORKPARAMETER_RESPONSE             ((UKNX_U16)0x03DB)    /* (s. Supplement S03)          */

/*
**  Point to Point.
*/
#define A_LINK_READ                             ((UKNX_U16)0x03E5)
#define A_LINK_RESPONSE                         ((UKNX_U16)0x03E6)
#define A_LINK_WRITE                            ((UKNX_U16)0x03E7)

/*
**  Multicast.
*/
#define A_GROUPPROPVALUE_READ                   ((UKNX_U16)0x03E8)    /* s. KNX 10_01 LTE             */
#define A_GROUPPROPVALUE_RESPONSE               ((UKNX_U16)0x03E9)
#define A_GROUPPROPVALUE_WRITE                  ((UKNX_U16)0x03EA)
#define A_GROUPPROPVALUE_INFOREPORT             ((UKNX_U16)0x03EB)

#define BARRAY(buf,index) (((const unsigned char *)(buf))[(index)])
#define APCI(tpdu) (((BARRAY(tpdu,0)&0x3)<<2)|((BARRAY(tpdu,1)&0xC0)>>6))
#define APCI_EXT(tpdu) (BARRAY(tpdu,1)&0x3F)

/*
**  APCI.
*/

#define APCI_GROUPVALUE_READ 0x0
#define APCI_GROUPVALUE_RESPONSE 0x1
#define APCI_GROUPVALUE_WRITE 0x2

#define APCI_EXTF 0xF
#define APCI_EXTF_AUTHORIZE_RESPONSE 0x12
#define APCI_EXTF_PROPERTYVALUE_RESPONSE 0x16
#define APCI_EXTF_PROPERTYDESCRIPTION_RESPONSE 0x19


#define UKnxMsg_GetFrameType(pb)            ((pb->ctrl) & (UKNX_U8)0xF0)
#define UKnxMsg_SetFrameType(pb, type)      (pb->ctrl |= ((type) & (UKNX_U8)0xF0))
#define UKnxMsg_GetSourceAddress_H(pb)        (pb->source[0])
#define UKnxMsg_GetSourceAddress_L(pb)        (pb->source[1])
#define UKnxMsg_GetDestAddress_H(pb)        (pb->dest[0])
#define UKnxMsg_GetDestAddress_L(pb)        (pb->dest[1])
#define UKnxMsg_SetSourceAddress_H(pb, a)        (pb->source[0] = a)
#define UKnxMsg_SetSourceAddress_L(pb, a)        (pb->source[1] = a)
#define UKnxMsg_SetDestAddress_H(pb, a)        (pb->dest[0] = a)
#define UKnxMsg_SetDestAddress_L(pb, a)        (pb->dest[1] = a)

#define UKnxMsg_GetPriority(pb)             ((pb->ctrl & (UKNX_U8)0x0C) >> 2)
#define UKnxMsg_SetPriority(pb, priority)   (pb->ctrl |= (((priority) & (UKNX_U8)0x03) << 2))

#define UKnxMsg_GetAddressType(pb)          ((pb->npci & (UKNX_U8)0x80))
#define UKnxMsg_SetAddressType(pb, at)      (pb->npci |= ((at) & (UKNX_U8)0x80))

#define UKnxMsg_IsMulticastAddressed(pb)    (UKnxMsg_GetAddressType((pb)) == KNX_ADDR_MULTICAST)
#define UKnxMsg_IsIndividualAddressed(pb)   (UKnxMsg_GetAddressType((pb)) == KNX_ADDR_INDIVIDUAL)

#define UKnxMsg_GetLSDULen(pb)              (pb->npci & (UKNX_U8)0x0f)
#define UKnxMsg_SetLSDULen(pb, len_lsdu)    ((pb)->npci |= ((len_lsdu) & (UKNX_U8)0x0f))

#define UKnxMsg_GetHopCount(pb)              ((pb->npci & 0x70)>>4)
#define UKnxMsg_SetHopCount(pb, hc)    ((pb)->npci |= (((hc) & (UKNX_U8)0x7)<<4))

#define UKnxMsg_GetTPCI(pb)                 (pb->tpci)
#define UKnxMsg_SetTPCI(pb, tp)             (pb->tpci |= (tp))

#define UKnxMsg_GetSeqNo(pb)                (((pb->tpci) & 0x3c) >> 2)
#define UKnxMsg_SetSeqNo(pb, SeqNo)         (pb->tpci |= (((SeqNo) & (uint8_t)0x0f) << 2))

#define UKnxMsg_GetAPCI(pb)                 ((UKNX_U16)(pb->tpci << 8) | (pb->apci))
#define UKnxMsg_SetAPCI(pb, a)           do {pb->tpci |= ((a>>8)&0x3); pb->apci = (a&0xff);} while(0)


#include <stdio.h>
#define UKNX_LOG printf

#ifdef __cplusplus
}
#endif

/** @} */

#endif
