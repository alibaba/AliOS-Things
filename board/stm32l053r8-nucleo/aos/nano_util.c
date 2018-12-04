/**
 ******************************************************************************
 * @file    nano_util.c
 * @author  MCU China FAE team
 * @version 1.0
 * @date    23/11/2018
 * @brief   utility of nano kernel certification
 ******************************************************************************
 *
 * COPYRIGHT(c) 2018 STMicroelectronics
 *
 ******************************************************************************
 */
#include <stdarg.h>
#include "stm32l0xx.h"
#include "stm32l0xx_hal.h"
#include "hal/hal.h"
#include "nano_util.h"

#define BUFSIZE 256
/* buffer */
static char buff[BUFSIZE];
extern UART_HandleTypeDef hal_uart_handle[];

void nano_util_Send(char *format, ...) {
  va_list args;
  va_start(args, format);
  uint8_t len;
  uint8_t lenTop;

  /*convert into string at buff[0] of length iw*/
  len = vsprintf(&buff[0], format, args);
  va_end(args);
  HAL_UART_Transmit(&hal_uart_handle[2], (uint8_t *)buff, len, 300);
}

/*===Begin of stub. Please remove it if not the case of nano certification===*/
off_t aos_lseek(int fd, off_t offset, int whence) { return 0; }

int aos_open(const char *path, int flags) { return 0; }

int aos_close(int fd) { return 0; }

ssize_t aos_read(int fd, void *buf, size_t nbytes) { return 0; }

ssize_t aos_write(int fd, const void *buf, size_t nbytes) { return 0; }

void *aos_malloc(unsigned int size) { return 0; }

void aos_free(void *mem) { return; }

void *krhino_task_find(char *name) { return NULL; }

int csp_printf(const char *fmt, ...) { return 0; }

void trace_start(void) {}
unsigned int aos_log_level = 0;
/*===End of stub. Please remove it if not the case of nano certification===*/