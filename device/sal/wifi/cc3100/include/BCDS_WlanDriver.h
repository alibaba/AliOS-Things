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
 * @ingroup BCDS_WLAN
 *
 * @defgroup BCDS_WLAN_DRIVER Wlan Driver Interfaces
 * @{
 *
 * @details  The interface header exports High-level Wlan Driver API's for integrating wifi stack which are used to initialize the Wlan Driver , Register Interrupt
 * 		    Handler and Wlan Driver Os Sync Objects creation and Deletion etc.,
 *
 *	@note
 *  	Data type 'Fd_t' is custom for third party wifi driver and hence for
 *  	porting we cannot change this to BCDS standard datatype
 *
 * @file
 *
 */

/* header definition ******************************************************** */
#ifndef BCDS_WLANDRIVER_H_
#define BCDS_WLANDRIVER_H_

/* public interface declaration ********************************************* */
#include "BCDS_Basics.h"
#include "BCDS_Retcode.h"
#include <time.h>

/* public type and macro definitions */
#define WLANDRIVER_OS_WAIT_FOREVER        (UINT32_MAX)  /**< Definition representing wait for ever time value for OS API TimeOut */
#define WLANDRIVER_OS_NO_WAIT              UINT32_C(0)      /**< Definition representing wait for ever time  value for OS API TimeOut */
#define WLANDRIVER_MAX_DELAY              (UINT32_MAX)      /**< Definition representing  Maximum delay*/
#define WLANDRIVER_SUCCESS                 INT32_C(0)       /**< Definition representing  Success*/
#define WLANDRIVER_FAILURE                 INT32_C(-1)      /**< Definition representing failure*/
#define WLANDRIVER_NOT_ENOUGH_MEMORY       (-1L)
#define WLANDRIVER_SPI_RW_ERROR     UINT32_C(0)
#define WLANDRIVER_SPI_ERROR     INT32_C(-1)
#define WLANDRIVER_COM_TIMEOUT  UINT32_C(5000)
#define WLANDRIVER_SPI_LOCK_TIMEOUT  UINT32_C(5000)

/** fileDescriptor for SPI port */
typedef signed int Fd_t;

/** type definition of wifi event handler*/
typedef void (*WLANDRIVER_EVENT_HANDLER)(void* pValue);
/**
 * @brief
 *      type definition for a spawn entry callback
 
 * @return 
 *       The function which gets invoked inside stack returns the type short, 
 *       hence in V1.2.0 simplelink stack this change is required 
 * @info
 *      the spawn mechanism enable to run a function on different context.
 * This mechanism allow to transfer the execution context from interrupt context to thread context
 * or changing the context from an unknown user thread to general context.
 * The implementation of the spawn mechanism depends on the user's system requirements and could varies
 * from implementation of serialized execution using single thread to creating thread per call
 *
 * @note
 *      The stack size of the execution thread must be at least of TBD bytes!
 */
typedef short (*WLANDRIVER_OSI_SPAWN_ENTRY)(void* pValue);


/**
 @brief  structure definition for spawn message
 */
struct WlanDriver_SlSpawnMsg_S
{
    WLANDRIVER_OSI_SPAWN_ENTRY pEntry;
    void* pValue;
};
typedef struct WlanDriver_SlSpawnMsg_S WlanDriver_SlSpawnMsg_T;

#include <aos/kernel.h>
#include <k_api.h>
#include <k_sem.h>
#include <k_mutex.h>

/*
 @brief     type definition for a sync object container

 @info  Sync object is object used to synchronize between two threads or thread and interrupt handler.One
 thread is waiting on the object and the other thread send a signal, which then release the waiting thread.
 The signal must be able to be sent from interrupt context. This object is generally implemented by binary
 semaphore or events.

 @note  On each porting or platform the type could be whatever is needed - integer, structure etc.
 */
typedef ksem_t WlanDriver_OsSyncObject;

/*
 @brief     type definition for a locking object container

 @info      Locking object are used to protect a resource from mutual accesses of two or more threads.
 The locking object should support re-entrant locks by a signal thread.
 This object is generally implemented by mutex semaphore

 @note  On each porting or platform the type could be whatever is needed - integer, structure etc.
 */
typedef kmutex_t WlanDriver_OsLockObject;

/* @brief  Type definition for a time value in milliseconds.
 @note  On each porting or platform the type could be whatever is needed - integer, pointer to structure etc.
 */
typedef uint32_t WlanDriver_OsTime;

/* public function prototype declarations */

/* public global variable declarations */

/**
 *  @brief
 *      Function to Initialize the WIFI Driver Layer
 *
 * @return   #RETCODE_OK when successful
 * @return   #RETCODE_OUT_OF_RESOURCES Failed to create Semaphore while calling xSemaphoreCreateBinary.
 * @return   #RETCODE_RTOS_QUEUE_ERROR Failed to create SpawnQAndTask while calling CreateSpawnQAndTask.
 *
 * @return   In case of any other error refer #BSP_WiFi_CC3100MOD_Connect error codes
 */
Retcode_T WlanDriver_Init(void);

/**
 *  @brief
 *      Function to register the Interrupt handler
 *  @return   #WLANDRIVER_SUCCESS when successful
 *  @return   #WLANDRIVER_FAILURE when the value of input pointer(pValue) is NULL
 *
 *  @note
 *  	This function will be called by the WiFi driver API mapped in <user.h> header file
 */
int WlanDriver_RegInterruptHdlr(WLANDRIVER_EVENT_HANDLER InterruptHdl, void* pValue);

/**
*  @brief
*      Function to write given no of  bytes over SPI
*      Function to disable the WIFI device
*  @return   any error value refer #BSP_WiFi_CC3100MOD_Disable error codes
*  @note
*      This function will be called by the WiFi driver API mapped in <user.h> header file
*/
Retcode_T WlanDriver_DeviceDisable(void);

/**
*  @brief
*      Function to enable the WIFI device
*  @return   any error value refer #BSP_WiFi_CC3100MOD_Enable error codes
*  @note
*      This function will be called by the WiFi driver API mapped in <user.h> header file
*/
Retcode_T WlanDriver_DeviceEnable(void);

/**
*  @brief
*      Function to close the SPI port
*
*  @return     #WLANDRIVER_SPI_ERROR - if spiHandle is NULL
*  @return   any error value refer #MCU_SPI_Deinitialize error codes
*  @note
*      The return type 'int' and input parameter 'Fd_t' cannot be replaced with BCDS datatype since prototype has
*      to match one on one with third party function data type
*/
int WlanDriver_SpiClose(Fd_t fd);

/**
*  @brief
*      Function to Initialize the SPI port
*
*  @return     #WLANDRIVER_SPI_ERROR - if spiHandle is NULL
*  @return     #WLANDRIVER_SPI_ERROR - if MCU_SPI_Initialize returns some error code other than RETCODE_OK
*  @return     #WLANDRIVER_SUCCESS when successful
*
*  @note
*      The return type 'Fd_t' and input parameters 'char*'and 'unsigned long' cannot be
*      replaced with BCDS datatype since prototype has to match one on one with third
*      party function data type
*/
Fd_t WlanDriver_SpiOpen(char *ifName, unsigned long flags);

/**
 *  @brief
 *      Function to write given no of  bytes over SPI
 *
 *  @param [in]  fd -file descriptor for spi port, which is not used in current implementation
 *
 *  @param [in]  pBuff - pointer to the first location of a buffer that contains the data to send
 *                        over the communication channel.
 *
 *  @param [in/out]  len - number of bytes to write to the communication channel
 *
 *  @return  	#WLANDRIVER_SPI_RW_ERROR - spi write failed if MCU_SPI_Send returns some value other than RETCODE_OK.
 *  @return     #RETCODE_TIMEOUT - if xSemaphoreTake fails.
 *  @return     #len - on success returns number of bytes sent.
 *
 *  @note
 *      The return type 'int' and input parameters 'Fd_t','unsigned char*'and 'int' cannot be
 *      replaced with BCDS datatype since prototype has to match one on one with third
 *      party function data type
 */
int WlanDriver_SpiWrite(Fd_t fd, unsigned char *pBuff, int len);

/**
 *  @brief
 *      Function to read given number of bytes over SPI
 *
 *  @param [in]  	fd -file descriptor for spi port, which is not used in current implementation
 *
 *  @param [in]  	pBuff - pointer to the first location of a buffer that contains enough
 *                       space for all expected data
 *
 *  @param [in/out] len -  number of bytes to read from the communication channel
 *
 *  @return    #WLANDRIVER_SPI_RW_ERROR - spi read failed
 *  @return    #RETCODE_TIMEOUT - if xSemaphoreTake fails.
 *
 *  @note  The return value consist of (First 1 MSByte represents Package ID,
 *        Next byte represents Severity and Last 2 LSBytes represents error code).
 *
 *  @return    #len - on success returns number of bytes received.
 *
 *  @note
 *      The return type 'int' and input parameters 'Fd_t','unsigned char*'and 'int' cannot be
 *      replaced with BCDS datatype since prototype has to match one on one with third
 *      party function data type
 */
int WlanDriver_SpiRead(Fd_t fd, unsigned char *pBuff, int len);

/** @brief This function creates a sync object (binary semaphore)
 *          The sync object is used for synchronization between different thread or ISR and
 *          a thread.
 *  @param [in] pSyncObj
 *          pointer to the sync object control block
 *
 * @return      #WLANDRIVER_SUCCESS - successfully created synchronous object
 * @return      #WLANDRIVER_FAILURE - Failure when input parameter is NULL.
 */
int32_t WlanDriver_OsSyncObjectCreate(WlanDriver_OsSyncObject* pSyncObj);

/** @brief This function deletes a sync object (binary semaphore) if the input parameter is valid
 *  @param [in] pSyncObj
 *          pointer to the sync object control block
 *
 * @return      #WLANDRIVER_SUCCESS - successfully deleted synchronous object
 * @return      #WLANDRIVER_FAILURE - Failure when input parameter is NULL.
 */
int32_t WlanDriver_OsSyncObjectDelete(WlanDriver_OsSyncObject* pSyncObj);

/** @brief This function generates a sync signal for the object (binary semaphore)
 *         All suspended threads waiting on this sync object are resumed
 *  @param [in] pSyncObj
 *          pointer to the sync object control block
 *
 *  @return 	#WLANDRIVER_SUCCESS - successfully released the synchronous object
 *  @return     #WLANDRIVER_FAILURE - Failure when input parameter is NULL.
 *  @note
 *          This API gives the synchronous object successfully if it was already taken by some one. If the sync object given as input was not
 *          taken and this API call simply returns since sync object shall not be given if it is not taken
 */
int32_t WlanDriver_OsSyncObjectSignal(WlanDriver_OsSyncObject* pSyncObj);

/** @brief This function generates a sync signal for the object from Interrupt
 *  @param [in] pSyncObj
 *         pointer to the sync object control block
 *
 *  @return  	#WLANDRIVER_SUCCESS - successfully released the synchronous object from interrupt
 *  @return     #WLANDRIVER_FAILURE - Failure when input parameter is NULL.
 */
int32_t WlanDriver_OsSyncSignalFromISR(WlanDriver_OsSyncObject* pSyncObj);

/** @brief This function waits for a sync signal of the specific sync object
 *  @param [in] pSyncObj
 *         pointer to the sync object control block
 *  @param [in] Timeout
 *         numeric value specifies the maximum number of mSec to stay suspended while waiting for the sync signal
 *  @return     #WLANDRIVER_SUCCESS -  reception of the signal within the TimeOut window
 *  @return     #WLANDRIVER_FAILURE - Failure when input parameter is NULL.
 */
int32_t WlanDriver_OsSyncObjectWait(WlanDriver_OsSyncObject* pSyncObj, WlanDriver_OsTime TimeOut);

/** @brief This API tries to take the sync object without no wait time
 *  @param [in] pSyncObj
 *          pointer to the sync object control block
 *  @return     #WLANDRIVER_SUCCESS -  successful reception of the signal
 *  @return     #WLANDRIVER_FAILURE - Failure when input parameter is NULL.
 */
int32_t WlanDriver_OsSyncObjectClear(WlanDriver_OsSyncObject* pSyncObj);

/** @brief This function creates a locking object (mutex semaphore)
 *         Locking object are used to protect a resource from mutual accesses of two or more threads.
 *  @param [in] pLockObj
 *         pointer to the locking object control block
 * @return     #WLANDRIVER_SUCCESS - successfully created lock object
 * @return     #WLANDRIVER_FAILURE - Failure when input parameter is NULL.
 */
int32_t WlanDriver_OsLockObjectCreate(WlanDriver_OsLockObject* pLockObj);

/** @brief This function deletes a locking object
 *  @param [in] pLockObj
 *         pointer to the locking object control block
 * @return     #WLANDRIVER_SUCCESS - successfully deleted lock object
 * @return     #WLANDRIVER_FAILURE - Failure when input parameter is NULL.
 */
int32_t WlanDriver_OsLockObjectDelete(WlanDriver_OsLockObject* pLockObj);

/** @brief
 *  For details refer API documentation is in the interface header file  WlanDriverInterface.h
 */
/** @brief This function locks a locking object
 *         All other threads that call this function before this thread calls the WlanDriver_osLockUnlock would be suspended
 *  @param [in] pLockObj
 *         pointer to the lock object control block
 *  @param [in] Timeout
 *         numeric value specifies the maximum number of mSec to  stay suspended while waiting for the locking object
 *  @return 	   #WLANDRIVER_SUCCESS -  reception of the locking object within the TimeOut window
 *  @return        #WLANDRIVER_FAILURE - Failure when input parameter is NULL.
 *  @return        #WLANDRIVER_FAILURE - Failure when xSemaphoreTake returns pdFAIL.
 */
int32_t WlanDriver_OsLockObjectWait(WlanDriver_OsLockObject* pLockObj, WlanDriver_OsTime TimeOut);

/** @brief This function unlock a locking object
 *  @param [in] pLockObj
 *         pointer to the lock object control block
 *  @return  	#WLANDRIVER_SUCCESS - successfully released the lock object
 *   @return    #WLANDRIVER_FAILURE - Failure when input parameter is NULL.
 */
int32_t WlanDriver_OsLockUnlock(WlanDriver_OsLockObject* pLockObj);

/**
 * @brief    This is the simplelink spawn task required to call SimpleLink Driver's callback from a different context
 * @param[in] pvParameters
 *            pointer to the task parameter
 */
void WlanDriver_SimpleLinkSpawnTask(void *pvParameters);

/** @brief This function call the pEntry callback from a different context
 *  @param[in] pEntry - pointer to the entry callback
 *  @param[in] pValue - pointer to any type of memory structure that would be passed to pEntry callback from the execution thread.
 *  @param[in] flags  - execution flags - reserved for future usage
 *
 *  @return #WLANDRIVER_SUCCESS - if flags set to SL_SPAWN_FLAG_FROM_SL_IRQ_HANDLER and xQueueSendFromISR is returning pdTRUE
 *  @return #WLANDRIVER_FAILURE - if flags set to SL_SPAWN_FLAG_FROM_SL_IRQ_HANDLER and xQueueSendFromISR is returning pdFAIL
 *  @return #WLANDRIVER_SUCCESS - if flags set to some value other than SL_SPAWN_FLAG_FROM_SL_IRQ_HANDLER and xQueueSendFromISR is returning pdTRUE
 *  @return #WLANDRIVER_FAILURE - if flags set to some value other than SL_SPAWN_FLAG_FROM_SL_IRQ_HANDLER and xQueueSendFromISR is returning pdFAIL
 */
int32_t WlanDriver_OsiSpawn(WLANDRIVER_OSI_SPAWN_ENTRY pEntry, void* pValue, unsigned long flags);

/**
 * @brief Function to set the current device time.
 *
 * @param[in] tm  - pointer to all kinds of useful information about the
 * current time.
 *
 * @return  RETCODE_OK on success, or an error code otherwise.
 */
Retcode_T WlanDriver_SetDeviceTime(struct tm *tm);

/**
 *  @brief
 *      Function to De-Initialize the WIFI Driver Layer.
 *      disabling WiFi clock , Powering off the WiFi module,Deleting the task and queue created
 *
 *  @return   In case of any other error refer #BSP_WiFi_CC3100MOD_Connect error codes
 */
Retcode_T WlanDriver_DeInit(void);

#endif /* BCDS_WLANDRIVER_H_ */

/** @}*/

/****************************************************************************/

