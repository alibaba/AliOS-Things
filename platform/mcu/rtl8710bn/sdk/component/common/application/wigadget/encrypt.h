#ifndef _WI_ENCRYPT_H_
#define _WI_ENCRYPT_H_

#include "rom_aes.h"

typedef union
{   unsigned int l;
    unsigned char b[4];
} aes_inf;

typedef struct
{
    aes_context ctx;
    aes_inf inf;
    
} aes_encrypt_ctx;

typedef struct
{
    aes_context ctx;
    aes_inf inf;
    
} aes_decrypt_ctx;

#endif