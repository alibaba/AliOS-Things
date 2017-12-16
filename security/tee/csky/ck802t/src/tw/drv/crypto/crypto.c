/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 **/

#include "tee_types.h"
#include "tee_dbg.h"
#include "device.h"
#include "crypto_drv.h"

typedef int32_t (*ioctl_func_t)(void *arg);

static int32_t _crypto_open(dev_t *dev);
static int32_t _crypto_close(dev_t *dev);
static int32_t _crypto_ioctl(dev_t *dev, int32_t cmd, void *arg);
static int32_t _crypto_suspend(dev_t *dev, int32_t state);
static int32_t _crypto_resume(dev_t *dev);

const static ioctl_func_t _g_crypto_ioctl_ops[] = {
#ifdef RSA_SUPPORT
        rsa_drv_mode_exp,
#endif

#ifdef AES_SUPPORT
        aes_drv_get_ctx_size,
        aes_drv_init,
        aes_drv_process,
        aes_drv_finish,
#endif

#ifdef SHA_SUPPORT
        sha_drv_get_ctx_size,
        sha_drv_init,
        sha_drv_process,
        sha_drv_finish,
#endif
#ifdef TRNG_SUPPORT
        trng_random_get,
#endif
};

const static dev_ops_t _g_crypto_ops = {
    .open = _crypto_open,
    .close = _crypto_close,
    .ioctl = _crypto_ioctl,
    .read = NULL,
    .write = NULL,
    .suspend = _crypto_suspend,
    .resume = _crypto_resume,
};

static struct _crypto_dev_t {
    int8_t *name;
    dev_t *crypto_dev;
    int32_t open_cnt;
    bool busy;
    dev_ops_t *ops;
#ifdef RSA_SUPPORT
    uint32_t rsa_addr_base;
#ifdef INTERRUPT_SUPPORT
    irq_handler_t *rsa_irq;
#endif
#endif

#ifdef AES_SUPPORT
    uint32_t aes_addr_base;
#ifdef INTERRUPT_SUPPORT
    irq_handler_t *aes_irq;
#endif
#endif

#ifdef SHA_SUPPORT
    uint32_t sha_addr_base;
#ifdef INTERRUPT_SUPPORT
    irq_handler_t *hash_irq;
#endif
#endif
} _g_crypto_dev;

static int32_t _crypto_probe(void)
{
    int32_t ret = 0;

    memset(&_g_crypto_dev, 0, sizeof(_g_crypto_dev));
    _g_crypto_dev.name = CRYPTO_DEV_NAME;
    _g_crypto_dev.open_cnt = 0;
    _g_crypto_dev.busy = false;
    _g_crypto_dev.ops = (dev_ops_t *)(&_g_crypto_ops);

#ifdef RSA_SUPPORT
    /* _g_crypto_dev.rsa_addr_base = PLATFORM_RSA_ADDRBASE; */
#ifdef INTERRUPT_SUPPORT
    _g_crypto_dev.rsa_irq = &_rsa_irq_handler;
#endif
#endif

#ifdef AES_SUPPORT
    /* _g_crypto_dev.aes_addr_base = PLATFORM_AES_ADDRBASE; */
#ifdef INTERRUPT_SUPPORT
    _g_crypto_dev.aes_irq = &_aes_irq_handler;
#endif
#endif

#ifdef SHA_SUPPORT
    /* _g_crypto_dev.hash_addr_base = CK_HASH_ADDRBASE; */
#ifdef INTERRUPT_SUPPORT
    _g_crypto_dev.hash_irq = &_hash_irq_handler;
#endif
#endif

    _g_crypto_dev.crypto_dev = dev_register(
            _g_crypto_dev.name, _g_crypto_dev.ops);
    if (NULL == _g_crypto_dev.crypto_dev) {
        ret = -1;
        goto __out;
    }

__out:
    return ret;
}

static int32_t _crypto_open(dev_t *dev)
{
    if (dev != _g_crypto_dev.crypto_dev) {
        return FAIL;
    }

    if (_g_crypto_dev.open_cnt) {
        return EBUSY;
    }

    _g_crypto_dev.open_cnt++;
    return SUCCESS;
}

static int32_t _crypto_close(dev_t *dev)
{
    if (1 != _g_crypto_dev.open_cnt) {
        return FAIL;
    }

    if (dev != _g_crypto_dev.crypto_dev) {
        return FAIL;
    }

    _g_crypto_dev.open_cnt--;
    return SUCCESS;
}

static int32_t _crypto_ioctl(dev_t *dev, int32_t cmd, void *arg)
{
    int32_t ret = 0;
    ioctl_func_t func;

    if (dev != _g_crypto_dev.crypto_dev) {
        return FAIL;
    }

    if (!arg) {
        return EARG;
    }

    if (1 != _g_crypto_dev.open_cnt) {
        return FAIL;
    }

    _g_crypto_dev.busy = true;

    if (cmd > sizeof(_g_crypto_ioctl_ops)/sizeof(_g_crypto_ioctl_ops[0])) {
        return EARG;
    }

    func = _g_crypto_ioctl_ops[cmd];
    if (!func) {
        return FAIL;
    }

    ret = func(arg);

    _g_crypto_dev.busy = false;

    return ret;
}

static int32_t _crypto_suspend(dev_t *dev, int32_t state)
{
    if (_g_crypto_dev.busy) {
        return EBUSY;
    }

#ifdef RSA_SUPPORT
    rsa_drv_suspend(dev, state);
#endif

#ifdef AES_SUPPORT
    aes_drv_suspend(dev, state);
#endif

#ifdef SHA_SUPPORT
    sha_drv_suspend(dev, state);
#endif

#ifdef TRNG_SUPPORT
    trng_drv_suspend(dev, state);
#endif

    return SUCCESS;
}

static int32_t _crypto_resume(dev_t *dev)
{

#ifdef RSA_SUPPORT
    rsa_drv_resume(dev);
#endif

#ifdef AES_SUPPORT
    aes_drv_resume(dev);
#endif

#ifdef SHA_SUPPORT
    sha_drv_resume(dev);
#endif

#ifdef TRNG_SUPPORT
    trng_drv_resume(dev);
#endif
    return SUCCESS;
}

DEV_INIT(_crypto_probe, (uint8_t *)CRYPTO_DEV_NAME);
