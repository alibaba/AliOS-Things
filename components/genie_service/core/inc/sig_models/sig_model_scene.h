/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __GENIE_MODEL_SCENE_H__
#define __GENIE_MODEL_SCENE_H__

#define GENIE_SCENE_OP_NUM 4

typedef enum _genie_scene_e
{
    GENIE_SCENE_NORMAL,
    GENIE_SCENE_READ = 3,
    GENIE_SCENE_CINEMA,
    GENIE_SCENE_WARM,
    GENIE_SCENE_NIGHT
} genie_scene_e;

#define SCENE_DEFAULT GENIE_SCENE_NORMAL

extern struct bt_mesh_model_pub g_scene_pub;
extern const struct bt_mesh_model_op g_scene_op[GENIE_SCENE_OP_NUM];
#define MESH_MODEL_SCENE_SRV(_user_data) BT_MESH_MODEL(BT_MESH_MODEL_ID_SCENE_SRV, g_scene_op, &g_scene_pub, _user_data)

#endif
