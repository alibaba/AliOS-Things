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
 * @ingroup UTILS
 *
 * @defgroup UARTTRANSCEIVER UARTTransceiver
 * @{
 *
 * @brief Advanced API functions for sending and reveiving via UART and LEUART
 *
 * @details The UARTTransceiver encapsulates the handling of buffers
 *          and the control flow.
 *
 *          The typical use of the UARTTransceiver starts with initializing it.
 *
 *          The transceiver supports two operation modes: synchronous mode and
 *          asynchronous mode.The transceiver is started in the synchronous mode
 *          by calling the function UARTTransceiver_Start().
 *          In order to operate the transceiver in the asynchronous mode, the function
 *          UARTTransceiver_StartInAsyncMode() should be used instead.
 *          After the start, the transceiver starts to receive
 *          bytes, in the ISR context, and store them internally into a ring buffer.
 *          While receiving the transceiver applies the end-of-frame check function to
 *          every received byte. If it returns true, then the transceiver will perform
 *          one of the following action depending on its operation mode:
 *              1. In the asynchronous mode, the registered callback is invoked to notify
 *              the upper-layer. The upper-layer will fetch the data from the transceiver's
 *              buffer using the function UARTTransceiver_ReadData().
 *              2. In the synchronous mode, the upper-layer is expected to call
 *              UARTTransceiver_ReadData(). The call is blocking. When the end-of-frame check
 *              function returns true, the semaphore is released such that the call
 *              of UARTTransceiver_ReadData() returns, where
 *              the frame is copied into the buffer which was passed as a parameter.
 *
 *          The function UARTTransceiver_WriteData() sends data via the under-laying UART
 *          or LEUART. In the asynchronous mode, the write operation triggers the
 *          sending and returns. A callback follows to inform the upper-layer about
 *          the success or failure of the operation.
 *          In the synchronous mode, the write operation blocks until all bytes are sent. No
 *          callback will follow.
 *
 *          UARTTransceiver provides also the functions UARTTransceiver_Suspend(),
 *          UARTTransceiver_Resume(), UARTTransceiver_Stop() to control the activity
 *          of the transceiver, in particular the receiving activity in the background.
 *
 * @note    UARTTransceiver is a passive component without an own task. It is driven
 *          by the caller task and the interrupts coming form UART or LEUART.
 *
 * @note    UARTTransceiver maintains its context in a struct which makes it ready
 *          for multiple-instance use. This means, multiple UARTTransceiver
 *          instances may co-exist in the same application. However, they must operate on
 *          different UART or LEUART ports.
 *
 * @file
 */

#ifndef UARTTRANSCEIVER_H_
#define UARTTRANSCEIVER_H_

#include "BCDS_Utils.h"

#if BCDS_FEATURE_UARTTRANSCEIVER

#include "BCDS_Basics.h"
#include "BCDS_Retcode.h"
#include "BCDS_HAL.h"
#include "BCDS_HALConfig.h"
#include "BCDS_RingBuffer.h"
#include "mico_rtos.h"


#if BCDS_FEATURE_UART || BCDS_FEATURE_LEUART

#if BCDS_FEATURE_UART && BCDS_FEATURE_LEUART
#error "UARTransceiver is supporting any of the Uart. it is enabled only for Uart module"
#endif


#if BCDS_FEATURE_UART

#include "BCDS_MCU_UART.h"
typedef void (*UARTransceiver_Callback_T)(struct MCU_UART_Event_S event);

#elif BCDS_FEATURE_LEUART
#include "BCDS_MCU_LEUART.h"
typedef void (*UARTransceiver_Callback_T)(struct MCU_LEUART_Event_S event);
#endif


typedef bool (*UARTTransceiver_EndofFrameCheckFunc_T)(uint8_t lastByte);

enum UARTTransceiver_State_E
{
    UART_TRANSCEIVER_STATE_RESET = 0,
    UART_TRANSCEIVER_STATE_IDLE,
    UART_TRANSCEIVER_STATE_INITIALIZED,
    UART_TRANSCEIVER_STATE_ACTIVE,
    UART_TRANSCEIVER_STATE_STOPPED,
    UART_TRANSCEIVER_STATE_SUSPENDED
};

enum UARTTransceiver_UartType_E
{
    UART_TRANSCEIVER_UART_TYPE_NONE = 0,
    UART_TRANSCEIVER_UART_TYPE_UART,
    UART_TRANSCEIVER_UART_TYPE_LEUART,
};

enum UARTTransceiver_Mode_E
{
    UART_TRANSCEIVER_MODE_NONE = 0,
    UART_TRANSCEIVER_MODE_SYNCH,
    UART_TRANSCEIVER_MODE_ASYNCH,
};

struct _UARTTransceiver_S
{
    /* UART Handle used by the UART driver in HAL/MCU */
    HWHandle_T handle;

    enum UARTTransceiver_UartType_E UartType;

    enum UARTTransceiver_Mode_E Mode;

    enum UARTTransceiver_State_E State;

    /* Initialized pointer to frame end check function*/
    UARTTransceiver_EndofFrameCheckFunc_T EndOfFrameCheck;

    UARTransceiver_Callback_T callback;

    /*receive ring buffer*/
    RingBuffer_T UartRxBufDescr;

    /* receive buffer*/
    uint8_t * RawRxBuffer;

    uint32_t RawRxBufferSize;

    /* currently received byte */
    uint8_t LastByte;

    /*semaphore used to synchronize the send process*/
    mico_semaphore_t TxSemaphore;

    /*semaphore used to synchronize the error handling process*/
    mico_semaphore_t RxSemaphore;
#if BCDS_FEATURE_UART
    union MCU_UART_Event_U AsyncEvent;
#elif BCDS_FEATURE_LEUART
    union MCU_LEUART_Event_U AsyncEvent;
#endif
    enum Retcode_General_E errorCode;
};

typedef struct _UARTTransceiver_S UARTTransceiver_T;

/**
 * @brief Initializes the transceiver for the use with the passed UART or
 * LEUART handle
 *
 * @param[in] transceiver
 * a pointer to the transceiver context structure to be initialized
 *
 * @param[in] handle
 * the handle of the UART or LEUART to be used by the transceiver. The handle
 * must be initialized before passing it here.
 *
 * @param[in] mode
 * The mode in which the transceiver will operate, i.e. synchronous or
 * asynchronous.
 *
 * @ rawRxBuffer
 * The buffer which will be used by the transceiver internally to save
 * the received bytes. It must not be NULL.
 *
 * @param[in] rawRxBufferSize
 * The size of the rawRxBuffer. It must be larger than zero.
 *
 * @retval #RETCODE_OK                       if successfully initialized
 * @retval #RETCODE_INVALID_PARAM            if any of the parameter is NULL
 * @retval #RETCODE_SEMAPHORE_ERROR          if the semaphores are not created (see #UARTTransceiver_T.TxSemaphore
 *                                           and #UARTTransceiver_T.RxSemaphore)
 * @retval #RETCODE_DOPPLE_INITIALIZATION    if you are trying to initialized the transceiver again
 */
Retcode_T UARTTransceiver_Initialize(
        UARTTransceiver_T *transceiver,
        HWHandle_T handle, uint8_t * rawRxBuffer, uint32_t rawRxBufferSize,
        enum UARTTransceiver_UartType_E type
);

/**
 * @brief De-initializes the transceiver.
 *
 * The transceiver must not be used after calling this function.
 *
 * @param[in] transceiver
 * a pointer to the transceiver context structure to be de-initialized
 *
 * @retval #RETCODE_OK if successfully deinitialize
 * @retval #RETCODE_INVALID_PARAM if the transceiver pointer parameter is NULL
 */
Retcode_T UARTTransceiver_Deinitialize(UARTTransceiver_T *transceiver);

/**
 * @brief It activates the transceiver to start receiving and sending
 * in the synchronous operation mode.
 *
 *
 * @param[in] transceiver
 * a pointer to the transceiver to be started.
 *
 * @param[in] frameEndCheckFunc
 * A function pointer which is used by the transceiver to detect the
 * end of the frame in order to invoke the callback.
 *
 * @retval #RETCODE_OK                  if successfully started
 * @retval #RETCODE_INVALID_PARAM       if transceiver pointer parameter is NULL
 * @retval #RETCODE_INCONSITENT_STATE   if the transceiver is not in an initialized
 *                                      state (see #UARTTransceiver_State_E)
 */
Retcode_T UARTTransceiver_Start(
        UARTTransceiver_T *transceiver,
        UARTTransceiver_EndofFrameCheckFunc_T frameEndCheckFunc);

/**
 * @brief It activates the transceiver to start receiving and sending
 * in the asynchronous operation mode.
 *
 * @note Beside the operation mode, this function does the same as
 * the function UARTTransceiver_Start().
 *
 * @param[in] transceiver
 * a pointer to the transceiver to be started.
 *
 * @param[in] frameEndCheckFunc
 * A function pointer which is used by the transceiver to detect the
 * end of the frame in order to invoke the callback.
 *
 * @param[in] callback
 * A callback function which will be invoked on RX and TX events, e.g.
 * errors or operation completeness in the asynchronous mode.
 *
 * @retval #RETCODE_OK                  if successfully started
 * @retval #RETCODE_INVALID_PARAM       if transceiver pointer parameter is NULL
 * @retval #RETCODE_INCONSITENT_STATE   if the transceiver is not in an initialized
 *                                      state (see #UARTTransceiver_State_E)
 */
Retcode_T UARTTransceiver_StartInAsyncMode(
        UARTTransceiver_T *transceiver,
        UARTTransceiver_EndofFrameCheckFunc_T frameEndCheckFunc,
        UARTransceiver_Callback_T callback);

/**
 * @brief It suspends the transceiver.
 *
 * @details The transceiver must be started when calling this function.
 *          When the transceiver
 *          is suspended read or write operations are not permitted.
 *
 * @param[in] transceiver
 * a pointer to the transceiver to be suspended.
 *
 * @retval #RETCODE_OK                  if successfully started
 * @retval #RETCODE_INVALID_PARAM       if transceiver pointer parameter is NULL
 * @retval #RETCODE_INCONSITENT_STATE   if the transceiver is not in an active
 *                                      state (see #UARTTransceiver_State_E)
 */
Retcode_T UARTTransceiver_Suspend(UARTTransceiver_T *transceiver);

/**
 * @brief It resumes the transceiver after it has been suspended.
 *
 * @details The transceiver must be suspended when calling this function.
 *
 * @param[in] transceiver
 * a pointer to the transceiver to be resumed.
 *
 * @retval #RETCODE_OK                  if successfully started
 * @retval #RETCODE_INVALID_PARAM       if transceiver pointer parameter is NULL
 * @retval #RETCODE_INCONSITENT_STATE   if the transceiver is not in an suspended
 *                                      state (see #UARTTransceiver_State_E)
 */
Retcode_T UARTTransceiver_Resume(UARTTransceiver_T *transceiver);

/**
 * @brief It stops the transceiver.
 *
 * The transceiver must be started when calling this function.
 * When the transceiver
 * is stopped, read or write operations are not permitted. Resuming
 * the transceiver after stopping it is not possible. It must be
 * started using the UARTTransceiver_Start() function.
 *
 * @param[in] transceiver
 * a pointer to the transceiver to be suspended.
 *
 * @retval #RETCODE_OK                  if successfully started
 * @retval #RETCODE_INVALID_PARAM       if transceiver pointer parameter is NULL
 * @retval #RETCODE_INCONSITENT_STATE   if the transceiver is not in an active or
 *                                      already suspended state (see #UARTTransceiver_State_E)
 */
Retcode_T UARTTransceiver_Stop(UARTTransceiver_T *transceiver);

/**
 * @brief It reads the data received by the transceiver.
 *
 * The transceiver must be started and not suspended when calling
 * this function.
 *
 * In the synchronous mode, this call will return only after the End-of-Frame
 * check function which is used by the transceiver returns true. This holds even
 * if the number of the received bytes exceeds the given buffer size.
 * In the asynchronous mode, the function
 * copies the bytes which have been already received an returns immediately.
 * In both cases, the number bytes saved to the given buffer never exceeds
 * the given buffer size. If more bytes are available in the transceiver internal
 * buffer, they will reside there.
 *
 *
 * @param[in] transceiver
 * a pointer to the transceiver
 *
 * @param[in] buffer
 * The buffer which the bytes are copied to.
 *
 * @param[in] size
 * The buffer size. Independent of the number of the bytes which the
 * transceiver has received, no more bytes are saved to the given buffer
 * than the given size.
 *
 * @param[out] length
 * The number of received bytes which were saved to the given buffer.
 *
 * @param[in] timeout_ms
 * in synchronous modeThe function will return with an error if this time elapsed
 * before receiving the end of frame character
 *
 * @retval #RETCODE_OK                  if successfully started
 * @retval #RETCODE_INVALID_PARAM       if any parameter is NULL
 * @retval #RETCODE_INCONSITENT_STATE   if the transceiver is not in an active
 *                                      state (see #UARTTransceiver_State_E)
 * @retval #RETCODE_SEMAPHORE_ERROR     if semaphore could not be taken with given timeout
 *                                      (see #UARTTransceiver_T.RxSemaphore)
 */
Retcode_T UARTTransceiver_ReadData(
        UARTTransceiver_T *transceiver,
        uint8_t *buffer, uint32_t size, uint32_t *length, uint32_t timeout_ms);

/**
 * @brief It writes the data for sending.
 *
 * The transceiver must be started and not suspended when calling
 * this function.
 *
 * In the synchronous mode, this call will return only after the passed
 * bytes have been send over UART/LEUART. In the asynchronous mode, the function
 * triggers the sending and returns immediately. A callback follows to inform
 * about the success or failure of the send operation.
 *
 * @param[in] transceiver
 * a pointer to the transceiver
 *
 * @param[in] buffer
 * The buffer which the bytes are copied to.
 *
 * @param[in] length
 * The buffer length and the number of bytes to be copied.
 * @param[in] timeout_ms
 * The time afterwhich if the function does not succeed for any reason it will
 * return an semaphore error.
 *
 * @retval #RETCODE_OK                  if successfully started
 * @retval #RETCODE_INVALID_PARAM       if any parameter is NULL
 * @retval #RETCODE_INCONSITENT_STATE   if the transceiver is not in an active
 *                                      state (see #UARTTransceiver_State_E)
 * @retval #RETCODE_SEMAPHORE_ERROR     if semaphore could not be taken with given timeout
 *                                      (see #UARTTransceiver_T.TxSemaphore)
 */
Retcode_T UARTTransceiver_WriteData(
        UARTTransceiver_T *transceiver,
        uint8_t* data, uint32_t length, uint32_t timeout_ms);



#if BCDS_FEATURE_UART
/**
 * @brief Function to loop the UART/LEUART callback.
 *
 * @details It is necessary for the UARTTransceiver to loop the UART/LEUART callback.
 * This means, the middle-layer driver which uses UART/LEUART and UARTTransceiver
 * must initialize UART/LEUART with a callback function which invokes
 * this function in order to loop the callbacks from UART/LEUART to the
 * transceiver.
 *
 * The middle-layer driver may use the macro
 * UART_TRANSCEIVER_DECLARE_LOOP_CALLBACK can be used to generate
 * such a callback.
 *
 * @param[in] transceiver
 * The transceiver
 *
 *
 * @param[in] event
 * The event which is notified by the callback
 */
void UARTTransceiver_LoopCallback(
        UARTTransceiver_T *transceiver,
        struct MCU_UART_Event_S event);
/**
 * @brief Generates a UART loop callback.
 *
 * @details The middle-layer driver, which uses UART/LEUART and UARTTransceiver,
 * may use this macro in oder to generate a callback function
 * to initialize UART/LEUART with it. The generated function
 * loops the callback to the transceiver by calling the function
 * UARTTransceiver_LoopCallback().
 *
 * @param[in] transceiver
 * The variable name of the transceiver. It must be a pointer to
 * the context structure but the context structure itself.
 */
#define UART_TRANSCEIVER_DECLARE_LOOP_CALLBACK(transceiver) \
static BCDS_ALWAYS_INLINE void UARTTransceiverLoopCallback(UART_T uart, struct MCU_UART_Event_S event) \
{ \
	UARTTransceiver_LoopCallback(&transceiver, event);\
}

#elif BCDS_FEATURE_LEUART

/**
 * @brief Function to loop the UART/LEUART callback.
 *
 * @details It is necessary for the UARTTransceiver to loop the UART/LEUART callback.
 * This means, the middle-layer driver which uses UART/LEUART and UARTTransceiver
 * must initialize UART/LEUART with a callback function which invokes
 * this function in order to loop the callbacks from UART/LEUART to the
 * transceiver.
 *
 * The middle-layer driver may use the macro
 * UART_TRANSCEIVER_DECLARE_LOOP_CALLBACK can be used to generate
 * such a callback.
 *
 * @param[in] transceiver
 * The transceiver
 *
 * @param[in] event
 * The event which is notified by the callback
 */
void UARTTransceiver_LoopCallbackLE(
        UARTTransceiver_T *transceiver,
        struct MCU_LEUART_Event_S event);
/**
 * @brief Generates a LEUART loop callback.
 *
 * @details The middle-layer driver, which uses UART/LEUART and UARTTransceiver,
 * may use this macro in oder to generate a callback function
 * to initialize UART/LEUART with it. The generated function
 * loops the callback to the transceiver by calling the function
 * UARTTransceiver_LoopCallback().
 *
 * @param[in] transceiver
 * The variable name of the transceiver. It must be a pointer to
 * the context structure but the context structure itself.
 */
#define UART_TRANSCEIVER_DECLARE_LOOP_CALLBACK_LE(transceiver) \
static BCDS_ALWAYS_INLINE void UARTTransceiverLoopCallbackLE(LEUART_T leuart, struct MCU_LEUART_Event_S event) \
{ \
    UARTTransceiver_LoopCallbackLE(&transceiver, event);\
}
#endif /* BCDS_FEATURE_LEUART */

#endif /* BCDS_FEATURE_UART || BCDS_FEATURE_LEUART */

#endif /* if BCDS_FEATURE_UARTTRANSCEIVER */

#endif /* UARTTRANSCEIVER_H_ */
/**@} */
