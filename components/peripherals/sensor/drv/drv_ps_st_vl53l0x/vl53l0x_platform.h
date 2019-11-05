/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 *
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __VL53L0X_PLATFORM_H
#define __VL53L0X_PLATFORM_H

#ifdef __cplusplus
extern "C"
{
#endif

    /* Includes
     * ------------------------------------------------------------------*/

#include "vl53l0x/vl53l0x_def.h"
#include "vl53l0x/vl53l0x_platform_log.h"

#define VL53L0X_OsDelay(...) aos_msleep(2)

    /* Exported types
     * ------------------------------------------------------------*/
    /**
     * @struct  VL53L0X_Dev_t
     * @brief    Generic PAL device type that does link between API and platform
     * abstraction layer
     *
     */
    typedef struct
    {
        VL53L0X_DevData_t Data; /*!< embed ST Ewok Dev  data as "Data"*/

        /*!< user specific field */

        uint8_t I2cDevAddr;

        char DevLetter;

        int Id;
        int Present;
        int Enabled;
        int Ready;

        uint8_t  comms_type;
        uint16_t comms_speed_khz;

        int            LeakyRange;
        int            LeakyFirst;
        uint8_t        RangeStatus;
        uint8_t        PreviousRangeStatus;
        FixPoint1616_t SignalRateRtnMegaCps;
        uint16_t       EffectiveSpadRtnCount;

    } VL53L0X_Dev_t;


    /**
     * @brief   Declare the device Handle as a pointer of the structure @a
     * VL53L0X_Dev_t.
     *
     */
    typedef VL53L0X_Dev_t *VL53L0X_DEV;

/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/**
 * @def PALDevDataGet
 * @brief Get ST private structure @a VL53L0X_DevData_t data access
 *
 * @param Dev       Device Handle
 * @param field     ST structure field name
 * It maybe used and as real data "ref" not just as "get" for sub-structure item
 * like PALDevDataGet(FilterData.field)[i] or
 * PALDevDataGet(FilterData.MeasurementIndex)++
 */
#define PALDevDataGet(Dev, field) (Dev->Data.field)

/**
 * @def PALDevDataSet(Dev, field, data)
 * @brief  Set ST private structure @a VL53L0X_DevData_t data field
 * @param Dev       Device Handle
 * @param field     ST structure field name
 * @param data      Data to be set
 */
#define PALDevDataSet(Dev, field, data) (Dev->Data.field) = (data)

#define VL53L0X_COPYSTRING(str, ...) strcpy(str, ##__VA_ARGS__)
    /* Private macros
     * ------------------------------------------------------------*/
    /* Exported functions
     * --------------------------------------------------------*/

    /**
     * Lock comms interface to serialize all commands to a shared I2C interface
     * for a specific device
     * @param   Dev       Device Handle
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_LockSequenceAccess(VL53L0X_DEV Dev);

    /**
     * Unlock comms interface to serialize all commands to a shared I2C
     * interface for a specific device
     * @param   Dev       Device Handle
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_UnlockSequenceAccess(VL53L0X_DEV Dev);


    /**
     * Writes the supplied byte buffer to the device
     * @param   Dev       Device Handle
     * @param   index     The register index
     * @param   pdata     Pointer to uint8_t buffer containing the data to be
     * written
     * @param   count     Number of bytes in the supplied byte buffer
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_WriteMulti(VL53L0X_DEV Dev, uint8_t index,
                                     uint8_t *pdata, uint32_t count);

    /**
     * Reads the requested number of bytes from the device
     * @param   Dev       Device Handle
     * @param   index     The register index
     * @param   pdata     Pointer to the uint8_t buffer to store read data
     * @param   count     Number of uint8_t's to read
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_ReadMulti(VL53L0X_DEV Dev, uint8_t index,
                                    uint8_t *pdata, uint32_t count);

    /**
     * Write single byte register
     * @param   Dev       Device Handle
     * @param   index     The register index
     * @param   data      8 bit register data
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_WrByte(VL53L0X_DEV Dev, uint8_t index, uint8_t data);

    /**
     * Write word register
     * @param   Dev       Device Handle
     * @param   index     The register index
     * @param   data      16 bit register data
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_WrWord(VL53L0X_DEV Dev, uint8_t index, uint16_t data);

    /**
     * Write double word (4 byte) register
     * @param   Dev       Device Handle
     * @param   index     The register index
     * @param   data      32 bit register data
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_WrDWord(VL53L0X_DEV Dev, uint8_t index,
                                  uint32_t data);

    /**
     * Read single byte register
     * @param   Dev       Device Handle
     * @param   index     The register index
     * @param   data      pointer to 8 bit data
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_RdByte(VL53L0X_DEV Dev, uint8_t index, uint8_t *data);

    /**
     * Read word (2byte) register
     * @param   Dev       Device Handle
     * @param   index     The register index
     * @param   data      pointer to 16 bit data
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_RdWord(VL53L0X_DEV Dev, uint8_t index,
                                 uint16_t *data);

    /**
     * Read dword (4byte) register
     * @param   Dev       Device Handle
     * @param   index     The register index
     * @param   data      pointer to 32 bit data
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_RdDWord(VL53L0X_DEV Dev, uint8_t index,
                                  uint32_t *data);

    /**
     * Threat safe Update (read/modify/write) single byte register
     *
     * Final_reg = (Initial_reg & and_data) |or_data
     *
     * @param   Dev        Device Handle
     * @param   index      The register index
     * @param   AndData    8 bit and data
     * @param   OrData     8 bit or data
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_UpdateByte(VL53L0X_DEV Dev, uint8_t index,
                                     uint8_t AndData, uint8_t OrData);

    /** @} end of VL53L0X_registerAccess_group */


    /**
     * @brief execute delay in all polling API call
     *
     * A typical multi-thread or RTOs implementation is to sleep the task for
     * some 5ms (with 100Hz max rate faster polling is not needed) if nothing
     * specific is need you can define it as an empty/void macro
     * @code
     * #define VL53L0X_PollingDelay(...) (void)0
     * @endcode
     * @param Dev       Device Handle
     * @return  VL53L0X_ERROR_NONE        Success
     * @return  "Other error code"    See ::VL53L0X_Error
     */
    VL53L0X_Error VL53L0X_PollingDelay(
      VL53L0X_DEV Dev); /* usually best implemented as a real function */

#ifdef __cplusplus
}
#endif

#endif /* __VL53L0X_PLATFORM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
