/* -------------------------------------------------------------------------- 
 * Copyright (c) 2013-2016 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Name:    ITM_Retarget.c
 * Purpose: CMSIS Retarget output to ITM Channel 0 template file
 *
 *----------------------------------------------------------------------------*/

#include <stdio.h> 

#include "LPC18xx.h"                    /* LPC18xx Definitions                */

#pragma import(__use_no_semihosting_swi)

volatile int ITM_RxBuffer = ITM_RXBUFFER_EMPTY;  /*  CMSIS Debug Input        */

int fputc(int c, FILE *f) {
  int  i;
  for (i = 10; i; i--) __NOP();
  return (ITM_SendChar(c));
}

int fgetc(FILE *f) {
  while (ITM_CheckChar() != 1) __NOP();
  return (ITM_ReceiveChar());
}
