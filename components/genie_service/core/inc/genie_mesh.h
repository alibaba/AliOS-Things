/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __GENIE_MESH_H__
#define __GENIE_MESH_H__

#include <stddef.h>
#include <net/buf.h>

#include <mesh/access.h>
#include <mesh/main.h>
#include <mesh/cfg_srv.h>
#include <mesh/health_srv.h>

#include "genie_service.h"

#define CONFIG_MESH_VENDOR_COMPANY_ID (0x01A8)
#define CONFIG_MESH_VENDOR_MODEL_SRV 0x0000
#define CONFIG_MESH_VENDOR_MODEL_CLI 0x0001

#define GENIE_MESH_INIT_PHARSE_II_HW_RESET_DELAY (2000)       //Unit:ms
#define GENIE_MESH_INIT_PHARSE_II_DELAY_START_MIN (1000)      //Unit:ms
#define GENIE_MESH_INIT_PHARSE_II_DELAY_START_MAX (10 * 1000) //Unit:ms

#define GENIE_MESH_INIT_PHARSE_II_CHECK_APPKEY_INTERVAL (1000)                                                //Unit:ms
#define GENIE_MESH_INIT_PHARSE_II_CHECK_APPKEY_TIMEOUT (30 * GENIE_MESH_INIT_PHARSE_II_CHECK_APPKEY_INTERVAL) //Unit:ms

#define ALI_MODEL_TAG "\t[ALI_MODEL]"

#define MODEL_D(f, ...) printf("%d " ALI_MODEL_TAG "[D] %s " f "\n", (bt_u32_t)aos_now_ms(), __func__, ##__VA_ARGS__)
#define MODEL_I(f, ...) printf(ALI_MODEL_TAG "[I] %s " f "\n", __func__, ##__VA_ARGS__)
#define MODEL_E(f, ...) printf(ALI_MODEL_TAG "[E] %s " f "\n", __func__, ##__VA_ARGS__)

typedef enum _genie_mesh_init_state_e
{
    GENIE_MESH_INIT_STATE_PROVISION = 1,
    GENIE_MESH_INIT_STATE_HW_RESET,
    GENIE_MESH_INIT_STATE_NORMAL_BOOT,
} genie_mesh_init_state_e;

#if 0
typedef enum _genie_mesh_work_type_e
{
    GENIE_MESH_WORK_TYPE_INVALID,
    GENIE_MESH_WORK_TYPE_HW_RESET,
    GENIE_MESH_WORK_TYPE_READY,
    GENIE_MESH_WORK_TYPE_PROV_TIMEOUT,
} genie_mesh_work_type_e;
#endif

struct bt_mesh_elem *genie_mesh_get_primary_element(void);

int genie_mesh_init(struct bt_mesh_elem *p_mesh_elem, unsigned int mesh_elem_counts);

int genie_mesh_init_pharse_ii(void);

void genie_mesh_ready_checktimer_restart(uint32_t timeout);

int genie_mesh_suspend(bool force);

int genie_mesh_resume(void);

void genie_mesh_load_group_addr(void);

#endif
