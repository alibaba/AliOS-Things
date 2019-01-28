#ifndef _REC_MD5_H_
#define _REC_MD5_H_

typedef struct
{
    unsigned int total[2];
    unsigned int state[4];
    unsigned char buffer[64];
}rec_md5_context;

#ifndef NULL
#define NULL    (0)
#endif

void rec_md5_free( rec_md5_context *ctx );
void rec_md5_init( rec_md5_context *ctx );
void rec_md5_starts( rec_md5_context *ctx );
void rec_md5_update( rec_md5_context *ctx, const unsigned char *input, unsigned int ilen );
void rec_md5_finish( rec_md5_context *ctx, unsigned char output[16] );
#endif

