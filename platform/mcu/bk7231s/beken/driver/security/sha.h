#ifndef _BK_SHA_H_
#define _BK_SHA_H_

#if (CFG_SOC_NAME == SOC_BK7221U)

#include "security_reg.h"


enum SHA_STEP
{
    STEP1       = 0,
    STEP2       = 1,
};


typedef enum
{
    SHA1        = 0,
    SHA224      = 2,
    SHA256      = 3,
    SHA512_224  = 4,
    SHA512_256  = 5,
    SHA384      = 6,
    SHA512      = 7
} SHA_MODE;

typedef struct
{
    UINT32 total[2];          /*!< number of bytes processed  */
    UINT32 state[8];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
    int step;                  /*if is inited*/
    SHA_MODE mode;

} hal_sha_context;


#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif

#ifndef PUT_UINT32_LE
#define PUT_UINT32_LE(n,b,i)                                    \
{                                                               \
    (b)[(i)    ] = (unsigned char) ( ( (n)       ) & 0xFF );    \
    (b)[(i) + 1] = (unsigned char) ( ( (n) >>  8 ) & 0xFF );    \
    (b)[(i) + 2] = (unsigned char) ( ( (n) >> 16 ) & 0xFF );    \
    (b)[(i) + 3] = (unsigned char) ( ( (n) >> 24 ) & 0xFF );    \
}
#endif


typedef struct
{
    unsigned long          *sha_block;

    /* mode: SHA mode
        SHA1        = 0,
        SHA224      = 2,
        SHA256      = 3,
        SHA512_224  = 4,
        SHA512_256  = 5,
        SHA384      = 6,
        SHA512      = 7
     */
    unsigned char           mode;

    /* mode:     SECURITY SHA step
        step1       = 0,
        step2       = 1
     */
    unsigned char           step;
} SECURITY_SHA_DRV_DESC;



#endif
#endif

