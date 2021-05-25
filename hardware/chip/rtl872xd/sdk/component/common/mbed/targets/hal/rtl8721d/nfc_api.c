/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2014, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */
#include "objects.h"
#include "pinmap.h"

#if CONFIG_NFC_NORMAL

#include "nfc_api.h"

/**
 *  @brief   The NFC tag write callback function wrapper
 *
 *  @return  None
 *         
 */
void nfc_tagwrite_callback(PNFC_ADAPTER pNFCAdp, uint32_t page, uint32_t wr_data)
{
    nfctag_t *obj;
    nfc_write_cb handler;

    obj = pNFCAdp->nfc_obj;
    
    handler = (nfc_write_cb)obj->nfc_wr_cb;
    if (NULL != handler) {
        handler(obj->wr_cb_arg, page, wr_data);
    }
}

/**
 *  @brief   The NFC tag read callback function wrapper
 *
 *  @return  None
 *         
 */
void nfc_event_callback(PNFC_ADAPTER pNFCAdp, uint32_t event)
{
    nfctag_t *obj;
    nfc_event_cb handler;

    obj = pNFCAdp->nfc_obj;
    
    handler = (nfc_event_cb)obj->nfc_ev_cb;
    if (NULL != handler) {
        if (obj->event_mask & event) {
            handler(obj->ev_cb_arg, event);
        }
    }
}

/**
 *  @brief   The NFC tag read callback function wrapper
 *
 *  @return  None
 *         
 */
void nfc_tagread_callback(PNFC_ADAPTER pNFCAdp, uint32_t page)
{
    // notify upper layer when read tag page 0 only
    if (0 == page) {
        nfc_event_callback(pNFCAdp, NFC_EV_READ);
    }
}


/**
 *  @brief   The NFC cache read done callback function wrapper
 *
 *  @return  None
 *         
 */
void nfc_cache_read_callback(PNFC_ADAPTER pNFCAdp, uint32_t start_pg, uint32_t *pbuf)
{
    nfctag_t *obj;
    nfc_write_cb handler;

    obj = pNFCAdp->nfc_obj;
    
    handler = (nfc_write_cb)obj->nfc_cache_rd_cb;
    if (NULL != handler) {
        handler(obj->cache_read_cb_arg, start_pg, (uint32_t)pbuf);
    }
}

/**
 *  @brief   To initial NFC tag hardware and resource
 *
 *  @return  The result
 *         
 */
int nfc_init(nfctag_t *obj, uint32_t *pg_init_val)
{
    _memset((void *)obj, 0, sizeof(nfctag_t));
    HalNFCDmemInit(pg_init_val, NFCTAGLENGTH);
    HalNFCInit(&(obj->NFCAdapter));    
    HalNFCFwDownload();
    obj->NFCAdapter.nfc_obj = obj;
    obj->pwr_status = NFC_PWR_RUNNING;
    
    return NFC_OK;
}

/**
 *  @brief   To free NFC tag hardware and resource
 *
 *  @return  The result
 *         
 */
int nfc_free(nfctag_t *obj)
{
    HalNFCDeinit(&(obj->NFCAdapter));
    return NFC_OK;
}

/**
 *  @brief   To register the callback function for NFC read occurred 
 *
 *  @return  None
 *         
 */
void nfc_read(nfctag_t *obj, nfc_read_cb handler, void *arg)
{
    obj->nfc_rd_cb = (void *)handler;
    obj->rd_cb_arg = arg;
}

/**
 *  @brief   To register the callback function for NFC write occurred 
 *
 *  @return  None
 *         
 */
void nfc_write(nfctag_t *obj, nfc_write_cb handler, void *arg)
{    
    obj->nfc_wr_cb = (void *)handler;
    obj->wr_cb_arg = arg;
}

/**
 *  @brief   To register the callback function for NFC events occurred
 *           and the event mask
 *
 *  @return  None
 *         
 */
void nfc_event(nfctag_t *obj, nfc_event_cb handler, void *arg, unsigned int event_mask)
{
    obj->nfc_ev_cb = (void *)handler;
    obj->ev_cb_arg = arg;
    obj->event_mask = event_mask;
}

/**
 *  @brief   To set a new power mode to the NFC device
 *
 *  @return  The result
 *         
 */
int nfc_power(nfctag_t *obj, int pwr_mode, int wake_event)
{
    // TODO:

    return NFC_OK;
}


/**
 *  @brief   to update the NFC read cache. The data in the NFC read cache 
 *           buffer will be transmitted out when NFC read occurred
 *
 *  @return  The result
 *         
 */
int nfc_cache_write(nfctag_t *obj, uint32_t *tbuf, unsigned int spage, unsigned int pg_num)
{
    u8 remain_pg;
    u8 pg_offset=0;
    u8 i;

    if ((spage+pg_num) > NFC_MAX_CACHE_PAGE_NUM) {
        return NFC_ERROR;
    }
    
    remain_pg = pg_num;
    while (remain_pg > 0) {
        if (remain_pg >= 4) {            
            A2NWriteCatch (&obj->NFCAdapter, (spage+pg_offset), 4, (u32*)(&tbuf[pg_offset]));
            remain_pg -= 4;
            pg_offset += 4;
        }
        else {
            for(i=0;i<remain_pg;i++) {
                A2NWriteCatch (&obj->NFCAdapter, (spage+pg_offset), 1, (u32*)(&tbuf[pg_offset]));
                pg_offset++;
            }
            remain_pg = 0;
        }
    }

    return NFC_OK;
}

/**
 *  @brief   To get current NFC status
 *
 *  @return  The result
 *         
 */
int nfc_cache_raed(nfctag_t *obj, nfc_cache_read_cb handler, 
    void *arg, unsigned int start_pg)
{
    if (start_pg > NFC_MAX_CACHE_PAGE_NUM) {
        return NFC_ERROR;
    }
    
    obj->nfc_cache_rd_cb = (void *)handler;
    obj->cache_read_cb_arg = arg;

    A2NReadCatch(&(obj->NFCAdapter), (u8)start_pg);

    return NFC_OK;
}

/**
 *  @brief   to read back the NFC read cache.
 *
 *  @return  The result
 *         
 */
int nfc_status(nfctag_t *obj)
{
    // TODO:

    return (obj->pwr_status);
}

#endif
