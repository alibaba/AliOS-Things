/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 *
 *
 * abstract(abs) data model
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <aos/yloop.h>
#include "sensor.h"
#include "abs_data_model.h"
#include "service_mgr.h"
#include "uData_queue.h"

typedef sensor_list_t abs_data_list_t;
#define DEFAULT_TIMER_INTERVAL 1000 /* 1000ms */
static bool            is_timer_work = false;
static int             cur_interval  = 0;
static uint32_t        abs_data_cnt  = 0;
static abs_data_pkg_t *g_abs_data_db[ABS_DATA_MAX_CNT];
static aos_timer_t     g_abs_data_timer;
static abs_data_list_t g_abs_data_table;

static aos_task_t data_poll_task;

static dev_sensor_full_info_t g_sensor_full_info[ABS_DATA_MAX_CNT];
/* NOTE: please follow the sensor order of the "sensor_tag_e"
         which is defined the the sensor.h file */
sensor_node_t g_sensor_node[] = {
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
};

extern int cali_example_example_init(void);

static bool abs_data_get_timer_status(void)
{
    return is_timer_work;
}

static void abs_data_set_timer_status(bool status)
{
    is_timer_work = status;
}

static int abs_data_create_obj_ctx(int arg, void *pdata)
{
    g_abs_data_db[abs_data_cnt] =
      (abs_data_pkg_t *)aos_malloc(sizeof(abs_data_pkg_t));
    if ((pdata == NULL) || (g_abs_data_db[abs_data_cnt] == NULL)) {
        return -1;
    }
    memset(g_abs_data_db[abs_data_cnt], 0, (sizeof(abs_data_pkg_t)));

    uData_service_t *service                            = pdata;
    g_abs_data_db[abs_data_cnt]->tag                    = service->tag;
    g_abs_data_db[abs_data_cnt]->interval               = arg;
    g_abs_data_db[abs_data_cnt]->poweron                = true;
    g_abs_data_db[abs_data_cnt]->cur_timestamp          = aos_now_ms();
    g_abs_data_db[abs_data_cnt]->full_info.config.odr   = service->config.odr;
    g_abs_data_db[abs_data_cnt]->full_info.config.range = service->config.range;
    g_abs_data_db[abs_data_cnt]->calibrated_algo_process_cb = NULL;
    g_abs_data_db[abs_data_cnt]->srv_cnt++;
    abs_data_cnt++;

    return 0;
}

static int abs_data_get_obj_index(sensor_tag_e tag)
{
    int i = 0;
    for (i = 0; i < abs_data_cnt; i++) {
        if (g_abs_data_db[i]->tag == tag) {
            return i;
        }
    }
    return -1;
}

void abs_sensor_read(sensor_tag_e tag)
{
    int      ret;
    void *   addr = NULL;
    uint32_t len  = 0;

    ret = uData_service_get_payload(tag, &addr, &len);
    if (unlikely(ret)) {
        return;
    }

    memset(addr, 0, len);
    ret = abs_data_read(tag, addr, len);
    if (ret <= 0) {
        return;
    }
}

static void abs_data_post_timer_expried_dev(uint64_t timestamp)
{
    uint8_t     i = 0;
    work_mode_e mode;
    for (i = 0; i < abs_data_cnt; i++) {
        mode = abs_sensor_mode_get(g_abs_data_db[i]->tag);
        if (DEV_POLLING != mode) {
            continue;
        }
        if ((timestamp - g_abs_data_db[i]->cur_timestamp) >=
            (g_abs_data_db[i]->interval)) {

            // aos_post_event(EV_UDATA, CODE_UDATA_DEV_READ,
            // (g_abs_data_db[i]->tag));
            abs_sensor_read(g_abs_data_db[i]->tag);
            /* update the timestamp of triggered sensor here for the next timer
             * hadler */
            g_abs_data_db[i]->cur_timestamp = timestamp;
        }
    }
}

int abs_data_dev_enable(sensor_tag_e tag)
{

    return 0;
}

int abs_data_dev_disable(sensor_tag_e tag)
{

    return 0;
}

static int abs_data_build_dev_tree(void *buf)
{
    abs_data_list_t *table = buf;
    if (table == NULL) {
        return -1;
    }

    g_abs_data_table.cnt = table->cnt;
    for (int i = 0; i < g_abs_data_table.cnt; i++) {
        g_abs_data_table.list[i] = table->list[i];
    }
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

#ifdef AOS_GPS
    fd = aos_open(gps_node_path, O_RDWR);
    if (fd < 0) {
        return -1;
    }

    ret = aos_close(fd);
    if (ret < 0) {
        return -1;
    }
#endif

    return 0;
}

bool abs_data_check_dev_tree(sensor_tag_e tag)
{
    for (int i = 0; i < g_abs_data_table.cnt; i++) {
        if (g_abs_data_table.list[i] == tag) {
            return true;
        }
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

static int abs_data_timer_update(sensor_tag_e tag, int interval)
{
    int ret = 0;

    if (interval <= 0) {
        return -1;
    }

    /* no need change the interval in this case here */
    if (interval > cur_interval) {
        return 0;
    }
    /* fill the timer info inot the interval lists of timer from sensor service
    side, set the timer of abs sensor model by the min interval */
    if (true == abs_data_get_timer_status()) {
        ret = abs_data_timer_stop();
        if (unlikely(ret)) {
            return -1;
        }
        ret = aos_timer_change(&g_abs_data_timer, interval);
        if (unlikely(ret)) {
            return -1;
        }
        ret = abs_data_timer_start();
        if (unlikely(ret)) {
            return -1;
        }
    } else {
        ret = aos_timer_change(&g_abs_data_timer, interval);
        if (unlikely(ret)) {
            return -1;
        }
    }

    cur_interval = interval;
    return 0;
}


static void abs_data_polling(void *p)
{
    uint8_t i = 0;

    uint64_t timestamp     = 0;
    uint64_t pre_timestamp = 0;

    while (1) {
        aos_msleep(200);
        timestamp = aos_now_ms();
        for (i = 0; i < abs_data_cnt; i++) {
            if ((timestamp - g_abs_data_db[i]->cur_timestamp) >=
                g_abs_data_db[i]->interval) {
                if ((aos_now_ms() - pre_timestamp) < 150)
                    aos_msleep(150);
                abs_sensor_read(g_abs_data_db[i]->tag);
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
void *abs_sensor_buff_addr_get(sensor_tag_e tag)
{
    if (tag >= TAG_DEV_SENSOR_NUM_MAX) {
        return NULL;
    }

    return g_sensor_full_info[tag].config.data_buf;
}

uint32_t abs_sensor_buff_len_get(sensor_tag_e tag)
{
    if (tag >= TAG_DEV_SENSOR_NUM_MAX) {
        return 0;
    }

    return g_sensor_full_info[tag].config.data_len;
}

work_mode_e abs_sensor_mode_get(sensor_tag_e tag)
{
    if (tag >= TAG_DEV_SENSOR_NUM_MAX) {
        return DEV_MODE_INVALID;
    }

    return g_sensor_full_info[tag].config.mode;
}

void abs_sensor_irq_callback(sensor_tag_e tag)
{
    int ret;
    if (tag >= TAG_DEV_SENSOR_NUM_MAX) {
        return;
    }

#ifdef UDATA_YLOOP
    ret = aos_post_event(EV_UDATA, CODE_UDATA_DEV_READ, tag);
    if (unlikely(ret)) {
        LOG("abs_sensor_irq_callback post event fail ret = %d tag = %d\n", ret,
            tag);
    }
#else
    sensor_msg_pkg_t data_msg;
    memset(&data_msg, 0, sizeof(data_msg));
    data_msg.cmd   = UDATA_MSG_DEV_READ;
    data_msg.value = tag;
    ret            = uData_post_msg(data_msg);
    if (unlikely(ret)) {
        LOG("abs_sensor_irq_callback post msg fail ret = %d tag = %d\n", ret,
            tag);
    }
#endif
}

int abs_sensor_irq_callback_reg(sensor_tag_e tag, SENSOR_IRQ_CALLBACK cb)
{
    int                    ret;
    dev_sensor_full_info_t sensor_temp;

    if (tag >= TAG_DEV_SENSOR_NUM_MAX) {
        return -1;
    }

    if (cb == NULL) {
        return -1;
    }

    memset(&sensor_temp, 0, sizeof(sensor_temp));
    sensor_temp.config.id           = SENSOR_IOCTL_SET_SENSOR_IRQ_CB;
    sensor_temp.config.irq_callback = abs_sensor_irq_callback;

    ret = abs_data_ioctl(tag, &sensor_temp);
    if (unlikely(ret)) {
        return -1;
    }

    return 0;
}
int abs_data_open(uData_service_t *service)
{
    int                     ret      = 0;
    int                     interval = 0;
    int                     fd       = 0;
    int                     index    = 0;
    dev_sensor_full_info_t  sensor_temp;
    dev_sensor_full_info_t *sensor;


    if (service == NULL) {
        LOG("service NULL\n");
        return -1;
    }

    /* check the target in the dev tree */
    if (!(abs_data_check_dev_tree(service->tag))) {
        return -1;
    }

#ifdef UDATA_MODBUS
    interval = service->config.odr;
#else
    interval = HZ_2_INTERVAL(service->config.odr);
    ret      = abs_data_timer_update(service->tag, interval);
    if (unlikely(ret)) {
        LOG("abs_data_timer_update error\n");
        return -1;
    }
#endif
    /* chekc the device if it already exists here */
    index = abs_data_get_obj_index(service->tag);
    if ((index >= 0) && (index < ABS_DATA_MAX_CNT)){
        /* no operation if the device is already open and power on */
        if ((g_abs_data_db[index]->poweron == 1)) {
            return 0;
        } else {
            /* just power on the device if the device is already open but power
             * off */
            ret = aos_ioctl(g_sensor_node[service->tag].fd,
                            SENSOR_IOCTL_SET_POWER, true);
            if (unlikely(ret)) {
                return -1;
            }
            g_abs_data_db[index]->poweron = 1;
            return 0;
        }
    }

    fd = aos_open((g_sensor_node[service->tag].path), O_RDWR);
    if (fd <= 0) {
        LOG(" aos_open tag %d error\n", service->tag);
        return -1;
    }

    g_sensor_node[service->tag].fd = fd;

    /* set the configuration para from the service algo */
    if ((service->config.id & SENSOR_IOCTL_ODR_SET) != 0) {
        ret = aos_ioctl(fd, service->config.id, service->config.odr);
        if (ret < 0) {
            return -1;
        }
    }

    if ((service->config.id & SENSOR_IOCTL_RANGE_SET) != 0) {
        ret = aos_ioctl(fd, service->config.id, service->config.range);
        if (ret < 0) {
            return -1;
        }
    }

#ifdef UDATA_MODBUS
    interval = service->config.odr;
#else

    memset(&sensor_temp, 0, sizeof(sensor_temp));
    sensor_temp.config.id = SENSOR_IOCTL_GET_SENSOR_MODE;

    ret = abs_data_ioctl(service->tag, &sensor_temp);
    if (0 != ret) {
        return -1;
    }

    sensor                  = &g_sensor_full_info[service->tag];
    sensor->config.mode     = sensor_temp.config.mode;
    sensor->config.data_len = sensor_temp.config.data_len;

    if (sensor->config.mode == DEV_POLLING) {
        interval = HZ_2_INTERVAL(service->config.odr);
        ret      = abs_data_timer_update(service->tag, interval);
        if (unlikely(ret)) {
            return -1;
        }
    } else if ((sensor->config.mode == DEV_INT) ||
               (sensor->config.mode == DEV_DATA_READY) ||
               (sensor->config.mode == DEV_FIFO)) {
        if (sensor_temp.config.data_len == 0) {
            return -1;
        }

        if (sensor->config.mode == DEV_FIFO) {
            sensor->config.data_buf = malloc(sensor_temp.config.data_len);
            if (sensor->config.data_buf == NULL) {
                return -1;
            }
        }

        ret =
          abs_sensor_irq_callback_reg(service->tag, abs_sensor_irq_callback);
        if (unlikely(ret)) {
            return -1;
        }

    } else {
        return -1;
    }

#endif

    ret = abs_data_create_obj_ctx(interval, service);
    if (unlikely(ret)) {
        LOG("abs_data_create_obj_ctx error\n");
        return -1;
    }
    LOG("%s %s successfully\n", uDATA_STR, __func__);
    return 0;
}

int abs_data_close(sensor_tag_e tag)
{
    int      ret   = 0;
    uint32_t index = 0;
    if (g_sensor_node[tag].fd <= 0) {
        return -1;
    }

    ret = aos_ioctl(g_sensor_node[tag].fd, SENSOR_IOCTL_SET_POWER, false);
    if (ret < 0) {
        return -1;
    }

    /*
    ret = aos_close(g_sensor_node[tag].fd);
    if(unlikely(ret)){
        return -1;
    }
    */
    index                         = abs_data_get_obj_index(tag);
    if ((index < 0) || (index >= ABS_DATA_MAX_CNT)){
        return -1;
    }
    g_abs_data_db[index]->poweron = false;
    for (index = 0; index < abs_data_cnt; index++) {
        if (g_abs_data_db[index]->poweron == true) {
            break;
        }
        abs_data_timer_stop();
    }
    return 0;
}

int abs_data_read_ext(sensor_tag_e tag, void *pdata)
{
    ssize_t size = 0;
    size         = aos_read(g_sensor_node[tag].fd, pdata, DATA_SIZE);
    if (size <= 0) {
        LOG("%s %s %s %d\n", uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }

    return (int)size;
}

int abs_data_read(sensor_tag_e tag, void *pdata, uint32_t nbyte)
{
    int    ret   = 0;
    size_t size  = 0;
    int    index = 0;
    /* read the physical sensor data by posix way */
    size = aos_read(g_sensor_node[tag].fd, pdata, nbyte);
    if (size <= 0) {
        LOG("%s %s %s %d\n", uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }

    /* check if calibrated aglo registed base on this sensor.
         yes for invoking the callback here */
    index = abs_data_get_obj_index(tag);
    if ((index < 0) || (index >= ABS_DATA_MAX_CNT)){
        return -1;
    }
    
    if ((g_abs_data_db[index] != NULL) &&
        (g_abs_data_db[index]->calibrated_algo_process_cb != NULL)) {
        g_abs_data_db[index]->calibrated_algo_process_cb(pdata);
    }

#ifdef UDATA_YLOOP
    ret = aos_post_event(EV_UDATA, CODE_UDATA_SERVICE_PROCESS, tag);
    if (ret < 0) {
        return -1;
    }
#else
    sensor_msg_pkg_t data_msg;
    memset(&data_msg, 0, sizeof(data_msg));
    data_msg.cmd   = UDATA_MSG_SERVICE_PROCESS;
    data_msg.value = tag;
    ret            = uData_post_msg(data_msg);
    if (unlikely(ret)) {
        return -1;
    }
#endif

    return (int)size;
}

int abs_data_ioctl(sensor_tag_e tag, void *config)
{
    int                     ret           = 0;
    unsigned long           arg           = 0;
    dev_sensor_full_info_t *sensor_config = config;

    if (sensor_config->config.id == SENSOR_IOCTL_ODR_SET) {
        arg = sensor_config->config.odr;
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

    ret = aos_ioctl(g_sensor_node[tag].fd, sensor_config->config.id, arg);
    if (ret < 0) {
        LOG(" %s %s %d\n", __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    return 0;
}

int abs_cali_data_register(sensor_tag_e tag, void *cb)
{
    int index = 0;

    if (cb == NULL) {
        return -1;
    }

    /* also can copy the data struct  here */
    index = abs_data_get_obj_index(tag);
    if ((index < 0) || (index >= ABS_DATA_MAX_CNT)){
        return -1;
    }
    if (g_abs_data_db[index] == NULL) {
        return -1;
    }
    g_abs_data_db[index]->calibrated_algo_process_cb = cb;

    return 0;
}

int abs_cali_data_unregister(udata_type_e type)
{
    // TODO;
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

int abs_data_model_init(void)
{
    int ret = 0;
    int i;

    memset((void *)(&g_sensor_full_info[0]), 0,
           ABS_DATA_MAX_CNT * sizeof(dev_sensor_full_info_t));

    for (i = 0; i < ABS_DATA_MAX_CNT; i++) {
        g_sensor_full_info[i].config.mode = DEV_MODE_INVALID;
    }

    ret = aos_timer_new_ext(&g_abs_data_timer, abs_data_timer_process, NULL,
                            DEFAULT_TIMER_INTERVAL, 0, 0);

    /* must stop the timer here, since the  timer is triggered after be created
     */
    if (unlikely(ret)) {
        LOG(" aos_timer_new_ext error\n");
        return -1;
    }

    abs_data_set_timer_status(false);
    cur_interval = DEFAULT_TIMER_INTERVAL;

    memset(&g_abs_data_table, 0, sizeof(g_abs_data_table));
    ret = abs_data_collect_dev_list(&g_abs_data_table);
    if (unlikely(ret)) {
        return -1;
    }
    /*
    ret = abs_data_build_dev_tree(&g_abs_data_table);
    if(unlikely(ret)){
        return -1;
    }
    */
#ifdef UDATA_MODBUS
    abs_data_poll_start();
#else
    ret = abs_data_timer_start();
    if (ret != 0) {
        LOG("error occur abs_data_timer_start \n");
        return ret;
    }
#endif

    return 0;
}
