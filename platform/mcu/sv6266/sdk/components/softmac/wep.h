
#ifndef _WEP_H_
#define _WEP_H_

#include "soc_types.h"

#define WEP_ENABLE         1

#define CRCPOLY		0xedb88320
#define ChallengeLen         128
#define StaticBufferSize     200
#define IVBYTE                     3
#define KEYINDEX               1
#define ICVBYTE                  4
#define ALGBYTE                 2
#define SEQBYTE                 2
#define STATUSBYTE          2
#define EIDBYTE                   1
#define EIDLENBYTE           1
#define PASSMAXBYTE     16

typedef enum _WEP_TYPE
{
    ENCRYPTION = 0,
    DECRYPTION,
} WEP_TYPE;

typedef struct t_TAG_WEP_INFO
{
    u8 NcrDecText[StaticBufferSize];
    u8 sonkey[StaticBufferSize];
    u8 iv[IVBYTE];
    u8 keyIndex;
    u8 totalKey[IVBYTE + PASSMAXBYTE];
    u8 totalKeyLen;
    u16 NcrDecTextLen;
} TAG_WEP_INFO;

void SetAuthChallengeText(TAG_WEP_INFO *ctx, u8 *src);
void SetAuthChallengeTextLen(TAG_WEP_INFO *ctx, u8 len);
void setBefEncData(TAG_WEP_INFO *ctx);
void initKeyAndIV(TAG_WEP_INFO *ctx);
void savePassToTotalKey(TAG_WEP_INFO *ctx, u8 wsid);
void initWEP(TAG_WEP_INFO *ctx, u8 othLen);
u16 saveAftEncData(TAG_WEP_INFO *ctx, u8 *dst);
#endif

