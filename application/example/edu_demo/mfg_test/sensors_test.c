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

#include "hal_oled.h"
#include "led.h"
#include "key.h"
#include "si7006.h"
#include "qmc5883l.h"
#include "spl06.h"

typedef int (*sensortest_cb)(int turn);
static int sensors_test_inited = 0;
static int si7006_test(uint8_t turn);
static int mpu6050_test(uint8_t turn);
static int qmc5883l_test(uint8_t turn);
static int spl06_test(uint8_t turn);
static int ap3216c_test(uint8_t turn);

sensortest_cb sensor_test_funs[] = {
    si7006_test,
    mpu6050_test,
    qmc5883l_test,
    spl06_test,
    ap3216c_test,
};
extern int usb_class_detect;

static int si7006_test(uint8_t turn)
{
    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("*********************** SI7006 Test ***************************\r\n");
    printf("** How to test: ***********************************************\r\n");
    printf("***************************************************************\r\n");
    printf("***************************************************************\r\n");
    printf("\r\n ===usb test start====\r\n");
    uint8_t id_buf[8] = {0};
    float Temp = 0.0, Humidity = 0.0;
    LOGI("SI7006_TEST", "test start, turn %d\n",turn);

    while(turn--){
    // get ID
        si7006_getID(id_buf);
        if (id_buf[4] == Si7006_TAG){
            LOGI("si7006_test", "READ Si7006 Chip OK");
        }else{
            goto _failed;
            /* code */
            LOGI("si7006_test", "READ Si7006 Chip ERROR");
        }

    //get version
        uint8_t ver = si7006_getVer();
        LOGI("si7006_test", "si7006_version %x\n", ver);
        if(ver != 0x20){
            goto _failed;
        }

    //get data
        si7006_getTempHumidity(&Humidity, &Temp);
        LOGI("si7006_test", "Temp :%f Humidity : %f\n", Temp, Humidity);
        if((Humidity > 300.0 || Humidity < -100.0) || (Temp > 200.0 || Temp < -50.0)){
            goto _failed;
        }
        printf("===Result : SI7006 test PASS !!! ===\r\n");
        return 0;
    }
_failed:
    printf("===Result: SI7006 test FAIL !!! ===\r\n");
    return -1;
}

static int mpu6050_test(uint8_t turn)
{
    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("*********************** MPU6050 Test **************************\r\n");
    printf("** How to test: ***********************************************\r\n");
    printf("***************************************************************\r\n");
    printf("***************************************************************\r\n");
    printf("\r\n ===mpu6050 test start====\r\n");
    short gx = 0, gy = 0, gz = 0;
    short ax = 0, ay = 0, az = 0;
    LOGI("MPU6050_TEST", "test start, turn %d\n",turn);
    while(turn --){
        short temp = MPU_Get_Temperature()/100;
        LOGI("mpu6050_test", "temp %0.1f\n", temp);
        MPU_Get_Gyroscope(&gx, &gy, &gz);
        LOGI("mpu6050_test", "gx-gy-gz:%d %d %d\n", gx, gy, gz);

        MPU_Get_Accelerometer(&ax, &ay, &az);
        LOGI("mpu6050_test", "ax-ay-az:%d %d %d\n", ax, ay, az);

        if(temp/100 > 200.0 || temp/100 < -50.0)
        {
            printf("===Result: MPU6050 test FAIL !!! ===\r\n");
            return -1;
        }
    }
    printf("===Result : MPU6050 test PASS !!! ===\r\n");
    return 0;
}

static int spl06_test(uint8_t turn)
{
    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("*********************** SPL06 Test ****************************\r\n");
    printf("** How to test: ***********************************************\r\n");
    printf("***************************************************************\r\n");
    printf("***************************************************************\r\n");
    printf("\r\n ===spl06 test start====\r\n");
	spl06_data_t sp;
    LOGI("SPL06_TEST", "test start, turn %d\n",turn);
    while(turn --){

		spl06_getdata(&sp);
		printf("Temperature: C %0.1f\n", sp.Ctemp);
		printf("Temperature: F %0.1f\n", sp.Ftemp);
		printf("Measured Air Pressure: : %0.2f mb\n", sp.pressure);
		printf("altitude: %0.2f m\n", sp.altitude);
  		//printf("altitude: %0.2f ft\n", sp.altitude * 3.281);
        if(sp.pressure > 3000.0 || sp.pressure < 500.0)
        {
            printf("===Result: MPU6050 test FAIL !!! ===\r\n");
            return -1;
        }
    }
    printf("===Result : MPU6050 test PASS !!! ===\r\n");
    return 0;
}

static int qmc5883l_test(uint8_t turn)
{
    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("*********************** QMC5883L Test **************************\r\n");
    printf("** How to test: ***********************************************\r\n");
    printf("***************************************************************\r\n");
    printf("***************************************************************\r\n");
    printf("\r\n ===QMC5883L test start====\r\n");
    LOGI("QMC5883L_TEST", "test start, turn %d\n",turn);
    while(turn --){
        if(qmc5883l_readHeading() <= 0)
        {
            printf("===Result : QMC5883L test FAILs !!! ===\r\n");
            return -1;
        }
    }
    printf("===Result: QMC5883L test PASS !!! ===\r\n");
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

    while(turn --){
		ALS = ap3216c_read_ambient_light();
		PS = ap3216c_read_ps_data();
		IR = ap3216c_read_ir_data();
		if ((PS >> 15) & 1)
			printf("物体接近\n");
		else
			printf("物体远离\n");
		LOGI("APP", "\n光照强度是：%d \n红外强度是：%d\n", ALS, IR);

        if(ALS <= 0)
        {
            printf("===Result : ap3216c test FAIL !!! ===\r\n");
            return -1;
        }
    }
    printf("===Result : ap3216c test PASS !!! ===\r\n");
    return 0;
}

static void handle_sensortest_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    if(argv != 2)
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
    if(0 == sensors_test_inited)
    {
        si7006_init();
        MPU_Init();
        spl06_init();
        qmc5883l_init();
        ap3216c_init();
        aos_cli_register_command(&sensortest_cmd);
        sensors_test_inited = 1;
    }
    if((index < 0 ) || (index > 4 )){
        printf("invalid index %d\n", index);
        return -1;
    }
    return (sensor_test_funs[index])(1);
}