#ifndef _BK_AES_H_
#define _BK_AES_H_

#if (CFG_SOC_NAME == SOC_BK7221U)

#include "security_reg.h"


enum AES_MODE
{
    AES128      = 0,
    AES256      = 1,
    AES192      = 2
};

enum AES_ENCODE
{
    DECODE      = 0,
    ENCODE      = 1
};



typedef struct
{
    unsigned long           aes_key[8];
    unsigned long          *aes_block;

    /* mode:   AES mode
        AES128      = 0,
        AES256      = 1,
        AES192      = 2
     */
    unsigned char           mode;

    /* encode: encode or decode
        decode      = 0,
        encode      = 1
     */
    unsigned char           encode;
} SECURITY_AES_DRV_DESC;

#endif
#endif

