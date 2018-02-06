/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __ESP_SSC_H__
#define __ESP_SSC_H__

#ifdef __cplusplus
extern "C" {
#endif

#define CMD_T_ASYNC   0x01
#define CMD_T_SYNC    0x02

typedef struct cmd_s {
    char *cmd_str;
    uint8 flag;
    uint8 id;
    void (* cmd_func)(void);
    void (* cmd_callback)(void *arg);
} ssc_cmd_t;

#define MAX_LINE_N  127

typedef enum {
    SSC_BR_9600   = 9600,
    SSC_BR_19200  = 19200,
    SSC_BR_38400  = 38400,
    SSC_BR_57600  = 57600,
    SSC_BR_74880  = 74880,
    SSC_BR_115200 = 115200,
    SSC_BR_230400 = 230400,
    SSC_BR_460800 = 460800,
    SSC_BR_921600 = 921600
} SscBaudRate;

/** \defgroup SSC_APIs SSC APIs
  * @brief SSC APIs
  *
  * SSC means simple serial command.
  * SSC APIs allows users to define their own command, users can refer to spiffs_test/test_main.c.
  *
  */

/** @addtogroup SSC_APIs
  * @{
  */

/**
  * @brief  Initial the ssc function.
  *
  * @param  SscBaudRate bandrate : baud rate
  *
  * @return null
  */
void ssc_attach(SscBaudRate bandrate);

/**
  * @brief   Get the length of the simple serial command.
  *
  * @param   null
  *
  * @return  length of the command.
  */
int ssc_param_len(void);

/**
  * @brief   Get the simple serial command string.
  *
  * @param   null
  *
  * @return  the command.
  */
char *ssc_param_str(void);

/**
  * @brief   Parse the simple serial command (ssc).
  *
  * @param   char *pLine  : [input] the ssc string
  * @param   char *argv[] : [output] parameters of the ssc
  *
  * @return  the number of parameters.
  */
int ssc_parse_param(char *pLine, char *argv[]);

/**
  * @brief  Register the user-defined simple serial command (ssc) set.
  *
  * @param  ssc_cmd_t *cmdset   : the ssc set
  * @param  uint8 cmdnum        : number of commands
  * @param  void (* help)(void) : callback of user-guide
  *
  * @return  null
  */
void ssc_register(ssc_cmd_t *cmdset, uint8 cmdnum, void (* help)(void));

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __ESP_SSC_H__ */
