#include "sys_rtos.h"
#include "rtos_pub.h"
#include "error.h"

#include "include.h"
#include "mem_pub.h"
#include "security_func.h"
#include "uart_pub.h"
#include "str_pub.h"

#if (CFG_SOC_NAME == SOC_BK7221U)
#include "security_pub.h"
#endif


#if (CFG_SOC_NAME == SOC_BK7221U)
void sec_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
#if USE_SEC_TEST_CMD
{
    int sec = 0;

    if(argc > 5 || argc < 2)
    {
        os_printf("Usage: sec.\r\n");
        return;
    }

    if(0 == os_strcmp(argv[1], "aes"))
    {
        unsigned char aes256_key[32] = {0x27, 0x73, 0x1a, 0xe6, 0x23, 0x4e, 0xd5, 0xab, 0xfb, 0x07, 0xd5, 0xc8, 0x69, 0x5f, 0x4a, 0x30,
                                        0x1c, 0x38, 0x2a, 0x09, 0x5b, 0x6f, 0x47, 0xd0, 0x2c, 0x9e, 0x1a, 0xf3, 0x0b, 0x54, 0x6d, 0x82
                                       };
        unsigned char aes256_block[16] = {0xa1, 0x2e, 0x84, 0x5c, 0x10, 0x93, 0xa6, 0x9f, 0x4e, 0xb0, 0x6d, 0xa4, 0x85, 0xa3, 0x8f, 0x5d};
        unsigned char aes256_ex_enc_expected[16] = {0x1b , 0x1e , 0x55 , 0x4e , 0x65, 0xcb , 0x8d , 0x10 , 0x9d , 0x95 , 0x4e , 0xd4 , 0x8d , 0xc2 , 0xad , 0x6f};


        unsigned char aes256_enc_expected[16];
        unsigned char ex_aes256_block[16];
        void *ctx;
        int i, key_len;

        if(argc == 3)
        {
            key_len = os_strtoul(argv[2], NULL, 10);;
        }
        else if(argc == 5)
        {
            key_len = os_strtoul(argv[2], NULL, 10);;

            os_memset(aes256_block, 0, 16);
            os_memset(aes256_key, 0, 32);
            os_memcpy(aes256_block, argv[4], os_strlen(argv[4]));
            os_memcpy(aes256_key, argv[3], os_strlen(argv[3]));
        }
        else
        {
            os_printf("Usage: sec aes_en key block\r\n");
            return;
        }

        os_printf("key len%d: ", key_len);
        for(i = 0; i < 32; i++)
            os_printf("%x ", aes256_key[i]);
        os_printf("\r\n");

        os_printf("block: ");
        for(i = 0; i < 16; i++)
            os_printf("%x ", aes256_block[i]);
        os_printf("\r\n");

        os_printf("hw encrypt\r\n");
        ctx = hal_aes_init(aes256_key, key_len);
        hal_aes_encrypt(ctx, aes256_block, aes256_enc_expected);
        os_printf("en: ");
        for(i = 0; i < 16; i++)
            os_printf("%x ", aes256_enc_expected[i]);
        os_printf("\r\n");

        hal_aes_deinit(ctx);


        os_printf("sw decrypt\r\n");
        ctx = aes_decrypt_init(aes256_key, key_len);
        aes_decrypt(ctx, aes256_enc_expected, ex_aes256_block);
        os_printf("de: ");
        for(i = 0; i < 16; i++)
            os_printf("%x ", ex_aes256_block[i]);
        os_printf("\r\n");

        aes_decrypt_deinit(ctx);

        if(0 != os_memcmp(aes256_block, ex_aes256_block, 16))
        {
            os_printf("encrypt decrypt err\r\n");
        }


        os_memset(aes256_enc_expected, 0, 16);
        os_memset(ex_aes256_block, 0, 16);

        os_printf("sw encrypt\r\n");
        ctx = aes_encrypt_init(aes256_key, key_len);
        aes_encrypt(ctx, aes256_block, aes256_enc_expected);
        os_printf("en: ");
        for(i = 0; i < 16; i++)
            os_printf("%x ", aes256_enc_expected[i]);
        os_printf("\r\n");

        aes_encrypt_deinit(ctx);


        os_printf("hw decrypt\r\n");
        ctx = hal_aes_init(aes256_key, key_len);
        hal_aes_decrypt(ctx, aes256_enc_expected, ex_aes256_block);
        os_printf("de: ");
        for(i = 0; i < 16; i++)
            os_printf("%x ", ex_aes256_block[i]);
        os_printf("\r\n");
        hal_aes_deinit(ctx);

    }
    else if(0 == os_strcmp(argv[1], "sha1"))
    {
        int i;
        unsigned long sha_digst[16];
        void *ctx;

        os_memset(sha_digst, 0, 16 * 4);

        if(argc == 2)
        {
            argv[2] = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";
        }

        ctx = hal_sha1_init( );

        hal_sha1_update( ctx, argv[2], os_strlen(argv[2]));
        hal_sha1_finish( ctx, (UINT8 *)sha_digst );

        os_printf("RESULT:");
        for(i = 0; i < 16 * 4; i++)
            os_printf("%x ", ((UINT8 *)sha_digst)[i]);
        os_printf("\r\n");

    }
    else if(0 == os_strcmp(argv[1], "sha256"))
    {
        int i;
        unsigned long sha_digst[16];
        void *ctx;

        os_memset(sha_digst, 0, 16 * 4);

        if(argc == 2)
        {
            argv[2] = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";
        }

        ctx = hal_sha256_init( );

        hal_sha256_update( ctx, argv[2], os_strlen(argv[2]));
        hal_sha256_finish( ctx, (UINT8 *)sha_digst );

        os_printf("RESULT:");
        for(i = 0; i < 16 * 4; i++)
            os_printf("%x ", ((UINT8 *)sha_digst)[i]);
        os_printf("\r\n");


    }
    else if(0 == os_strcmp(argv[1], "sha256_st"))
    {
        hal_sha256_self_test(1);
    }
}
#else
{
}
#endif

#endif

