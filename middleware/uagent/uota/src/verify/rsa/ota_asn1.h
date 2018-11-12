#ifndef _OTA_ASN1_H
#define _OTA_ASN1_H
#include "ota_pub.h"
#include "ota_bignum.h"


#define OTA_ERR_ASN1_OUT_OF_DATA                      -0x0060
#define OTA_ERR_ASN1_UNEXPECTED_TAG                   -0x0062
#define OTA_ERR_ASN1_INVALID_LENGTH                   -0x0064
#define OTA_ERR_ASN1_LENGTH_MISMATCH                  -0x0066
#define OTA_ERR_ASN1_INVALID_DATA                     -0x0068
#define OTA_ERR_ASN1_ALLOC_FAILED                     -0x006A
#define OTA_ERR_ASN1_BUF_TOO_SMALL                    -0x006C

#define OTA_ASN1_BOOLEAN                 0x01
#define OTA_ASN1_INTEGER                 0x02
#define OTA_ASN1_BIT_STRING              0x03
#define OTA_ASN1_OCTET_STRING            0x04
#define OTA_ASN1_NULL                    0x05
#define OTA_ASN1_OID                     0x06
#define OTA_ASN1_UTF8_STRING             0x0C
#define OTA_ASN1_SEQUENCE                0x10
#define OTA_ASN1_SET                     0x11
#define OTA_ASN1_PRINTABLE_STRING        0x13
#define OTA_ASN1_T61_STRING              0x14
#define OTA_ASN1_IA5_STRING              0x16
#define OTA_ASN1_UTC_TIME                0x17
#define OTA_ASN1_GENERALIZED_TIME        0x18
#define OTA_ASN1_UNIVERSAL_STRING        0x1C
#define OTA_ASN1_BMP_STRING              0x1E
#define OTA_ASN1_PRIMITIVE               0x00
#define OTA_ASN1_CONSTRUCTED             0x20
#define OTA_ASN1_CONTEXT_SPECIFIC        0x80


#define OTA_OID_SIZE(x) (sizeof(x) - 1)

#define OTA_OID_CMP(oid_str, oid_buf)                                   \
        ( ( OTA_OID_SIZE(oid_str) != (oid_buf)->len ) ||                \
          memcmp( (oid_str), (oid_buf)->p, (oid_buf)->len) != 0 )

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ota_asn1_buf
{
    int tag;
    size_t len;
    unsigned char *p;
}
ota_asn1_buf;

/**
 * Container for ASN1 bit strings.
 */
typedef struct ota_asn1_bitstring
{
    size_t len;
    unsigned char unused_bits;
    unsigned char *p;
}
ota_asn1_bitstring;


typedef struct ota_asn1_sequence
{
    ota_asn1_buf buf;
    struct ota_asn1_sequence *next;
}
ota_asn1_sequence;

typedef struct ota_asn1_named_data
{
    ota_asn1_buf oid;
    ota_asn1_buf val;
    struct ota_asn1_named_data *next;
    unsigned char next_merged;
}
ota_asn1_named_data;

int ota_asn1_get_len( unsigned char **p,
                  const unsigned char *end,
                  unsigned int*len );

int ota_asn1_get_tag( unsigned char **p,
                  const unsigned char *end,
                  unsigned int*len, int tag );

#ifdef __cplusplus
}
#endif

#endif 
