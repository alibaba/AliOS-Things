/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __SIG_MODEL_BIND_OPS_H__
#define __SIG_MODEL_BIND_OPS_H__

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
u16_t sig_model_generic_onoff_bind_ops(sig_model_element_state_t *p_elem);
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
u16_t sig_model_generic_lightness_bind_ops(sig_model_element_state_t *p_elem, u8_t type);
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
u16_t sig_model_generic_color_temperature_bind_ops(sig_model_element_state_t *p_elem);
#endif

#endif
