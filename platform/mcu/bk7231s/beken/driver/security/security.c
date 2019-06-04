#include "rtos_pub.h"
#include "uart_pub.h"
#include "arm_arch.h"
#include "icu_pub.h"
#include "intc_pub.h"

#if (CFG_SOC_NAME == SOC_BK7221U)

#include "str_pub.h"
#include "drv_model_pub.h"

#include "security_pub.h"
#include "security.h"

static volatile unsigned long sgul_security_flag = 0;

int security_aes_init(SECURITY_AES_DRV_DESC *p_security_aes_drv_desc)
{
    int i;
    UINT32 param;
    UINT8 tmp_data[4];

    if (sgul_security_flag & 0x01)
    {
        return -1;
    }

    REG_WRITE(SECURITY_AES_CTRL, 0);

    param = FIQ_SECURITY_BIT;
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);

    for (i = 0; i < 8; i++)
    {
        PUT_UINT32_BE(p_security_aes_drv_desc->aes_key[i], tmp_data, 0);
        REG_WRITE(SECURITY_AES_KEY_X(i), *((UINT32 *)tmp_data));
    }

    for (i = 0; i < 4; i++)
    {
        PUT_UINT32_BE(p_security_aes_drv_desc->aes_block[i], tmp_data, 0);
        REG_WRITE(SECURITY_AES_BLOCK_X(i), *((UINT32 *)tmp_data));
    }

    REG_WRITE(SECURITY_AES_CONFIG , (((p_security_aes_drv_desc->mode   & 0x03) << 1)
                                     | ((p_security_aes_drv_desc->encode & 0x01) << 0)));
    REG_WRITE(SECURITY_AES_CTRL , (1 << 3) | (1 << 2));

    return 0;
}

void get_security_aes_data(unsigned long *pul_data)
{
    int i;
    UINT8 tmp_data[4];

    for (i = 0; i < 4; i++)
    {
        PUT_UINT32_BE(REG_READ(SECURITY_AES_RESULT_X(i)), tmp_data, 0);
        pul_data[i] = *((UINT32 *)tmp_data);
    }
}



int security_sha_init(SECURITY_SHA_DRV_DESC *p_security_sha_drv_desc)
{
    int i;
    UINT32 param;
    UINT8 tmp_data[4];

    if (sgul_security_flag & 0x02)
    {
        return -1;
    }

    REG_WRITE(SECURITY_SHA_CONFIG , 0);

    REG_WRITE(SECURITY_SHA_CONFIG , (1 << 3) | p_security_sha_drv_desc->mode);

    for (i = 0; i < 16; i++)
    {
        PUT_UINT32_BE(p_security_sha_drv_desc->sha_block[i], tmp_data, 0);
        REG_WRITE(SECURITY_SHA_BLOCK_X(15 - i), *((UINT32 *)tmp_data));
    }

    REG_WRITE(SECURITY_SHA_CTRL , (0x01 << p_security_sha_drv_desc->step));

#if (CFG_SOC_NAME == SOC_BK7221U)
    param = FIQ_SECURITY_BIT;
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);
#endif

    return 0;
}

void get_security_sha_data(hal_sha_context *ctx, unsigned long *pul_data)
{
    int i;
    int result_len;
    UINT8 tmp_data[4];

    if(ctx->mode == SHA1)
    {
        result_len = 160;
    }
    else
    {
        result_len = 256;
    }

    result_len /= (8 * 4);

    for (i = 0; i < result_len; i++)
    {
        PUT_UINT32_BE(REG_READ(SECURITY_SHA_DIGEST_X(result_len - 1 - i)), tmp_data, 0);
        pul_data[i] = *((UINT32 *)tmp_data);
    }
}


void bk_secrity_isr(void)
{
    unsigned long secrity_state;

    secrity_state = REG_READ(SECURITY_AES_STATUS);
    if ((secrity_state & 0x07) == 0x07)
    {
        sgul_security_flag |= 0x01;
        REG_WRITE(SECURITY_AES_STATUS , secrity_state);
    }

    secrity_state = REG_READ(SECURITY_SHA_STATUS);
    if ((secrity_state & 0x05) == 0x05)
    {
        sgul_security_flag |= 0x02;
        REG_WRITE(SECURITY_SHA_STATUS , secrity_state);
    }

    secrity_state = REG_READ(SECURITY_RSA_STATE);
    if ((secrity_state & 0x03) == 0x03)
    {
        sgul_security_flag |= 0x04;
        REG_WRITE(SECURITY_RSA_STATE , secrity_state);
    }
}


int is_secrity_aes_busy(void)
{
    if ((sgul_security_flag & 0x01) == 0)
    {
        return 0;
    }
    sgul_security_flag &= (~0x01);
    return -1;
}

int is_secrity_sha_busy(void)
{
    if ((sgul_security_flag & 0x02) == 0)
    {
        return 0;
    }
    sgul_security_flag &= (~0x02);
    return -1;
}

int is_secrity_rsa_busy(void)
{
    if ((sgul_security_flag & 0x04) == 0)
    {
        return 0;
    }
    sgul_security_flag &= (~0x04);
    return -1;
}


void bk_secrity_init(void)
{
    intc_service_register(FIQ_SECURITY, PRI_FIQ_SECURITY, bk_secrity_isr);
}

void bk_secrity_exit(void)
{
}

#endif

