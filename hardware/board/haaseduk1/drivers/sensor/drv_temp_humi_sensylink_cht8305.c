/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

/**********************************CHT8305编程说明***************************************/
// CHT8305兼容I2C/SMBus通讯协议，操作简单，读取温湿度数据分三步操作：
// 1）主机向从机地址(有4种可选)发送读0x00寄存器指令，此操作将启动温湿度转换;
//    [指令结构：start+address(ack)+Reg(ack/unack)+stop]
// 2）等待转换完成(15ms);
// 3）主机读取从机数据————00：16bit温度数据;01:16bit湿度数据。两个寄存器数据可以一次性读取。
//    [指令结构：start+address(ack)+T高8位(ack)+T低8位(ack)+H高8位(ack)+H低8位(ack)+stop]
// 特别提醒：1）VCC上电时序要先于或等于I2C上电时序
//         2）从波形上看，每个bit的SDA上升沿和下降沿应充分包裹SCL的上升沿和下降沿
//         3) 建议CHT8305独享一个I2C总线，否则需要保证总线上每个star都对应一个stop；
//         4) 采用外部电阻上拉时，MCU的IO口应配置成开漏模式；
/****************************************************************************************/

#include "drv_temp_humi_sensylink_cht8305.h"
#include "aos/kernel.h"
#include "ulog/ulog.h"
#include <math.h>
#include <stdlib.h>

#define CHT8305_I2C_PORT 1
#define CHT8305_ADDRESS 0x40

void cht8305_init(void)
{
    int32_t ret = sensor_i2c_open(CHT8305_I2C_PORT, CHT8305_ADDRESS, I2C_BUS_BIT_RATES_400K, 0);
    if (ret) {
        printf("sensor i2c open failed, ret:%d\n", ret);
        return;
    }

}
// get temp and humidity
void cht8305_getTempHumidity(float *humidity, float *temperature)
{
    char reg = 0;
    uint8_t buf[4] = {0};
    unsigned int cht8305_temp = 0;
    unsigned int cht8305_humi = 0;

    sensor_i2c_master_send(CHT8305_I2C_PORT, CHT8305_ADDRESS, &reg, 1, 1000);
    aos_msleep(30);
    sensor_i2c_master_recv(CHT8305_I2C_PORT, CHT8305_ADDRESS, buf, 4, 1000);
    // printf("%d-%d-%d-%d\n",buf[0],buf[1],buf[2],buf[3]);

    cht8305_temp = (buf[0] << 8) + buf[1];
    if (cht8305_temp & 0xFFFC) {
        // *temperature = (175.72f * (float)value) / 65536.0f - 46.85f;
        *temperature = (165.0f * (float)cht8305_temp) / 65536.0f - 40.0f;
        LOGD("SENDOR", "temperature: %2f\n", *temperature);
    } else {
        LOGE("SENDOR", "Error on temp\n");
        return 1;
    }

    cht8305_humi = (buf[2] << 8) + buf[3];
    if (cht8305_humi & 0xFFFE) {
        // *humidity = (((float)cht8305_humi) / 65535.0f);
        *humidity = ((125.0f * (float)cht8305_humi) / 65535.0f) - 6.0f;
        LOGD("SENDOR", "humidity: %f\n", *humidity);
    } else {
        LOGE("SENDOR", "Error on humidity\n");
        return 1;
    }
}

void cht8305_deinit(void)
{
    int32_t ret = sensor_i2c_close(CHT8305_I2C_PORT);
    if (ret) {
        LOGE("SENSOR", "sensor i2c close failed, ret:%d\n", ret);
    }

    return;
}
