#ifndef _OTA_OID_H
#define _OTA_OID_H
#include "ota_asn1.h"

#define OTA_OID_ISO_MEMBER_BODIES           "\x2a"        
#define OTA_OID_ISO_ITU_COUNTRY             "\x60"          

#define OTA_OID_COUNTRY_US                  "\x86\x48"      
#define OTA_OID_ORG_RSA_DATA_SECURITY       "\x86\xf7\x0d"  
#define OTA_OID_RSA_COMPANY                 OTA_OID_ISO_MEMBER_BODIES OTA_OID_COUNTRY_US \
                                        OTA_OID_ORG_RSA_DATA_SECURITY 

#define OTA_OID_ORGANIZATION                "\x01"        
#define OTA_OID_ISO_ITU_US_ORG              OTA_OID_ISO_ITU_COUNTRY OTA_OID_COUNTRY_US OTA_OID_ORGANIZATION 

#define OTA_OID_ORG_GOV                     "\x65"          
#define OTA_OID_GOV                         OTA_OID_ISO_ITU_US_ORG OTA_OID_ORG_GOV 

#define OTA_OID_DIGEST_ALG_MD5              OTA_OID_RSA_COMPANY "\x02\x05"
#define OTA_OID_DIGEST_ALG_SHA256           OTA_OID_GOV "\x03\x04\x02\x01"
#define OTA_OID_DIGEST_ALG_SHA512           OTA_OID_GOV "\x03\x04\x02\x03" 


typedef struct {
    const char *asn1;               
    unsigned int asn1_len;                
    const char *name;               
    const char *description;       
} ota_oid_descriptor_t;


int ota_oid_get_md_alg( const ota_asn1_buf *oid, ota_md_type_t * md_alg ); 

#endif 
