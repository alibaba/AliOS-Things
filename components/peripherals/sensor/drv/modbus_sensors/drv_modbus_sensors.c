#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos/kernel.h"
#include "network/hal/base.h"
#include "udata/udata.h"
#include "sensor_drv_conf.h"
#include "mbmaster_api.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif
#define MODBUS_CONFIG_STACK_SIZE (256 * 4)
#define MODBUS_UART_PORT 2
#define MODBUS_UART_BAUDRATE 9600

mb_handler_t* modbus_handler = NULL;

// extern const modbus_sensor_t modbus_sensors[];

#define DATA_PACK_2B(x, buff) \
    {                         \
        x = buff[0];          \
        x <<= 8;              \
        x |= buff[1];         \
    }
#define DATA_PACK_2B_REVERSE(x, buff) \
    {                                 \
        x = buff[1];                  \
        x <<= 8;                      \
        x |= buff[0];                 \
    }
#define DATA_PACK_4B(x, buff)                 \
    {                                         \
        unsigned int __temp = 0;              \
        __temp              = buff[0];        \
        x                   = (__temp << 24); \
        __temp              = buff[1];        \
        x |= (__temp << 16);                  \
        __temp = buff[2];                     \
        x |= (__temp << 8);                   \
        __temp = buff[3];                     \
        x |= __temp;                          \
    }
#define DATA_PACK_4B_REVERSE(x, buff)                         \
    {                                                         \
        unsigned int __temp_reverse = 0;                      \
        __temp_reverse              = buff[3];                \
        x                           = (__temp_reverse << 24); \
        __temp_reverse              = buff[2];                \
        x |= (__temp_reverse << 16);                          \
        __temp_reverse = buff[1];                             \
        x |= (__temp_reverse << 8);                           \
        __temp_reverse = buff[0];                             \
        x |= __temp_reverse;                                  \
    }


/* support read up to 8 bytes register */
#define SNS_VALUE_BYTES_MAX 8


static int modbus_sensor_open(void)
{
    return 0;
}

static int modbus_sensor_close(void)
{
    return 0;
}

int modbus_sensor_read(void *buf, size_t len)
{
    if(modbus_handler == NULL|| buf == NULL){
        return -1;
    }
    int modbus_index = *(int *)buf;
    
    integer_data_t *pdata = (integer_data_t *)buf;

    modbus_sensor_t *sns = &modbus_sensors[modbus_index];

    if (sns->reg_cnt > SNS_VALUE_BYTES_MAX) {
        LOG("%s: register length invalid !\n", __func__);
        return -EINVAL;
    }
    mb_status_t status;

    unsigned int fdata   = 0;
    int          datalen = 2 * sns->reg_cnt;
    // unsigned char data[datalen];
    unsigned char data[10];
    unsigned int  respond_cnt = 0;

    status = mbmaster_read_holding_reginster(modbus_handler,sns->slave, sns->addr, sns->reg_cnt,
                                       data, (uint8_t*)&respond_cnt);
    if (status != MB_SUCCESS) {
        LOG("mb_read_holding_reginster error status:0x%x.", status);
        return -1;
    }

    if (sns->byte_reverse == 0) {
        for (int m = 0; m < datalen; m++)
            fdata += (data[m] << ((datalen - m - 1) * 8));
    } else {
        for (int m = 0; m < datalen; m++)
            fdata += (data[m] << (m * 8));
    }

    pdata->data      = fdata;
    pdata->timestamp = aos_now_ms();
    LOG(" modbus_sensor_read start data=%d\n", pdata->data);

    return sizeof(integer_data_t);
}


static int modbus_sensor_ioctl(int cmd, unsigned long arg)
{
    switch (cmd) {
        default:
            break;
    }

    return 0;

}

int modbus_sensor_init(modbus_sensor_t modbus_sensor,uint8_t drv_index)
{
    sensor_obj_t sensor;
    sensor_tag_e tag = modbus_sensor.tag;

    memset(&sensor, 0, sizeof(sensor_obj_t));

    sensor.io_port = MODBUS_PORT;
    sensor.tag     = tag;
    sensor.instance = modbus_sensor.instance;
    sensor.open    = modbus_sensor_open;
    sensor.close   = modbus_sensor_close;
    sensor.path    = modbus_sensor.path;
    sensor.read    = modbus_sensor_read;
    sensor.ioctl   = modbus_sensor_ioctl;
    sensor.mode    = DEV_POLLING;
    sensor.data_len = sizeof(integer_data_t);
    sensor.write       = NULL;
    sensor.irq_handle  =  NULL;
    sensor.drv_index   = drv_index;
    if (sensor_create_obj(&sensor)) {
        LOG("%s create modbus sensor %d obj failed !\n", SENSOR_STR, tag);
        return -ENODEV;
    }
    printf("yftest sensor.index=%d\n",sensor.drv_index);
    LOG("%s modbus sensor %d init success\n", SENSOR_STR, tag);
    return 0;
}


void modbus_sensor_drv_init()
{
    for (int i = 0; i < ARRAY_SIZE(modbus_sensors); i++) {
        if (modbus_sensors[i].ability == SENSOR_OPEN)
            modbus_sensor_init(modbus_sensors[i],i);
    }
}


int find_ModbusSensors(char *path)
{
    int i = 0;

    if (path == NULL) {
        return -1;
    }

    for (i = 0; i < ARRAY_SIZE(modbus_sensors); i++) {
        if (modbus_sensors[i].ability == SENSOR_CLOSE)
            continue;
        if (strncmp(modbus_sensors[i].path, path, strlen(path)) == 0) {
            return i;
        }
    }
    return -1;
}


int modbus_init(void)
{

    mb_status_t status = mbmaster_rtu_init(&modbus_handler,MODBUS_UART_PORT,MODBUS_UART_BAUDRATE,MB_PAR_NONE);
    if(status != MB_SUCCESS)
    {        
        LOG("mbmaster_rtu_init  error STATUS=%d\n",status);
        return -1;
    }
    modbus_sensor_drv_init();
  
    
    LOG("modbus initialized\n");
    return 0;
}

