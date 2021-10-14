/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef _ALI_MODEL_GEN_ONOFF_SRV_H_
#define _ALI_MODEL_GEN_ONOFF_SRV_H_

#include "genie_service.h"

#define GEN_ONOFF_OPC_NUM 4
#define GEN_ONOFF_DEFAULT 1

#define MESH_MODEL_GEN_ONOFF_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_SRV, \
                                                           g_gen_onoff_op, &g_gen_onoff_pub, _user_data)

extern struct bt_mesh_model_pub g_gen_onoff_pub;
extern const struct bt_mesh_model_op g_gen_onoff_op[GEN_ONOFF_OPC_NUM];

/**
 * @brief generic on/off model publication
 * @param[in] p_model: model details
 */
void gen_onoff_publication(struct bt_mesh_model *p_model);

/**
 * @brief bind the on/off element with lightness app.
 * @param[in] p_elem refer to the element to be bound.
 * @param[in] type TYPE_PRESENT or TYPE_TARGET
 */
void sig_model_onoff_update_by_lightness(sig_model_element_state_t *p_elem, E_VALUE_TYPE type);

#ifdef CONFIG_MESH_MODEL_CTL_SRV
void sig_model_onoff_update_by_color_temperature(sig_model_element_state_t *p_elem);
#endif

#endif // _ALI_MODEL_GEN_ONOFF_SRV_H_
