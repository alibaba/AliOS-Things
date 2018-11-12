/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH. 
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/

/**
 * @file 
 * @brief CDDK SPI Driver
 *
 * @details This driver does stuff
 */

#ifndef BCDS_SPIDRIVER_H_
#define BCDS_SPIDRIVER_H_

#include <BCDS_Retcode.h>

#ifndef BCDS_FEATURE_SPIDRIVER
#define BCDS_FEATURE_SPIDRIVER                                               (1)
#endif

#if(1 == BCDS_FEATURE_SPIDRIVER)

/* Include the HAL/MCU layer */

struct SpiDriver_Bus_S
{
    /* MCU-Level resource */
    void *Spi; /**< Pointer for MCU-level control created by the BSP */
    /* SPI Driver context specific variables */
    void * BusLock; /**< Bus lock mutex used to enforce single process master */
    void * XferSyncSignal; /**< Transfer sync semaphore */
    Retcode_T XferStatus; /**< Async-level callback return status for validation */

};

/**
 * @brief Handle type for the SPI controller
 */
typedef struct SpiDriver_Bus_S * SpiDriver_BusHandle_T;

/**
 * @brief This data type represents a function pointer which is used by SPI driver to as
 * select/deselect functionality
 *
 * @details Application should enable/disable the SPI device select pin and
 * should set SPI controller modes, clocks, etc based on the SPI bus configurations
 *
 * @param [in] params : a generic pointer to an arbitrary context data structure which will be passed from select API
 *
 * @return  RETCODE_OK upon successful execution.
 */
typedef Retcode_T (*SpiDriver_SelectFun)(void * params);

/**
 * @brief Structure to represent the SPI Device structure
 *
 */
struct SpiDriver_Device_S
{
    SpiDriver_BusHandle_T Controller; /**< Controller connected to the device */
    SpiDriver_SelectFun Select; /**< Function to perform the device selection. Device should select required SPI mode, speed, etc */
    SpiDriver_SelectFun Deselect; /**< Function to perform the device de-selection */
};

/**
 * @brief Handle type for a SPI Device
 */
typedef struct SpiDriver_Device_S * SpiDriver_DeviceHandle_T;

/**
 * @brief Initialize the SPI Device Interface.
 *
 * @details Call this function before any other SPI Driver API function if you want
 * to use SPI driver. It will initialize SPI driver to default values and create necessary software resources
 *
 * @param [in] busHandle
 * bus handle
 *
 * @return  RETCODE_OK upon successful execution.
 */
Retcode_T SpiDriver_Initialize(SpiDriver_BusHandle_T busHandle);

/**
 * @brief   Deinitializes the SPI device. If no devices are in use, SPI driver resources are released
 *
 * @param [in] busHandle : bus handle
 *
 * @return RETCODE_OK upon successful execution.
 */
Retcode_T SpiDriver_Deinitialize(SpiDriver_BusHandle_T busHandle);


/**
 * @brief  Select API to configure the SPI controller and enable the slave select pin for communication
 *
 * @details SPI controller should be configured based on device communication requirement like SPI mode,
 * clock, controlling slave select pin, etc in Select function which is pointed in device handler structurethe.
 * The select operation is a blocking call.
 * if SPI controller is busy, the calling thread will be blocked unit timeout/Spi controller free
 *
 * @param [in] deviceHandle : device handle
 *
 * @param [in] selectParam : a generic pointer to an arbitrary context data structure which will be passed to the device select function
 *
 * @param [in] timeout : wait timeout in mS
 *
 * @return  RETCODE_OK upon successful execution.
 */
Retcode_T SpiDriver_Select(SpiDriver_DeviceHandle_T deviceHandle, void * selectParam, uint32_t timeout);

/**
 * @brief  Deselect API to deconfigure the SPI controller and disable the slave select pin for communication
 *
 * @details The Deselect operation is a blocking call.
 * if SPI controller is busy, the calling thread will be blocked unit timeout/Spi controller free
 *
 * @param [in] deviceHandle : device handle
 *
 * @param [in] selectParam : a generic pointer to an arbitrary context data structure which will be passed to the device deselect function
 *
 * @param [in] timeout : wait timeout in mS
 *
 * @return  RETCODE_OK upon successful execution.
 */
Retcode_T SpiDriver_Deselect(SpiDriver_DeviceHandle_T deviceHandle, void * deselectParam);

/**
 * @brief send data via SPI
 *
 * @details The send operation is a blocking call. Application should call SpiDriver_Select() before calling send API
 *
 * @param [in] deviceHandle
 * device handle
 *
 * @param [in] pDataOut
 * pointer on the buffer containing the data to be send
 *
 * @param [in] nDataOut
 * number of data elements to send
 *
 * @param [in] timeout
 * wait timeout in mS
 *
 * @return  RETCODE_OK upon successful execution.
 */
Retcode_T SpiDriver_Send(SpiDriver_DeviceHandle_T deviceHandle, void * pDataOut, size_t nDataOut, uint32_t timeout);

/**
 * @brief receive data via SPI
 *
 * @details The receive operation is a blocking call. Application should call SpiDriver_Select() before calling receive API
 *
 * @param [in] deviceHandle : device handle
 *
 * @param [in] pDataIn : pointer on the buffer into which to store received data.
 *
 * @param [in] nDataIn : number of data elements to receive.
 *
 * @param [in] timeout : wait timeout in mS
 *
 * @return  RETCODE_OK upon successful execution.
 */
Retcode_T SpiDriver_Receive(SpiDriver_DeviceHandle_T deviceHandle, void * pDataIn, size_t nDataIn, uint32_t timeout);

/**
 * @brief send/receive data to/from SPI
 *
 * @details The transfer operation is a blocking call. Application should call SpiDriver_Select() before calling transfer API
 *
 * @param [in] deviceHandle : device handle
 *
 * @param [in] pDataIn : pointer on the buffer into which to store received data.
 *
 * @param [in] pDataIn : pointer on the buffer into which to store received data.
 *
 * @param [in] nDataInOut : number of data elements to transfer.
 *
 * @param [in] timeout : wait timeout in mS
 *
 * @return  RETCODE_OK upon successful execution.
 */
Retcode_T SpiDriver_Transfer(SpiDriver_DeviceHandle_T deviceHandle, void * pDataIn, void * pDataOut, size_t nDataInOut, uint32_t timeout);

#endif /* BCDS_FEATURE_SPIDRIVER */

#endif /* BCDS_SPIDRIVER_H_ */

