/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 **/

#include "tee_types.h"
#include "tee_dbg.h"
#include "aes_drv.h"
#include "device.h"
#include "crypto_drv.h"

#undef INTERRUPT_SUPPORT

/* pAES->CTRL: endian bit read always return 0 */
static volatile bool _g_aes_big_endian = false;

#ifdef INTERRUPT_SUPPORT
void _aes_intr(int32_t irq);

/* interrupt callback function point */
static void (*_aes_intr_callback)(void) = NULL;
/* irq handler infomations */
static irq_handler_t _aes_irq_handler = { "AES_INTR", INTC_AES, INTC_AES % 4,
                                          _aes_intr, false };

static volatile bool _g_is_key_expand     = false;
static volatile bool _g_is_data_completed = false;

void _aes_intr(int32_t irq)
{
    if (_aes_get_intr_status(AES_IT_KEYINT)) {
        _g_is_key_expand = true;
    }
    if (_aes_get_intr_status(AES_IT_DATAINT)) {
        _g_is_data_completed = true;
    }
    if (NULL != _aes_intr_callback) {
        _aes_intr_callback();
    }
    _aes_clear_intr_status(AES_IT_ALL);
}
#endif

/**********************************************************************
 * Name: _aes_set_data
 * Function: write 128 bits Data input
 * Params: @pdata ---- a pointer point to the address of 4 words data
 * Return: SUCCESS
 *
 *********************************************************************/
static int32_t _aes_set_data(uint32_t *pdata)
{
    volatile paes_reg_t ptr = (paes_reg_t)PLATFORM_AES_ADDRBASE;
    int32_t    i   = 0;

    if (_g_aes_big_endian) {
        for (i = 0; i < 4; i++) {
            // Endian is big endian
            ptr->Din[3 - i] = *pdata++;
        }
    } else {
        for (i = 0; i < 4; i++) {
            // Endian is little endian
            ptr->Din[i] = *pdata++;
        }
    }
    return SUCCESS;
}

/**************************************************
 * Name: _aes_set_key
 * Function: Write 255 bits Key
 * Params: @pkey ---- a pointer point to key array
 * Return: SUCCESS
 *
 *****************************************************/
static int32_t _aes_set_key(uint32_t *pkey, aes_keylength_t keylength)
{
    volatile paes_reg_t ptr    = (paes_reg_t)PLATFORM_AES_ADDRBASE;
    uint32_t   length = keylength ? ((keylength & 0x1) ? 6 : 8) : 4;
    int32_t    i      = 0;

    if (_g_aes_big_endian) {
        // if Endian is Big endian
        for (i = 0; i < length; i++) {
            ptr->KEY[length - 1 - i] = *pkey++;
        }
    } else {
        for (i = 0; i < length; i++) {
            ptr->KEY[i] = *pkey++;
        }
    }

    return SUCCESS;
}

static int32_t _aes_set_iv(uint32_t *piv)
{
    volatile paes_reg_t ptr = (paes_reg_t)PLATFORM_AES_ADDRBASE;
    int32_t    i   = 0;

    if (_g_aes_big_endian) {
        // if Endian is Big endian
        for (i = 0; i < 4; i++) {
            ptr->IV[3 - i] = *piv++;
        }
    } else {
        for (i = 0; i < 4; i++) {
            ptr->IV[i] = *piv++;
        }
    }

    return SUCCESS;
}

/**************************************************************************
 * Name: _aes_set_endian
 * Function: Set AES Endian
 * Params: @endian ---- aes_endian_t : AES_Endian_Little or AES_Endian_Big
 * Return: SUCCESS
 **************************************************************************/
static int32_t _aes_set_endian(aes_endian_t endian)
{
    _g_aes_big_endian = ((endian == AES_ENDIAN_BIG) ? (true) : (false));

    volatile paes_reg_t ptr = (paes_reg_t)PLATFORM_AES_ADDRBASE;

    /*****  Endian Control bit is bit 8 in CTRL register ******/
    ptr->CTRL = ((_g_aes_big_endian == false) ? (ptr->CTRL | 0x100)
                                              : (ptr->CTRL & (~0x00000100)));

    return SUCCESS;
}

/***********************************************************
 * Name: _aes_set_opcode
 * Function: Set opcode , encrypt or decrypt or key expand
 * Params: @opcode ---- aes_opcode_t
 *                      AES_OPCODE_ENCRYPT
 *                      AES_OPCODE_DECRYPT
 *                      AES_OPCODE_KEYEXPAND
 * Return: SUCCESS
 *
 **********************************************************/
static int32_t _aes_set_opcode(aes_opcode_t opcode)
{
    volatile paes_reg_t ptr = (paes_reg_t)PLATFORM_AES_ADDRBASE;

    /****** Opcode Control bits is bit[7:6] *******/
    ptr->CTRL &= ~0x00C0;       // clear bit[7:6]
    ptr->CTRL |= (opcode << 6); // set opcode

    return SUCCESS;
}

/***************************************************************
 * Name: _aes_set_keylen
 * Function: Set Key Length , 128 bits or 192 bits or 256 bits
 * Params: @keylength ---- aes_keylen_t
 *                         AES_KeyLen_128
 *                         AES_KeyLen_192
 *                         AES_KeyLen_256
 * Return: SUCCESS
 *
 **************************************************************/
static int32_t _aes_set_keylen(aes_keylength_t keylength)
{
    volatile paes_reg_t ptr = (paes_reg_t)PLATFORM_AES_ADDRBASE;

    /******* Key Length control bit is bit[5:4] *****/
    ptr->CTRL &= ~0x0030;          // clear bit[5:4]
    ptr->CTRL |= (keylength << 4); // Set key length

    return SUCCESS;
}

/*****************************************************
 * Name: _aes_set_mode
 * Function: Set AES Mode, CBC or EBC
 * Params: @mode ---- aes_mode_t
 *                    AES_MODE_CBC
 *                    AES_Mode_EBC
 * Return: SUCCESS
 *
 *****************************************************/
static int32_t _aes_set_mode(aes_mode_t mode)
{
    volatile paes_reg_t ptr = (paes_reg_t)PLATFORM_AES_ADDRBASE;

    /******* Mode control bit is bit 3 ********/
    ptr->CTRL &= ~0x0008;     // clear bit 3
    ptr->CTRL |= (mode << 3); // set mode

    return SUCCESS;
}

#if INTERRUPT_SUPPORT
/*****************************************************
 * Name: _aes_enable_data_intr
 * Function: Enable data interrupter
 * Params: None
 * Return: None
 *
 *****************************************************/
static void _aes_enable_data_intr(void)
{
    volatile paes_reg_t ptr = (paes_reg_t)PLATFORM_AES_ADDRBASE;

    /****** Interrupt Control bit is bit 2 ********/
    ptr->CTRL |= (1 << 2);
}
#endif
/*****************************************************
 * Name: _aes_disable_data_intr
 * Function: Disable data interrupter
 * Params: None
 * Return: None
 *
 *****************************************************/
static void _aes_disable_data_intr(void)
{
    volatile paes_reg_t ptr = (paes_reg_t)PLATFORM_AES_ADDRBASE;

    /****** Interrupt Control bit is bit 2 ********/
    ptr->CTRL &= ~(1 << 2);
}

#if INTERRUPT_SUPPORT
/*****************************************************
 * Name: _aes_enable_key_intr
 * Function: Enable key interrupter
 * Params: None
 * Return: None
 *
 *****************************************************/
static void _aes_enable_key_intr(void)
{
    volatile paes_reg_t ptr = (paes_reg_t)PLATFORM_AES_ADDRBASE;

    /****** Interrupt Control bit is bit 2 ********/
    ptr->CTRL |= (1 << 1);
}
#endif

/*****************************************************
 * Name: _aes_disable_key_intr
 * Function: Disable key interrupter
 * Params: None
 * Return: None
 *
 *****************************************************/
static void _aes_disable_key_intr(void)
{
    volatile paes_reg_t ptr = (paes_reg_t)PLATFORM_AES_ADDRBASE;

    /****** Interrupt Control bit is bit 2 ********/
    ptr->CTRL &= ~(1 << 1);
}

/*****************************************************
 * Name: _aes_enable
 * Function: Enable AES
 * Params: None
 * Return: None
 *
 *****************************************************/
static void _aes_enable(void)
{
    volatile paes_reg_t ptr = (paes_reg_t)PLATFORM_AES_ADDRBASE;

    /****** AES Control bit is bit 2 ********/
    ptr->CTRL |= (1 << 0);
}

/****************************************************
 * Name: _aes_disable
 * Function: Disable AES
 * Params: None
 * Return: None
 *
 *****************************************************/
static void _aes_disable(void)
{
    volatile paes_reg_t ptr = (paes_reg_t)PLATFORM_AES_ADDRBASE;

    /****** AES Control bit is bit 2 ********/
    ptr->CTRL &= ~(1 << 0);
}

/*****************************************************
 * Name: _aes_get_intr_status
 * Function: Get AES interrupter status
 * Params: @AES_IT ---- AES states bit
 *                      AES_IT_DATAINT
 *                      AES_IT_KEYINT
 *                      AES_IT_BUSY
 * Return: if interrupter happened, return true
 *          else return false
 *
 *****************************************************/
static bool _aes_get_intr_status(uint32_t AES_IT)
{
    volatile paes_reg_t ptr = (paes_reg_t)PLATFORM_AES_ADDRBASE;

    /**** if AES_IT hava been set, it return true ******/
    return (ptr->STATE & AES_IT) ? true : false;
}

#ifdef INTERRUPT_SUPPORT
/*****************************************************
 * Name: _aes_clear_intr_status
 * Function: Clear AES interrupter
 * Params: @AES_IT ----  AES states bit
 *                       AES_IT_DATAINT
 *                       AES_IT_KEYINT
 *                       AES_IT_BUSY
 *                       AES_IT_ALL
 * Return: None
 *****************************************************/
static void _aes_clear_intr_status(uint32_t AES_IT)
{
    volatile paes_reg_t ptr = (paes_reg_t)PLATFORM_AES_ADDRBASE;

    /***** write 1 to AES_IT's bit , it will clear Interrupter ***/
    ptr->STATE &= ~AES_IT;
}
#endif
/******************************************************************************
 * Name:_aes_get_data
 * Function: Clear AES get data
 * Params: @data ---- a pointer point to data address which used to save data
 * Return: None
 ******************************************************************************/
static uint32_t *_aes_get_data(uint32_t *data)
{
    volatile paes_reg_t ptr = (paes_reg_t)PLATFORM_AES_ADDRBASE;
    int32_t    i   = 0;

#ifdef INTERRUPT_SUPPORT
    // wait for aes busy
    while (!_g_is_data_completed)
        ;
    _g_is_data_completed = false;
#else
    while (_aes_get_intr_status(AES_IT_BUSY))
        ;
#endif

    if (_g_aes_big_endian) {
        // if Endian is Big endian
        for (i = 0; i < 4; i++) {
            *data++ = ptr->Dout[3 - i];
        }
    } else {
        for (i = 0; i < 4; i++) {
            *data++ = ptr->Dout[i];
        }
    }

    return data;
}

int32_t _aes_clk_on(void)
{
    return 0;
}

int32_t _aes_clk_off(void)
{
    return 0;
}

int32_t aes_drv_suspend(dev_t *dev, int32_t state)
{
    return SUCCESS;
}

int32_t aes_drv_resume(dev_t *dev)
{
    return SUCCESS;
}

uint32_t _aes_save_context(uint32_t *buf)
{
    volatile paes_reg_t ptr = (paes_reg_t)PLATFORM_AES_ADDRBASE;

    uint32_t i, sz;
    sz = (sizeof(aes_reg_t) >> 2);
    for (i = 0; i < sz; i++) {
        *(buf + i) = *((uint32_t *)ptr + i);
    }
    return SUCCESS;
}

uint32_t _aes_restore_context(uint32_t *buf)
{
    volatile paes_reg_t ptr = (paes_reg_t)PLATFORM_AES_ADDRBASE;
    uint32_t   i, sz;
    sz = (sizeof(aes_reg_t) >> 2);
    for (i = 0; i < sz; i++) {
        *((uint32_t *)ptr + i) = *(buf + i);
    }
    return SUCCESS;
}

int32_t aes_drv_get_ctx_size(void *arg)
{
    aes_drv_ioctl_buf_size_t *p = (aes_drv_ioctl_buf_size_t *)arg;

    if (!VALID_MAGIC(p->magic)) {
        return EARG;
    }

    p->ctx_size = sizeof(aes_reg_t);
    return SUCCESS;
}

int32_t aes_drv_init(void *arg)
{
    aes_drv_ioctl_init_t *p = (aes_drv_ioctl_init_t *)arg;
    aes_mode_t            mode;
    bool                  is_enc;
    uint32_t *            key;
    aes_keylength_t       keylen;
    uint32_t *            iv;
    void *                ctx;

    if (!VALID_MAGIC(p->magic)) {
        return EARG;
    }

    mode   = p->mode;
    is_enc = p->is_enc;
    key    = p->key;
    keylen = p->keylen;
    iv     = p->iv;
    ctx    = p->ctx;

    _aes_clk_on();

    /* disable aes for safe */
    _aes_disable();

    /* reset aes engine */
    _aes_disable_key_intr();
    _aes_disable_data_intr();
    _aes_disable();

    _aes_set_endian(AES_ENDIAN_LITTLE);
    _aes_set_key(key, keylen);
    _aes_set_keylen(keylen);
    _aes_set_mode(mode);

    if (AES_MODE_CBC == mode) {
        _aes_set_iv(iv);
    }

    if (!is_enc) {
        _aes_set_opcode(AES_OPCODE_KEYEXPAND);
        _aes_enable();
        while (_aes_get_intr_status(AES_IT_KEYINT));

        _aes_set_opcode(AES_OPCODE_DECRYPT);
    } else {
        _aes_set_opcode(AES_OPCODE_ENCRYPT);
    }

    /* disable aes after initialization*/
    _aes_disable();

    /* save aes context */
    _aes_save_context((uint32_t *)(p->ctx));

    _aes_clk_off();

    return SUCCESS;
}

int32_t aes_drv_process(void *arg)
{
    aes_drv_ioctl_process_t *p = (aes_drv_ioctl_process_t *)arg;

    if (!VALID_MAGIC(p->magic)) {
        return EARG;
    }

    _aes_clk_on();

    /* disable aes for safe */
    _aes_disable();

    /* restore aes context */
    _aes_restore_context((uint32_t *)(p->ctx));

    /* set data */
    _aes_set_data(p->src_blk);

    /* start aes engine and get data */
    _aes_enable();
    _aes_get_data(p->dst_blk);
    _aes_disable();

    /* save aes context */
    _aes_save_context((uint32_t *)(p->ctx));

    _aes_clk_off();

    return SUCCESS;
}

int32_t aes_drv_finish(void *arg)
{
    aes_drv_ioctl_final_t *p = (aes_drv_ioctl_final_t *)arg;

    if (!VALID_MAGIC(p->magic)) {
        return EARG;
    }

    _aes_clk_on();
    _aes_disable();
    _aes_clk_off();

    memset(p->ctx, 0, sizeof(aes_reg_t));

    return SUCCESS;
}
