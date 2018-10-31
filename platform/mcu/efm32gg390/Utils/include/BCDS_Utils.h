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
 *
 * @defgroup UTILS Utils
 * @{
 *
 * @brief Utility module
 *
 * @details
 *
 * @file
 **/

#ifndef BCDS_UTILS_H_
#define BCDS_UTILS_H_

/* interface dependency checks */
#include "BCDS_UtilsConfig.h"
#include "BCDS_Retcode.h"

/**
 * @brief Enumeration to represent the return codes of Utils module.
 */
enum Utils_Retcode_E
{
    RETCODE_LEANB2CAP_SD_ERROR = RETCODE_FIRST_CUSTOM_CODE,
    RETCODE_LEANB2CAP_LENGTH_ERROR,
    RETCODE_LEANB2CAP_CMD_TYPE_ERROR,
    RETCODE_LEANB2CAP_FRAME_ERROR,
    RETCODE_LEANB2CAP_CRC_ERROR,
    RETCODE_LEANB2CAP_RX_PROCESSOR_ERROR,
    RETCODE_LEANB2CAP_RX_PROCESSOR_FAIL,
    RETCODE_LEANB2CAP_OP_BUFFER_OVERFLOW,
    RETCODE_LEANB2CAP_OP_BUFFER_INSUFFICIENT,
    RETCODE_LEANB2CAP_INVALID_CALLBACK,
    RETCODE_XPROTOCOL_INTEGRITY_FAILED,
    RETCODE_XPROTOCOL_FRAME_BUFFER_TOO_SMALL,
    RETCODE_XPROTOCOL_DATA_BUFFER_TOO_SMALL,
    RETCODE_XPROTOCOL_DATA_EXCEEDES_MAX_PAYLOAD_LENGTH,
    RETCODE_XPROTOCOL_FRAME_NOT_COMPLETE_YET,
    RETCODE_XPROTOCOL_START_DELIMITER_MISSING,
    RETCODE_XPROTOCOL_END_DELIMITER_MISSING,
    RETCODE_CMDLINE_DEBUGGER_COMMAND_NOT_FOUND,
    RETCODE_CMDPROCESSOR_TASK_INIT_FAIL,  /**< Failed to initialize Command Processor Task */
    RETCODE_CMDPROCESSOR_QUEUE_INIT_FAIL, /**< Failed to initialize CmdProcessor Queue */
    RETCODE_CMDPROCESSOR_QUEUE_ENQUEUE_FAIL, /**< Failed to enqueue element, probably because of a timeout */
    RETCODE_CMDPROCESSOR_QUEUE_DEQUEUE_FAIL, /**< Failed to dequeue element, probably because of a timeout */
    RETCODE_CMDPROCESSOR_EXECUTE_FAILURE,   /**< Command to be executed was a NULL pointer */
    RETCODE_CMDPROCESSOR_QUEUE_ERROR,
    RETCODE_GUARDEDTASK_SEMAPHORE_ALREADY_GIVEN,
    RETCODE_GUARDEDTASK_SEMAPHORE_ERROR,
    RETCODE_TASKMONITOR_BUFFER_FULL_ERROR,
    RETCODE_SLEEPCONTROL_NOSLEEP,
    RETCODE_I2CTRANSCEIVER_TRANSFER_ERROR,
    RETCODE_MAX_ERROR,
};

/* Put the type and macro definitions here */
/**
 * @brief BCDS_MODULE_ID for each Utils C module
 * @details Usage:
 * @code{.c}
 * #undef BCDS_MODULE_ID
 * #define BCDS_MODULE_ID BCDS_UTILS_MODULE_ID_xxx
 * @endcode
 */
enum BCDS_UTILS_ModuleID_E
{
    BCDS_UTILS_MODULE_ID_CRC = 1,
    BCDS_UTILS_MODULE_ID_LEANB2CAP,
    BCDS_UTILS_MODULE_ID_RINGBUFFER,
    BCDS_UTILS_MODULE_ID_TLV,
    BCDS_UTILS_MODULE_ID_XPROTOCOL,
    BCDS_UTILS_MODULE_ID_LOGGING,
    BCDS_UTILS_MODULE_ID_LOGGING_FILTER,
    BCDS_UTILS_MODULE_ID_LOGGING_RECORD_ASYNCHRONOUS,
    BCDS_UTILS_MODULE_ID_LOGGING_APPENDER_UART,
    BCDS_UTILS_MODULE_ID_LOGGING_APPENDER_SWO,
    BCDS_UTILS_MODULE_ID_CMD_LINE_DEBUGGER,
    BCDS_UTILS_MODULE_ID_UART_TRANSCEIVER,
    BCDS_UTILS_MODULE_ID_I2C_TRANSCEIVER,
    BCDS_UTILS_MODULE_ID_ERRORLOGGER,
    BCDS_UTILS_MODULE_ID_CMDPROCESSOR,
    BCDS_UTILS_MODULE_ID_GUARDEDTASK,
    BCDS_UTILS_MODULE_ID_TASKMONITOR,
    BCDS_UTILS_MODULE_ID_QUEUE,
    BCDS_UTILS_MODULE_ID_EVENTHUB,
    BCDS_UTILS_MODULE_ID_SLEEPCONTROL
};

#endif /* BCDS_UTILS_H_ */
/**@} */
