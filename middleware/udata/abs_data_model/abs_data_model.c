/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 *
 *
 * abstract(abs) data model
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include "aos/vfs.h"
#include "abs_data_model.h"
#include "service_mgr.h"
#include "udata_queue.h"
#include "udata_parse.h"

#define ABS_QUEUE_MAXSLOTS      8
#define ABS_TASK_STACK_SIZE     1024
#define ABS_TASK_PRIO           (AOS_DEFAULT_APP_PRI - 4)

#define ABS_QUEUE_MAX_MSG_SIZE  (sizeof(sensor_msg_pkg_t))
#define ABS_QUEUE_MAX_MSG_COUNT (16)
#define ABS_QUEUE_SIZE          (ABS_QUEUE_MAX_MSG_SIZE * ABS_QUEUE_MAX_MSG_COUNT)
#define DEFAULT_TIMER_INTERVAL 1000 /* 1000ms */
typedef sensor_list_t abs_data_list_t;

typedef struct _abs_sensor_node_t
{
    sensor_tag_e tag;
    uint8_t      instance;
    int          fd;
    char         path[SENSOR_NAME_LEN];
} abs_sensor_node_t;

aos_queue_t             g_abs_queue;
char                    g_abs_msg[ABS_QUEUE_SIZE] = {0};
aos_task_t              g_abs_task;
static bool            is_timer_work = false;
static int             cur_interval  = 0;
static uint32_t        abs_data_cnt  = 0;
static uint32_t         abs_data_max_num = 0;
static abs_data_pkg_t*  g_abs_data_db[SENSOR_MAX_NUM] = {NULL};
static aos_timer_t     g_abs_data_timer;
static abs_data_list_t g_abs_data_table;

static aos_task_t data_poll_task;

static dev_sensor_config_t  g_sensor_config[SENSOR_MAX_NUM];
/* NOTE: please follow the sensor order of the "sensor_tag_e"
         which is defined the the sensor.h file */
abs_sensor_node_t g_sensor_node[SENSOR_MAX_NUM] = {0};
sensor_node_t  g_sensor_tag_node[] = {
    { TAG_DEV_ACC, dev_acc_path, 0 },   { TAG_DEV_MAG, dev_mag_path, 0 },
    { TAG_DEV_GYRO, dev_gyro_path, 0 }, { TAG_DEV_ALS, dev_als_path, 0 },
    { TAG_DEV_PS, dev_ps_path, 0 },     { TAG_DEV_BARO, dev_baro_path, 0 },
    { TAG_DEV_TEMP, dev_temp_path, 0 }, { TAG_DEV_UV, dev_uv_path, 0 },
    { TAG_DEV_HUMI, dev_humi_path, 0 }, { TAG_DEV_NOISE, dev_noise_path, 0 },
    { TAG_DEV_PM25, dev_pm25_path, 0 }, { TAG_DEV_PM1P0, dev_pm1_path, 0 },
    { TAG_DEV_PM10, dev_pm10_path, 0 }, { TAG_DEV_CO2, dev_co2_path, 0 },
    { TAG_DEV_HCHO, dev_hcho_path, 0 }, { TAG_DEV_TVOC, dev_tvoc_path, 0 },
    { TAG_DEV_PH, dev_ph_path, 0 },     { TAG_DEV_VWC, dev_vwc_path, 0 },
    { TAG_DEV_EC, dev_ec_path, 0 },     { TAG_DEV_SALINITY, dev_salinity_path, 0 },
    { TAG_DEV_TDS, dev_tds_path, 0 },   { TAG_DEV_WINDSPD, dev_windspeed_path, 0 },
    { TAG_DEV_WINDDIR, dev_winddirection_path, 0 },{ TAG_DEV_RAIN, dev_rainfall_path, 0 },
    { TAG_DEV_HALL, dev_hall_path, 0 }, { TAG_DEV_HR, dev_hr_path, 0 },
    { TAG_DEV_RGB, dev_rgb_path, 0 }, { TAG_DEV_GS, dev_gs_path, 0 },
    { TAG_DEV_IR, dev_ir_path, 0 }, { TAG_DEV_GPS, dev_gps_path, 0 },
    { TAG_DEV_RTC, dev_rtc_path, 0},
};


int abs_post_msg(sensor_msg_pkg_t msg)
{
    int ret;
    ret = aos_queue_send(&g_abs_queue, (void *)&msg, sizeof(msg));
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static int abs_data_compute_timer_interval(uint32_t time1, uint32_t time2, uint32_t* result)
{
    if((time1 == 0) || (time2 == 0)){
        return -1;
    }
    if(result == NULL){
        return -1;
    }

    while(time1 != time2){
        if(time1 > time2){
            time1 = time1 - time2;
        }
        else{
            time2 = time2 - time1;
        }
    }

    *result = time1;
    return 0;
}
static int abs_data_set_timer_interval(uint32_t abs_index, uint32_t inerval)
{

    if(abs_index >= abs_data_max_num){
        return -1;
    }

    if(0 == inerval){
        return -1;
    }

    g_abs_data_db[abs_index]->full_info.config.inerval = inerval;

    return 0;
}

bool abs_data_get_timer_status(void)
{
    return is_timer_work;
}

uint32_t abs_data_get_maxmum(void)
{
    return abs_data_max_num;
}
static void abs_data_set_timer_status(bool status)
{
    is_timer_work = status;
}
int abs_data_get_sensor_info(uint32_t abs_index, sensor_tag_e* ptag, uint8_t* pinstance)
{
    if(abs_index >= abs_data_max_num){
        return -1;
    }

    if((ptag == NULL) || (pinstance == NULL)){
        return -1;
    }
    *ptag = g_abs_data_table.list[abs_index].tag;
    *pinstance = g_abs_data_table.list[abs_index].instance;
    return 0;
}

int abs_data_get_abs_index(sensor_tag_e tag, uint8_t instance, uint32_t* pindex)
{
    uint32_t i;
    if(tag >= TAG_DEV_SENSOR_NUM_MAX){
        return -1;
    }
    if(pindex == NULL){
        return -1;
    }
    for(i = 0; i < abs_data_max_num; i++){
        if((g_abs_data_table.list[i].instance == instance) && (g_abs_data_table.list[i].tag == tag)){
            *pindex = i;
            return 0;
        }
    }
    return -1;
}

static int abs_data_create_obj_ctx(int index, int arg, void *pdata)
{
    int ret = 0;
    sensor_tag_e tag;
    uint8_t      instance;

    if(index >= abs_data_max_num){
        return -1;
    }
    if (pdata == NULL){
        return -1;
    }
    g_abs_data_db[index] =
      (abs_data_pkg_t *)aos_malloc(sizeof(abs_data_pkg_t));
    if (g_abs_data_db[index] == NULL) {
        return -1;
    }
    memset(g_abs_data_db[index], 0, (sizeof(abs_data_pkg_t)));

    ret = abs_data_get_sensor_info(index, &tag, &instance);
    if(unlikely(ret)){
        goto error;
    }

    udata_service_t *service                            = pdata;
    g_abs_data_db[index]->tag                           = tag;
    g_abs_data_db[index]->instance                      = instance;
    g_abs_data_db[index]->interval                      = arg;
    g_abs_data_db[index]->poweron                       = true;
    g_abs_data_db[index]->cur_timestamp                 = aos_now_ms();
    g_abs_data_db[index]->full_info.config.inerval      = service->config.inerval;
    g_abs_data_db[index]->full_info.config.range        = service->config.range;
    g_abs_data_db[index]->calibrated_algo_process_cb    = NULL;
    g_abs_data_db[index]->srv_cnt++;

    abs_data_cnt++;
    return 0;
error:
    if(g_abs_data_db[index] != NULL){
        aos_free(g_abs_data_db[index]);
        g_abs_data_db[index] = NULL;
    }
    return -1;
}


void abs_sensor_read(uint32_t       abs_index)
{
    int      ret;
    void*    data = NULL;
    uint32_t len  = 0;

    ret = udata_service_get_payload(abs_index, &data, &len);
    if (unlikely(ret)) {
        return;
    }

    //memset(addr, 0, len);
    ret = abs_data_read(abs_index, data, len);
    if (ret <= 0) {
        return;
    }
}

static void abs_data_post_timer_expried_dev(uint64_t timestamp)
{
    uint8_t     i = 0;
    int         ret;
    work_mode_e mode;
    sensor_msg_pkg_t data_msg;
    for (i = 0; i < abs_data_max_num; i++) {
        if ((g_abs_data_db[i] == NULL) || (g_abs_data_db[i]->poweron != true)){
            continue;
        }
        if ((g_abs_data_table.list[i].io_port == MODBUS_PORT) || (g_abs_data_table.list[i].io_port == CAN_PORT)){
            continue;
        }
        mode = abs_sensor_mode_get(i);
        if (DEV_POLLING != mode) {
            continue;
        }

        if ((timestamp - g_abs_data_db[i]->cur_timestamp) >=
            (g_abs_data_db[i]->interval)) {
            memset(&data_msg, 0, sizeof(data_msg));
            data_msg.cmd   = UDATA_MSG_DEV_READ;
            data_msg.index = i;
            ret            = abs_post_msg(data_msg);
            if (unlikely(ret)) {
                return;
            }
            g_abs_data_db[i]->cur_timestamp = timestamp;
        }
    }
}

int abs_data_dev_enable(uint32_t abs_index)
{

    return 0;
}

int abs_data_dev_disable(uint32_t abs_index)
{

    return 0;
}

static int abs_data_collect_dev_list(void *buf)
{
    int fd  = 0;
    int ret = 0;
    if (buf == NULL) {
        return -1;
    }

#ifdef AOS_SENSOR
    fd = aos_open(sensor_node_path, O_RDWR);
    if (fd < 0) {
        return -1;
    }

    ret = aos_ioctl(fd, SENSOR_IOCTL_GET_SENSOR_LIST, (unsigned long)buf);
    if (ret < 0) {
        return -1;
    }
    ret = aos_close(fd);
    if (ret < 0) {
        return -1;
    }
#endif

    return 0;
}

bool abs_data_check_dev_tree(uint32_t abs_index)
{
    if(abs_index < abs_data_max_num){
        return true;
    }
    return false;
}

int abs_data_timer_start(void)
{
    int ret;
    if (is_timer_work == true)
        return 0;
    ret = aos_timer_start(&g_abs_data_timer);
    if (unlikely(ret)) {
        return -1;
    }
    is_timer_work = true;
    return 0;
}

int abs_data_timer_stop(void)
{
    int ret;
    if (is_timer_work == false)
        return 0;
    ret = aos_timer_stop(&g_abs_data_timer);
    if (unlikely(ret)) {
        return -1;
    }
    is_timer_work = false;
    return 0;
}

#ifndef UDATA_MODBUS
static int abs_data_timer_update(uint32_t abs_index, int interval)
{
    int ret = 0;
    uint32_t value = 0;
    if (interval <= 0) {
        return -1;
    }

    /* no need change the interval in this case here */
    ret = abs_data_compute_timer_interval(cur_interval,interval,&value);
    if (unlikely(ret)) {
        return -1;
    }

    /* fill the timer info inot the interval lists of timer from sensor service
    side, set the timer of abs sensor model by the min interval */
    if (true == abs_data_get_timer_status()) {
        ret = abs_data_timer_stop();
        if (unlikely(ret)) {
            return -1;
        }
        ret = aos_timer_change(&g_abs_data_timer, (int)value);
        if (unlikely(ret)) {
            return -1;
        }
        ret = abs_data_timer_start();
        if (unlikely(ret)) {
            return -1;
        }
    } else {
        ret = aos_timer_change(&g_abs_data_timer,  (int)value);
        if (unlikely(ret)) {
            return -1;
        }
    }
    cur_interval = value;
    return 0;
}

#endif

static void abs_data_polling(void *p)
{
    uint8_t i = 0;

    uint64_t timestamp     = 0;
    uint64_t pre_timestamp = 0;

    while (1) {
        aos_msleep(200);
        timestamp = aos_now_ms();
        for (i = 0; i < abs_data_max_num; i++) {
            if ((g_abs_data_db[i] == NULL) || (g_abs_data_db[i]->poweron != true)){
                continue;
            }
            if ((g_abs_data_table.list[i].io_port != MODBUS_PORT) && (g_abs_data_table.list[i].io_port != CAN_PORT)){
                continue;
            }
            if ((timestamp - g_abs_data_db[i]->cur_timestamp) >=
                g_abs_data_db[i]->interval) {
                if ((aos_now_ms() - pre_timestamp) < 150)
                    aos_msleep(150);
                abs_sensor_read(i);
                timestamp                       = aos_now_ms();
                pre_timestamp                   = timestamp;
                g_abs_data_db[i]->cur_timestamp = timestamp;
            }
        }
    }
}

int abs_data_poll_start(void)
{
    LOG("%s abs data poll task start\n", uDATA_STR);
    aos_task_new_ext(&data_poll_task, "abs_data_poll", abs_data_polling, NULL,
                     1024 * 2, AOS_DEFAULT_APP_PRI - 1);
    return 0;
}
void *abs_sensor_buff_addr_get(uint32_t abs_index)
{
    if (abs_index >= abs_data_max_num) {
        return NULL;
    }

    return g_sensor_config[abs_index].data_buf;
}

uint32_t abs_sensor_buff_len_get(uint32_t abs_index)
{
    if (abs_index >= abs_data_max_num) {
        return 0;
    }

    return g_sensor_config[abs_index].data_len;
}

work_mode_e abs_sensor_mode_get(uint32_t abs_index)
{
    if (abs_index >= abs_data_max_num) {
        return DEV_MODE_INVALID;
    }

    return g_sensor_config[abs_index].mode;
}

void abs_sensor_irq_callback(sensor_tag_e tag, uint8_t instance)
{
    int ret;
    uint32_t abs_index;
    sensor_msg_pkg_t data_msg;

    ret = abs_data_get_abs_index(tag,instance,&abs_index);
    if(unlikely(ret)){
        return;
    }

    memset(&data_msg, 0, sizeof(data_msg));
    data_msg.cmd   = UDATA_MSG_DEV_READ;
    data_msg.index = abs_index;
    ret            = udata_post_msg(data_msg);
    if (unlikely(ret)) {
        LOG("abs_sensor_irq_callback post msg fail ret = %d index = %d\n", ret,
            (int)abs_index);
    }

}

int abs_sensor_irq_callback_reg(uint32_t abs_index, SENSOR_IRQ_CALLBACK cb)
{
    int                    ret;
    dev_sensor_full_info_t sensor_temp;

    if (abs_index >= abs_data_max_num) {
        return -1;
    }

    if (cb == NULL) {
        return -1;
    }

    memset(&sensor_temp, 0, sizeof(sensor_temp));
    sensor_temp.config.id           = SENSOR_IOCTL_SET_SENSOR_IRQ_CB;
    sensor_temp.config.irq_callback = abs_sensor_irq_callback;

    ret = abs_data_ioctl(abs_index, &sensor_temp);
    if (unlikely(ret)) {
        return -1;
    }

    return 0;
}

static int abs_data_timer_config(uint32_t abs_index, udata_service_t *service)
{
    int                     ret      = 0;
    uint32_t                interval = 0;
    int                     fd       = 0;

    if (service == NULL) {
        LOG("service NULL\n");
        return -1;
    }

    if(abs_index >= abs_data_max_num){
        return -1;
    }

    if(!UDATA_BITMAP_COMPARE(service->abs_bitmap, abs_index))
    {
       return -1;
    }
    fd = g_sensor_node[abs_index].fd;

    if ((g_abs_data_db[abs_index]->poweron != true)) {
        ret = aos_ioctl(fd, SENSOR_IOCTL_SET_POWER, true);
        if (unlikely(ret)) {
            return -1;
        }
        g_abs_data_db[abs_index]->poweron = true;
    }

    ret = abs_data_compute_timer_interval((g_abs_data_db[abs_index]->interval),(service->interval[abs_index]),&interval);
    if(unlikely(ret)){
        return -1;
    }

    if(service->interval[abs_index] == interval){
        return 0;
    }

    if (service->config.id == SENSOR_IOCTL_ODR_SET) {
        ret = aos_ioctl(fd, service->config.id, interval);
        if (unlikely(ret)) {
            LOG("aos_ioctl cmd %d\n",SENSOR_IOCTL_ODR_SET);
        }
    }

    if (service->config.id == SENSOR_IOCTL_RANGE_SET) {
        ret = aos_ioctl(fd, service->config.id, service->config.range);
        if (unlikely(ret)) {
            LOG("aos_ioctl cmd %d\n",SENSOR_IOCTL_RANGE_SET);
        }
    }

    ret = abs_data_set_timer_interval(abs_index, interval);
    if (unlikely(ret)) {
        return -1;
    }

#ifndef UDATA_MODBUS
    if (g_sensor_config[abs_index].mode == DEV_POLLING) {
        ret      = abs_data_timer_update(abs_index, interval);
        if (unlikely(ret)) {
            return -1;
        }
    }
#endif

    return 0;
}


static int abs_data_create_obj(uint32_t abs_index, udata_service_t *service)
{
    int                     ret      = 0;
    uint32_t                interval = 0;
    int                     fd;
    dev_sensor_full_info_t  info;
    dev_sensor_config_t     *sensor;

    if (service == NULL) {
        LOG("service NULL\n");
        return -1;
    }
    if(abs_index >= abs_data_max_num){
        return -1;
    }
    if(!UDATA_BITMAP_COMPARE(service->abs_bitmap, abs_index))
    {
        return -1;
    }
    fd = g_sensor_node[abs_index].fd;
    interval = service->interval[abs_index];

    if (service->config.id == SENSOR_IOCTL_ODR_SET) {
        ret = aos_ioctl(fd, service->config.id, interval);
        if (unlikely(ret)) {
            LOG("aos_ioctl cmd %d\n",SENSOR_IOCTL_ODR_SET);
        }
    }

    if (service->config.id == SENSOR_IOCTL_RANGE_SET) {
        ret = aos_ioctl(fd, service->config.id, service->config.range);
        if (unlikely(ret)) {
            LOG("aos_ioctl cmd %d\n",SENSOR_IOCTL_RANGE_SET);
        }
    }

    memset(&info, 0, sizeof(info));
    info.config.id = SENSOR_IOCTL_GET_SENSOR_MODE;

    ret = abs_data_ioctl(abs_index, &info);
    if (0 != ret) {
        return -1;
    }

    sensor           = &g_sensor_config[abs_index];
    sensor->mode     = info.config.mode;
    sensor->data_len = info.config.data_len;

    if(sensor->mode >= DEV_MODE_INVALID){
        return -1;
    }

#ifndef UDATA_MODBUS
    if (sensor->mode == DEV_POLLING) {
        ret      = abs_data_timer_update(abs_index, interval);
        if (unlikely(ret)) {
            return -1;
        }
    }
#endif

    if ((sensor->mode == DEV_INT) ||
               (sensor->mode == DEV_DATA_READY) ||
               (sensor->mode == DEV_FIFO)) {

        if (sensor->mode == DEV_FIFO) {
            if(sensor->data_buf == NULL){
                if (info.config.data_len == 0) {
                    return -1;
                }

                sensor->data_buf = aos_malloc(info.config.data_len);
                if (sensor->data_buf == NULL) {
                    return -1;
                }
            }
        }

        ret = abs_sensor_irq_callback_reg(abs_index, abs_sensor_irq_callback);
        if (unlikely(ret)) {
            if((sensor->mode == DEV_FIFO) && (sensor->data_buf != NULL)){
                aos_free(sensor->data_buf);
                sensor->data_buf = NULL;
            }

            return -1;
        }

    }
    ret = abs_data_create_obj_ctx(abs_index, interval,  service);
    if (unlikely(ret)) {
        if((sensor->mode == DEV_FIFO) && (sensor->data_buf != NULL)){
            aos_free(sensor->data_buf);
            sensor->data_buf = NULL;
        }
        LOG("abs_data_create_obj_ctx error\n");
        return -1;
    }

    return 0;
}

int abs_data_open(udata_service_t *service)
{
    int                     ret      = 0;
    int                     fd       = 0;
    uint32_t                index    = 0;

    if (service == NULL) {
        LOG("service NULL\n");
        return -1;
    }
    /* check the target in the dev tree */

    for(index = 0; index < abs_data_max_num; index++){
        if(!UDATA_BITMAP_COMPARE(service->abs_bitmap, index))
        {
           continue;
        }
        if(0 == g_sensor_node[index].fd){
            fd = aos_open((g_sensor_node[index].path), O_RDWR);
            if (fd <= 0) {
                LOG(" aos_open index %d error\n", (int)index);
                return -1;
            }

            g_sensor_node[index].fd = fd;
        }

        if(g_abs_data_db[index] != NULL){
            ret = abs_data_timer_config(index, service);
            if(unlikely(ret)){
                return -1;
            }
            else{
                continue;
            }
        }

        ret = abs_data_create_obj(index, service);
        if(unlikely(ret)){
            return -1;
        }
        continue;

    }
    LOG("%s %s successfully\n", uDATA_STR, __func__);
    return 0;
}

int abs_data_close(uint32_t abs_index)
{
    int      ret   = 0;
    uint32_t index = 0;

    if(abs_index >= abs_data_max_num){
        return -1;
    }
    if (g_sensor_node[abs_index].fd <= 0) {
        return -1;
    }

    ret = aos_ioctl(g_sensor_node[abs_index].fd, SENSOR_IOCTL_SET_POWER, false);
    if (ret < 0) {
        return -1;
    }
    g_sensor_node[abs_index].fd = 0;
    g_abs_data_db[abs_index]->poweron = false;
    for (index = 0; index < abs_data_cnt; index++) {
        if (g_abs_data_db[index]->poweron == true) {
            break;
        }
        abs_data_timer_stop();
    }
    return 0;
}

int abs_data_read_ext(uint32_t abs_index, void *pdata)
{
    ssize_t size = 0;

    if(abs_index >= abs_data_max_num){
        return -1;
    }

    if(pdata == NULL){
        return -1;
    }

    size         = aos_read(g_sensor_node[abs_index].fd, pdata, DATA_SIZE);
    if (size <= 0) {
        LOG("%s %s %s %d\n", uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }

    return (int)size;
}

int abs_data_read(uint32_t abs_index, void *pdata, uint32_t nbyte)
{
    int    ret   = 0;
    int    size  = 0;
    sensor_msg_pkg_t data_msg;

    if(abs_index >= abs_data_max_num){
        return -1;
    }

    if(pdata == NULL){
        return -1;
    }

    /* read the physical sensor data by posix way */
    size = aos_read(g_sensor_node[abs_index].fd, pdata, nbyte);
    if (size <= 0) {
        LOG("%s %s %s %d\n", uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }

    /* check if calibrated aglo registed base on this sensor.
         yes for invoking the callback here */
    if ((g_abs_data_db[abs_index] != NULL) &&
        (g_abs_data_db[abs_index]->calibrated_algo_process_cb != NULL)) {
        g_abs_data_db[abs_index]->calibrated_algo_process_cb(pdata, size);
    }
    data_msg.cmd   = UDATA_MSG_SERVICE_PROCESS;
    data_msg.index = abs_index;
    ret            = udata_post_msg(data_msg);
    if (unlikely(ret)) {
        return -1;
    }

    return (int)size;
}

int abs_data_ioctl(uint32_t abs_index, void *config)
{
    int                     ret           = 0;
    unsigned long           arg           = 0;
    dev_sensor_full_info_t *sensor_config = config;

    if(abs_index >= abs_data_max_num){
        return -1;
    }

    if(sensor_config == NULL){
        return -1;
    }

    if (sensor_config->config.id == SENSOR_IOCTL_ODR_SET) {
        arg = sensor_config->config.inerval;
    } else if (sensor_config->config.id == SENSOR_IOCTL_RANGE_SET) {
        arg = sensor_config->config.range;
    } else if (sensor_config->config.id == SENSOR_IOCTL_GET_INFO) {
        /* fill the dev info addr, then get back after filling by the phy sensor
         */
        arg = (unsigned long)(&sensor_config->info);
    } else if (sensor_config->config.id == SENSOR_IOCTL_GET_SENSOR_LIST) {
        /* should open the sensor hale node to get all the senor list */
        // ret = abs_data_get_dev_list(&(sensor_config->info.list));
        // if(unlikely(ret)){
        // LOG("%s %s %s %d\n",  uDATA_STR, __func__, ERROR_LINE, __LINE__);
        //    return -1;
        //}
        // return 0;
    } else if (sensor_config->config.id == SENSOR_IOCTL_GET_SENSOR_MODE) {
        arg = (unsigned long)(&sensor_config->config);
    } else if (sensor_config->config.id == SENSOR_IOCTL_SET_SENSOR_IRQ_CB) {
        arg = (unsigned long)(&sensor_config->config);
    } else {
        arg = (unsigned long)(&sensor_config->info);
    }

    ret = aos_ioctl(g_sensor_node[abs_index].fd, sensor_config->config.id, arg);
    if (ret < 0) {
        LOG(" %s %s %d\n", __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    return 0;
}

int abs_cali_data_register(uint32_t abs_index, void *cb)
{
    if (cb == NULL) {
        return -1;
    }
    if (abs_index >= abs_data_max_num) {
        return -1;
    }

    if (g_abs_data_db[abs_index] == NULL) {
        return -1;
    }
    g_abs_data_db[abs_index]->calibrated_algo_process_cb = cb;

    return 0;
}

int abs_cali_data_unregister(uint32_t abs_index)
{
    // TODO;
    return 0;
}

static void abs_data_timer_process(void *arg1, void *arg2)
{
    abs_data_post_timer_expried_dev(aos_now_ms());
}

int abs_data_cali_init(void)
{
    int ret = 0;
    ret     = cali_example_example_init();
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", uDATA_STR, __func__);
    return 0;
}

static void abs_msg_dispatcher(void *arg)
{

    int  ret = 0;
    char data[256];
    uint32_t          size = 0;
    sensor_msg_pkg_t *msg  = NULL;

    while (DO_FOREVER) {
        size = 0;
        ret = aos_queue_recv(&g_abs_queue, AOS_WAIT_FOREVER, (void *)data, (unsigned int *)(&size));
        if ((ret != 0) || (size == 0)){
            continue;
        }

        msg = (sensor_msg_pkg_t *)data;
        switch(msg->cmd){
            case UDATA_MSG_DEV_READ:
                abs_sensor_read(msg->index);
                break;

            default:
                break;
        }
    }
}

int abs_read_task_creat(void)
{
    int ret;
    ret = aos_queue_new(&g_abs_queue, (void *)&g_abs_msg, ABS_QUEUE_SIZE, ABS_QUEUE_MAX_MSG_SIZE);
    if (unlikely(ret)) {
        return -1;
    }

    ret = aos_task_new_ext(&g_abs_task, "abs_read_task", abs_msg_dispatcher, NULL, ABS_TASK_STACK_SIZE, ABS_TASK_PRIO);
    if (unlikely(ret)) {
        return -1;
    }

    return 0;
}

int abs_data_node_init(void)
{
    int i;

    for(i = 0; i < abs_data_max_num; i++){
        g_sensor_node[i].instance  = g_abs_data_table.list[i].instance;
        g_sensor_node[i].tag       = g_abs_data_table.list[i].tag;
        g_sensor_node[i].fd        = 0;
        snprintf(g_sensor_node[i].path,SENSOR_NAME_LEN,"%s/%d",g_sensor_tag_node[g_sensor_node[i].tag].path,g_sensor_node[i].instance);
    }

    return 0;
}


int abs_data_model_init(void)
{
    int ret = 0;
    int i;
    ret = aos_timer_new_ext(&g_abs_data_timer, abs_data_timer_process, NULL,
                            DEFAULT_TIMER_INTERVAL, 0, 0);

    /* must stop the timer here, since the  timer is triggered after be created
     */
    if (unlikely(ret)) {
        LOG(" aos_timer_new_ext error\n");
        return -1;
    }
    abs_data_set_timer_status(false);

    memset(g_abs_data_db, 0, SENSOR_MAX_NUM * sizeof(abs_data_pkg_t*));
    memset(g_sensor_node, 0, SENSOR_MAX_NUM * sizeof(sensor_node_t));
    memset(g_sensor_config, 0, SENSOR_MAX_NUM * sizeof(dev_sensor_config_t));
    memset(&g_abs_data_table, 0, sizeof(g_abs_data_table));
    cur_interval = DEFAULT_TIMER_INTERVAL;

    ret = abs_data_collect_dev_list(&g_abs_data_table);
    if (unlikely(ret)) {
        return -1;
    }
    abs_data_max_num = g_abs_data_table.cnt;
    if(0 == abs_data_max_num){
        return -1;
    }

    for (i = 0; i < abs_data_max_num; i++) {
        g_sensor_config[i].mode = DEV_MODE_INVALID;
    }
    ret = abs_data_node_init();
    if (unlikely(ret)) {
        return -1;
    }

#ifdef UDATA_MODBUS
    abs_data_poll_start();
#else
    ret = abs_data_timer_start();
    if (ret != 0) {
        LOG("error occur abs_data_timer_start \n");
        return -1;
    }

    ret = abs_read_task_creat();
    if (unlikely(ret)) {
        LOG("error occur abs_read_task_creat\n");
        return -1;
    }
#endif

    return 0;
}
