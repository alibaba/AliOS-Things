/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <hal/hal.h>

static hal_ota_module_t *ota_module;

hal_ota_module_t *hal_ota_get_default_module(void)
{
    return ota_module;
}
EXPORT_SYMBOL_K(1, hal_ota_get_default_module, "hal_ota_module_t *hal_ota_get_default_module(void)")

void hal_ota_register_module(hal_ota_module_t *module)
{
    ota_module = module;
}
EXPORT_SYMBOL_K(1, hal_ota_register_module, "void hal_ota_register_module(hal_ota_module_t *module)")

hal_stat_t hal_ota_init(void *something)
{
    return ota_module->init(ota_module, something);
}
EXPORT_SYMBOL_K(1, hal_ota_init, "hal_stat_t hal_ota_init(void *something)")

hal_stat_t hal_ota_write(hal_ota_module_t *m, volatile uint32_t *off_set, uint8_t *in_buf , uint32_t in_buf_len)
{
    if (m == NULL) {
        m = hal_ota_get_default_module();
    }

    if (m != NULL && m->ota_write != NULL) {
        return m->ota_write(m, off_set, in_buf, in_buf_len);
    }

    return 0;
}
EXPORT_SYMBOL_K(1, hal_ota_write, "hal_stat_t hal_ota_write(hal_ota_module_t *m, volatile uint32_t *off_set, uint8_t *in_buf , uint32_t in_buf_len)")

hal_stat_t hal_ota_read(hal_ota_module_t *m, volatile uint32_t *off_set, uint8_t *out_buf, uint32_t out_buf_len)
{
    if (m == NULL) {
        m = hal_ota_get_default_module();
    }

    if (m != NULL && m->ota_read != NULL) {
        return m->ota_read(m, off_set, out_buf, out_buf_len);
    }

    return 0;
}
EXPORT_SYMBOL_K(1, hal_ota_read, "hal_stat_t hal_ota_read(hal_ota_module_t *m, volatile uint32_t *off_set, uint8_t *out_buf, uint32_t out_buf_len)")

hal_stat_t hal_ota_set_boot(hal_ota_module_t *m, void *something)
{
    if (m == NULL) {
        m = hal_ota_get_default_module();
    }

    if (m != NULL && m->ota_set_boot != NULL) {
        return m->ota_set_boot(m, something);
    }

    return 0;
}
EXPORT_SYMBOL_K(1, hal_ota_set_boot, "hal_stat_t hal_ota_set_boot(hal_ota_module_t *m, void *something)")

