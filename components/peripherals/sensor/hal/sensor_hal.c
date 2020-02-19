/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "aos/vfs.h"
#include "network/hal/base.h"
#include "sensor_hal.h"

#define SENSOR_MS_TO_ODR(ms)     ((1000/(ms)) + 1)

static int sensor_open(inode_t *node, file_t *file);
static int sensor_close(file_t *file);
static ssize_t sensor_read(file_t *f, void *buf, size_t len);
static ssize_t sensor_write(file_t *f, const void *buf, size_t len);
static int sensor_ioctl(file_t *f, int cmd, unsigned long arg);


file_ops_t sensor_fops = {
    .open  = sensor_open,
    .close = sensor_close,
    .read  = sensor_read,
    .write = sensor_write,
    .ioctl = sensor_ioctl,
};

static bool                g_sensor_init_flag = false;
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
static int sensor_get_sensor_mode_config(uint32_t index, dev_sensor_config_t *config)
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

void sensor_irq_handle(void *arg)
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
    int ret = -1;

    if (0 == g_sensor_obj[index]->data_len) {
        return -1;
    }
#if SENSOR_CONFIG_INT_ENABLE
    ret =
      hal_gpio_enable_irq(&(g_sensor_obj[index]->gpio), *(gpio_irq_trigger_t*)(g_sensor_obj[index]->gpio.priv),
                          sensor_irq_handle, (void *)index);
    if (unlikely(ret)) {
        return -1;
    }
#endif
    return ret;
}

static int find_selected_sensor(char *path, sensor_tag_e* ptag, uint8_t* pinstance)
{
    uint32_t i = 0;
    if (path == NULL) {
        return -1;
    }

    if (ptag == NULL) {
        return -1;
    }

    if (pinstance == NULL) {
        return -1;
    }
    for (i = 0; i < g_sensor_cnt; i++) {
        if(strlen(path) != strlen(g_sensor_path[i])){
            continue;
        }
        if (strncmp(g_sensor_path[i], path, strlen(path)) == 0) {
            break;
        }
    }

    if (i >= g_sensor_cnt){
        return -1;
    }

    *ptag = g_sensor_obj[i]->tag;
    *pinstance = g_sensor_obj[i]->instance;

    return 0;
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
    sensor_tag_e  tag;
    uint8_t       instance;

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

    ret = find_selected_sensor(g_sensor_path[index], &tag, &instance);
    if(ret == 0){
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
    int index;
    sensor_list_t *sensor_list = buf;
    if (buf == NULL)
        return -1;

    /* load the sensor count and tag list here */

    if (sensor_list->cnt != 0) {
        return -1;
    }

    for(index = 0; (index < g_sensor_cnt) && (index < SENSOR_MAX_NUM); index++){
        sensor_list->list[index].tag    = g_sensor_obj[index]->tag;
        sensor_list->list[index].instance  = g_sensor_obj[index]->instance;
        sensor_list->list[index].io_port    = g_sensor_obj[index]->io_port;
    }

    sensor_list->cnt = g_sensor_cnt;

    return 0;
}


static int sensor_obj_get(sensor_tag_e tag, uint8_t instance,uint32_t* pindex)
{
    uint32_t i;

    if (tag >= TAG_DEV_SENSOR_NUM_MAX){
        return -1;
    }

    if (pindex == NULL){
        return -1;
    }

    for (i = 0; i < g_sensor_cnt; i++) {
        if ((g_sensor_obj[i]->tag == tag) && (g_sensor_obj[i]->instance == instance)){
            break;
        }
    }

    if (i >= g_sensor_cnt){
        return -1;
    }

    *pindex = i;

    return 0;
}

int sensor_hal_open(sensor_tag_e tag, uint8_t instance)
{
    int ret;
    uint32_t index;

    if (tag >= TAG_DEV_SENSOR_NUM_MAX){
        return -1;
    }

    ret = sensor_obj_get(tag, instance, &index);
    if (unlikely(ret)){
        return -1;
    }

    if( g_sensor_obj[index]->open == NULL){
        return -1;
    }

    if(g_sensor_obj[index]->ref == 0){
        ret = g_sensor_obj[index]->open();
        if (unlikely(ret)){
            return -1;
        }
    }
    g_sensor_obj[index]->ref++;

    LOGD(SENSOR_STR, "%s successfully \n", __func__);
    return 0;
}

int sensor_hal_close(sensor_tag_e tag, uint8_t instance)
{
    int ret;
    uint32_t index;

    if (tag >= TAG_DEV_SENSOR_NUM_MAX){
        return -1;
    }

    ret = sensor_obj_get(tag, instance, &index);
    if (unlikely(ret)){
        return -1;
    }

    if( g_sensor_obj[index]->close == NULL){
        return -1;
    }
    //if the ref counter is less than 2, then close the sensor
    if(g_sensor_obj[index]->ref < 2){
        ret = g_sensor_obj[index]->close();
        if (unlikely(ret)){
            return -1;
        }
    }

    if(g_sensor_obj[index]->ref > 0){
        g_sensor_obj[index]->ref--;
    }
    return 0;
}


ssize_t sensor_hal_read(sensor_tag_e tag, uint8_t instance, void *buf, size_t len)
{
    int ret;
    uint32_t index;

    if (tag >= TAG_DEV_SENSOR_NUM_MAX){
        goto error;
    }

    ret = sensor_obj_get(tag, instance, &index);
    if (unlikely(ret)){
        goto error;
    }

#if SENSOR_CONFIG_MODBUS_ENABLE
    if(g_sensor_obj[index]->io_port == MODBUS_PORT){
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

ssize_t sensor_hal_write(sensor_tag_e tag, uint8_t instance, const void *buf, size_t len)
{
    /* no need this functionality recently */
    return 0;
}

int sensor_hal_ioctl(sensor_tag_e tag, uint8_t instance, sensor_cmd_type cmd, unsigned long arg)
{
    int                  ret   = 0;
    uint32_t             index = 0;
    unsigned long        value = arg;
    dev_sensor_config_t *config;

    if (tag >= TAG_DEV_SENSOR_NUM_MAX){
        return -1;
    }

    ret = sensor_obj_get(tag, instance, &index);
    if (unlikely(ret)){
        return -1;
    }

    switch (cmd){
        case SENSOR_IOCTL_GET_SENSOR_LIST:
            return -1;

        case SENSOR_IOCTL_SET_SENSOR_IRQ_CB: {
            config = (dev_sensor_config_t *)value;
            if (NULL == config) {
                return -1;
            }
            ret = sensor_set_sensor_service(config->irq_callback);
            if (unlikely(ret)) {
                return -1;
            }
            return 0;
        }
        case SENSOR_IOCTL_GET_SENSOR_MODE: {
            config = (dev_sensor_config_t *)value;
            if (NULL == config) {
                return -1;
            }

            ret = sensor_get_sensor_mode_config(index, config);
            if (unlikely(ret)) {
                return -1;
            }
            return 0;
        }
        case SENSOR_IOCTL_ODR_SET: {
            if (0 == value) {
                return -1;
            }
            value = SENSOR_MS_TO_ODR(value);
            break;
        }

        default :
            break;

    }

    if (g_sensor_obj[index]->ioctl) {
        g_sensor_obj[index]->ioctl(cmd, value);
    }

    LOGD(SENSOR_STR, "%s successfully \n", __func__);
    return 0;
}

int sensor_hal_init(void)
{
    int ret      = 0;
    int i;
    if(g_sensor_init_flag != false){
        return 0;
    }

    g_sensor_cnt = 0;

    if(g_sensor_drv_num > SENSOR_MAX_NUM){
        return -1;
    }

    for(i = 0; i < g_sensor_drv_num; i++){
        if(g_sensor_func[i] == NULL){
            continue;
        }
        ret = g_sensor_func[i]();
        if(unlikely(ret)){
            LOGD(SENSOR_STR, "%s %d fail \n", __func__,i);
        }
    }

#if SENSOR_CONFIG_MODBUS_ENABLE
    modbus_init();
#endif
    g_sensor_init_flag = true;

    return 0;
}
static int sensor_open(inode_t *node, file_t *file)
{
    int ret;
    sensor_tag_e  tag;
    uint8_t       instance;

    if ((node == NULL) || (file == NULL)) {
        return -1;
    }

    /* just open the /dev/sensor node here */
    if ((strlen(node->i_name) == strlen(sensor_node_path)) && (strncmp(sensor_node_path, node->i_name, strlen(node->i_name)) == 0)){
        return 0;
    }

    ret = find_selected_sensor(node->i_name, &tag, &instance);
    if (unlikely(ret)){
        return -1;
    }

    return sensor_hal_open(tag, instance);
}

static int sensor_close(file_t *file)
{
    int ret;
    sensor_tag_e  tag;
    uint8_t       instance;

    if (file == NULL) {
        return -1;
    }
    if (file->node == NULL) {
        return -1;
    }

    /* just close the /dev/sensor node here */
    if ((strlen(file->node->i_name) == strlen(sensor_node_path)) && (strncmp(sensor_node_path, (file->node->i_name), strlen(file->node->i_name)) == 0)){
        return 0;
    }

    ret = find_selected_sensor(file->node->i_name, &tag, &instance);
    if (unlikely(ret)){
        return -1;
    }

    return sensor_hal_close(tag, instance);
}


static ssize_t sensor_read(file_t *f, void *buf, size_t len)
{
    int ret;
    sensor_tag_e  tag;
    uint8_t       instance;

    if (f == NULL) {
        return 0;
    }
    if (f->node == NULL) {
        return 0;
    }

    if (buf == NULL) {
        return 0;
    }

    ret = find_selected_sensor(f->node->i_name, &tag, &instance);
    if (unlikely(ret)){
        return 0;
    }

    return sensor_hal_read(tag, instance,buf, len);

}

static ssize_t sensor_write(file_t *f, const void *buf, size_t len)
{
    int ret;
    sensor_tag_e  tag;
    uint8_t       instance;

    if (f == NULL) {
        return 0;
    }
    if (f->node == NULL) {
        return 0;
    }

    if (buf == NULL) {
        return 0;
    }
    ret = find_selected_sensor(f->node->i_name, &tag, &instance);
    if (unlikely(ret)){
        return 0;
    }

    /* no need this functionality recently */
    return sensor_hal_write(tag, instance,buf, len);
}

static int sensor_ioctl(file_t *f, int cmd, unsigned long arg)
{
    int                  ret   = 0;
    sensor_tag_e         tag;
    uint8_t              instance;

    if (f == NULL) {
        LOGD("%s %s fail line: %d\n", SENSOR_STR, __func__, __LINE__);
        return -1;
    }

    if (cmd >= SENSOR_IOCTL_MAX) {
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

    if (f->node == NULL) {
        LOGD("%s %s fail line: %d\n", SENSOR_STR, __func__, __LINE__);
        return -1;
    }

    ret = find_selected_sensor(f->node->i_name, &tag, &instance);
    if (unlikely(ret)){
        return -1;
    }

    ret = sensor_hal_ioctl(tag,instance,(sensor_cmd_type)cmd,arg);
    if (unlikely(ret)){
        return -1;
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

    ret = sensor_hal_init();
    if(unlikely(ret)){
        return -1;
    }

    ret = sensor_hal_register();
    if(unlikely(ret)){
        return -1;
    }

    LOGD(SENSOR_STR, "%s successfully \n", __func__);
    return 0;
}

