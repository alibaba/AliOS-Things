/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file       drv/sha.h
 * @brief      Header File for SHA Driver
 * @version    V1.0
 * @date       9. Oct 2020
 * @model      sha
 ******************************************************************************/

#ifndef _DRV_SHA_H_
#define _DRV_SHA_H_

#include <drv/common.h>
#include <drv/dma.h>

#ifdef __cplusplus
extern "C" {
#endif

/****** SHA mode ******/
typedef enum {
    SHA_MODE_1                    = 1U,   ///< SHA_1 mode
    SHA_MODE_256,                         ///< SHA_256 mode
    SHA_MODE_224,                         ///< SHA_224 mode
    SHA_MODE_512,                         ///< SHA_512 mode
    SHA_MODE_384,                         ///< SHA_384 mode
    SHA_MODE_512_256,                     ///< SHA_512_256 mode
    SHA_MODE_512_224                      ///< SHA_512_224 mode
} csi_sha_mode_t;

/****** SHA State ******/
typedef struct {
    uint32_t busy             : 1;        ///< Calculate busy flag
    uint32_t error            : 1;        ///< Calculate error flag
} csi_sha_state_t;

typedef struct {
    csi_sha_mode_t  mode;                 ///< SHA mode
    uint32_t        total[2];             ///< Number of bytes processed
    uint32_t        state[16];            ///< Intermediate digest state
    uint8_t         buffer[128];          ///< Data block being processed
} csi_sha_context_t;

/****** SHA Event ******/
typedef enum {
    SHA_EVENT_COMPLETE    = 0U,           ///< Calculate completed
    SHA_EVENT_ERROR                       ///< Calculate error
} csi_sha_event_t;

typedef struct csi_sha csi_sha_t;

struct csi_sha {
    csi_dev_t               dev;                                          ///< SHA hw-device info
    void (*callback)(csi_sha_t *sha, csi_sha_event_t event, void *arg);   ///< SHA event callback for user
    void                    *arg;                                         ///< SHA custom designed param passed to evt_cb
    csi_dma_ch_t            *dma_in;                                      ///< SHA in dma handle param
    csi_sha_state_t         state;                                        ///< SHA state
    void                    *priv;
};

/**
  \brief       Initialize SHA Interface. Initializes the resources needed for the SHA interface
  \param[in]   sha    Operate handle
  \param[in]   idx    Index of SHA
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_sha_init(csi_sha_t *sha, uint32_t idx);

/**
  \brief       De-initialize SHA Interface. Stops operation and releases the software resources used by the interface
  \param[in]   sha    SHA handle to operate
  \return      None
*/
void csi_sha_uninit(csi_sha_t *sha);

/**
  \brief       Attach the callback handler to SHA
  \param[in]   sha         Handle to operate
  \param[in]   callback    Callback function
  \param[in]   arg         Callback's param
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_sha_attach_callback(csi_sha_t *sha, void *callback, void *arg);

/**
  \brief       Detach the callback handler
  \param[in]   sha    Handle to operate
  \return      None
*/
void csi_sha_detach_callback(csi_sha_t *sha);

/**
  \brief       Start the engine
  \param[in]   sha        Handle to operate
  \param[in]   context    Pointer to the SHA context \ref csi_sha_context_t
  \param[in]   mode       SHA mode \ref csi_sha_mode_t
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_sha_start(csi_sha_t *sha, csi_sha_context_t *context, csi_sha_mode_t mode);

/**
  \brief       Update the engine
  \param[in]   sha        Handle to operate
  \param[in]   context    Pointer to the SHA context \ref csi_sha_context_t
  \param[in]   input      Pointer to the Source data
  \param[in]   size       The data size
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_sha_update(csi_sha_t *sha, csi_sha_context_t *context, const void *input, uint32_t size);

/**
  \brief       Accumulate the engine (async mode)
  \param[in]   sha        Handle to operate
  \param[in]   context    Pointer to the SHA context \ref csi_sha_context_t
  \param[in]   input      Pointer to the Source data
  \param[in]   size       The data size
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_sha_update_async(csi_sha_t *sha, csi_sha_context_t *context, const void *input, uint32_t size);

/**
  \brief       Finish the engine
  \param[in]   sha         Handle to operate
  \param[in]   context     Pointer to the SHA context \ref csi_sha_context_t
  \param[out]  output      Pointer to the result data
  \param[out]  out_size    Pointer to the result data size(bytes)
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_sha_finish(csi_sha_t *sha, csi_sha_context_t *context, void *output, uint32_t *out_size);

/**
  \brief       Get SHA state
  \param[in]   sha      Handle to operate
  \param[out]  state    SHA state \ref csi_sha_state_t
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_sha_get_state(csi_sha_t *sha, csi_sha_state_t *state);

/**
  \brief       Enable SHA power manage
  \param[in]   sha     Handle to operate
  \return      Error code \ref csi_error_t
*/
csi_error_t csi_sha_enable_pm(csi_sha_t *sha);

/**
  \brief       Disable SHA power manage
  \param[in]   sha    Handle to operate
  \return      None
*/
void csi_sha_disable_pm(csi_sha_t *sha);

#ifdef __cplusplus
}
#endif

#endif /* _DRV_SHA_H_ */
