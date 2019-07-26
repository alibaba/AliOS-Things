/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <unistd.h>

#include "aos/kernel.h"
#include "udata/udata.h"

#include "yts/yunit.h"



#define UDATA_SUCCES    0

static int udata_sensor_open(inode_t *node, file_t *file);
static int udata_sensor_close(file_t *file);
static ssize_t udata_sensor_read(file_t *f, void *buf, size_t len);
static ssize_t udata_sensor_write(file_t *f, const void *buf, size_t len);
static int udata_sensor_ioctl(file_t *f, int cmd, unsigned long arg);

file_ops_t udata_sensor_fops = {
    .open  = udata_sensor_open,
    .close = udata_sensor_close,
    .read  = udata_sensor_read,
    .write = udata_sensor_write,
    .ioctl = udata_sensor_ioctl,
};

static sensor_obj_t *g_sensor_obj[TAG_DEV_SENSOR_NUM_MAX];
static uint32_t g_sensor_cnt = 0;

static int drv_virtual_sensor_open(void)
{
    return 0;

}

static int drv_virtual_sensor_close(void)
{
    return 0;
}


static int drv_virtual_sensor_read(void *buf, size_t len)
{
    return 0;
}

static int drv_virtual_sensor_ioctl(int cmd, unsigned long arg)
{
    return 0;
}

int drv_virtual_sensor_init(void){
    int ret = 0;
    sensor_obj_t sensor;

    /* fill the sensor obj parameters here */
    sensor.tag = TAG_DEV_BARO;
    sensor.path = dev_baro_path;
    sensor.io_port = I2C_PORT;
    sensor.open = drv_virtual_sensor_open;
    sensor.close = drv_virtual_sensor_close;
    sensor.read = drv_virtual_sensor_read;
    sensor.write = NULL;
    sensor.ioctl = drv_virtual_sensor_ioctl;
    sensor.irq_handle = NULL;

    ret = udata_sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


static int udata_find_selected_sensor(char* path )
{
    int i = 0;

    if(path == NULL){
        return -1;
    }

    for(i = 0; i < g_sensor_cnt; i++){
        if(strncmp(g_sensor_obj[i]->path, path, strlen(path)) == 0){
            return i;
        }
    }
    return -1;
}
static int udata_load_sensor_config(int index )
{
    int ret = 0;
    g_sensor_obj[index] = (sensor_obj_t*)aos_malloc(sizeof(sensor_obj_t));
    if(g_sensor_obj[index] == NULL){
        return -1;
    }

    return 0;
}

static int udata_sensor_obj_register(int index )
{
    int ret = 0;
    ret = aos_register_driver(g_sensor_obj[index]->path, &udata_sensor_fops, NULL);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int udata_sensor_create_obj(sensor_obj_t* sensor)
{
    int ret = 0;

    g_sensor_obj[g_sensor_cnt] = (sensor_obj_t*)aos_malloc(sizeof(sensor_obj_t));
    if(g_sensor_obj[g_sensor_cnt] == NULL){
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
    g_sensor_obj[g_sensor_cnt]->bus        = sensor->bus;
    g_sensor_obj[g_sensor_cnt]->power      = DEV_POWER_OFF; // will update the status later

    /* register the sensor object into the irq list and vfs */
    ret = udata_sensor_obj_register(g_sensor_cnt);
    if (ret != 0) {
        goto error;
    }

    g_sensor_cnt++;
    return 0;

error:
    free(g_sensor_obj[g_sensor_cnt]);
    return -1;
}

static int udata_sensor_hal_get_dev_list(void* buf)
{
    sensor_list_t* list = buf;
    if(buf == NULL)
        return -1;

    /* load the sensor count and tag list here */
    list->cnt = g_sensor_cnt;
    for(int index = 0; index < g_sensor_cnt; index++){
        list->list[index] = g_sensor_obj[index]->tag;
    }

    return 0;
}

static int udata_sensor_open(inode_t *node, file_t *file)
{
    int index = 0;

    if((node == NULL)||(file == NULL)){
        return -1;
    }


    /* just open the /dev/sensor node here */
    if(strncmp(sensor_node_path, node->i_name, strlen(node->i_name)) == 0){
        return 0;
    }

    index = udata_find_selected_sensor(node->i_name);
    if(( g_sensor_obj[index]->open == NULL)||(index < 0)){
        return -1;
    }

    g_sensor_obj[index]->open();
    return 0;
}

static int udata_sensor_close(file_t *file)
{
    int index = 0;

    /* just close the /dev/sensor node here */
    if(strncmp(sensor_node_path, (file->node->i_name), strlen(file->node->i_name)) == 0){
        return 0;
    }

    index = udata_find_selected_sensor(file->node->i_name);
    if(( g_sensor_obj[index]->close == NULL)||(index < 0)){
        return -1;
    }
    g_sensor_obj[index]->close();
    return 0;
}

static ssize_t udata_sensor_read(file_t *f, void *buf, size_t len)
{
    int index = 0;
    int ret = 0;

    if(f == NULL){
        return -1;
    }

    index = udata_find_selected_sensor(f->node->i_name);
    if(( g_sensor_obj[index]->read == NULL)||(index < 0)){
        goto error;
    }

    if(buf == NULL){
        goto error;
    }

    ret = g_sensor_obj[index]->read(buf, len);
    if(ret != 0){
        goto error;
    }

    return len;

error:
    return -1;
}

static ssize_t udata_sensor_write(file_t *f, const void *buf, size_t len)
{
    /* no need this functionality recently */
    return 0;
}

static int udata_sensor_ioctl(file_t *f, int cmd, unsigned long arg)
{
    int ret = 0;
    int index = 0;

    if(f == NULL){
        return -1;
    }

    if(cmd == SENSOR_IOCTL_GET_SENSOR_LIST){
        ret = udata_sensor_hal_get_dev_list(arg);
        if(ret != 0){
            return -1;
        }
        return 0;
    }

    index = udata_find_selected_sensor(f->node->i_name);
    if(( g_sensor_obj[index]->ioctl == NULL)||(index < 0)){
        return -1;
    }

    ret = g_sensor_obj[index]->ioctl(cmd, arg);
    if(ret != 0){
        return -1;
    }
    return 0;
}

static int udata_sensor_hal_register(void)
{
    int ret = 0;
    ret = aos_register_driver(sensor_node_path, &udata_sensor_fops, NULL);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int udata_sensor_init(void){
    int ret   = 0;
    int index = 0;
    g_sensor_cnt = 0 ;

    drv_virtual_sensor_init();

    ret = udata_sensor_hal_register();
    if(ret != 0){
        return -1;
    }

    return 0;
}

static void test_udata_subscribe_case(void)
{
    int ret = 0;
    udata_sensor_init();
    udata_init();
    ret = udata_subscribe(UDATA_SERVICE_BARO);
    YUNIT_ASSERT(ret == UDATA_SUCCES);
}

static void test_udata_unsubscribe_case(void)
{
    int ret = 0;
    ret = udata_unsubscribe(UDATA_SERVICE_BARO);
    YUNIT_ASSERT(ret == UDATA_SUCCES);
}

static yunit_test_case_t aos_udata_testcases[] = {
    { "subscribe", test_udata_subscribe_case },
    { "unsubscribe", test_udata_unsubscribe_case },
    //{ "get list", test_udata_getlist_case },
    //{ "open", test_udata_open_case},
    //{ "close", test_udata_close_case},
    //{ "ioctl", test_udata_ioctl_case},
    YUNIT_TEST_CASE_NULL
};

static int init(void)
{
    return 0;
}

static int cleanup(void)
{
    return 0;
}

static void setup(void)
{
}

static void teardown(void)
{
}

static yunit_test_suite_t suites[] = {
    { "uData", init, cleanup, setup, teardown, aos_udata_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_uData(void)
{
    yunit_add_test_suites(suites);
}

AOS_TESTCASE(test_uData);


