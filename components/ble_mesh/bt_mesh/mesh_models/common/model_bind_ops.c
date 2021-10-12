/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <api/mesh.h>
#include <mesh_model.h>

#define TAG "MESH BIND"

typedef u16_t (*BIND_OPS_HANDLER)(S_ELEM_STATE *p_elem, u8_t type);

u16_t model_bind_operation(BIND_OPERATION_ID id, S_ELEM_STATE *p_elem, u8_t type);


static u16_t _gen_onoff_operation(S_ELEM_STATE *p_elem, u8_t type);
static u16_t _gen_onpowerup_operation(S_ELEM_STATE *p_elem, u8_t type);

static u16_t _gen_level_operation(S_ELEM_STATE *p_elem, u8_t type);
static u16_t _gen_power_actual_operation(S_ELEM_STATE *p_elem, u8_t type);
static u16_t _gen_lightness_linear_operation(S_ELEM_STATE *p_elem, u8_t type);
static u16_t _gen_lightness_actual_operation(S_ELEM_STATE *p_elem, u8_t type);
static u16_t _gen_power_range_operation(S_ELEM_STATE *p_elem, u8_t type);


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
    _gen_power_actual_operation,//B_GEN_PWR_ACTUAL_ID,
    NULL,//B_GEN_PWR_LAST_ID,
    NULL,//B_GEN_PWR_DFT_ID,
    _gen_power_range_operation,//B_GEN_PWR_RANGE_ID,
    /*lightness*/
    _gen_lightness_linear_operation,//B_GEN_LIGHTNESS_LINEAR_ID
    _gen_lightness_actual_operation,//B_GEN_LIGHTNESS_ACTUAL_ID
    /* !!!END!!! --- Don't add new ID after this one */
    NULL
};

u16_t model_bind_operation(BIND_OPERATION_ID id, S_ELEM_STATE *p_elem, u8_t type)
{
    BIND_OPS_HANDLER p_func = NULL;

    LOGD(TAG, "bind ops - id: %d, ele:%p\n", id, p_elem);

    if (id <= B_OPS_START_ID || id >= B_OPS_END_ID || !p_elem) {
        LOGE(TAG, "invalid args, ignore\n");
        return -1;
    }

#if 1
    p_func = bind_handler[id];
    LOGD(TAG, "p_func:%p\n", p_func);

    return p_func ? p_func(p_elem, type) : -1;
#else
    {
        u8_t i = 0;

        for (i = 0; i < sizeof(bind_handler) / sizeof(bind_handler[0]); i++) {
            func = bind_handler[i];

            if (func) {
                func(i, p_elem, type);
            } else {
                LOGD(TAG, "id:%d, func is NULL\n", i);
            }
        }

        return 0;
    }
#endif
}

/*
static u16_t _constrain_actual(S_ELEM_STATE *p_elem, u16_t var)
{

    S_MESH_STATE *p_state = &p_elem->state;

    if (var > 0 && var < p_state->power_range.range_min) {
        var = p_state->power_range.range_min;
    } else if (var > p_state->power_range.range_max) {
        var =  p_state->power_range.range_max;
    }
    return var;
}*/

/*
static u16_t _constrain_lightness(S_ELEM_STATE *p_elem, u16_t var)
{

    S_MESH_STATE *p_state = &p_elem->state;
    S_MESH_POWERUP *p_powerup = &p_elem->powerup;
    if (var > 0 && var < p_powerup->lightness_range.range_min) {
        var = p_powerup->lightness_range.range_min;
    } else if (var > p_powerup->lightness_range.range_max) {
        var =  p_powerup->lightness_range.range_max;
    }
    return var;
}

static u16_t _gen_onoff_operation(S_ELEM_STATE *p_elem, u8_t type) {
    S_MESH_STATE *p_state = &p_elem->state;
    S_MESH_POWERUP *p_power_up = &p_elem->powerup;
    u16_t actual = 0;

    if (p_state->onoff[T_CUR]){
        if(!p_power_up->lightness_default){
         p_state->lightness_actual[T_CUR] = p_power_up->lightness_last;
        }else{
         p_state->lightness_actual[T_CUR] = p_power_up->lightness_default;
        }
    }
    else{
      p_state->actual[T_CUR] = p_state->actual[T_TAR] = actual;
      p_state->lightness_actual[T_CUR] = 0;
    }

    BT_INFO("onoff[T_TAR]:%d, onoff[T_CUR]:%d, actual[TAR]:0x%02x", p_state->onoff[T_TAR], p_state->onoff[T_CUR], p_state->actual[T_TAR]);

    return 0;
}

static u16_t _gen_level_operation(S_ELEM_STATE *p_elem, u8_t type) {
    LOGD(TAG, "");
    S_MESH_STATE *p_state = &p_elem->state;
    p_state->actual[T_TAR] = p_state->level[T_CUR] + 32768;
    p_state->lightness_actual[T_TAR] = p_state->level[T_CUR] + 32768;
    return 0;
}

/*
static u16_t _gen_onpowerup_operation(S_ELEM_STATE *p_elem, u8_t type) {
    S_MESH_STATE *p_state = &p_elem->state;
    S_MESH_POWERUP *p_powerUp = &p_elem->powerup;
    //Ethan: not supported, give 1  for the moment, don't need to adjust target actual here, will only take affect when next power on cycle

    if(p_state->powerUp_status == 0x00){
      p_state->actual[T_CUR] = 0;
      p_state->lightness_actual[T_CUR] = 0;
    }else if(p_state->powerUp_status == 0x01 ){
          if(p_state->power_default != 0x00){
             p_state->actual[T_CUR] = p_state->power_default;
          }else{
              p_state->actual[T_CUR] = p_state->power_last;
          }
          if(p_powerUp->lightness_default != 0x00){
             p_state->lightness_actual[T_CUR] = p_powerUp->lightness_default;
          }else{
             p_state->lightness_actual[T_CUR] = p_powerUp->lightness_last;
          }
    }else if(p_state->powerUp_status == 0x02){
            p_state->actual[T_CUR] = p_state->power_last;
            p_state->lightness_actual[T_CUR] = p_powerUp->lightness_last;
    }

    BT_INFO("onpowerup ops, actual[TAR]:0x%02x\n", p_state->actual[type]);

    return 0;
}*/

/*
static u16_t _gen_power_actual_operation(S_ELEM_STATE *p_elem, u8_t type)
{
  LOGD(TAG, "");
  S_MESH_STATE *p_state = &p_elem->state;
  p_state->actual[type] = _constrain_actual(p_elem , p_state->actual[type]);
  p_state->level[T_CUR] = p_state->actual - 32768;
  if(p_state->actual == 0X0000){
    p_state->onoff[T_TAR] = 0X00;
  }else{
    p_state->onoff[T_TAR] = 0X01;
  }
  p_state->power_last = p_state->actual[type];
  return 0;
}*/

/*
static u16_t _gen_power_range_operation(S_ELEM_STATE *p_elem, u8_t type)
{
 LOGD(TAG, "");

}

static u16_t _gen_lightness_linear_operation(S_ELEM_STATE *p_elem, u8_t type)
{
 LOGD(TAG, "");
 S_MESH_STATE *p_state = &p_elem->state;
 p_state->lightness_actual[type] = (uint16_t)(sqrt(p_state->lightness_linear / 65535) * 65535);
}

static u16_t _gen_lightness_actual_operation(S_ELEM_STATE *p_elem, u8_t type)
{
 LOGD(TAG, "");
 S_MESH_STATE *p_state = &p_elem->state;
 p_state->lightness_linear = p_state->lightness_actual[type] * p_state->lightness_actual[type] / 65535;
 p_state->level = p_state->lightness_actual[type] - 32768;
 if(!p_state->lightness_actual[T_CUR]){
   p_state->onoff[type]= 0x00;
 }else{
   p_state->onoff[type]= 0x01;
 }
}*/



