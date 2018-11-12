#ifndef _OTA_SHA256_H_
#define _OTA_SHA256_H_

typedef struct {
    unsigned int total[2];
    unsigned int state[8];
    unsigned char buffer[64];
    int is224;
}ota_sha256_context;

#ifndef NULL
#define NULL    (0)
#endif

void ota_sha256_free( ota_sha256_context *ctx );
void ota_sha256_init( ota_sha256_context *ctx );
void ota_sha256_starts( ota_sha256_context *ctx, int is224 );
void ota_sha256_update( ota_sha256_context *ctx, const unsigned char *input,
                            unsigned int ilen );
void ota_sha256_finish( ota_sha256_context *ctx, unsigned char output[32] );
#endif

