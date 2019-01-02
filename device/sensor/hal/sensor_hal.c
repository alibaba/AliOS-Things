/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include <hal/base.h>

#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_register.h>

#include "common.h"
#include "sensor.h"
#ifdef UDATA_MODBUS
#include "mbmaster_api.h"
#endif

#define MODBUS_CONFIG_STACK_SIZE (256 * 4)

static int sensor_open(inode_t *node, file_t *file);
static int sensor_close(file_t *file);
static ssize_t sensor_read(file_t *f, void *buf, size_t len);
static ssize_t sensor_write(file_t *f, const void *buf, size_t len);
static int sensor_ioctl(file_t *f, int cmd, unsigned long arg);
static int sensor_poll(file_t *f, bool setup, poll_notify_t notify, struct pollfd *fd, void *opa);

file_ops_t sensor_fops = {
    .open  = sensor_open,
    .close = sensor_close,
    .read  = sensor_read,
    .write = sensor_write,
    .ioctl = sensor_ioctl,
};

static SENSOR_IRQ_CALLBACK g_sensor_irq_cb = NULL;
static sensor_obj_t       *g_sensor_obj[TAG_DEV_SENSOR_NUM_MAX];
static uint32_t            g_sensor_cnt = 0;

static void sensor_set_power_mode(dev_power_mode_e power, int index)
{
    g_sensor_obj[index]->power = power;

}

static int sensor_set_sensor_service(SENSOR_IRQ_CALLBACK func)
{
    if (func == NULL) {
        return -1;
    }

    if (g_sensor_irq_cb != NULL) {
        return 0;
    }

    g_sensor_irq_cb = func;

    return 0;
}
static int sensor_get_sensor_mode_config(int index, dev_sensor_config_t *config)
{
    if (index >= g_sensor_cnt) {
        return -1;
    }
    if (config == NULL) {
        return -1;
    }

    config->mode     = g_sensor_obj[index]->mode;
    config->data_buf = g_sensor_obj[index]->data_buf;
    config->data_len = g_sensor_obj[index]->data_len;

    return 0;
}

static void sensor_irq_handle(void *arg)
{
    uint32_t index = (uint32_t)arg;

    if (index >= TAG_DEV_SENSOR_NUM_MAX) {
        return;
    }

    if ((g_sensor_obj[index]->mode != DEV_FIFO) &&
                 (g_sensor_obj[index]->mode != DEV_INT) &&
                 (g_sensor_obj[index]->mode != DEV_DATA_READY)) {
        return;
    }

    if (NULL != g_sensor_obj[index]->irq_handle) {
        g_sensor_obj[index]->irq_handle();

        if (NULL != g_sensor_irq_cb) {
            g_sensor_irq_cb(g_sensor_obj[index]->tag);
        }
    }
    return;
}

static int  sensor_register_irq(int index )
{
    int ret;

    if (0 == g_sensor_obj[index]->data_len) {
        return -1;
    }

    ret =
      hal_gpio_enable_irq(&(g_sensor_obj[index]->gpio), *(gpio_irq_trigger_t*)(g_sensor_obj[index]->gpio.priv),
                          sensor_irq_handle, (void *)index);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static int find_selected_sensor(char *path)
{
    int i = 0;

    if (path == NULL) {
        return -1;
    }

    for (i = 0; i < g_sensor_cnt; i++) {
        if (strncmp(g_sensor_obj[i]->path, path, strlen(path)) == 0) {
            return i;
        }
    }
    return -1;
}
static int load_sensor_config(int index)
{
    g_sensor_obj[index] = (sensor_obj_t *)aos_malloc(sizeof(sensor_obj_t));
    if (g_sensor_obj[index] == NULL) {
        /* plan to add the other bus register here like spi */
        return -1;
    }
    return 0;
}

static int sensor_obj_register(int index )
{
    int ret = 0;

    if ((g_sensor_obj[index]->mode == DEV_INT) ||
        (g_sensor_obj[index]->mode == DEV_DATA_READY) ||
        (g_sensor_obj[index]->mode == DEV_FIFO)) {
        ret = sensor_register_irq(index);
        if (unlikely(ret)) {
            return -1;
        }
    }

    ret = aos_register_driver(g_sensor_obj[index]->path, &sensor_fops, NULL);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

int sensor_create_obj(sensor_obj_t* sensor)
{
    int ret = 0;

    g_sensor_obj[g_sensor_cnt] =
      (sensor_obj_t *)aos_malloc(sizeof(sensor_obj_t));
    if (g_sensor_obj[g_sensor_cnt] == NULL) {
        return -1;
    }
    memset(g_sensor_obj[g_sensor_cnt], 0, sizeof(sensor_obj_t));

    /* install the phy sensor info into the sensor object datebase here */
    g_sensor_obj[g_sensor_cnt]->io_port    = sensor->io_port;
    g_sensor_obj[g_sensor_cnt]->path       = sensor->path;
    g_sensor_obj[g_sensor_cnt]->tag        = sensor->tag;
    g_sensor_obj[g_sensor_cnt]->open       = sensor->open;
    g_sensor_obj[g_sensor_cnt]->close      = sensor->close;
    g_sensor_obj[g_sensor_cnt]->ioctl      = sensor->ioctl;
    g_sensor_obj[g_sensor_cnt]->read       = sensor->read;
    g_sensor_obj[g_sensor_cnt]->write      = sensor->write;
    g_sensor_obj[g_sensor_cnt]->irq_handle = sensor->irq_handle;
    g_sensor_obj[g_sensor_cnt]->mode       = sensor->mode;
    g_sensor_obj[g_sensor_cnt]->data_buf   = 0;
    g_sensor_obj[g_sensor_cnt]->data_len   = sensor->data_len;
    g_sensor_obj[g_sensor_cnt]->power =
      DEV_POWER_OFF;                     // will update the status later
    g_sensor_obj[g_sensor_cnt]->ref = 0; // count the ref of this sensor

    if ((sensor->mode == DEV_INT) || (sensor->mode == DEV_DATA_READY) ||
        (sensor->mode == DEV_FIFO)) {
        g_sensor_obj[g_sensor_cnt]->gpio.port   = sensor->gpio.port;
        g_sensor_obj[g_sensor_cnt]->gpio.config = sensor->gpio.config;
        g_sensor_obj[g_sensor_cnt]->gpio.priv   = sensor->gpio.priv;
    }

    /* register the sensor object into the irq list and vfs */

    ret = sensor_obj_register(g_sensor_cnt);
    if (unlikely(ret)) {
        goto error;
    }

    g_sensor_cnt++;
    LOGD(SENSOR_STR, "%s successfully \n", __func__);
    return 0;

error:
    free(g_sensor_obj[g_sensor_cnt]);
    return -1;
}

static int sensor_hal_get_dev_list(void* buf)
{
    sensor_list_t *list = buf;
    if (buf == NULL)
        return -1;

    /* load the sensor count and tag list here */

    if (list->cnt >= TAG_DEV_SENSOR_NUM_MAX) {
        return -1;
    }

    for(int index = 0; index < g_sensor_cnt; index++){
        list->list[list->cnt+index] = g_sensor_obj[index]->tag;
    }

    list->cnt += g_sensor_cnt;

    return 0;
}

static int sensor_open(inode_t *node, file_t *file)
{
    int index = 0;

    if ((node == NULL) || (file == NULL)) {
        return -1;
    }


    /* just open the /dev/sensor node here */
    if(strncmp(sensor_node_path, node->i_name, strlen(node->i_name)) == 0){
        return 0;
    }

    index = find_selected_sensor(node->i_name);
    if(( g_sensor_obj[index]->open == NULL)||(index < 0)){
        return -1;
    }

    if(g_sensor_obj[index]->ref == 0){
        g_sensor_obj[index]->open();
    }
    g_sensor_obj[index]->ref++;

    LOGD(SENSOR_STR, "%s successfully \n", __func__);
    return 0;
}

static int sensor_close(file_t *file)
{
    int index = 0;

    /* just close the /dev/sensor node here */
    if(strncmp(sensor_node_path, (file->node->i_name), strlen(file->node->i_name)) == 0){
        return 0;
    }

    index = find_selected_sensor(file->node->i_name);
    if(( g_sensor_obj[index]->close == NULL)||(index < 0)){
        return -1;
    }
    //if the ref counter is less than 2, then close the sensor
    if(g_sensor_obj[index]->ref < 2){
        g_sensor_obj[index]->close();
    }

    if(g_sensor_obj[index]->ref > 0){
        g_sensor_obj[index]->ref--;
    }
    return 0;
}


static ssize_t sensor_read(file_t *f, void *buf, size_t len)
{

    int index = -1;
    int ret   = 0;

    if (f == NULL) {
        return -1;
    }
#ifdef UDATA_MODBUS
    index = find_ModbusSensors(f->node->i_name);
    if (index >= 0) {
        ret = modbus_sensor_read(buf, len, index);
        if (ret <= 0) {
            goto error;
        }
        return ret;
    }
#endif
    index = find_selected_sensor(f->node->i_name);
    if ((g_sensor_obj[index]->read == NULL) || (index < 0)) {
        goto error;
    }

    if (buf == NULL) {
        goto error;
    }

    ret = g_sensor_obj[index]->read(buf, len);
    if (ret < 0) {
        goto error;
    }

    return ret;

error:
    return -1;
}

static ssize_t sensor_write(file_t *f, const void *buf, size_t len)
{
    /* no need this functionality recently */
    return 0;
}

static int sensor_ioctl(file_t *f, int cmd, unsigned long arg)
{
    int                  ret   = 0;
    int                  index = 0;
    dev_sensor_config_t *config;

    if (f == NULL) {
        LOGD("%s %s fail line: %d\n", SENSOR_STR, __func__, __LINE__);
        return -1;
    }

    if (cmd == SENSOR_IOCTL_GET_SENSOR_LIST) {
        ret = sensor_hal_get_dev_list((void *)arg);
        if (unlikely(ret)) {
            LOGD("%s %s fail line: %d\n", SENSOR_STR, __func__, __LINE__);
            return -1;
        }
        return 0;
    }

    index = find_selected_sensor(f->node->i_name);
    if (index < 0) {
        return -1;
    }
    if (cmd == SENSOR_IOCTL_SET_SENSOR_IRQ_CB) {
        config = (dev_sensor_config_t *)arg;
        if (0 == arg) {
            LOGD("%s %s fail line: %d\n", SENSOR_STR, __func__, __LINE__);
            return -1;
        }
        ret = sensor_set_sensor_service(config->irq_callback);
        if (unlikely(ret)) {
            LOGD("%s %s fail line: %d\n", SENSOR_STR, __func__, __LINE__);
            return -1;
        }

        return 0;
    }

    if (cmd == SENSOR_IOCTL_GET_SENSOR_MODE) {
        config = (dev_sensor_config_t *)arg;
        if (0 == arg) {
            LOGD("%s %s fail line: %d\n", SENSOR_STR, __func__, __LINE__);
            return -1;
        }

        ret = sensor_get_sensor_mode_config(index, config);
        if (unlikely(ret)) {
            LOGD("%s %s fail line: %d\n", SENSOR_STR, __func__, __LINE__);
            return -1;
        }

        return 0;
    }

    if (g_sensor_obj[index]->ioctl) {
        ret = g_sensor_obj[index]->ioctl(cmd, arg);
        if (unlikely(ret)) {
            LOGD("%s %s fail line: %d\n", SENSOR_STR, __func__, __LINE__);
            return -1;
        }
    }
    LOGD(SENSOR_STR, "%s successfully \n", __func__);
    return 0;
}

static int sensor_hal_register(void)
{
    int ret = 0;
    ret     = aos_register_driver(sensor_node_path, &sensor_fops, NULL);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

#ifdef UDATA_MODBUS
static void mb_task()
{

    mb_init(MB_RTU, 2, 9600, MB_PAR_NONE); /* uart2 */
    mb_enable();

    while (1) {
        mb_poll();
    }
}


int modbus_init(void)
{
    int rc = 0;
    int ret;

    ret = aos_task_new("mbtask", mb_task, NULL, MODBUS_CONFIG_STACK_SIZE);
    if (ret != 0) {
        printf("mbtask create error\n");
        return -1;
    }

    modbus_sensor_drv_init();

    LOG("modbus initialized\n");
    return rc;
}
#endif

int sensor_init(void)
{
    int ret      = 0;
    g_sensor_cnt = 0;

#ifdef AOS_SENSOR_HUMI_BOSCH_BME280
    drv_humi_bosch_bme280_init();
#endif /* AOS_SENSOR_HUMI_BOSCH_BME280 */

#ifdef AOS_SENSOR_ACC_BOSCH_BMA253
    drv_acc_bosch_bma253_init();
#endif /* AOS_SENSOR_ACC_BOSCH_BMA253 */

#ifdef AOS_SENSOR_BARO_BOSCH_BMP280
    drv_baro_bosch_bmp280_init();
#endif /* AOS_SENSOR_BARO_BOSCH_BMP280 */

#ifdef AOS_SENSOR_ACC_ST_LSM6DSL
    drv_acc_st_lsm6dsl_init();
#endif /* AOS_SENSOR_ACC_ST_LSM6DSL */

#ifdef AOS_SENSOR_GYRO_ST_LSM6DSL
    drv_gyro_st_lsm6dsl_init();
#endif /* AOS_SENSOR_GYRO_ST_LSM6DSL */

#ifdef AOS_SENSOR_BARO_ST_LPS22HB
    drv_baro_st_lps22hb_init();
#endif /* AOS_SENSOR_BARO_ST_LPS22HB */


#ifdef AOS_SENSOR_ACC_MIR3_DA217
    drv_acc_mir3_da217_init();
#endif /* AOS_SENSOR_ACC_MIR3_DA217 */

#ifdef AOS_SENSOR_ALS_LITEON_LTR553
    drv_als_liteon_ltr553_init();
#endif /* AOS_SENSOR_ALS_LITEON_LTR553 */

#ifdef AOS_SENSOR_PS_LITEON_LTR553
    drv_ps_liteon_ltr553_init();
#endif /* AOS_SENSOR_PS_LITEON_LTR553 */

#ifdef AOS_SENSOR_TEMP_SENSIRION_SHTC1
    drv_temp_sensirion_shtc1_init();
#endif /* AOS_SENSOR_TEMP_SENSIRION_SHTC1 */

#ifdef AOS_SENSOR_HUMI_SENSIRION_SHTC1
    drv_humi_sensirion_shtc1_init();
#endif /* AOS_SENSOR_HUMI_SENSIRION_SHTC1 */

#ifdef AOS_SENSOR_TEMP_SENSIRION_HTS221
    drv_temp_st_hts221_init();
#endif /* AOS_SENSOR_TEMP_SENSIRION_HTS221 */

#ifdef AOS_SENSOR_HUMI_SENSIRION_HTS221
    drv_humi_st_hts221_init();
#endif /* AOS_SENSOR_HUMI_SENSIRION_HTS221 */


#ifdef AOS_SENSOR_MAG_ST_LIS3MDL
    drv_mag_st_lis3mdl_init();
#endif /*AOS_SENSOR_MAG_ST_LIS3MDL*/

#ifdef AOS_SENSOR_MAG_MEMSIC_MMC3680KJ
    drv_mag_memsic_mmc3680kj_init();
#endif /* AOS_SENSOR_MAG_MEMSIC_MMC3680KJ */

#ifdef AOS_SENSOR_TEMP_MEMSIC_MMC3680KJ
    drv_temp_memsic_mmc3680kj_init();
#endif /* AOS_SENSOR_TEMP_MEMSIC_MMC3680KJ */


#ifdef UDATA_MODBUS
    modbus_init();
#endif /* UDATA_MODBUS */

    ret = sensor_hal_register();
    if(ret != 0){
        return -1;
    }

    LOGD(SENSOR_STR, "%s successfully \n", __func__);
    return 0;
}

