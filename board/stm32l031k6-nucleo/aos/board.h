/**
 ******************************************************************************
 * @file    board.h
 * @author  MCU China FAE team
 * @version 1.0
 * @date    23/11/2018
 * @brief   aos porting layer
 ******************************************************************************
 *
 * COPYRIGHT(c) 2018 STMicroelectronics
 *
 ******************************************************************************
 */
#define HARDWARE_REVISION   "V1.0"
#define MODEL               "STM32L0"

#ifdef BOOTLOADER
#define STDIO_UART 0
#define STDIO_UART_BUADRATE 115200
#else
#define STDIO_UART 2
#define STDIO_UART_BUADRATE 115200
#endif
