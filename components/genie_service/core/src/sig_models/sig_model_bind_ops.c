/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */
//

#include <api/mesh.h>
#include "genie_service.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#include "common/log.h"

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
u16_t sig_model_generic_onoff_bind_ops(sig_model_element_state_t *p_elem)
{
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    sig_model_lightness_update_by_onoff(p_elem);
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
    sig_model_light_ctl_update_by_onoff(p_elem);
#endif

    return 0;
}
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
u16_t sig_model_generic_lightness_bind_ops(sig_model_element_state_t *p_elem, u8_t type)
{
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    sig_model_onoff_update_by_lightness(p_elem, type);
#endif

    return 0;
}
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
u16_t sig_model_generic_color_temperature_bind_ops(sig_model_element_state_t *p_elem)
{
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    sig_model_onoff_update_by_color_temperature(p_elem);
#endif

    return 0;
}
#endif
