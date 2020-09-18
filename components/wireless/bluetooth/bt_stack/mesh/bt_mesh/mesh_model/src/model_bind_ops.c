//
// Created by ethan on 2019/7/8.

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)

#include <api/mesh.h>
#include "common/log.h"

static u16_t _lightness_actual_operation(S_ELEM_STATE *p_elem, u8_t type);
static u16_t _lightness_linear_operation(S_ELEM_STATE *p_elem, u8_t type);
static u16_t _lightness_range_operation(S_ELEM_STATE *p_elem, u8_t type);
static u16_t _lightness_dft_operation(S_ELEM_STATE *p_elem, u8_t type);
static u16_t _gen_onpowerup_operation(S_ELEM_STATE *p_elem, u8_t type);
static u16_t _gen_onoff_operation(S_ELEM_STATE *p_elem, u8_t type);
static u16_t _light_ctl_temperature_operation(S_ELEM_STATE *p_elem, u8_t type);
static u16_t light_ctl_temp_dft_operation(S_ELEM_STATE *p_elem, u8_t type);
static u16_t light_ctl_temp_range_operation(S_ELEM_STATE *p_elem, u8_t type);
static u16_t _generic_level_operation(S_ELEM_STATE *p_elem, u8_t type);

BIND_OPS_HANDLER bind_handler[B_OPS_END_ID] = {
/* !!!START!!! --- Don't add new ID before this one */
    //B_OPS_START_ID = -1,

/* Generic OnOff */
    _gen_onoff_operation,//B_GEN_ONOFF_ID = 0,

/* Generic Level */
    NULL,//B_GEN_LEVEL_ID,
    NULL,//B_GEN_DELTA_ID,
    NULL,//B_GEN_MOVE_ID,

/* Generic Default Transition Time */
    NULL,//B_GEN_DFT_TRANS_TIME_ID,

/* Generic Power OnOff */
    _gen_onpowerup_operation,//B_GEN_ON_PWR_UP_ID,

/* Generic Power Level */
    NULL,//B_GEN_PWR_LEVEL_ID,
    NULL,//B_GEN_PWR_LAST_ID,
    NULL,//B_GEN_PWR_DFT_ID,
    NULL,//B_GEN_PWR_RANGE_ID,

/* Light Lightness */
    _lightness_actual_operation,//B_LIGHTNESS_ID,
    NULL,//B_LIGHTNESS_LINEAR_ID,
    NULL,//B_LIGHTNESS_LAST_ID,
    NULL,//B_LIGHTNESS_DFT_ID,
    NULL,//B_LIGHTNESS_RANGE_ID,

/* Light CTL */

    NULL,//B_LIGHT_CTL_ID,
    _light_ctl_temperature_operation,//B_LIGHT_CTL_TEMP_ID,
    NULL,//B_LIGHT_CTL_DFT_TEMP_ID,
    NULL,//B_LIGHT_CTL_TEMP_RANGE_ID,

/* !!!END!!! --- Don't add new ID after this one */
    NULL
};

u16_t model_bind_operation(BIND_OPERATION_ID id, S_ELEM_STATE *p_elem, u8_t type) {
    BIND_OPS_HANDLER p_func = NULL;

    BT_DBG("bind ops - id: %d, ele:%p\n", id, p_elem);

    if (id <= B_OPS_START_ID || id >= B_OPS_END_ID || !p_elem) {
        BT_ERR("invalid args, ignore\n");
        return -1;
    }
#if 1
    p_func = bind_handler[id];
    BT_DBG("p_func:%p\n", p_func);

    return p_func ? p_func(p_elem, type) : -1;
#else
    {
        u8_t i = 0;
        for (i = 0; i < sizeof(bind_handler)/sizeof(bind_handler[0]); i++) {
            func = bind_handler[i];
            if (func)
                func(i, p_elem, type);
            else
                BT_DBG("id:%d, func is NULL\n", i);
        }
       return 0;
    }
#endif
}

static u16_t _lightness_actual_to_linear(S_ELEM_STATE *p_elem, u16_t val)
{
    /* Ethan:TODO need to replace ceiling */
#if 0
    float tmp;
    tmp = ((float) val / UINT16_MAX);

    return (u16_t) ceiling(UINT16_MAX * tmp * tmp);
#else
    return p_elem->state.linear[T_CUR];
#endif
}

static u16_t _lightness_linear_to_actual(S_ELEM_STATE *p_elem, u16_t val)
{

    /* Ethan:TODO need to replace sqrt */
#if 0
    //return (u16_t) (UINT16_MAX * sqrt(((float) val / UINT16_MAX)));
#else
    return p_elem->state.actual[T_CUR];
#endif
}

static u16_t _constrain_lightness(S_ELEM_STATE *p_elem, u16_t var)
{

    S_MESH_POWERUP *p_powerup = &p_elem->powerup;

    if (var > 0 && var < p_powerup->min_actual) {
        var = p_powerup->min_actual;
    } else if (var > p_powerup->max_actual) {
        var = p_powerup->max_actual;
    }
    return var;
}

static u16_t _lightness_linear_operation(S_ELEM_STATE *p_elem, u8_t type) {
    S_MESH_STATE *p_state = &p_elem->state;
    u16_t actual = _lightness_linear_to_actual(p_elem, p_state->linear[T_TAR]);

    p_state->actual[T_TAR] = _constrain_lightness(p_elem, actual);

    return 0;
}

static u16_t _lightness_actual_to_gen_level(u16_t val)
{
    return (u16_t) (val - 32768);
}

static u16_t _gen_level_to_lightness_actual(u16_t val)
{
    return (u16_t) (val + 32768);
}

static u16_t _lightness_actual_to_gen_onoff(u16_t val)
{
    return val ? 1 : 0;
}

static u16_t _gen_onoff_to_lightness_actual(S_ELEM_STATE *p_elem, u16_t val)
{
    S_MESH_POWERUP *p_powerup = &p_elem->powerup;

    /* for power off operation, return 0 */
    if (!val)
        return 0;

    /* Ethan:we assume val = 1 here */
    /* TODO if both last_actual and last_actual are 0, set last_actual to 90%, namely 0xE665 */
    if (!p_powerup->default_actual && !p_powerup->last_actual)
        p_powerup->last_actual = 0xE665;

    return p_powerup->default_actual ? p_powerup->default_actual : p_powerup->last_actual;
}

static u16_t _gen_onpowerup_to_lightness_actual(S_ELEM_STATE *p_elem, u16_t val)
{
    u16_t actual = 0;
    S_MESH_POWERUP *p_powerup = &p_elem->powerup;

    /* Ethan: TODO if both last_actual and last_actual are 0, set last_actual to 0xE665 */
    if (!p_powerup->default_actual && !p_powerup->last_actual)
        p_powerup->last_actual = 0xE665;

    switch(val) {
        case 0:
            actual = 0;
            break;
        case 2:
            actual = p_powerup->last_actual;
            break;
        case 1:
        default:
            /* invalid, choose to follow 1 */
            actual = p_powerup->default_actual ? p_powerup->default_actual : p_powerup->last_actual;
            break;
    }

    return actual;
}

static u16_t _lightness_actual_operation(S_ELEM_STATE *p_elem, u8_t type) {
    S_MESH_STATE *p_state = &p_elem->state;
    S_MESH_POWERUP *p_powerup = &p_elem->powerup;
    u16_t actual = 0;
    BT_DBG("");

    actual =_constrain_lightness(p_elem, p_state->actual[type]);

    actual = _constrain_lightness(p_elem, actual);
    p_state->actual[type] = actual;
    /* lightness linear */
    p_state->linear[type] = _lightness_actual_to_linear(p_elem, actual);

    /* generic level */
    p_state->level[type] = _lightness_actual_to_gen_level(actual);

    /* generic onoff */
    p_state->onoff[type] = _lightness_actual_to_gen_onoff(actual);

    /* lightness last, only update last_actual when current lightness changes */
    if (type == T_CUR && actual)
        p_powerup->last_actual = actual;

    BT_INFO("actual ops, type:%s, actual[CUR]:0x%02x, actual[TAR]:0x%02x, linear:0x%02x, level:0x%02x, onoff:0x%01x, last_actual:0x%02x\n",
           type == T_CUR ? "CUR" : "TAR",
           p_state->actual[T_CUR],
           p_state->actual[T_TAR],
           p_state->linear[type],
           p_state->level[type],
           p_state->onoff[type],
           p_powerup->last_actual);

    return 0;
}

static u16_t _constrain_lightness_actual(S_ELEM_STATE *p_elem, u16_t var)
{
    S_MESH_POWERUP *p_powerup = &p_elem->powerup;
    u16_t min = p_powerup->min_actual;
    u16_t max = p_powerup->max_actual;

    return (var > max) ? max : ((var < min) ? min : var);
}

static u16_t _lightness_dft_operation(S_ELEM_STATE *p_elem, u8_t type) {

    S_MESH_POWERUP *p_powerup = &p_elem->powerup;

    p_powerup->default_actual = _constrain_lightness_actual(p_elem, p_powerup->default_actual);
    BT_INFO("default actual:0x%02x\n", p_powerup->default_actual);
    return 0;
}

static u16_t _lightness_range_operation(S_ELEM_STATE *p_elem, u8_t type) {
    S_MESH_POWERUP *p_powerup = &p_elem->powerup;
    S_MESH_STATE *p_state = &p_elem->state;

    /* bind to default actual */
    p_powerup->default_actual = _constrain_lightness_actual(p_elem, p_powerup->default_actual);

    /* bind to actual's target value */
    p_state->actual[T_TAR] = _constrain_lightness_actual(p_elem, p_state->actual[T_TAR]);
    BT_INFO("range ops, default_actual:0x%02x, actual[TAR]:0x%02x\n", p_powerup->default_actual, p_state->actual[T_TAR]);

    return 0;
}

static u16_t _light_ctl_temperature_operation(S_ELEM_STATE *p_elem, u8_t type) {
    BT_DBG("");
    return 0;
}

static u16_t light_ctl_temp_dft_operation(S_ELEM_STATE *p_elem, u8_t type) {
    BT_DBG("");
    return 0;
}

static u16_t light_ctl_temp_range_operation(S_ELEM_STATE *p_elem, u8_t type) {
    BT_DBG("");
    return 0;
}

static u16_t _generic_level_operation(S_ELEM_STATE *p_elem, u8_t type) {
    BT_DBG("");
    return 0;
}

static u16_t _gen_onoff_operation(S_ELEM_STATE *p_elem, u8_t type) {
    S_MESH_STATE *p_state = &p_elem->state;
    u16_t actual = 0;

    actual = _gen_onoff_to_lightness_actual(p_elem, p_state->onoff[T_CUR]);
    if (p_state->onoff[T_CUR])
        p_state->actual[type] = _constrain_lightness(p_elem, actual);
    else
        p_state->actual[T_CUR] = p_state->actual[T_TAR] = actual;

    BT_INFO("onoff[T_TAR]:%d, onoff[T_CUR]:%d, actual[TAR]:0x%02x", p_state->onoff[T_TAR], p_state->onoff[T_CUR], p_state->actual[T_TAR]);

    return 0;
}

static u16_t _gen_onpowerup_operation(S_ELEM_STATE *p_elem, u8_t type) {
    S_MESH_STATE *p_state = &p_elem->state;

    /* Ethan: not supported, give 1  for the moment, don't need to adjust target actual here, will only take affect when next power on cycle */
    u16_t actual = _gen_onpowerup_to_lightness_actual(p_elem, 2);

    p_state->actual[type] = _constrain_lightness(p_elem, actual);
    BT_INFO("onpowerup ops, actual[TAR]:0x%02x\n", p_state->actual[type]);

    return 0;
}


