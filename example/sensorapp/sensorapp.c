/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*INCLUDES*******************************************************************
 *                          SYSTEM HEADER FILES          
 *END***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "iot_import.h"
//#include "iot_export.h"
#include "aos/log.h"
#include "aos/yloop.h"
#include "aos/network.h"
#include <netmgr.h>
#include <aos/kernel.h>
#include <k_err.h>
#include <netmgr.h>
#include <aos/aos.h>
#include <aos/cloud.h>
#include "soc_init.h"
//#include "sensors.h"


/*FUNCTIONS****************************************************************** 
 *                          FUNCTION PROTOTYPES                    
 *END***********************************************************************/
extern int init_sensors(void);
extern void Temperature_Test(void);
extern void Humidity_Test(void);
extern void Pressure_Test(void);
extern void Magneto_Test(void);
extern void Gyro_Test(void);
extern void Accelero_Test(void);
extern void Proximity_Test(void);
static void upt_sensor_msg();


/*FUNCTIONS***************************************************************** 
 * @fn      upt_sensor_msg
 * @brief   updates the sensor message 
 * @param   none
 * @ret     none            
 *END***********************************************************************/
static void upt_sensor_msg()
{
   
    static uint32_t num = 0;
    
    printf("***************************************************  \n");
    printf("SENSOR MESSAGE NO.%d\n", num++);
    /* Get system current time in mini seconds */
    printf("System time: %lld\n", aos_now_ms());         
    printf("***************************************************  \n");
    
    /* system sleep in mini seconds */
    aos_msleep(1000);
    
    //QSPI_demo();
    //QSPI_MemoryMapped_demo();    
    Temperature_Test();
    Humidity_Test();
    Pressure_Test();
    Magneto_Test();
    Gyro_Test();
    Accelero_Test();
    Proximity_Test();
    
}

/*FUNCTIONS***************************************************************** 
 * @fn      app_delayed_action
 * @brief   application delayed action 
 * @param   arg
 * @ret     none            
 *END***********************************************************************/
static void app_delayed_action(void *arg)
{
    LOG("%s:%d %s\r\n", __func__, __LINE__, aos_task_name());    
    upt_sensor_msg();
    aos_post_delayed_action(3000, app_delayed_action, NULL);
}

/*FUNCTIONS***************************************************************** 
 * @fn      application_start
 * @brief   user's application entry
 * @param   argc
 * @param   argv
 * @ret     int            
 *END***********************************************************************/
int application_start(int argc, char *argv[])
{
    printf("\n\n");
    printf("***************************************************  \n");
    printf("*  AliOS-Things sensorapp demo is started!        *  \n");
    printf("*  This demo only supports DISCO-L475VG-IOT01A    *  \n");
    printf("*  so far.                                        *  \n");
    printf("*  The sensor message updating period is 5s.      *  \n");
    printf("***************************************************  \n");
    printf("*  Sensor list:                                   *  \n");
    printf("***************************************************  \n");
    printf("\tTemperature \t: HTS221 \n");
    printf("\tHumidity \t: HTS221 \n");
    printf("\tPressure \t: LPS22HB \n");
    printf("\tmagnetometer \t: LIS3MDL \n");
    printf("\tgyroscope \t: LIS3MDL \n");
    printf("\tAccelerometer \t: LSM6DSL \n");
    printf("\tProximity \t: VL53L0X \n");
    printf("***************************************************  \n");
    
    printf("\nBoard gpio and button pre-testing: \n");
    printf("Press User button to put LED2 ON \n");
    while(BSP_PB_GetState(BUTTON_USER) == GPIO_PIN_RESET);
    while(BSP_PB_GetState(BUTTON_USER) == GPIO_PIN_SET);
    BSP_LED_On(LED2); 
  
    init_sensors();       
    
    aos_set_log_level(AOS_LL_DEBUG);


    netmgr_init();
    netmgr_start(false);

    aos_post_delayed_action(1000, app_delayed_action, NULL);
    aos_loop_run();
    return 0;
}


/**
  * @brief  Gets numeric values from the Hyperterminal.
  * @param  Maximun value allowed (value)
  * @retval The character received
  */
//uint32_t Serial_Scanf(uint32_t value)
//{
//  uint16_t tmp = 0;
//  
//  tmp = getchar();
//  
//  if (tmp > value)
//  {
//    printf("\n\r  !!! Please enter valid number between 0 and %lu \n", value);
//    return 0xFF;
//  }
//  return tmp;
//}

/* EOF */
