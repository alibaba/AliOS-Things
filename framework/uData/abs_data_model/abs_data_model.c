/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*
* abstract(abs) data model
*
*
*/

#include "abs_data_model.h"
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <aos/yloop.h>

typedef sensor_list_t abs_data_list_t;
#define DEFAULT_TIMER_INTERVAL          1000  /* 1000ms */
static bool is_timer_work     = false;
static int cur_interval       = 0;
static uint32_t abs_data_cnt  = 0;
static abs_data_pkg_t* g_abs_data_db[ABS_DATA_MAX_CNT];
static aos_timer_t g_abs_data_timer;
static abs_data_list_t g_abs_data_table;

/* NOTE: please follow the sensor order of the "sensor_tag_e"
         which is defined the the sensor.h file */
sensor_node_t g_sensor_node[] = {
    { TAG_DEV_ACC,    dev_acc_path,   0},
    { TAG_DEV_MAG,    dev_mag_path,   0},
    { TAG_DEV_GYRO,   dev_gyro_path,  0},
    { TAG_DEV_ALS,    dev_als_path,   0},
    { TAG_DEV_PS,     dev_ps_path,    0},
    { TAG_DEV_BARO,   dev_baro_path,  0},
    { TAG_DEV_TEMP,   dev_temp_path,  0},
    { TAG_DEV_UV,     dev_uv_path,    0},
    { TAG_DEV_HUMI,   dev_humi_path,  0},
    { TAG_DEV_HALL,   dev_hall_path,  0},
    { TAG_DEV_HR,     dev_hr_path,    0},
    { TAG_DEV_GPS,    dev_gps_path,   0},
};

static bool abs_data_get_timer_status(void)
{
    return is_timer_work;
}

static void abs_data_set_timer_status(bool status)
{
    is_timer_work = status;
}

static int abs_data_create_obj_ctx(int arg, void* pdata)
{
    g_abs_data_db[abs_data_cnt] = (abs_data_pkg_t*)aos_malloc(sizeof(abs_data_pkg_t));
    if((pdata == NULL)||(g_abs_data_db[abs_data_cnt] == NULL)){
        return -1;
    }
    memset(g_abs_data_db[abs_data_cnt], 0, (sizeof(abs_data_pkg_t)));

    uData_service_t* service = pdata;
    g_abs_data_db[abs_data_cnt]->tag = service->tag;
    g_abs_data_db[abs_data_cnt]->interval = arg;
    g_abs_data_db[abs_data_cnt]->poweron = true;
    g_abs_data_db[abs_data_cnt]->cur_timestamp = aos_now_ms();
    g_abs_data_db[abs_data_cnt]->full_info.config.odr = service->config.odr;
    g_abs_data_db[abs_data_cnt]->full_info.config.range = service->config.range;
    g_abs_data_db[abs_data_cnt]->calibrated_algo_process_cb = NULL;
    g_abs_data_db[abs_data_cnt]->srv_cnt++;
    abs_data_cnt++;
    
    return 0;
}

static int abs_data_get_obj_index(sensor_tag_e tag){
    int i = 0;
    for(i = 0; i< abs_data_cnt; i++){
        if(g_abs_data_db[i]->tag == tag){
            return i;
        }
    }
    return -1;
}

static void abs_data_post_timer_expried_dev(uint64_t timestamp)
{
    uint8_t i = 0;
    for(i = 0; i < abs_data_cnt; i++){
        if((timestamp - g_abs_data_db[i]->cur_timestamp) >= (g_abs_data_db[i]->interval)){
            aos_post_event(EV_UDATA, CODE_UDATA_DEV_READ, (g_abs_data_db[i]->tag));
            /* update the timestamp of triggered sensor here for the next timer hadler */
            g_abs_data_db[i]->cur_timestamp = timestamp;
        }
    }

}

int abs_data_dev_enable(sensor_tag_e tag)
{
    int ret = 0;

    
    if(abs_data_get_timer_status()){
        return 0;
    }

    ret = aos_timer_start(&g_abs_data_timer);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

int abs_data_dev_disable(sensor_tag_e tag)
{
    int ret = 0;
    if(!abs_data_get_timer_status()){
        return 0;
    }
    ret = aos_timer_stop(&g_abs_data_timer);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int abs_data_biuld_dev_tree(void* buf)
{
    abs_data_list_t* table = buf;
    if(table == NULL){
        return -1;
    }

    g_abs_data_table.cnt = table->cnt;
    for(int i = 0; i < g_abs_data_table.cnt; i++){
        g_abs_data_table.list[i] =  table->list[i];
    }
}


static int abs_data_collect_dev_list(void* buf)
{
    int fd = 0;
    int ret = 0;
    if(buf == NULL){
        return -1;
    }

#ifdef AOS_SENSOR
    fd = aos_open(sensor_node_path, O_RDWR);
    if(fd < 0){
        return -1;
    }
    
    ret = aos_ioctl(fd, SENSOR_IOCTL_GET_SENSOR_LIST, buf);
    if(ret < 0){
        return -1;
    }
    ret = aos_close(fd);
    if(ret < 0){
        return -1;
    }

#endif

#ifdef AOS_GPS
    fd = aos_open(gps_node_path, O_RDWR);
    if(fd < 0){
        return -1;
    }

    ret = aos_ioctl(fd, SENSOR_IOCTL_GET_SENSOR_LIST, buf);
    if(ret < 0){
        return -1;
    }

    ret = aos_close(fd);
    if(ret < 0){
        return -1;
    }
#endif

    return 0;
}

static bool abs_data_check_dev_tree(sensor_tag_e tag)
{
    for(int i = 0; i < g_abs_data_table.cnt; i++){
        if(g_abs_data_table.list[i] ==  tag){
            return true;
        }
    }
    return false;
}

static int abs_data_timer_update(sensor_tag_e tag, int interval)
{
    int ret = 0;

    if(interval <= 0){
        return -1;
    }

    /* no need change the interval in this case here */
    if(interval > cur_interval){
        return 0;
    }
    /* fill the timer info inot the interval lists of timer from sensor service side,
    set the timer of abs sensor model by the min interval */
    
    ret = aos_timer_stop(&g_abs_data_timer);
    if(unlikely(ret)){
        return -1;
    }
    ret = aos_timer_change(&g_abs_data_timer, interval);
    if(unlikely(ret)){
        return -1;
    }

    ret = aos_timer_start(&g_abs_data_timer);
    if(unlikely(ret)){
        return -1;
    }
    
    abs_data_set_timer_status(true);
    cur_interval = interval;
    return 0;
}

int abs_data_open(uData_service_t *service)
{
    int ret = 0;
    int interval = 0;
    int fd = 0;
    int index = 0;

    if(service == NULL){
        return -1;
    }

    /* check the target in the dev tree */
    if(!(abs_data_check_dev_tree(service->tag))){
        return -1;
    }

    interval = HZ_2_INTERVAL(service->config.odr);
    ret = abs_data_timer_update(service->tag, interval);
    if(unlikely(ret)){
        return -1;
    }

    /* chekc the device if it already exists here */
    index = abs_data_get_obj_index(service->tag);
    if(index >= 0){
        /* no operation if the device is already open and power on */
        if((g_abs_data_db[index]->poweron == 1)){
            return 0;
        }
        else{
            /* just power on the device if the device is already open but power off */
            ret = aos_ioctl(g_sensor_node[service->tag].fd, SENSOR_IOCTL_SET_POWER, true);
            if(ret <= 0){
                return -1;
            }
            g_abs_data_db[index]->poweron = 1;
            return 0;
        }
    }

    fd = aos_open((g_sensor_node[service->tag].path), O_RDWR);
    if(fd <= 0){
        return -1;
    }

    g_sensor_node[service->tag].fd = fd;

    /* set the configuration para from the service algo */
    if((service->config.id & SENSOR_IOCTL_ODR_SET) != 0){
        ret = aos_ioctl(fd,service->config.id, service->config.odr);
        if(ret < 0){
            return -1;
        }
    }

    if((service->config.id & SENSOR_IOCTL_RANGE_SET) != 0){
        ret = aos_ioctl(fd,service->config.id, service->config.range);
        if(ret < 0){
            return -1;
        }
    }

    ret = abs_data_create_obj_ctx(interval, service);
    if(unlikely(ret)){
        return -1;
    }
    LOG("%s %s successfully\n", uDATA_STR, __func__);
    return 0;
}

int abs_data_close(sensor_tag_e tag)
{
    int ret      = 0;
    uint32_t index = 0;
    if(g_sensor_node[tag].fd <= 0){
        return -1;
    }
    ret = aos_ioctl(g_sensor_node[tag].fd, SENSOR_IOCTL_SET_POWER, false);
    if(ret < 0){
        return -1;
    }

    /*
    ret = aos_close(g_sensor_node[tag].fd);
    if(unlikely(ret)){
        return -1;
    }
    */
    index = abs_data_get_obj_index(tag);
    g_abs_data_db[index]->poweron = false;
    for(index = 0; index < abs_data_cnt; index++){
       if(g_abs_data_db[index]->poweron == true){
            break;
       }
       if(abs_data_get_timer_status() != false){
            aos_timer_stop(&g_abs_data_timer);
       }
    }
    return 0;
}

int abs_data_read(sensor_tag_e tag, void* pdata, uint32_t nbyte)
{
    int ret =  0;
    size_t size = 0;
    int index = 0;
    /* read the physical sensor data by posix way */
    size = aos_read(g_sensor_node[tag].fd, pdata, nbyte);
    if(size <= 0){
        LOG("%s %s %s %d\n", uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    
    /* check if calibrated aglo registed base on this sensor.
         yes for invoking the callback here */
    index = abs_data_get_obj_index(tag);
    if((g_abs_data_db[index] != NULL)&&(g_abs_data_db[index]->calibrated_algo_process_cb != NULL)){
       g_abs_data_db[index]->calibrated_algo_process_cb(pdata); 
    }

    ret = aos_post_event(EV_UDATA, CODE_UDATA_SERVICE_PROCESS, tag);
    if(ret < 0){
        return -1;
    }

    LOG("%s %s successfully \n", uDATA_STR, __func__);
    return (int)size;
}

int abs_data_ioctl(sensor_tag_e tag, void* config)
{
    int ret = 0;
    unsigned long arg = 0;
    dev_sensor_full_info_t* sensor_config = config;

    if(sensor_config->config.id == SENSOR_IOCTL_ODR_SET){
        arg = sensor_config->config.odr;
    }
    else if(sensor_config->config.id == SENSOR_IOCTL_RANGE_SET){
        arg = sensor_config->config.range;
    }
    else if(sensor_config->config.id == SENSOR_IOCTL_GET_INFO){
        /* fill the dev info addr, then get back after filling by the phy sensor */
        arg = &sensor_config->info;
    }
    else if(sensor_config->config.id == SENSOR_IOCTL_GET_SENSOR_LIST){
        /* should open the sensor hale node to get all the senor list */
        //ret = abs_data_get_dev_list(&(sensor_config->info.list));
        //if(unlikely(ret)){
            //LOG("%s %s %s %d\n",  uDATA_STR, __func__, ERROR_LINE, __LINE__);
        //    return -1;
        //}
        //return 0;
    }

    ret = aos_ioctl(g_sensor_node[tag].fd, sensor_config->config.id, arg);
    if(ret <= 0){
        return -1;
    }
    return 0;
}

int abs_cali_data_register(sensor_tag_e tag, void* cb)
{
    int ret = 0;
    int index = 0;

    if(cb == NULL){
        return -1;
    }

    /* also can copy the data struct  here */
    index = abs_data_get_obj_index(tag);
    if(g_abs_data_db[index] == NULL){
        return -1;
    }
    g_abs_data_db[index]->calibrated_algo_process_cb = cb;

    return 0;
}

int abs_cali_data_unregister(udata_type_e type)
{
    // TODO;
}

static void abs_data_timer_process(void* arg)
{
    abs_data_post_timer_expried_dev(aos_now_ms());
}

int abs_data_cali_init(void){
    int ret = 0;
    ret = cali_example_example_init();
    if(unlikely(ret)){
        return -1;
    }
   
    LOG("%s %s successfully \n", uDATA_STR, __func__);
    return 0;
}

int abs_data_model_init(void)
{
    int ret = 0;

    ret = aos_timer_new_ext(&g_abs_data_timer, abs_data_timer_process, NULL, DEFAULT_TIMER_INTERVAL, 0, 0);
    
     /* must stop the timer here, since the  timer is triggered after be created */
    if(unlikely(ret)){
        return -1;
    }

    abs_data_set_timer_status(false);
    cur_interval  = DEFAULT_TIMER_INTERVAL;

    memset(&g_abs_data_table, 0, sizeof(g_abs_data_table));
    ret = abs_data_collect_dev_list(&g_abs_data_table);
    if(unlikely(ret)){
        return -1;
    }
    /*
    ret = abs_data_biuld_dev_tree(&g_abs_data_table);
    if(unlikely(ret)){
        return -1;
    }
    */
   
    LOG("%s %s successfully \n", uDATA_STR, __func__);
    return 0;
}
