/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "objects.h"
#include "ameba_soc.h"
#include "cmsis.h"
#include <drv/wdt.h>

csi_error_t csi_wdt_init(csi_wdt_t *wdt, uint32_t idx)
{
    if(!wdt)
        return CSI_ERROR;

    wdt->priv = (WDG_InitTypeDef *)malloc(sizeof(WDG_InitTypeDef));
    wdt->dev.idx = idx;
    return CSI_OK;
}

void csi_wdt_uninit(csi_wdt_t *wdt)
{
    if(wdt && wdt->priv) {
        free(wdt->priv);
        wdt->priv = NULL;
    }
}

csi_error_t csi_wdt_set_timeout(csi_wdt_t *wdt, uint32_t ms)
{
    WDG_InitTypeDef *WDG_InitStruct = (WDG_InitTypeDef *)wdt->priv;

    uint32_t CountProcess;
    uint32_t DivFacProcess;

    WDG_Scalar(ms, &CountProcess, &DivFacProcess);
    WDG_InitStruct->CountProcess = CountProcess;
    WDG_InitStruct->DivFacProcess = DivFacProcess;
    WDG_Init(WDG_InitStruct);
    return CSI_OK;
}

csi_error_t csi_wdt_start(csi_wdt_t *wdt)
{
    WDG_Cmd(ENABLE);
    return CSI_OK;
}

void csi_wdt_stop(csi_wdt_t *wdt)
{
    WDG_Cmd(DISABLE);
}

csi_error_t csi_wdt_feed(csi_wdt_t *wdt)
{
    WDG_Refresh();
    return CSI_OK;
}

static uint32_t wdt_irqhandler(IN VOID *Data)
{
    csi_wdt_t *wdt = (csi_wdt_t *)Data;

    if (wdt->callback)
    	wdt->callback(wdt,NULL);
    else
	printf("wdt irq happens, but no callback is set\r\n");

    return CSI_OK;
}

csi_error_t csi_wdt_attach_callback(csi_wdt_t *wdt, void *callback, void *arg)
{
    wdt->callback = callback;
    WDG_IrqInit((VOID *)wdt_irqhandler, (uint32_t)wdt);
    return CSI_OK;
}

void csi_wdt_detach_callback(csi_wdt_t *wdt)
{
    printf("%s - %d\r\n", __func__, __LINE__);
    wdt->callback = NULL;
    return;
}
