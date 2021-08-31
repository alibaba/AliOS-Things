/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_TDM_H__
#define __HAL_TDM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "reg_tdm.h"
#include "hal_i2s.h"

#define TDM_BUF_ALIGN __attribute__((aligned(0x100)))

enum HAL_TDM_ENABLE_T {
    HAL_TDM_DISABLE,
    HAL_TDM_ENABLE,
    HAL_TDM_ENABLE_NUM,
};

enum HAL_TDM_MODE_T {
    HAL_TDM_MODE_FS_ASSERTED_AT_FIRST,
    HAL_TDM_MODE_FS_ASSERTED_AT_LAST,
    HAL_TDM_MODE_NUM,
};

enum HAL_TDM_FS_EDGE_T {
    HAL_TDM_FS_EDGE_POSEDGE,
    HAL_TDM_FS_EDGE_NEGEDGE,
    HAL_TDM_FS_EDGE_NUM,
};

enum HAL_TDM_CYCLES_T {
    HAL_TDM_CYCLES_16     = 16,
    HAL_TDM_CYCLES_32     = 32,
    HAL_TDM_CYCLES_64     = 64,
    HAL_TDM_CYCLES_128    = 128,
    HAL_TDM_CYCLES_256    = 256,
    HAL_TDM_CYCLES_512    = 512,
};

enum HAL_TDM_FS_CYCLES {
    HAL_TDM_FS_CYCLES_ONE_LESS = 0,
    HAL_TDM_FS_CYCLES_1     = 1,
    HAL_TDM_FS_CYCLES_8    = 8,
    HAL_TDM_FS_CYCLES_16   = 16,
    HAL_TDM_FS_CYCLES_32   = 32,
    HAL_TDM_FS_CYCLES_64   = 64,
    HAL_TDM_FS_CYCLES_128  = 128,
    HAL_TDM_FS_CYCLES_256  = 256,
};

enum HAL_TDM_SLOT_CYCLES_T {
    HAL_TDM_SLOT_CYCLES_32 = 32,
    HAL_TDM_SLOT_CYCLES_16 = 16,
};

struct HAL_TDM_CONFIG_T {
    enum HAL_TDM_MODE_T mode;
    enum HAL_TDM_FS_EDGE_T edge;
    enum HAL_TDM_CYCLES_T cycles;
    enum HAL_TDM_FS_CYCLES fs_cycles;
    enum HAL_TDM_SLOT_CYCLES_T slot_cycles;
    uint32_t data_offset;
    bool master_clk_wait;
};

int32_t hal_tdm_open(enum HAL_I2S_ID_T i2s_id,enum AUD_STREAM_T stream,enum HAL_I2S_MODE_T mode);
int32_t hal_tdm_setup_stream(enum HAL_I2S_ID_T i2s_id,
                                       enum AUD_STREAM_T stream,
                                       uint32_t sample_rate,
                                       struct HAL_TDM_CONFIG_T *tdm_cfg);
int32_t hal_tdm_as_i2s_setup_stream(enum HAL_I2S_ID_T i2s_id,
                                       enum AUD_STREAM_T stream,
                                       uint32_t sample_rate);
int32_t hal_tdm_start_stream(enum HAL_I2S_ID_T i2s_id, enum AUD_STREAM_T stream);
int32_t hal_tdm_stop_stream(enum HAL_I2S_ID_T i2s_id, enum AUD_STREAM_T stream);
int32_t hal_tdm_close(enum HAL_I2S_ID_T i2s_id, enum AUD_STREAM_T stream);
void hal_tdm_get_config(enum HAL_I2S_ID_T i2s_id,struct HAL_TDM_CONFIG_T *tdm_cfg);
void hal_tdm_set_config(enum HAL_I2S_ID_T i2s_id,struct HAL_TDM_CONFIG_T *tdm_cfg);
#ifdef __cplusplus
}
#endif

#endif // __HAL_TDM_H__