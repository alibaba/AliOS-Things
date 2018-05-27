/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     ck_etb.c
 * @brief    CSI Source File for ETB Driver
 * @version  V1.0
 * @date     28. Octorber 2017
 ******************************************************************************/
#include <soc.h>
#include <io.h>
#include <drv_etb.h>
#include <drv_errno.h>
#include <ck_etb.h>
#include <string.h>

#define ERR_ETB(errno) (CSI_DRV_ERRNO_ETB_BASE | errno)

#define ETB_NULL_PARAM_CHK(para)                  \
    do {                                    \
        if (para == NULL) {                 \
            return ERR_ETB(DRV_ERROR_PARAMETER);   \
        }                                   \
    } while (0)

typedef struct {
    uint32_t base;
    uint16_t source;
    uint16_t dest;
    etb_status_t status;
} ck_etb_priv_t;

extern int32_t target_get_etb_count(void);
extern int32_t target_get_etb(int32_t idx, uint32_t *base, uint32_t *irq);

static ck_etb_priv_t etb_instance[CONFIG_ETB_NUM];

/* Driver Capabilities */
static const etb_capabilities_t etb_capabilities = {
    .sync_trigger = 1,
    .async_trigger = 1,
    .etb_31_channel = 1,
    .one_trigger_one = 1,
    .one_trigger_more = 1,
    .more_trigger_one = 1
};

static uint32_t g_etb_ch3_31_mark[CK_ETB_CH31 - CK_ETB_CH3 + 1] = {0};

static int32_t etb_ch0_config(uint32_t source_loc, uint32_t dest_loc, etb_source_type_e source_type)
{
    if (source_loc > CK_ETB_CH0_SOURCE_TRIGGER_MAX || dest_loc > CK_ETB_CH0_DEST_TRIGGER_MAX) {
        return -ETB_NO_SUCH_TRIGGER;
    }
    uint8_t src_trigger = source_loc;
    uint8_t dest_trigger = dest_loc;

    if (!HAL_IS_BIT_SET(getreg32((volatile uint32_t *)CK_ETB_CFG0_CH0), CK_ETB_SRC0_EN_CH0_BIT)) {
        set_bit(CK_ETB_SRC0_EN_CH0_BIT, (volatile uint32_t *)CK_ETB_CFG0_CH0);
        write_bits(CK_ETB_CH0_SOURCE_TRIGGER_BITS_WIDTH, CK_ETB_SRC0_EN_CH0_BIT + 1, (volatile uint32_t *)CK_ETB_CFG0_CH0, src_trigger);
    } else if (!HAL_IS_BIT_SET(getreg32((volatile uint32_t *)CK_ETB_CFG0_CH0), CK_ETB_SRC1_EN_CH0_BIT)) {
        set_bit(CK_ETB_SRC0_EN_CH0_BIT, (volatile uint32_t *)CK_ETB_CFG0_CH0);
        write_bits(CK_ETB_CH0_SOURCE_TRIGGER_BITS_WIDTH, CK_ETB_SRC1_EN_CH0_BIT + 1, (volatile uint32_t *)CK_ETB_CFG0_CH0, src_trigger);
    } else if (!HAL_IS_BIT_SET(getreg32((volatile uint32_t *)CK_ETB_CFG0_CH0), CK_ETB_SRC2_EN_CH0_BIT)) {
        set_bit(CK_ETB_SRC0_EN_CH0_BIT, (volatile uint32_t *)CK_ETB_CFG0_CH0);
        write_bits(CK_ETB_CH0_SOURCE_TRIGGER_BITS_WIDTH, CK_ETB_SRC2_EN_CH0_BIT + 1, (volatile uint32_t *)CK_ETB_CFG0_CH0, src_trigger);
    } else {
        return -ETB_NO_SUCH_CHANNEL_AVAIL;
    }

    write_bits(CK_ETB_CH0_DEST_TRIGGER_BITS_WIDTH, CK_ETB_CH0_DEST_TRIGGER_BIT_BASE, (volatile uint32_t *)CK_ETB_CFG1_CH0, dest_trigger);
    write_bits(1, CK_ETB_TRIGGER_MODE_BIT, (volatile uint32_t *)CK_ETB_CFG1_CH0, source_type);
    return CK_ETB_CH0;
}


static int32_t etb_ch1_2_config(uint32_t source_loc, uint32_t dest_loc, etb_source_type_e source_type)
{
    if (source_loc > CK_ETB_CH1_SOURCE_TRIGGER_MAX || dest_loc > CK_ETB_CH1_DEST_TRIGGER_MAX) {
        return -ETB_NO_SUCH_TRIGGER;
    }

    uint8_t src_trigger = source_loc;
    uint8_t dest_trigger = dest_loc;
    int32_t ch_ret;

    if (!HAL_IS_BIT_SET(getreg32((volatile uint32_t *)CK_ETB_CFG0_CH1), CK_ETB_DEST0_EN_CH1_BIT)) {
        set_bit(CK_ETB_DEST0_EN_CH1_BIT, (volatile uint32_t *)CK_ETB_CFG0_CH1);
        write_bits(CK_ETB_CH1_DEST_TRIGGER_BITS_WIDTH, CK_ETB_DEST0_EN_CH1_BIT + 1, (volatile uint32_t *)CK_ETB_CFG0_CH1, dest_trigger);
        ch_ret = CK_ETB_CH1;
    } else if (!HAL_IS_BIT_SET(getreg32((volatile uint32_t *)CK_ETB_CFG0_CH1), CK_ETB_DEST1_EN_CH1_BIT)) {
        set_bit(CK_ETB_DEST0_EN_CH1_BIT, (volatile uint32_t *)CK_ETB_CFG0_CH1);
        write_bits(CK_ETB_CH1_DEST_TRIGGER_BITS_WIDTH, CK_ETB_DEST1_EN_CH1_BIT + 1, (volatile uint32_t *)CK_ETB_CFG0_CH1, dest_trigger);
        ch_ret = CK_ETB_CH1;
    } else if (!HAL_IS_BIT_SET(getreg32((volatile uint32_t *)CK_ETB_CFG0_CH1), CK_ETB_DEST2_EN_CH1_BIT)) {
        set_bit(CK_ETB_DEST0_EN_CH1_BIT, (volatile uint32_t *)CK_ETB_CFG0_CH1);
        write_bits(CK_ETB_CH1_DEST_TRIGGER_BITS_WIDTH, CK_ETB_DEST2_EN_CH1_BIT + 1, (volatile uint32_t *)CK_ETB_CFG0_CH1, dest_trigger);
        ch_ret = CK_ETB_CH1;
    } else {
        goto next_channel2_label;
    }


next_channel2_label:
    if (!HAL_IS_BIT_SET(getreg32((volatile uint32_t *)CK_ETB_CFG0_CH2), CK_ETB_DEST0_EN_CH2_BIT)) {
        set_bit(CK_ETB_DEST0_EN_CH1_BIT, (volatile uint32_t *)CK_ETB_CFG0_CH1);
        write_bits(CK_ETB_CH2_DEST_TRIGGER_BITS_WIDTH, CK_ETB_DEST0_EN_CH2_BIT + 1, (volatile uint32_t *)CK_ETB_CFG0_CH2, dest_trigger);
        ch_ret = CK_ETB_CH2;
    } else if (!HAL_IS_BIT_SET(getreg32((volatile uint32_t *)CK_ETB_CFG0_CH2), CK_ETB_DEST1_EN_CH2_BIT)) {
        set_bit(CK_ETB_DEST0_EN_CH2_BIT, (volatile uint32_t *)CK_ETB_CFG0_CH2);
        write_bits(CK_ETB_CH2_DEST_TRIGGER_BITS_WIDTH, CK_ETB_DEST1_EN_CH2_BIT + 1, (volatile uint32_t *)CK_ETB_CFG0_CH2, dest_trigger);
        ch_ret = CK_ETB_CH2;
    } else if (!HAL_IS_BIT_SET(getreg32((volatile uint32_t *)CK_ETB_CFG0_CH2), CK_ETB_DEST2_EN_CH2_BIT)) {
        set_bit(CK_ETB_DEST0_EN_CH2_BIT, (volatile uint32_t *)CK_ETB_CFG0_CH2);
        write_bits(CK_ETB_CH2_DEST_TRIGGER_BITS_WIDTH, CK_ETB_DEST2_EN_CH2_BIT + 1, (volatile uint32_t *)CK_ETB_CFG0_CH2, dest_trigger);
        ch_ret = CK_ETB_CH2;
    } else {
        return -ETB_NO_SUCH_CHANNEL_AVAIL;
    }

    if (ch_ret == CK_ETB_CH1) {
        write_bits(CK_ETB_CH1_SOURCE_TRIGGER_BITS_WIDTH, CK_ETB_CH1_SOURCE_TRIGGER_BIT_BASE, (volatile uint32_t *)CK_ETB_CFG1_CH1, src_trigger);
        write_bits(1, CK_ETB_TRIGGER_MODE_BIT, (volatile uint32_t *)CK_ETB_CFG1_CH1, source_type);
        return CK_ETB_CH1;
    } else if (ch_ret == CK_ETB_CH2) {
        write_bits(CK_ETB_CH2_SOURCE_TRIGGER_BITS_WIDTH, CK_ETB_CH2_SOURCE_TRIGGER_BIT_BASE, (volatile uint32_t *)CK_ETB_CFG1_CH2, src_trigger);
        write_bits(1, CK_ETB_TRIGGER_MODE_BIT, (volatile uint32_t *)CK_ETB_CFG1_CH1, source_type);
        return CK_ETB_CH2;
    } else {
        return -ETB_NO_SUCH_CHANNEL_AVAIL;
    }
}

static int32_t search_fresh(void)
{
    int i = 0;

    for (; i < (CK_ETB_CH31 - CK_ETB_CH3 + 1); i++) {
        if (!g_etb_ch3_31_mark[i]) {
            return (i + 3);
        }
    }

    return 0;
}

static int32_t etb_ch3_to_31_config(uint32_t source_loc, uint32_t dest_loc, etb_source_type_e source_type)
{
    if (source_loc > CK_ETB_CH3_31_SOURCE_TRIGGER_MAX || dest_loc > CK_ETB_CH3_31_DEST_TRIGGER_MAX) {
        return -ETB_NO_SUCH_TRIGGER;
    }

    uint8_t source = source_loc;
    uint8_t dest = dest_loc;
    uint32_t ch_ret;

    ch_ret = search_fresh();

    if (ch_ret >= CK_ETB_CH3 && ch_ret <= CK_ETB_CH31) {
        write_bits(CK_ETB_CH3_31_SOURCE_TRIGGER_BITS_WIDTH, CK_ETB_CH3_31_SOURCE_BASE_BIT, (volatile uint32_t *)(CK_ETB_CFG_CH3 + 0x4 * (ch_ret -3)), source);
        write_bits(CK_ETB_CH3_31_DEST_TRIGGER_BITS_WIDTH, CK_ETB_CH3_31_DEST_BASE_BIT, (volatile uint32_t *)(CK_ETB_CFG_CH3 + 0x4 * (ch_ret -3)), dest);
        write_bits(1, CK_ETB_TRIGGER_MODE_BIT, (volatile uint32_t *)(CK_ETB_CFG_CH3 + 0x4 * (ch_ret -3)), source_type);
        set_bit(ch_ret, (volatile uint32_t *)(CK_ETB_CFG_CH3 + 0x4 * (ch_ret -3)));
        g_etb_ch3_31_mark[ch_ret] = 1;
        return ch_ret;
    } else {
        return -ETB_NO_SUCH_CHANNEL_AVAIL;
    }
}

static int32_t channle_op(int32_t channel, int32_t command)
{
    if (channel == CK_ETB_CH0) {
        write_bits(1, 0, (volatile uint32_t *)CK_ETB_CFG1_CH0, command);
        return 0;
    } else if (channel == CK_ETB_CH1) {
        write_bits(1, 0, (volatile uint32_t *)CK_ETB_CFG1_CH1, command);
        return 0;
    } else if (channel == CK_ETB_CH2) {
        write_bits(1, 0, (volatile uint32_t *)CK_ETB_CFG1_CH2, command);
        return 0;
    } else if ((channel >= CK_ETB_CH3) && (channel <= CK_ETB_CH31)) {
        write_bits(1, 0, (volatile uint32_t *)(CK_ETB_CFG_CH3 + 0x4 * (channel -3)), command);
        return 0;
    } else {
        return -ETB_NO_SUCH_CHANNEL_AVAIL;
    }

    return -ETB_NO_SUCH_CHANNEL_AVAIL;
}

void ck_etb_irqhandler(int32_t idx)
{
    return;
}

/**
  \brief       get etb instance count.
  \return      etb instance count
*/
int32_t csi_etb_get_instance_count(void)
{
    return target_get_etb_count();
}

/**
  \brief       Initialize etb Interface. 1. Initializes the resources needed for the etb interface 2.registers event callback function.
  \param[in]   idx       etb index.
  \param[in]   cb_event  event call back function \ref etb_event_cb_t
  \return      return etb handle if success
*/
etb_handle_t csi_etb_initialize(int32_t idx, etb_event_cb_t cb_event)
{
    if (idx != 0) {
        return NULL;
    }
    return (etb_handle_t)etb_instance;
}

/**
  \brief       De-initialize etb Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle   etb handle to operate.
  \return      error code
*/
int32_t csi_etb_uninitialize(etb_handle_t handle)
{
    ETB_NULL_PARAM_CHK(handle);

    return 0;
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx       etb index.
  \return      \ref etb_capabilities_t
*/
etb_capabilities_t csi_etb_get_capabilities(int32_t idx)
{
    if (idx != 0) {
        etb_capabilities_t ret;
        memset(&ret, 0, sizeof(etb_capabilities_t));
        return ret;
    }
    return etb_capabilities;
}

/**
  \brief       config etb channel.
  \param[in]   handle       etb handle to operate.
  \param[in]   source_lo    a specific number represent a location in an source trigger location map to trigger other ip(s).
  \param[in]   dest_lo      a specific number represent an location in an dest trigger map to wait signal(s) from source ip(s) or location(s).
  \param[in]   source_type  \ref etb_source_type_e the input source is hardware trigger or software trigger.
  \param[in]   mode         \ref etb_channel_func_e channel function.
  \return      channel nubmber or error code (negative).
*/
int32_t csi_etb_channel_config(etb_handle_t  handle, uint32_t source_ip, uint32_t dest_ip, etb_source_type_e source_type, etb_channel_func_e mode)
{
    ETB_NULL_PARAM_CHK(handle);

    int32_t ch_ret;

    if (mode == ETB_ONE_TRIGGER_MORE) {
        ch_ret = etb_ch1_2_config(source_ip, dest_ip, source_type);
    } else if (mode == ETB_MORE_TRIGGER_ONE) {
        ch_ret = etb_ch0_config(source_ip, dest_ip, source_type);
    } else if (mode == ETB_ONE_TRIGGER_ONE) {
        ch_ret = etb_ch3_to_31_config(source_ip, dest_ip, source_type);
    } else {
        return -ETB_MODE_ERROR;
    }

    write_bits(1, ch_ret, (volatile uint32_t *)CK_ETB_SOFTTRIG, source_type);

    return ch_ret; 
}

/**
  \brief       start etb.
  \param[in]   handle    etb handle to operate.
  \param[in]   channel   etb channel number to operate.
  \return      error code
*/
int32_t csi_etb_start(etb_handle_t  handle, int32_t channel)
{
    int32_t ret = 0;
    ETB_NULL_PARAM_CHK(handle);

    ret = channle_op(channel, CHANNEL_ENABLE_COMMAND);
    if (ret < 0) {
        return -ETB_NO_SUCH_CHANNEL_AVAIL;
    }

    set_bit(0, (volatile uint32_t *)CK_ETB_EN);
    return 0;
}

/**
  \brief       stop etb.
  \param[in]   handle    etb handle to operate.
  \param[in]   channel   etb channel number to operate.
  \return      error code
*/
int32_t csi_etb_stop(etb_handle_t  handle, int32_t channel)
{
    int32_t ret = 0;
    ETB_NULL_PARAM_CHK(handle);

    ret = channle_op(channel, CHANNEL_DISABLE_COMMAND);
    if (ret < 0) {
        return -ETB_NO_SUCH_CHANNEL_AVAIL;
    }

    clear_bit(0, (volatile uint32_t *)CK_ETB_EN);
    return 0;
}

/**
  \brief       Get ETB status.
  \param[in]   handle etb handle to operate.
  \return      ETB status \ref etb_status_t
*/
etb_status_t csi_etb_get_status(etb_handle_t handle)
{
    etb_status_t etb_status = {0};

    if (handle == NULL) {
        return etb_status;
    }

    ck_etb_priv_t *etb_priv = handle;

    return etb_priv->status;
}
