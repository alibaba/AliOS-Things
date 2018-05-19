/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*
*/

#include "gps_hal.h"

static gps_obj_t *g_gps_obj;
static uint32_t g_gps_sensor_cnt = 0;
static int      g_gps_init_flag = 0;

static int gps_hal_get_dev_list(void* buf)
{
    sensor_list_t* list = buf;
    if(buf == NULL)
        return -1;
    
    if (list->cnt >= TAG_DEV_SENSOR_NUM_MAX){
        LOG("list->cnt == %d    %d\n",list->cnt,TAG_DEV_SENSOR_NUM_MAX);
        return -1;
    }
    
    list->list[list->cnt] = g_gps_obj->tag;
    
    list->cnt ++;

    return 0;
}

  
static int gps_open(inode_t *node, file_t *file)
{
    if((node == NULL)||(file == NULL)){
        return -1;
    }

    /* just open the /dev/nodegps node here */
    if(strncmp(gps_node_path, node->i_name, strlen(node->i_name)) == 0){
        return 0;
    }
    if( g_gps_obj->open == NULL){
        return -1;
    }

    if(g_gps_obj->ref == 0){
        g_gps_obj->open();
    }
    g_gps_obj->ref++;
    
    LOG("%s %s successfully\n", GPS_STR, __func__);
    return 0;
}


static int gps_close(file_t *file)
{
    if(strncmp(gps_node_path, (file->node->i_name), strlen(file->node->i_name)) == 0){
        return 0;
    }    
    
    if( g_gps_obj->close == NULL){
        return -1;
    }
    //if the ref counter is less than 2, then close the sensor
    if(g_gps_obj->ref < 2){
        g_gps_obj->close();
    }
    
    if(g_gps_obj->ref > 0){
        g_gps_obj->ref--;
    }
    return 0;
}



static ssize_t gps_read(file_t *f, void *buf, size_t len)
{
    int ret = 0;
    
    if(f == NULL){
        return -1;
    }
    
    if( g_gps_obj->read == NULL){
        goto error;
    }

    if(buf == NULL){
        goto error;
    }
    
    ret = g_gps_obj->read(buf, len);
    if(ret < 0){
        goto error;
    }
    
    LOG("%s %s successfully\n", GPS_STR, __func__);
    return ret;
    
error:
    return -1;
}

static ssize_t gps_write(file_t *f, const void *buf, size_t len)
{
    /* no need this functionality recently */
    return 0;
}


static int gps_ioctl(file_t *f, int cmd, unsigned long arg)
{
    int ret = 0;
    int index = 0;

    if(f == NULL){
        return -1;
    }
    
    if(cmd == SENSOR_IOCTL_GET_SENSOR_LIST){
        ret = gps_hal_get_dev_list(arg);
        if(ret != 0){
            return -1;
        }
        return 0;
    }
    
    if(g_gps_obj->ioctl == NULL){
        return -1;
    }

    ret = g_gps_obj->ioctl(cmd, arg);
    if(ret != 0){
        return -1;
    }
    LOG("%s %s successfully\n", GPS_STR, __func__);
    return 0;
}

static int  gps_register_irq(void)
{
    // will implement later
    return 0;
}

file_ops_t gps_fops = {
    .open  = gps_open,
    .close = gps_close,
    .read  = gps_read,
    .write = gps_write,
    .ioctl = gps_ioctl,
};


static int gps_obj_register( )
{
    int ret = 0;

    if((g_gps_obj->mode == DEV_INT)){
        ret = gps_register_irq();
        if (ret != 0) {
            return -1;
        }
    }

    ret = aos_register_driver(g_gps_obj->path, &gps_fops, NULL);
    if (ret != 0) {
        return -1;
    }
    return 0;
}    

int gps_create_obj(gps_obj_t* gpsobj)
{   
    int ret = 0;
    
    g_gps_obj = (gps_obj_t*)aos_malloc(sizeof(gps_obj_t));
    if(g_gps_obj == NULL){
        return -1;
    }
    memset(g_gps_obj, 0, sizeof(gps_obj_t));

    /* install the phy sensor info into the sensor object datebase here */
    g_gps_obj->io_port    = gpsobj->io_port;
    g_gps_obj->path       = gpsobj->path;
    g_gps_obj->tag        = gpsobj->tag;
    g_gps_obj->open       = gpsobj->open;
    g_gps_obj->close      = gpsobj->close;
    g_gps_obj->ioctl      = gpsobj->ioctl;
    g_gps_obj->read       = gpsobj->read;
    g_gps_obj->write      = gpsobj->write;
    g_gps_obj->irq_handle = gpsobj->irq_handle;
    g_gps_obj->mode       = gpsobj->mode;
    g_gps_obj->power      = DEV_POWER_OFF; // will update the status later
    g_gps_obj->ref        = 0; // count the ref of this sensor
    /* register the sensor object into the irq list and vfs */
    ret = gps_obj_register();
    if (ret != 0) {
        goto error;
    }
    LOG("%s %s successfully \n", GPS_STR, __func__);
    return 0;

error:
    free(g_gps_obj);
    return -1;
}


static int gps_hal_register(void)
{
    int ret = 0;
    ret = aos_register_driver(gps_node_path, &gps_fops, NULL);
    if (ret != 0) {
        return -1;
    }
    return 0;
}

work_mode_e gps_mode_get(void)
{
    if(0 == g_gps_init_flag){
        LOG("%s : %d\n",__func__,__LINE__);
        return DEV_MODE_INV;
    }
    
    if(NULL == g_gps_obj){
        LOG("%s : %d\n",__func__,__LINE__);
        return DEV_MODE_INV;
    }

    if(g_gps_obj->mode >= DEV_MODE_INV){
        LOG("%s : %d\n",__func__,__LINE__);
        return DEV_MODE_INV;
    }

    return g_gps_obj->mode;
}

int gps_init(void)
{
    int ret   = 0;
    
#ifdef AOS_GPS_SIM868
    ret = gps_simcom_sim868_init();
    if(ret != 0){
        LOG("%s %s fail \n", GPS_STR, __func__);
        return -1;
    }
#endif

    ret = gps_hal_register();
    if(ret != 0){
        
        LOG("%s %s fail \n", GPS_STR, __func__);
        return -1;
    }
    g_gps_init_flag = 1;

    LOG("%s %s successfully\n", GPS_STR, __func__);
    return 0;
}



