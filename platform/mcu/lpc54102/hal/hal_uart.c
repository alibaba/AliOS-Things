/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * POSIX getopt for Windows
 * Code given out at the 1985 UNIFORUM conference in Dallas.
 *
 * From std-unix@ut-sally.UUCP (Moderator, John Quarterman) Sun Nov  3 14:34:15 1985
 * Relay-Version: version B 2.10.3 4.3bsd-beta 6/6/85; site gatech.CSNET
 * Posting-Version: version B 2.10.2 9/18/84; site ut-sally.UUCP
 * Path: gatech!akgua!mhuxv!mhuxt!mhuxr!ulysses!allegra!mit-eddie!genrad!panda!talcott!harvard!seismo!ut-sally!std-unix
 * From: std-unix@ut-sally.UUCP (Moderator, John Quarterman)
 * Newsgroups: mod.std.unix
 * Subject: public domain AT&T getopt source
 * Message-ID: <3352@ut-sally.UUCP>
 * Date: 3 Nov 85 19:34:15 GMT
 * Date-Received: 4 Nov 85 12:25:09 GMT
 * Organization: IEEE/P1003 Portable Operating System Environment Committee
 * Lines: 91
 * Approved: jsq@ut-sally.UUC
 * Here's something you've all been waiting for:  the AT&T public domain
 * source for getopt(3).  It is the code which was given out at the 1985
 * UNIFORUM conference in Dallas.  I obtained it by electronic mail
 * directly from AT&T.  The people there assure me that it is indeed
 * in the public domain
 * There is no manual page.  That is because the one they gave out at
 * UNIFORUM was slightly different from the current System V Release 2
 * manual page.  The difference apparently involved a note about the
 * famous rules 5 and 6, recommending using white space between an option
 * and its first argument, and not grouping options that have arguments.
 * Getopt itself is currently lenient about both of these things White
 * space is allowed, but not mandatory, and the last option in a group can
 * have an argument.  That particular version of the man page evidently
 * has no official existence, and my source at AT&T did not send a copy.
 * The current SVR2 man page reflects the actual behavor of this getopt.
 * However, I am not about to post a copy of anything licensed by AT&T.
 */
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <k_api.h>
#include <aos/aos.h>
#include "hal/hal.h"
#include "hal/soc/soc.h"
#include "fsl_device_registers.h"
#include "fsl_common.h"
#include "fsl_clock.h"
#include "fsl_usart.h"

#include "fsl_debug_console.h"

#define LOG_TAG		"uart"


static const uint32_t s_uartBaseAddrs[FSL_FEATURE_SOC_USART_COUNT] = USART_BASE_ADDRS;

/* Global Variable for VFS DEBUG Output */
uart_dev_t uart_0 = {
   .port = 0,    /* uart port */
   .config = {115200, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED},  /* uart config */
   .priv = NULL    /* priv data */
};

uart_dev_t uart_1 = {
   .port = 1,    /* uart port */
   .config = {115200, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED},  /* uart config */
   .priv = NULL    /* priv data */
};

uart_dev_t uart_2 = {
   .port = 2,    /* uart port */
   .config = {115200, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED},  /* uart config */
   .priv = NULL    /* priv data */
};

uart_dev_t uart_3 = {
   .port = 3,    /* uart port */
   .config = {115200, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED},  /* uart config */
   .priv = NULL    /* priv data */
};

uart_dev_t uart_4 = {
   .port = 4,    /* uart port */
   .config = {115200, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED},  /* uart config */
   .priv = NULL    /* priv data */
};


#define UART_RING_BUFFER_SIZE	1024
static usart_handle_t *uart_handle[FSL_FEATURE_SOC_USART_COUNT];
static uint8_t *uart_ringbuffer[FSL_FEATURE_SOC_USART_COUNT];
static ksem_t uart_recv_sem[FSL_FEATURE_SOC_USART_COUNT];
static ksem_t uart_send_sem[FSL_FEATURE_SOC_USART_COUNT];
static kmutex_t uart_recv_lock[FSL_FEATURE_SOC_USART_COUNT];
static kmutex_t uart_send_lock[FSL_FEATURE_SOC_USART_COUNT];

static void hal_uart_callback(USART_Type *base, usart_handle_t *handle, status_t status, void *userData)
{
   uart_dev_t *uart = (uart_dev_t *)userData;

   if (!uart)
      return;

   if (status == kStatus_USART_RxError)
      LOGE(LOG_TAG, "%s: rx error !", __func__);
   if (status == kStatus_USART_RxRingBufferOverrun)
      LOGE(LOG_TAG, "%s: rx ring buffer overrun !", __func__);
   if (status == kStatus_USART_RxIdle)
      krhino_sem_give(&uart_recv_sem[uart->port]);
   if (status == kStatus_USART_TxIdle)
      krhino_sem_give(&uart_send_sem[uart->port]);
}


/**
 * Initialises a UART interface
 *
 *
 * @param[in]  uart  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_init(uart_dev_t *uart)
{
   usart_config_t config = {0};
   status_t status;

   config.baudRate_Bps = uart->config.baud_rate;
   config.enableRx = true;
   config.enableTx = true;
   config.loopback = false;
   config.fifoConfig.enableTxFifo = false;
   config.fifoConfig.enableRxFifo = false;

   if (uart->port != 0) {
      config.fifoConfig.enableTxFifo = true;
      config.fifoConfig.enableRxFifo = true;
      config.fifoConfig.txFifoSize = 8;
      config.fifoConfig.rxFifoSize = 8;
      config.fifoConfig.txFifoThreshold = 0;
      config.fifoConfig.rxFifoThreshold = 0;
   }
 
   switch(uart->config.parity)
   {
      case NO_PARITY:
          config.parityMode = kUSART_ParityDisabled;
          break;
      case ODD_PARITY:
          config.parityMode = kUSART_ParityOdd;
          break;
      case EVEN_PARITY:
          config.parityMode = kUSART_ParityEven;
          break;
      default:
          return -EIO;
   }

   switch(uart->config.data_width) 
   {
      case DATA_WIDTH_7BIT:
          config.bitCountPerChar = kUSART_7BitsPerChar;
          break;
      case DATA_WIDTH_8BIT:
          config.bitCountPerChar = kUSART_8BitsPerChar;
          break;
      default:
          return -EIO;
   }

   switch(uart->config.stop_bits)
   {
      case STOP_BITS_1:
         config.stopBitCount = kUSART_OneStopBit;
         break;
      case STOP_BITS_2:
         config.stopBitCount = kUSART_TwoStopBit;
         break;
      default:
         return -EIO;
   }
   
   status = USART_Init((USART_Type *)s_uartBaseAddrs[uart->port], &config, CLOCK_GetFreq(kCLOCK_Usart));

   if(kStatus_Success != status)
      return -EIO;

   if (uart->port != 0) {
      if (krhino_sem_create(&uart_send_sem[uart->port], "uartSendSem", 0))
         return -EIO;
      if (krhino_sem_create(&uart_recv_sem[uart->port], "uartRecvSem", 0))
         return -EIO;
      if (krhino_mutex_create(&uart_send_lock[uart->port], "uartSendLock"))
         return -EIO;
      if (krhino_mutex_create(&uart_recv_lock[uart->port], "uartRecvLock"))
         return -EIO;

      uart_handle[uart->port] = (usart_handle_t *)aos_malloc(sizeof(usart_handle_t));
      if (!uart_handle[uart->port]) {
         LOGE(LOG_TAG, "%s: alloc uart handle failed !", __func__);
         return -ENOMEM;
      }

      uart_ringbuffer[uart->port] = (uint8_t *)aos_malloc(UART_RING_BUFFER_SIZE);
      if (!uart_ringbuffer[uart->port]) {
         LOGE(LOG_TAG, "%s: alloc uart ring buffer failed !", __func__);
         free(uart_handle[uart->port]);
         return -ENOMEM;
      }

      USART_TransferCreateHandle((USART_Type *)s_uartBaseAddrs[uart->port],
         uart_handle[uart->port], hal_uart_callback, uart);
      USART_TransferStartRingBuffer((USART_Type *)s_uartBaseAddrs[uart->port],
         uart_handle[uart->port], uart_ringbuffer[uart->port], UART_RING_BUFFER_SIZE);
	  LOG("%s: init uart%u success", __func__, uart->port);
   }
   return 0;
}

/**
 * Transmit data on a UART interface
 *
 * @param[in]  uart  the UART interface
 * @param[in]  data  pointer to the start of data
 * @param[in]  size  number of bytes to transmit
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
   USART_Type *base = (USART_Type *)s_uartBaseAddrs[uart->port];
   usart_transfer_t xfer;
   uint8_t send_data[size];
   int32_t ret = 0;

   if (timeout <= 0)
      timeout = 1000;

   if (uart_handle[uart->port]) {
      if (krhino_mutex_lock(&uart_send_lock[uart->port], timeout))
         return -EIO;
      memcpy(send_data, data, size);
      xfer.data = send_data;
      xfer.dataSize = size;
      USART_TransferSendNonBlocking(base, uart_handle[uart->port], &xfer);
      if (krhino_sem_take(&uart_send_sem[uart->port], timeout) != RHINO_SUCCESS) {
         LOGD(LOG_TAG, "%s: send %u bytes timeout !", __func__, size);
         USART_TransferAbortSend(base, uart_handle[uart->port]);
         krhino_sem_count_set(&uart_send_sem[uart->port], 0);
         ret = -EIO;
      }
	  krhino_mutex_unlock(&uart_send_lock[uart->port]);
   } else {
      USART_WriteBlocking((USART_Type *)s_uartBaseAddrs[uart->port], data, size);
   }

   return ret;
}

/**
 * Receive data on a UART interface
 *
 * @param[in]   uart         the UART interface
 * @param[out]  data         pointer to the buffer which will store incoming data
 * @param[in]   expect_size  number of bytes to receive
 * @param[out]  recv_size    number of bytes received
 * @param[in]   timeout      timeout in milisecond
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
   USART_Type *base = (USART_Type *)s_uartBaseAddrs[uart->port];
   uint8_t *data8 = (uint8_t *)data;
   uint32_t status;
   uint32_t count = 0;
   uint32_t instance = 0U;
   usart_transfer_t xfer;

   if(recv_size != NULL)
      *recv_size = 0;

   if (timeout <= 0)
      timeout = 1000;

   instance = USART_GetInstance(base);

   if (uart_handle[uart->port]) {
   	  memset(&xfer, 0, sizeof(usart_transfer_t));
   	  if (krhino_mutex_lock(&uart_recv_lock[uart->port], timeout))
   	     return -EFAULT;
   	  xfer.data = data8;
   	  xfer.dataSize = expect_size;
   	  if (USART_TransferReceiveNonBlocking(base, uart_handle[uart->port], &xfer, recv_size) != kStatus_Success) {
   	     LOGD(LOG_TAG, "%s: recv %u bytes failed !", __func__, expect_size);
   	     krhino_mutex_unlock(&uart_recv_lock[uart->port]);
   	     return -EIO;
   	  }
   	  if (krhino_sem_take(&uart_recv_sem[uart->port], timeout) != RHINO_SUCCESS) {
   	     LOGD(LOG_TAG, "%s: recv %u bytes timeout !", __func__, expect_size);
   	     USART_TransferAbortReceive(base, uart_handle[uart->port]);
   	     krhino_sem_count_set(&uart_recv_sem[uart->port], 0);
   	     *recv_size = 0;
   	  } else {
   	     *recv_size = expect_size;
   	  }
   	  krhino_mutex_unlock(&uart_recv_lock[uart->port]);
   } else {
      for (; expect_size > 0; expect_size--)
      {
         if (USART_IsRxFifoEnable(base)) 
         {
            if((VFIFO->USART[instance].STATUSART & VFIFO_USART_STATUSART_RXEMPTY_MASK))
            {
               return -EIO;
            }

            status = VFIFO->USART[instance].STATUSART;
            if (status & VFIFO_USART_STATUSART_BUSERR_MASK)
            {
               return -EIO;
            }
            status = VFIFO->USART[instance].RXDATSTATUSART;
            if (status & VFIFO_USART_RXDATSTATUSART_FRAMERR_MASK)
            {
               return -EIO;
            }
            if (status & VFIFO_USART_RXDATSTATUSART_PARITYERR_MASK)
            {
               return -EIO;
            }
            if (status & VFIFO_USART_RXDATSTATUSART_RXNOISE_MASK)
            {
               return -EIO;
            }
            *data8 = (status & VFIFO_USART_RXDATSTATUSART_RXDAT_MASK);
         }
         else
         {
            if (!(base->STAT & USART_STAT_RXRDY_MASK))
            {
               return -EIO;
            }
            /* Check receive status */
            status = base->STAT;

            if (status & USART_STAT_FRAMERRINT_MASK)
            {
               base->STAT |= USART_STAT_FRAMERRINT_MASK;
               return -EIO;
            }
            if (status & USART_STAT_PARITYERRINT_MASK)
            {
               base->STAT |= USART_STAT_PARITYERRINT_MASK;
               return -EIO;
            }
            if (status & USART_STAT_RXNOISEINT_MASK)
            {
                base->STAT |= USART_STAT_RXNOISEINT_MASK;
                return -EIO;
            }
            if (base->STAT & USART_STAT_OVERRUNINT_MASK)
            {
                base->STAT |= USART_STAT_OVERRUNINT_MASK;
                return -EIO;
            }
            *data8 = base->RXDAT;
         }
         data8++;
         count++;
         if(recv_size != NULL)
            *recv_size = count;
      }
   }

   return 0;
}

/**
 * Deinitialises a UART interface
 *
 * @param[in]  uart  the interface which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_finalize(uart_dev_t *uart)
{
   USART_Deinit((USART_Type *)s_uartBaseAddrs[uart->port]);
   if (uart_handle[uart->port])
      free(uart_handle[uart->port]);
   if (uart_ringbuffer[uart->port])
      free(uart_ringbuffer[uart->port]);
   if (uart->port != 0) {
      krhino_sem_del(&uart_send_sem[uart->port]);
      krhino_sem_del(&uart_recv_sem[uart->port]);
      krhino_mutex_del(&uart_send_lock[uart->port]);
      krhino_mutex_del(&uart_recv_lock[uart->port]);
   }
   return 0;
}


