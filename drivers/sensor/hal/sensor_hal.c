/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include <hal/base.h>

#include "sensor.h"
#include "sensor_hal.h"

#define SENSOR_MS_TO_ODR(ms)     ((1000/(ms)) + 1)

static int sensor_open(inode_t *node, file_t *file);
static int sensor_close(file_t *file);
static ssize_t sensor_read(file_t *f, void *buf, size_t len);
static ssize_t sensor_write(file_t *f, const void *buf, size_t len);
static int sensor_ioctl(file_t *f, int cmd, unsigned long arg);


#if defined(__CC_ARM) && defined(SENSOR_DRV_AUTO_INIT)
extern unsigned int Image$$RW_SENSOR$$Base;
extern unsigned int Image$$RW_SENSOR$$Limit;
void* g_sensor_start = (void*)&(Image$$RW_SENSOR$$Base);
void* g_sensor_end = (void*)&(Image$$RW_SENSOR$$Limit);
#elif defined(__ICCARM__) && defined(SENSOR_DRV_AUTO_INIT)

#elif defined(__GNUC__) && defined(SENSOR_DRV_AUTO_INIT)
extern unsigned int  _sensor_start;
extern unsigned int  _sensor_limit;
void* g_sensor_start = (void*)&(_sensor_start);
void* g_sensor_end = (void*)&(_sensor_limit);
#endif


file_ops_t sensor_fops = {
    .open  = sensor_open,
    .close = sensor_close,
    .read  = sensor_read,
    .write = sensor_write,
    .ioctl = sensor_ioctl,
};

static SENSOR_IRQ_CALLBACK g_sensor_irq_cb = NULL;
static sensor_obj_t *      g_sensor_obj[SENSOR_MAX_NUM];
static char                g_sensor_path[SENSOR_MAX_NUM][SENSOR_NAME_LEN];
static uint32_t            g_sensor_cnt = 0;
extern int                 g_sensor_drv_num;
extern SENSOR_INIT_FUN     g_sensor_func[];

UNUSED static void sensor_set_power_mode(dev_power_mode_e power, int index)
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

    if (index >= SENSOR_MAX_NUM) {
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
            g_sensor_irq_cb(g_sensor_obj[index]->tag, g_sensor_obj[index]->instance);
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
        if(strlen(path) != strlen(g_sensor_path[i])){
            continue;
        }
        if (strncmp(g_sensor_path[i], path, strlen(path)) == 0) {
            return i;
        }
    }
    return -1;
}

UNUSED static int load_sensor_config(int index)
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

    ret = aos_register_driver(g_sensor_path[index], &sensor_fops, NULL);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

int sensor_create_obj(sensor_obj_t* sensor)
{
    int ret = 0;
    int index;

    index = g_sensor_cnt;
    if(index >= SENSOR_MAX_NUM){
        return -1;
    }

    g_sensor_obj[index] =
      (sensor_obj_t *)aos_malloc(sizeof(sensor_obj_t));
    if (g_sensor_obj[index] == NULL) {
        return -1;
    }

    memset(g_sensor_obj[index], 0, sizeof(sensor_obj_t));
    memset(g_sensor_path[index], 0, SENSOR_NAME_LEN);

    /* install the phy sensor info into the sensor object datebase here */
    ret = snprintf(g_sensor_path[index], SENSOR_NAME_LEN, "%s/%d", sensor->path, sensor->instance);
    if(ret < 0){
        goto error;
    }

    ret = find_selected_sensor(g_sensor_path[index]);
    if(ret >= 0){
        goto error;
    }
    
    g_sensor_obj[index]->io_port    = sensor->io_port;
    g_sensor_obj[index]->tag        = sensor->tag;
    g_sensor_obj[index]->instance   = sensor->instance;
    g_sensor_obj[index]->open       = sensor->open;
    g_sensor_obj[index]->close      = sensor->close;
    g_sensor_obj[index]->ioctl      = sensor->ioctl;
    g_sensor_obj[index]->read       = sensor->read;
    g_sensor_obj[index]->write      = sensor->write;
    g_sensor_obj[index]->irq_handle = sensor->irq_handle;
    g_sensor_obj[index]->mode       = sensor->mode;
    g_sensor_obj[index]->data_buf   = 0;
    g_sensor_obj[index]->data_len   = sensor->data_len;
    g_sensor_obj[index]->power =
      DEV_POWER_OFF;                     // will update the status later
    g_sensor_obj[index]->ref = 0; // count the ref of this sensor
    g_sensor_obj[index]->drv_index  = sensor->drv_index;

    if ((sensor->mode == DEV_INT) || (sensor->mode == DEV_DATA_READY) ||
        (sensor->mode == DEV_FIFO)) {
        g_sensor_obj[index]->gpio.port   = sensor->gpio.port;
        g_sensor_obj[index]->gpio.config = sensor->gpio.config;
        g_sensor_obj[index]->gpio.priv   = sensor->gpio.priv;
    }

    /* register the sensor object into the irq list and vfs */
    ret = sensor_obj_register(index);
    if (unlikely(ret)) {
        goto error;
    }

    g_sensor_cnt++;
    return 0;

error:
    if(g_sensor_obj[index] != NULL){
        aos_free(g_sensor_obj[index]);
    }
    return -1;
}

static int sensor_hal_get_dev_list(void* buf)
{
    sensor_list_t *sensor_list = buf;
    if (buf == NULL)
        return -1;

    /* load the sensor count and tag list here */

    if (sensor_list->cnt != 0) {
        return -1;
    }

    for(int index = 0; (index < g_sensor_cnt) && (index < SENSOR_MAX_NUM); index++){
        sensor_list->list[index].tag    = g_sensor_obj[index]->tag;
        sensor_list->list[index].instance  = g_sensor_obj[index]->instance;
        sensor_list->list[index].io_port    = g_sensor_obj[index]->io_port;
    }

    sensor_list->cnt = g_sensor_cnt;

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
    if ((index < 0) || (index >= SENSOR_MAX_NUM)){
        return -1;
    }
    if( g_sensor_obj[index]->open == NULL){
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
    if ((index < 0) || (index >= SENSOR_MAX_NUM)){
        return -1;
    }
    if( g_sensor_obj[index]->close == NULL){
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
    if (buf == NULL) {
        goto error;
    }

    index = find_selected_sensor(f->node->i_name);
    if ((index < 0) || (index >= SENSOR_MAX_NUM)){
        goto error;
    }
#ifdef UDATA_MODBUS
    if(g_sensor_obj[index]->io_port = MODBUS_PORT){
        int* index_data = (int *)buf;
        *index_data = g_sensor_obj[index]->drv_index;    
        if(*index_data < 0)
        {
            goto error;
        }
    }
#endif

    if ((g_sensor_obj[index]->read == NULL)) {
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
    unsigned long        value;
    dev_sensor_config_t *config;

    if (f == NULL) {
        LOGD("%s %s fail line: %d\n", SENSOR_STR, __func__, __LINE__);
        return -1;
    }
    value = arg;

    if (cmd == SENSOR_IOCTL_GET_SENSOR_LIST) {
        ret = sensor_hal_get_dev_list((void *)value);
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
        config = (dev_sensor_config_t *)value;
        if (0 == value) {
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
        config = (dev_sensor_config_t *)value;
        if (0 == value) {
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

    if (cmd == SENSOR_IOCTL_ODR_SET) {
        if (0 == value) {
            LOGD("%s %s fail line: %d\n", SENSOR_STR, __func__, __LINE__);
            return -1;
        }
        
        value = SENSOR_MS_TO_ODR(value);
    }

    if (g_sensor_obj[index]->ioctl) {
        ret = g_sensor_obj[index]->ioctl(cmd, value);
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




int sensor_init(void)
{
    int ret      = 0;
    g_sensor_cnt = 0;

    if(g_sensor_drv_num > SENSOR_MAX_NUM){
        return -1;
    }

    for(int i = 0; i < g_sensor_drv_num; i++){
        if(g_sensor_func[i] == NULL){
            continue;
        }
        ret = g_sensor_func[i]();
        if(unlikely(ret)){
            LOGD(SENSOR_STR, "%s %d fail \n", __func__,i);
        }
    }

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

