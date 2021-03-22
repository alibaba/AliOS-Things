/*
 * Copyright (c) 2013-2017 ARM Limited. All rights reserved.
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
 * $Date:        1. December 2017
 * $Revision:    V2.0.0
 *
 * Project:      CMSIS-DAP Validation
 * Title:        test.c CMSIS-DAP debug unit test module
 *
 *---------------------------------------------------------------------------*/

// Debug Variables
volatile int  test_state = 0;
volatile int  test_success = 0;
volatile int  bpTestCounter = 0;
volatile char mem_rw_success = 0;
         int  test_array1[256] = {0}; 
         int  test_array2[256] = {0};

// Breakpoint Test function
static void BP_Test (void) {
  int i;

  for (i = 0; i < 10; i++) {
    // increment counter so we know on which iteration breakpoint is hit
    bpTestCounter++;
    test_state++;
  }
}

// Test function
static void Test(void) {
  int i;

  test_state++;                 // 'test_state' = 11
  i = test_success;             // 'test_success' read access

  test_state++;                 // 'test_state' = 12
  test_success = i;             // 'test_success' write access

  test_state++;                 // 'test_state' = 13

  // test_array1 should have already been written by debugger
  // copy test_array1 into test_array2 for future comparison
  mem_rw_success = 1;           // assume all values were written correctly
  for (i = 0; i < 256; i++) {
    if (test_array1[i] != (0x1000+i)) {
      mem_rw_success = 0;
    }
    test_array2[i] = test_array1[i];
  }

  test_state++;                 // 'test_state' = 14
  test_state++;                 // 'test_state' = 15
  test_state++;                 // 'test_state' = 16
  // execute 'test_state -= 16' from debugger
  test_state++;                 // 'test_state' = 1

  if (test_state == 1) {
    test_success = 1;
  } else {
    test_success = 0;
  }
}

// 'main' function
int main (void) {

  BP_Test();
  Test();

  for (;;) {};
}
