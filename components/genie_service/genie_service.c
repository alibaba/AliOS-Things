/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
//#include <aos/aos.h>
#include <aos/kernel.h>

#include <misc/printk.h>
#include <hal/hal.h>

#include "genie_service.h"

static genie_service_ctx_t genie_service_ctx;

static int genie_service_check_ctx(genie_service_ctx_t *p_ctx)
{
    if (!p_ctx || !p_ctx->event_cb || !p_ctx->p_mesh_elem || p_ctx->mesh_elem_counts < 1)
    {
        GENIE_LOG_ERR("ctx:%p %p %p %d", p_ctx, p_ctx->event_cb, p_ctx->p_mesh_elem, p_ctx->mesh_elem_counts);
        return -1;
    }

    memset(&genie_service_ctx, 0, sizeof(genie_service_ctx_t));
    memcpy(&genie_service_ctx, p_ctx, sizeof(genie_service_ctx_t));

    return 0;
}

genie_service_ctx_t *genie_service_get_context(void)
{
    return &genie_service_ctx;
}

int genie_service_init(genie_service_ctx_t *p_ctx)
{
    int ret = 0;

    GENIE_LOG_ERR("genie service init");
    if (genie_service_check_ctx(p_ctx) < 0)
    {
        GENIE_LOG_ERR("genie service context error");
        return GENIE_SERVICE_ERRCODE_USER_INPUT;
    }

    genie_storage_init();

    genie_cli_init();

#if defined(CONIFG_GENIE_MESH_BINARY_CMD) || defined(CONFIG_GENIE_MESH_AT_CMD)
    genie_sal_uart_init();
#endif

    genie_reset_init();

    genie_transport_init();

#ifdef CONFIG_GENIE_MESH_GLP
    p_ctx->lpm_conf.sleep_ms = GENIE_GLP_SLEEP_TIME;
    p_ctx->lpm_conf.wakeup_ms = GENIE_GLP_WAKEUP_TIME;
#endif

#ifdef CONFIG_PM_SLEEP
    genie_lpm_init(&p_ctx->lpm_conf);
#endif

    if (genie_triple_init() < 0)
    {
#ifdef CONFIG_PM_SLEEP
        genie_sal_sleep_disable();
#endif
        return GENIE_SERVICE_ERRCODE_NO_TRIPLE;
    }

    ret = genie_mesh_init(genie_service_ctx.p_mesh_elem, genie_service_ctx.mesh_elem_counts);
    if (ret != 0)
    {
        GENIE_LOG_ERR("genie service init fail(%d)", ret);
        return GENIE_SERVICE_ERRCODE_MESH_INIT;
    }
    else
    {
        return GENIE_SERVICE_SUCCESS;
    }
}
