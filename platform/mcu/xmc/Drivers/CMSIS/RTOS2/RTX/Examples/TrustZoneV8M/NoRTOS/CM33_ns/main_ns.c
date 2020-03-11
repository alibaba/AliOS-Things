/*
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
 * ----------------------------------------------------------------------
 *
 * main_ns.c      Non-secure main function 
 *
 * Version 1.0
 *    Initial Release
 *---------------------------------------------------------------------------*/

#include "..\CM33_s\interface.h"        // Interface API

extern volatile int val1, val2;
volatile int val1, val2;

/* Non-secure function */
int func3 (int x); 

int func3 (int x)  {
  return (x+4);
}

/* Non-secure main() */
int main(void) {
 
  /* Call non-secure callable function func1 */
  val1 = func1 (1);
 
  /* Call non-secure callable function func2
     with callback to non-secure function func3 */
  val2 = func2 (func3, 2);
 
  while (1);
}
