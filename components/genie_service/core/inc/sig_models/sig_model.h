#ifndef __SIG_MODEL_H__
#define __SIG_MODEL_H__

#define ATTR_TYPE_GENERIC_ONOFF 0x0100
#define ATTR_TYPE_LIGHTNESS 0x0121
#define ATTR_TYPE_COLOR_TEMPERATURE 0x0122
#define ATTR_TYPE_SENCE 0xF004

typedef enum
{
    TYPE_PRESENT = 0,
    TYPE_TARGET,
    TYPE_NUM,
} E_VALUE_TYPE;

typedef struct
{
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    u8_t onoff[TYPE_NUM];
#endif

#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    u16_t lightness[TYPE_NUM];
#endif

#ifdef CONFIG_MESH_MODEL_CTL_SRV
    u16_t color_temperature[TYPE_NUM];
#endif

#ifdef CONFIG_MESH_MODEL_SCENE_SRV
    u16_t scene[TYPE_NUM];
#endif

    u8_t delay; //unit:5ms
    struct k_timer delay_timer;
#ifdef CONFIG_MESH_MODEL_TRANS
    u8_t trans; //unit:100ms

    bt_u32_t trans_start_time;
    bt_u32_t trans_end_time;

    struct k_timer trans_timer;
#endif
} sig_model_state_t;

typedef struct
{
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    u8_t last_onoff;
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
    u16_t last_lightness;
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
    u16_t last_color_temperature;
#endif
#ifdef CONFIG_MESH_MODEL_SCENE_SRV
    u16_t last_scene;
#endif
} __packed sig_model_powerup_t;

typedef struct
{
    u8_t element_id;
    sig_model_state_t state;
    sig_model_powerup_t powerup;
    void *user_data;
} sig_model_element_state_t;

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
#include "sig_model_onoff_srv.h"
#include "sig_model_bind_ops.h"
#endif
#ifdef CONFIG_MESH_MODEL_LIGHTNESS_SRV
#include "sig_model_lightness_srv.h"
#endif
#ifdef CONFIG_MESH_MODEL_CTL_SRV
#include "sig_model_light_ctl_srv.h"
#endif

#ifdef CONFIG_MESH_MODEL_SCENE_SRV
#include "sig_model_scene.h"
#endif

#include "sig_model_transition.h"
#include "sig_model_opcode.h"
#include "sig_model_event.h"

#endif
