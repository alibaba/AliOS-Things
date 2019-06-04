#include "rtos_pub.h"
#include "uart_pub.h"
#include "arm_arch.h"
#include "icu_pub.h"

#if (CFG_SOC_NAME == SOC_BK7221U)

#include "str_pub.h"
#include "mem_pub.h"
#include "drv_model_pub.h"

#include "security_pub.h"
#include "security.h"


void *hal_aes_init(const u8 *key, size_t key_size)
{
    SECURITY_AES_DRV_DESC *rk;
    int res;
    rk = (SECURITY_AES_DRV_DESC *)os_malloc(sizeof(SECURITY_AES_DRV_DESC));
    if (rk == NULL)
        return NULL;

    os_memcpy(rk->aes_key, key, key_size);

    if(key_size == 16)
        rk->mode      = AES128;
    else if(key_size == 24)
        rk->mode      = AES192;
    else if(key_size == 32)
        rk->mode      = AES256;
    else
        rk->mode      = AES256;

    return (void *)rk;
}


void hal_aes_encrypt(void *ctx, const u8 *plain, u8 *cipher)
{
    int i;
    int err_num;
    SECURITY_AES_DRV_DESC *aes_drv_desc;
    aes_drv_desc = (SECURITY_AES_DRV_DESC *)ctx;

    aes_drv_desc->aes_block = (unsigned long *)plain;
    aes_drv_desc->encode    = ENCODE;
    if (security_aes_init(aes_drv_desc) != 0)
    {
        os_printf("aes %d encrypt failed\r\n", i);
        return;
    }

    while (is_secrity_aes_busy() == 0)
    {
    }

    get_security_aes_data((unsigned long *)cipher);

}

void hal_aes_decrypt(void *ctx, const u8 *cipher, u8 *plain)
{

    int i;
    int err_num;
    SECURITY_AES_DRV_DESC *aes_drv_desc;
    aes_drv_desc = ctx;

    aes_drv_desc->aes_block = (unsigned long *)cipher;
    aes_drv_desc->encode    = DECODE;
    if (security_aes_init(aes_drv_desc) != 0)
    {
        os_printf("aes %d encrypt failed\r\n", i);
        return;
    }

    while (is_secrity_aes_busy() == 0)
    {
    }

    get_security_aes_data((unsigned long *)plain);
}

void hal_aes_deinit(void *ctx)
{
    os_memset(ctx, 0, sizeof(SECURITY_AES_DRV_DESC));
    os_free(ctx);
    ctx = NULL;
}

#endif


