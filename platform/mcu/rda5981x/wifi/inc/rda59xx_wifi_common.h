#ifndef _RDA59XX_WIFI_COMMON_H_
#define _RDA59XX_WIFI_COMMON_H_

#include "rda_def.h"

#define MAC_HDR_LEN                     24          /* No Address4 - non-ESS         */
#define FCS_LEN                         4
#define TIME_STAMP_LEN                  8
#define BEACON_INTERVAL_LEN             2
#define CAP_INFO_LEN                    2
#define LISTEN_INTERVAL_LEN             2
#define STATUS_CODE_LEN                 2
#define AID_LEN                         2
#define IE_HDR_LEN                      2
#define MAC_HDR_ADDR2_OFFSET            10
#define MAX_SSID_LEN                    32
#define ETH_ALEN                        6

#define MACDBG                          "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC2STRDBG(ea)                  (ea)[0], (ea)[1], (ea)[2], (ea)[3], (ea)[4], (ea)[5]


#ifndef MIN
#define MIN(a, b)                   (((a) < (b)) ? (a) : (b))
#endif /* MIN */

#ifndef MAX
#define MAX(a, b)                   (((a) > (b)) ? (a) : (b))
#endif /* MAX */

#define READ_REG32(REG)          *((volatile unsigned int*)(REG))
#define WRITE_REG32(REG, VAL)    ((*(volatile unsigned int*)(REG)) = (unsigned int)(VAL))


typedef struct {
    unsigned int type;
    unsigned int arg1;
    unsigned int arg2;
    unsigned int arg3;
}rda_msg;


/* Frame Type and Subtype Codes (6-bit) */
enum TYPESUBTYPE_T
{
    ASSOC_REQ             = 0x00,
    ASSOC_RSP             = 0x10,
    REASSOC_REQ           = 0x20,
    REASSOC_RSP           = 0x30,
    PROBE_REQ             = 0x40,
    PROBE_RSP             = 0x50,
    BEACON                = 0x80,
    ATIM                  = 0x90,
    DISASOC               = 0xA0,
    AUTH                  = 0xB0,
    DEAUTH                = 0xC0,
    ACTION                = 0xD0,
    PS_POLL               = 0xA4,
    RTS                   = 0xB4,
    CTS                   = 0xC4,
    ACK                   = 0xD4,
    CFEND                 = 0xE4,
    CFEND_ACK             = 0xF4,
    DATA                  = 0x08,
    DATA_ACK              = 0x18,
    DATA_POLL             = 0x28,
    DATA_POLL_ACK         = 0x38,
    NULL_FRAME            = 0x48,
    CFACK                 = 0x58,
    CFPOLL                = 0x68,
    CFPOLL_ACK            = 0x78,
    QOS_DATA              = 0x88,
    QOS_DATA_ACK          = 0x98,
    QOS_DATA_POLL         = 0xA8,
    QOS_DATA_POLL_ACK     = 0xB8,
    QOS_NULL_FRAME        = 0xC8,
    QOS_CFPOLL            = 0xE8,
    QOS_CFPOLL_ACK        = 0xF8,
    BLOCKACK_REQ          = 0x84,
    BLOCKACK              = 0x94,
};

/* Element ID  of various Information Elements */
enum ELEMENTID_T{
    ISSID         = 0,   /* Service Set Identifier   */
    ISUPRATES     = 1,   /* Supported Rates          */
    IFHPARMS      = 2,   /* FH parameter set         */
    IDSPARMS      = 3,   /* DS parameter set         */
    ICFPARMS      = 4,   /* CF parameter set         */
    ITIM          = 5,   /* Traffic Information Map  */
    IIBPARMS      = 6,   /* IBSS parameter set       */
    ICTEXT        = 16,  /* Challenge Text           */
    IERPINFO      = 42,  /* ERP Information          */
    IEXSUPRATES   = 50,  /* Extended Supported Rates */
    IWAPI         = 68
};

enum COUNTRY_CODE_T
{
    JP = 0,
    NA = 1,
    EU = 2
};

/* Scan type parameter for scan request */
enum SCANTYPE_T {
    PASSIVE_SCAN = 0,
    ACTIVE_SCAN  = 1,
    NUM_SCANTYPE
};

/**
 * is_same_ether_addr - Determine if give Ethernet address is same.
 * @addr1/addr2: Pointer to a six-byte array containing the Ethernet address
 *
 * Return true if the address is all zeroes.
 */
inline r_s32 is_same_ether_addr(const r_u8 *addr1, const r_u8 *addr2)
{
    return (addr1[0] == addr2[0] && addr1[1] == addr2[1] && addr1[2] == addr2[2] \
        && addr1[3] == addr2[3] && addr1[4] == addr2[4] && addr1[5] == addr2[5]);
}

/**
 * is_zero_ether_addr - Determine if give Ethernet address is all zeros.
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Return true if the address is all zeroes.
 */
inline r_s32 is_zero_ether_addr(const r_u8 *addr)
{
    return !(addr[0] | addr[1] | addr[2] | addr[3] | addr[4] | addr[5]);
}

/**
 * is_multicast_ether_addr - Determine if the Ethernet address is a multicast.
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Return true if the address is a multicast address.
 * By definition the broadcast address is also a multicast address.
 */
inline r_s32 is_multicast_ether_addr(const r_u8 *addr)
{
    return (0x01 & addr[0]);
}

/**
 * is_broadcast_ether_addr - Determine if the Ethernet address is broadcast
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Return true if the address is the broadcast address.
 */
inline r_s32 is_broadcast_ether_addr(const r_u8 *addr)
{
    return (addr[0] & addr[1] & addr[2] & addr[3] & addr[4] & addr[5]) == 0xff;
}

#endif /* _RDA59XX_WIFI_COMMON_H_ */


