/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "board.h"
#include <k_api.h>
#include "aos/cli.h"
#include "ulog/ulog.h"

#include "led.h"
#include "key.h"

#include "hal_oled.h"

#include "drv_temp_humi_si_si7006.h"
#include "drv_temp_humi_sensylink_cht8305.h"

#include "drv_acc_gyro_inv_mpu6050.h"
#include "drv_acc_gyro_qst_qmi8610.h"

#include "drv_baro_goertek_spl06.h"
#include "drv_baro_qst_qmp6988.h"

#include "drv_mag_qst_qmc5883l.h"
#include "drv_mag_qst_qmc6310.h"

#include "drv_als_ps_ir_liteon_ap3216c.h"

typedef int (*sensortest_cb)(int turn);
static int sensors_test_inited = 0;
static int humiture_test(uint8_t turn);
static int gyro_test(uint8_t turn);
static int mag_test(uint8_t turn);
static int baro_test(uint8_t turn);
static int ap3216c_test(uint8_t turn);

static int g_haasboard_is_k1c = 0;

sensortest_cb sensor_test_funs[] = {
    humiture_test,
    gyro_test,
    mag_test,
    baro_test,
    ap3216c_test,
};
extern int usb_class_detect;

static int humiture_test(uint8_t turn)
{
    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("******************* Sensor HUMITURE Test **********************\r\n");
    printf("** How to test: ***********************************************\r\n");
    printf("***************************************************************\r\n");
    printf("***************************************************************\r\n");
    printf("\r\n === test start====\r\n");
    uint8_t id_buf[8] = {0};
    float Temp = 0.0, Humidity = 0.0;
    LOGI("SENSOR_HUMI_TEST", "test start, turn %d\n", turn);
    if (g_haasboard_is_k1c) {
        cht8305_init();
    } else {
        si7006_init();
    }
    aos_msleep(200);

    while (turn--) {
        if (g_haasboard_is_k1c) {
            cht8305_getTempHumidity(&Humidity, &Temp);
            LOGI("humiture_test", "Temp :%f Humidity : %f\n", Temp, Humidity);
            if ((Humidity > 300.0 || Humidity < -100.0) || (Temp > 200.0 || Temp < -50.0)) {
                cht8305_deinit();
                goto _failed;
            }
            printf("===Result : SENSOR HUMI TEST PASS !!! ===\r\n");
            cht8305_deinit();
            return 0;
        } else {
            // get data
            si7006_getTempHumidity(&Humidity, &Temp);
            LOGI("humiture_test", "Temp :%f Humidity : %f\n", Temp, Humidity);
            if ((Humidity > 300.0 || Humidity < -100.0) || (Temp > 200.0 || Temp < -50.0)) {
                si7006_deinit();
                goto _failed;
            }
            printf("===Result : SENSOR HUMI TEST PASS !!! ===\r\n");
            si7006_deinit();
            return 0;
        }
    }

_failed:
    printf("===Result: SENSOR HUMI TEST FAIL !!! ===\r\n");
    return -1;
}

static int gyro_test(uint8_t turn)
{
    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("************************ GYRO Test ****************************\r\n");
    printf("** How to test: ***********************************************\r\n");
    printf("***************************************************************\r\n");
    printf("***************************************************************\r\n");
    printf("\r\n ===GYRO test start====\r\n");
    short gx = 0, gy = 0, gz = 0;
    short ax = 0, ay = 0, az = 0;
    float acc[3];
    LOGI("GYRO_TEST", "test start, turn %d\n", turn);
    if (g_haasboard_is_k1c) {
        FisImu_init();
        LOGI("GYRO_TEST", "FisImu_init done\n");
    } else {
        MPU_Init();
        LOGI("GYRO_TEST", "MPU_Init done\n");
    }
    aos_msleep(200);

    while (turn--) {
        if (g_haasboard_is_k1c) {
            FisImu_read_acc_xyz(acc);

            if (acc[0] > 4000.0 || acc[0] < -4000.0) {
                printf("===Result: GYRO test FAIL !!! ===\r\n");
                FisImu_deinit();
                return -1;
            }
            FisImu_deinit();
        } else {
            float temp = MPU_Get_Temperature();
            MPU_Deinit();
            LOGI("GYRO_TEST", "temp %0.1f\n", temp);
            if (temp / 100 > 200.0 || temp / 100 < -50.0) {
                printf("===Result: GYRO test FAIL !!! ===\r\n");
                return -1;
            }
        }
    }
    printf("===Result : GYRO test PASS !!! ===\r\n");
    return 0;
}

static int baro_test(uint8_t turn)
{
    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("*********************** BARO Test ****************************\r\n");
    printf("** How to test: ***********************************************\r\n");
    printf("***************************************************************\r\n");
    printf("***************************************************************\r\n");
    printf("\r\n ===BARAO test start====\r\n");
    spl06_data_t spl06_data = {0};
    qmp6988_data p_qmp6988_data = {0};

    LOGI("BARO_TEST", "test start, turn %d\n", turn);
    if (g_haasboard_is_k1c) {
        p_qmp6988_data.slave = QMP6988_SLAVE_ADDRESS_H;
        qmp6988_init(&p_qmp6988_data);
        LOGI("BARO_TEST", "qmp6988_init done\n");
    } else {
        spl06_init();
        LOGI("BARO_TEST", "spl06_init done\n");
    }
    aos_msleep(200);
    while (turn--) {
        if (g_haasboard_is_k1c) {
            qmp6988_calc_pressure(&p_qmp6988_data);
            printf("Temperature: C %0.1f\n", p_qmp6988_data.temperature);
            printf("Temperature: F %0.1f\n", ((p_qmp6988_data.temperature * 9 / 5) + 32));
            printf("Measured Air Pressure: : %0.2f mb\n", p_qmp6988_data.pressure);
            printf("altitude: %0.2f m\n", p_qmp6988_data.altitude);
            if (p_qmp6988_data.pressure / 100.0 > 3000.0 || p_qmp6988_data.pressure / 100.0 < 500.0) {
                printf("===Result: BARO_TEST test FAIL !!! ===\r\n");
                qmp6988_deinit();
                return -1;
            }
            qmp6988_deinit();
        } else {
            spl06_getdata(&spl06_data);
            printf("Temperature: C %0.1f\n", spl06_data.Ctemp);
            printf("Temperature: F %0.1f\n", spl06_data.Ftemp);
            printf("Measured Air Pressure: : %0.2f mb\n", spl06_data.pressure);
            printf("altitude: %0.2f m\n", spl06_data.altitude);
            // printf("altitude: %0.2f ft\n", spl06_data.altitude * 3.281);
            if (spl06_data.pressure > 3000.0 || spl06_data.pressure < 500.0) {
                printf("===Result: BARO_TEST test FAIL !!! ===\r\n");
                spl06_deinit();
                return -1;
            }
            spl06_deinit();
        }
    }
    printf("===Result : BARO_TEST test PASS !!! ===\r\n");
    return 0;
}

static int mag_test(uint8_t turn)
{
    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("*********************** MAG Test **************************\r\n");
    printf("** How to test: ***********************************************\r\n");
    printf("***************************************************************\r\n");
    printf("***************************************************************\r\n");
    printf("\r\n ===MAG test start====\r\n");
    LOGI("MAG_TEST", "test start, turn %d\n", turn);
    int heading = 0;
    int times = 50;

    if (g_haasboard_is_k1c) {
        qmc6310_init();
        LOGI("MAG_TEST", "qmc6310_init done\n");
    } else {
        qmc5883l_init();
        LOGI("MAG_TEST", "qmc5883l_init done\n");
    }

    while (turn--) {
        if (g_haasboard_is_k1c) {
            while(times --){
                heading = qmc6310_readHeading();
            }
            heading /= 50;
            qmc6310_deinit();
            LOGD("MAG_TEST", "heading %d\n", heading);
        } else {
            while(times --){
                heading += qmc5883l_readHeading();
            }
            heading /= 50;
            qmc5883l_deinit();
            LOGD("MAG_TEST", "heading %d\n", heading);
        }
        if (heading == 0) {
            printf("===Result : MAG test FAILs !!! ===\r\n");
            return -1;
        }
    }
    printf("===Result: MAG test PASS !!! ===\r\n");
    return 0;
}

static int ap3216c_test(uint8_t turn)
{
    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("*********************** AP3216C Test ***************************\r\n");
    printf("** How to test: ***********************************************\r\n");
    printf("***************************************************************\r\n");
    printf("***************************************************************\r\n");
    printf("\r\n ===ap3216c test start====\r\n");
    LOGI("AP3216C_TEST", "test start, turn %d\n", turn);

    uint16_t ALS;
    uint16_t PS = 0, IR = 0;
    ap3216c_init();

    while (turn--) {
        ALS = ap3216c_read_ambient_light();
        PS = ap3216c_read_ps_data();
        IR = ap3216c_read_ir_data();
        ap3216c_deinit();
        if ((PS >> 15) & 1)
            printf("物体接近\n");
        else
            printf("物体远离\n");
        LOGI("APP", "\n光照强度是：%d\n红外强度是：%d\n", ALS, IR);

        if (ALS <= 0) {
            printf("===Result : ap3216c test FAIL !!! ===\r\n");
            return -1;
        }
    }
    printf("===Result : ap3216c test PASS !!! ===\r\n");
    return 0;
}

static void handle_sensortest_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    if (argv != 2)
        return;

    sensortest_cb fun = sensor_test_funs[atoi(argv[0])];
    (fun)(atoi(argv[0]));
}

static struct cli_command sensortest_cmd = {
    .name     = "sensortest",
    .help     = "sensortest",
    .function = handle_sensortest_cmd
};

int sensors_test(int index)
{
    if (!sensors_test_inited) {
        g_haasboard_is_k1c = haasedu_is_k1c();
        aos_cli_register_command(&sensortest_cmd);
        sensors_test_inited = 1;
    }

    if ((index < 0) || (index > 4)) {
        printf("invalid index %d\n", index);
        return -1;
    }
    return (sensor_test_funs[index])(1);
}
