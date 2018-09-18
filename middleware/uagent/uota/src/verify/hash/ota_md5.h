#ifndef _OTA_MD5_H_
#define _OTA_MD5_H_

typedef struct
{
    unsigned int total[2]; 
    unsigned int state[4];
    unsigned char buffer[64];
}ota_md5_context;

#ifndef NULL
#define NULL    (0)
#endif

void ota_md5_free( ota_md5_context *ctx );
void ota_md5_init( ota_md5_context *ctx );
void ota_md5_starts( ota_md5_context *ctx );
void ota_md5_update( ota_md5_context *ctx, const unsigned char *input, unsigned int ilen );
void ota_md5_finish( ota_md5_context *ctx, unsigned char output[16] );
#endif

