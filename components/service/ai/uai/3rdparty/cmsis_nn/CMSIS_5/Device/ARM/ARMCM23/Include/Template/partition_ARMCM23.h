/**************************************************************************//**
 * @file     partition_ARMCM23.h
 * @brief    CMSIS-CORE Initial Setup for Secure / Non-Secure Zones for ARMCM23
 * @version  V5.3.1
 * @date     09. July 2018
 ******************************************************************************/
/*
 * Copyright (c) 2009-2018 Arm Limited. All rights reserved.
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
 */

#ifndef PARTITION_ARMCM23_H
#define PARTITION_ARMCM23_H

/*
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
*/

/*
// <e>Initialize Security Attribution Unit (SAU) CTRL register
*/
#define SAU_INIT_CTRL          1

/*
//   <q> Enable SAU
//   <i> Value for SAU->CTRL register bit ENABLE
*/
#define SAU_INIT_CTRL_ENABLE   1

/*
//   <o> When SAU is disabled
//     <0=> All Memory is Secure
//     <1=> All Memory is Non-Secure
//   <i> Value for SAU->CTRL register bit ALLNS
//   <i> When all Memory is Non-Secure (ALLNS is 1), IDAU can override memory map configuration.
*/
#define SAU_INIT_CTRL_ALLNS  0

/*
// </e>
*/

/*
// <h>Initialize Security Attribution Unit (SAU) Address Regions
// <i>SAU configuration specifies regions to be one of:
// <i> - Secure and Non-Secure Callable
// <i> - Non-Secure
// <i>Note: All memory regions not configured by SAU are Secure
*/
#define SAU_REGIONS_MAX   8                 /* Max. number of SAU regions */

/*
//   <e>Initialize SAU Region 0
//   <i> Setup SAU Region 0 memory attributes
*/
#define SAU_INIT_REGION0    1

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START0     0x00000000      /* start address of SAU region 0 */

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END0       0x001FFFFF      /* end address of SAU region 0 */

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC0       1
/*
//   </e>
*/

/*
//   <e>Initialize SAU Region 1
//   <i> Setup SAU Region 1 memory attributes
*/
#define SAU_INIT_REGION1    1

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START1     0x00200000

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END1       0x003FFFFF

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC1       0
/*
//   </e>
*/

/*
//   <e>Initialize SAU Region 2
//   <i> Setup SAU Region 2 memory attributes
*/
#define SAU_INIT_REGION2    1

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START2     0x20200000

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END2       0x203FFFFF

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC2       0
/*
//   </e>
*/

/*
//   <e>Initialize SAU Region 3
//   <i> Setup SAU Region 3 memory attributes
*/
#define SAU_INIT_REGION3    1

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START3     0x40000000

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END3       0x40040000

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC3       0
/*
//   </e>
*/

/*
//   <e>Initialize SAU Region 4
//   <i> Setup SAU Region 4 memory attributes
*/
#define SAU_INIT_REGION4    0

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START4     0x00000000      /* start address of SAU region 4 */

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END4       0x00000000      /* end address of SAU region 4 */

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC4       0
/*
//   </e>
*/

/*
//   <e>Initialize SAU Region 5
//   <i> Setup SAU Region 5 memory attributes
*/
#define SAU_INIT_REGION5    0

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START5     0x00000000

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END5       0x00000000

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC5       0
/*
//   </e>
*/

/*
//   <e>Initialize SAU Region 6
//   <i> Setup SAU Region 6 memory attributes
*/
#define SAU_INIT_REGION6    0

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START6     0x00000000

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END6       0x00000000

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC6       0
/*
//   </e>
*/

/*
//   <e>Initialize SAU Region 7
//   <i> Setup SAU Region 7 memory attributes
*/
#define SAU_INIT_REGION7    0

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START7     0x00000000

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END7       0x00000000

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC7       0
/*
//   </e>
*/

/*
// </h>
*/

/*
// <e>Setup behaviour of Sleep and Exception Handling
*/
#define SCB_CSR_AIRCR_INIT  1

/*
//   <o> Deep Sleep can be enabled by
//     <0=>Secure and Non-Secure state
//     <1=>Secure state only
//   <i> Value for SCB->CSR register bit DEEPSLEEPS
*/
#define SCB_CSR_DEEPSLEEPS_VAL  1

/*
//   <o>System reset request accessible from
//     <0=> Secure and Non-Secure state
//     <1=> Secure state only
//   <i> Value for SCB->AIRCR register bit SYSRESETREQS
*/
#define SCB_AIRCR_SYSRESETREQS_VAL  1

/*
//   <o>Priority of Non-Secure exceptions is
//     <0=> Not altered
//     <1=> Lowered to 0x80-0xFF
//   <i> Value for SCB->AIRCR register bit PRIS
*/
#define SCB_AIRCR_PRIS_VAL      1

/*
//   <o>BusFault, HardFault, and NMI target
//     <0=> Secure state
//     <1=> Non-Secure state
//   <i> Value for SCB->AIRCR register bit BFHFNMINS
*/
#define SCB_AIRCR_BFHFNMINS_VAL 0

/*
// </e>
*/


/*
// <e>Setup behaviour of single SysTick
*/
#define SCB_ICSR_INIT 0

/*
//   <o> in a single SysTick implementation, SysTick is
//     <0=>Secure
//     <1=>Non-Secure
//   <i> Value for SCB->ICSR register bit STTNS
//   <i> only for single SysTick implementation 
*/
#define SCB_ICSR_STTNS_VAL  0

/*
// </e>
*/


/*
// <h>Setup Interrupt Target
*/

/*
//   <e>Initialize ITNS 0 (Interrupts 0..31)
*/
#define NVIC_INIT_ITNS0    1

/*
// Interrupts 0..31
//   <o.0>  Interrupt 0   <0=> Secure state <1=> Non-Secure state
//   <o.1>  Interrupt 1   <0=> Secure state <1=> Non-Secure state
//   <o.2>  Interrupt 2   <0=> Secure state <1=> Non-Secure state
//   <o.3>  Interrupt 3   <0=> Secure state <1=> Non-Secure state
//   <o.4>  Interrupt 4   <0=> Secure state <1=> Non-Secure state
//   <o.5>  Interrupt 5   <0=> Secure state <1=> Non-Secure state
//   <o.6>  Interrupt 6   <0=> Secure state <1=> Non-Secure state
//   <o.7>  Interrupt 7   <0=> Secure state <1=> Non-Secure state
//   <o.8>  Interrupt 8   <0=> Secure state <1=> Non-Secure state
//   <o.9>  Interrupt 9   <0=> Secure state <1=> Non-Secure state
//   <o.10> Interrupt 10  <0=> Secure state <1=> Non-Secure state
//   <o.11> Interrupt 11  <0=> Secure state <1=> Non-Secure state
//   <o.12> Interrupt 12  <0=> Secure state <1=> Non-Secure state
//   <o.13> Interrupt 13  <0=> Secure state <1=> Non-Secure state
//   <o.14> Interrupt 14  <0=> Secure state <1=> Non-Secure state
//   <o.15> Interrupt 15  <0=> Secure state <1=> Non-Secure state
//   <o.16> Interrupt 16  <0=> Secure state <1=> Non-Secure state
//   <o.17> Interrupt 17  <0=> Secure state <1=> Non-Secure state
//   <o.18> Interrupt 18  <0=> Secure state <1=> Non-Secure state
//   <o.19> Interrupt 19  <0=> Secure state <1=> Non-Secure state
//   <o.20> Interrupt 20  <0=> Secure state <1=> Non-Secure state
//   <o.21> Interrupt 21  <0=> Secure state <1=> Non-Secure state
//   <o.22> Interrupt 22  <0=> Secure state <1=> Non-Secure state
//   <o.23> Interrupt 23  <0=> Secure state <1=> Non-Secure state
//   <o.24> Interrupt 24  <0=> Secure state <1=> Non-Secure state
//   <o.25> Interrupt 25  <0=> Secure state <1=> Non-Secure state
//   <o.26> Interrupt 26  <0=> Secure state <1=> Non-Secure state
//   <o.27> Interrupt 27  <0=> Secure state <1=> Non-Secure state
//   <o.28> Interrupt 28  <0=> Secure state <1=> Non-Secure state
//   <o.29> Interrupt 29  <0=> Secure state <1=> Non-Secure state
//   <o.30> Interrupt 30  <0=> Secure state <1=> Non-Secure state
//   <o.31> Interrupt 31  <0=> Secure state <1=> Non-Secure state
*/
#define NVIC_INIT_ITNS0_VAL      0x00000000

/*
//   </e>
*/

/*
//   <e>Initialize ITNS 1 (Interrupts 32..63)
*/
#define NVIC_INIT_ITNS1    1

/*
// Interrupts 32..63
//   <o.0>  Interrupt 32  <0=> Secure state <1=> Non-Secure state
//   <o.1>  Interrupt 33  <0=> Secure state <1=> Non-Secure state
//   <o.2>  Interrupt 34  <0=> Secure state <1=> Non-Secure state
//   <o.3>  Interrupt 35  <0=> Secure state <1=> Non-Secure state
//   <o.4>  Interrupt 36  <0=> Secure state <1=> Non-Secure state
//   <o.5>  Interrupt 37  <0=> Secure state <1=> Non-Secure state
//   <o.6>  Interrupt 38  <0=> Secure state <1=> Non-Secure state
//   <o.7>  Interrupt 39  <0=> Secure state <1=> Non-Secure state
//   <o.8>  Interrupt 40  <0=> Secure state <1=> Non-Secure state
//   <o.9>  Interrupt 41  <0=> Secure state <1=> Non-Secure state
//   <o.10> Interrupt 42  <0=> Secure state <1=> Non-Secure state
//   <o.11> Interrupt 43  <0=> Secure state <1=> Non-Secure state
//   <o.12> Interrupt 44  <0=> Secure state <1=> Non-Secure state
//   <o.13> Interrupt 45  <0=> Secure state <1=> Non-Secure state
//   <o.14> Interrupt 46  <0=> Secure state <1=> Non-Secure state
//   <o.15> Interrupt 47  <0=> Secure state <1=> Non-Secure state
//   <o.16> Interrupt 48  <0=> Secure state <1=> Non-Secure state
//   <o.17> Interrupt 49  <0=> Secure state <1=> Non-Secure state
//   <o.18> Interrupt 50  <0=> Secure state <1=> Non-Secure state
//   <o.19> Interrupt 51  <0=> Secure state <1=> Non-Secure state
//   <o.20> Interrupt 52  <0=> Secure state <1=> Non-Secure state
//   <o.21> Interrupt 53  <0=> Secure state <1=> Non-Secure state
//   <o.22> Interrupt 54  <0=> Secure state <1=> Non-Secure state
//   <o.23> Interrupt 55  <0=> Secure state <1=> Non-Secure state
//   <o.24> Interrupt 56  <0=> Secure state <1=> Non-Secure state
//   <o.25> Interrupt 57  <0=> Secure state <1=> Non-Secure state
//   <o.26> Interrupt 58  <0=> Secure state <1=> Non-Secure state
//   <o.27> Interrupt 59  <0=> Secure state <1=> Non-Secure state
//   <o.28> Interrupt 60  <0=> Secure state <1=> Non-Secure state
//   <o.29> Interrupt 61  <0=> Secure state <1=> Non-Secure state
//   <o.30> Interrupt 62  <0=> Secure state <1=> Non-Secure state
//   <o.31> Interrupt 63  <0=> Secure state <1=> Non-Secure state
*/
#define NVIC_INIT_ITNS1_VAL      0x00000000

/*
//   </e>
*/

/*
//   <e>Initialize ITNS 2 (Interrupts 64..95)
*/
#define NVIC_INIT_ITNS2    0

/*
// Interrupts 64..95
//   <o.0>  Interrupt 64  <0=> Secure state <1=> Non-Secure state
//   <o.1>  Interrupt 65  <0=> Secure state <1=> Non-Secure state
//   <o.2>  Interrupt 66  <0=> Secure state <1=> Non-Secure state
//   <o.3>  Interrupt 67  <0=> Secure state <1=> Non-Secure state
//   <o.4>  Interrupt 68  <0=> Secure state <1=> Non-Secure state
//   <o.5>  Interrupt 69  <0=> Secure state <1=> Non-Secure state
//   <o.6>  Interrupt 70  <0=> Secure state <1=> Non-Secure state
//   <o.7>  Interrupt 71  <0=> Secure state <1=> Non-Secure state
//   <o.8>  Interrupt 72  <0=> Secure state <1=> Non-Secure state
//   <o.9>  Interrupt 73  <0=> Secure state <1=> Non-Secure state
//   <o.10> Interrupt 74  <0=> Secure state <1=> Non-Secure state
//   <o.11> Interrupt 75  <0=> Secure state <1=> Non-Secure state
//   <o.12> Interrupt 76  <0=> Secure state <1=> Non-Secure state
//   <o.13> Interrupt 77  <0=> Secure state <1=> Non-Secure state
//   <o.14> Interrupt 78  <0=> Secure state <1=> Non-Secure state
//   <o.15> Interrupt 79  <0=> Secure state <1=> Non-Secure state
//   <o.16> Interrupt 80  <0=> Secure state <1=> Non-Secure state
//   <o.17> Interrupt 81  <0=> Secure state <1=> Non-Secure state
//   <o.18> Interrupt 82  <0=> Secure state <1=> Non-Secure state
//   <o.19> Interrupt 83  <0=> Secure state <1=> Non-Secure state
//   <o.20> Interrupt 84  <0=> Secure state <1=> Non-Secure state
//   <o.21> Interrupt 85  <0=> Secure state <1=> Non-Secure state
//   <o.22> Interrupt 86  <0=> Secure state <1=> Non-Secure state
//   <o.23> Interrupt 87  <0=> Secure state <1=> Non-Secure state
//   <o.24> Interrupt 88  <0=> Secure state <1=> Non-Secure state
//   <o.25> Interrupt 89  <0=> Secure state <1=> Non-Secure state
//   <o.26> Interrupt 90  <0=> Secure state <1=> Non-Secure state
//   <o.27> Interrupt 91  <0=> Secure state <1=> Non-Secure state
//   <o.28> Interrupt 92  <0=> Secure state <1=> Non-Secure state
//   <o.29> Interrupt 93  <0=> Secure state <1=> Non-Secure state
//   <o.30> Interrupt 94  <0=> Secure state <1=> Non-Secure state
//   <o.31> Interrupt 95  <0=> Secure state <1=> Non-Secure state
*/
#define NVIC_INIT_ITNS2_VAL      0x00000000

/*
//   </e>
*/

/*
//   <e>Initialize ITNS 3 (Interrupts 96..127)
*/
#define NVIC_INIT_ITNS3    0

/*
// Interrupts 96..127
//   <o.0>  Interrupt 96  <0=> Secure state <1=> Non-Secure state
//   <o.1>  Interrupt 97  <0=> Secure state <1=> Non-Secure state
//   <o.2>  Interrupt 98  <0=> Secure state <1=> Non-Secure state
//   <o.3>  Interrupt 99  <0=> Secure state <1=> Non-Secure state
//   <o.4>  Interrupt 100 <0=> Secure state <1=> Non-Secure state
//   <o.5>  Interrupt 101 <0=> Secure state <1=> Non-Secure state
//   <o.6>  Interrupt 102 <0=> Secure state <1=> Non-Secure state
//   <o.7>  Interrupt 103 <0=> Secure state <1=> Non-Secure state
//   <o.8>  Interrupt 104 <0=> Secure state <1=> Non-Secure state
//   <o.9>  Interrupt 105 <0=> Secure state <1=> Non-Secure state
//   <o.10> Interrupt 106 <0=> Secure state <1=> Non-Secure state
//   <o.11> Interrupt 107 <0=> Secure state <1=> Non-Secure state
//   <o.12> Interrupt 108 <0=> Secure state <1=> Non-Secure state
//   <o.13> Interrupt 109 <0=> Secure state <1=> Non-Secure state
//   <o.14> Interrupt 110 <0=> Secure state <1=> Non-Secure state
//   <o.15> Interrupt 111 <0=> Secure state <1=> Non-Secure state
//   <o.16> Interrupt 112 <0=> Secure state <1=> Non-Secure state
//   <o.17> Interrupt 113 <0=> Secure state <1=> Non-Secure state
//   <o.18> Interrupt 114 <0=> Secure state <1=> Non-Secure state
//   <o.19> Interrupt 115 <0=> Secure state <1=> Non-Secure state
//   <o.20> Interrupt 116 <0=> Secure state <1=> Non-Secure state
//   <o.21> Interrupt 117 <0=> Secure state <1=> Non-Secure state
//   <o.22> Interrupt 118 <0=> Secure state <1=> Non-Secure state
//   <o.23> Interrupt 119 <0=> Secure state <1=> Non-Secure state
//   <o.24> Interrupt 120 <0=> Secure state <1=> Non-Secure state
//   <o.25> Interrupt 121 <0=> Secure state <1=> Non-Secure state
//   <o.26> Interrupt 122 <0=> Secure state <1=> Non-Secure state
//   <o.27> Interrupt 123 <0=> Secure state <1=> Non-Secure state
//   <o.28> Interrupt 124 <0=> Secure state <1=> Non-Secure state
//   <o.29> Interrupt 125 <0=> Secure state <1=> Non-Secure state
//   <o.30> Interrupt 126 <0=> Secure state <1=> Non-Secure state
//   <o.31> Interrupt 127 <0=> Secure state <1=> Non-Secure state
*/
#define NVIC_INIT_ITNS3_VAL      0x00000000

/*
//   </e>
*/

/*
//   <e>Initialize ITNS 4 (Interrupts 128..159)
*/
#define NVIC_INIT_ITNS4    0

/*
// Interrupts 128..159
//   <o.0>  Interrupt 128 <0=> Secure state <1=> Non-Secure state
//   <o.1>  Interrupt 129 <0=> Secure state <1=> Non-Secure state
//   <o.2>  Interrupt 130 <0=> Secure state <1=> Non-Secure state
//   <o.3>  Interrupt 131 <0=> Secure state <1=> Non-Secure state
//   <o.4>  Interrupt 132 <0=> Secure state <1=> Non-Secure state
//   <o.5>  Interrupt 133 <0=> Secure state <1=> Non-Secure state
//   <o.6>  Interrupt 134 <0=> Secure state <1=> Non-Secure state
//   <o.7>  Interrupt 135 <0=> Secure state <1=> Non-Secure state
//   <o.8>  Interrupt 136 <0=> Secure state <1=> Non-Secure state
//   <o.9>  Interrupt 137 <0=> Secure state <1=> Non-Secure state
//   <o.10> Interrupt 138 <0=> Secure state <1=> Non-Secure state
//   <o.11> Interrupt 139 <0=> Secure state <1=> Non-Secure state
//   <o.12> Interrupt 140 <0=> Secure state <1=> Non-Secure state
//   <o.13> Interrupt 141 <0=> Secure state <1=> Non-Secure state
//   <o.14> Interrupt 142 <0=> Secure state <1=> Non-Secure state
//   <o.15> Interrupt 143 <0=> Secure state <1=> Non-Secure state
//   <o.16> Interrupt 144 <0=> Secure state <1=> Non-Secure state
//   <o.17> Interrupt 145 <0=> Secure state <1=> Non-Secure state
//   <o.18> Interrupt 146 <0=> Secure state <1=> Non-Secure state
//   <o.19> Interrupt 147 <0=> Secure state <1=> Non-Secure state
//   <o.20> Interrupt 148 <0=> Secure state <1=> Non-Secure state
//   <o.21> Interrupt 149 <0=> Secure state <1=> Non-Secure state
//   <o.22> Interrupt 150 <0=> Secure state <1=> Non-Secure state
//   <o.23> Interrupt 151 <0=> Secure state <1=> Non-Secure state
//   <o.24> Interrupt 152 <0=> Secure state <1=> Non-Secure state
//   <o.25> Interrupt 153 <0=> Secure state <1=> Non-Secure state
//   <o.26> Interrupt 154 <0=> Secure state <1=> Non-Secure state
//   <o.27> Interrupt 155 <0=> Secure state <1=> Non-Secure state
//   <o.28> Interrupt 156 <0=> Secure state <1=> Non-Secure state
//   <o.29> Interrupt 157 <0=> Secure state <1=> Non-Secure state
//   <o.30> Interrupt 158 <0=> Secure state <1=> Non-Secure state
//   <o.31> Interrupt 159 <0=> Secure state <1=> Non-Secure state
*/
#define NVIC_INIT_ITNS4_VAL      0x00000000

/*
//   </e>
*/

/*
//   <e>Initialize ITNS 5 (Interrupts 160..191)
*/
#define NVIC_INIT_ITNS5    0

/*
// Interrupts 160..191
//   <o.0>  Interrupt 160 <0=> Secure state <1=> Non-Secure state
//   <o.1>  Interrupt 161 <0=> Secure state <1=> Non-Secure state
//   <o.2>  Interrupt 162 <0=> Secure state <1=> Non-Secure state
//   <o.3>  Interrupt 163 <0=> Secure state <1=> Non-Secure state
//   <o.4>  Interrupt 164 <0=> Secure state <1=> Non-Secure state
//   <o.5>  Interrupt 165 <0=> Secure state <1=> Non-Secure state
//   <o.6>  Interrupt 166 <0=> Secure state <1=> Non-Secure state
//   <o.7>  Interrupt 167 <0=> Secure state <1=> Non-Secure state
//   <o.8>  Interrupt 168 <0=> Secure state <1=> Non-Secure state
//   <o.9>  Interrupt 169 <0=> Secure state <1=> Non-Secure state
//   <o.10> Interrupt 170 <0=> Secure state <1=> Non-Secure state
//   <o.11> Interrupt 171 <0=> Secure state <1=> Non-Secure state
//   <o.12> Interrupt 172 <0=> Secure state <1=> Non-Secure state
//   <o.13> Interrupt 173 <0=> Secure state <1=> Non-Secure state
//   <o.14> Interrupt 174 <0=> Secure state <1=> Non-Secure state
//   <o.15> Interrupt 175 <0=> Secure state <1=> Non-Secure state
//   <o.16> Interrupt 176 <0=> Secure state <1=> Non-Secure state
//   <o.17> Interrupt 177 <0=> Secure state <1=> Non-Secure state
//   <o.18> Interrupt 178 <0=> Secure state <1=> Non-Secure state
//   <o.19> Interrupt 179 <0=> Secure state <1=> Non-Secure state
//   <o.20> Interrupt 180 <0=> Secure state <1=> Non-Secure state
//   <o.21> Interrupt 181 <0=> Secure state <1=> Non-Secure state
//   <o.22> Interrupt 182 <0=> Secure state <1=> Non-Secure state
//   <o.23> Interrupt 183 <0=> Secure state <1=> Non-Secure state
//   <o.24> Interrupt 184 <0=> Secure state <1=> Non-Secure state
//   <o.25> Interrupt 185 <0=> Secure state <1=> Non-Secure state
//   <o.26> Interrupt 186 <0=> Secure state <1=> Non-Secure state
//   <o.27> Interrupt 187 <0=> Secure state <1=> Non-Secure state
//   <o.28> Interrupt 188 <0=> Secure state <1=> Non-Secure state
//   <o.29> Interrupt 189 <0=> Secure state <1=> Non-Secure state
//   <o.30> Interrupt 190 <0=> Secure state <1=> Non-Secure state
//   <o.31> Interrupt 191 <0=> Secure state <1=> Non-Secure state
*/
#define NVIC_INIT_ITNS5_VAL      0x00000000

/*
//   </e>
*/

/*
//   <e>Initialize ITNS 6 (Interrupts 192..223)
*/
#define NVIC_INIT_ITNS6    0

/*
// Interrupts 192..223
//   <o.0>  Interrupt 192 <0=> Secure state <1=> Non-Secure state
//   <o.1>  Interrupt 193 <0=> Secure state <1=> Non-Secure state
//   <o.2>  Interrupt 194 <0=> Secure state <1=> Non-Secure state
//   <o.3>  Interrupt 195 <0=> Secure state <1=> Non-Secure state
//   <o.4>  Interrupt 196 <0=> Secure state <1=> Non-Secure state
//   <o.5>  Interrupt 197 <0=> Secure state <1=> Non-Secure state
//   <o.6>  Interrupt 198 <0=> Secure state <1=> Non-Secure state
//   <o.7>  Interrupt 199 <0=> Secure state <1=> Non-Secure state
//   <o.8>  Interrupt 200 <0=> Secure state <1=> Non-Secure state
//   <o.9>  Interrupt 201 <0=> Secure state <1=> Non-Secure state
//   <o.10> Interrupt 202 <0=> Secure state <1=> Non-Secure state
//   <o.11> Interrupt 203 <0=> Secure state <1=> Non-Secure state
//   <o.12> Interrupt 204 <0=> Secure state <1=> Non-Secure state
//   <o.13> Interrupt 205 <0=> Secure state <1=> Non-Secure state
//   <o.14> Interrupt 206 <0=> Secure state <1=> Non-Secure state
//   <o.15> Interrupt 207 <0=> Secure state <1=> Non-Secure state
//   <o.16> Interrupt 208 <0=> Secure state <1=> Non-Secure state
//   <o.17> Interrupt 209 <0=> Secure state <1=> Non-Secure state
//   <o.18> Interrupt 210 <0=> Secure state <1=> Non-Secure state
//   <o.19> Interrupt 211 <0=> Secure state <1=> Non-Secure state
//   <o.20> Interrupt 212 <0=> Secure state <1=> Non-Secure state
//   <o.21> Interrupt 213 <0=> Secure state <1=> Non-Secure state
//   <o.22> Interrupt 214 <0=> Secure state <1=> Non-Secure state
//   <o.23> Interrupt 215 <0=> Secure state <1=> Non-Secure state
//   <o.24> Interrupt 216 <0=> Secure state <1=> Non-Secure state
//   <o.25> Interrupt 217 <0=> Secure state <1=> Non-Secure state
//   <o.26> Interrupt 218 <0=> Secure state <1=> Non-Secure state
//   <o.27> Interrupt 219 <0=> Secure state <1=> Non-Secure state
//   <o.28> Interrupt 220 <0=> Secure state <1=> Non-Secure state
//   <o.29> Interrupt 221 <0=> Secure state <1=> Non-Secure state
//   <o.30> Interrupt 222 <0=> Secure state <1=> Non-Secure state
//   <o.31> Interrupt 223 <0=> Secure state <1=> Non-Secure state
*/
#define NVIC_INIT_ITNS6_VAL      0x00000000

/*
//   </e>
*/

/*
//   <e>Initialize ITNS 7 (Interrupts 224..255)
*/
#define NVIC_INIT_ITNS7    0

/*
// Interrupts 224..255
//   <o.0>  Interrupt 224 <0=> Secure state <1=> Non-Secure state
//   <o.1>  Interrupt 225 <0=> Secure state <1=> Non-Secure state
//   <o.2>  Interrupt 226 <0=> Secure state <1=> Non-Secure state
//   <o.3>  Interrupt 227 <0=> Secure state <1=> Non-Secure state
//   <o.4>  Interrupt 228 <0=> Secure state <1=> Non-Secure state
//   <o.5>  Interrupt 229 <0=> Secure state <1=> Non-Secure state
//   <o.6>  Interrupt 230 <0=> Secure state <1=> Non-Secure state
//   <o.7>  Interrupt 231 <0=> Secure state <1=> Non-Secure state
//   <o.8>  Interrupt 232 <0=> Secure state <1=> Non-Secure state
//   <o.9>  Interrupt 233 <0=> Secure state <1=> Non-Secure state
//   <o.10> Interrupt 234 <0=> Secure state <1=> Non-Secure state
//   <o.11> Interrupt 235 <0=> Secure state <1=> Non-Secure state
//   <o.12> Interrupt 236 <0=> Secure state <1=> Non-Secure state
//   <o.13> Interrupt 237 <0=> Secure state <1=> Non-Secure state
//   <o.14> Interrupt 238 <0=> Secure state <1=> Non-Secure state
//   <o.15> Interrupt 239 <0=> Secure state <1=> Non-Secure state
//   <o.16> Interrupt 240 <0=> Secure state <1=> Non-Secure state
//   <o.17> Interrupt 241 <0=> Secure state <1=> Non-Secure state
//   <o.18> Interrupt 242 <0=> Secure state <1=> Non-Secure state
//   <o.19> Interrupt 243 <0=> Secure state <1=> Non-Secure state
//   <o.20> Interrupt 244 <0=> Secure state <1=> Non-Secure state
//   <o.21> Interrupt 245 <0=> Secure state <1=> Non-Secure state
//   <o.22> Interrupt 246 <0=> Secure state <1=> Non-Secure state
//   <o.23> Interrupt 247 <0=> Secure state <1=> Non-Secure state
//   <o.24> Interrupt 248 <0=> Secure state <1=> Non-Secure state
//   <o.25> Interrupt 249 <0=> Secure state <1=> Non-Secure state
//   <o.26> Interrupt 250 <0=> Secure state <1=> Non-Secure state
//   <o.27> Interrupt 251 <0=> Secure state <1=> Non-Secure state
//   <o.28> Interrupt 252 <0=> Secure state <1=> Non-Secure state
//   <o.29> Interrupt 253 <0=> Secure state <1=> Non-Secure state
//   <o.30> Interrupt 254 <0=> Secure state <1=> Non-Secure state
//   <o.31> Interrupt 255 <0=> Secure state <1=> Non-Secure state
*/
#define NVIC_INIT_ITNS7_VAL      0x00000000

/*
//   </e>
*/

/*
// </h>
*/



/*
    max 128 SAU regions.
    SAU regions are defined in partition.h
 */

#define SAU_INIT_REGION(n) \
    SAU->RNR  =  (n                                     & SAU_RNR_REGION_Msk); \
    SAU->RBAR =  (SAU_INIT_START##n                     & SAU_RBAR_BADDR_Msk); \
    SAU->RLAR =  (SAU_INIT_END##n                       & SAU_RLAR_LADDR_Msk) | \
                ((SAU_INIT_NSC##n << SAU_RLAR_NSC_Pos)  & SAU_RLAR_NSC_Msk)   | 1U

/**
  \brief   Setup a SAU Region
  \details Writes the region information contained in SAU_Region to the
           registers SAU_RNR, SAU_RBAR, and SAU_RLAR
 */
__STATIC_INLINE void TZ_SAU_Setup (void)
{

#if defined (__SAUREGION_PRESENT) && (__SAUREGION_PRESENT == 1U)

  #if defined (SAU_INIT_REGION0) && (SAU_INIT_REGION0 == 1U)
    SAU_INIT_REGION(0);
  #endif

  #if defined (SAU_INIT_REGION1) && (SAU_INIT_REGION1 == 1U)
    SAU_INIT_REGION(1);
  #endif

  #if defined (SAU_INIT_REGION2) && (SAU_INIT_REGION2 == 1U)
    SAU_INIT_REGION(2);
  #endif

  #if defined (SAU_INIT_REGION3) && (SAU_INIT_REGION3 == 1U)
    SAU_INIT_REGION(3);
  #endif

  #if defined (SAU_INIT_REGION4) && (SAU_INIT_REGION4 == 1U)
    SAU_INIT_REGION(4);
  #endif

  #if defined (SAU_INIT_REGION5) && (SAU_INIT_REGION5 == 1U)
    SAU_INIT_REGION(5);
  #endif

  #if defined (SAU_INIT_REGION6) && (SAU_INIT_REGION6 == 1U)
    SAU_INIT_REGION(6);
  #endif

  #if defined (SAU_INIT_REGION7) && (SAU_INIT_REGION7 == 1U)
    SAU_INIT_REGION(7);
  #endif

  /* repeat this for all possible SAU regions */

#endif /* defined (__SAUREGION_PRESENT) && (__SAUREGION_PRESENT == 1U) */


  #if defined (SAU_INIT_CTRL) && (SAU_INIT_CTRL == 1U)
    SAU->CTRL = ((SAU_INIT_CTRL_ENABLE << SAU_CTRL_ENABLE_Pos) & SAU_CTRL_ENABLE_Msk) |
                ((SAU_INIT_CTRL_ALLNS  << SAU_CTRL_ALLNS_Pos)  & SAU_CTRL_ALLNS_Msk)   ;
  #endif

  #if defined (SCB_CSR_AIRCR_INIT) && (SCB_CSR_AIRCR_INIT == 1U)
    SCB->SCR   = (SCB->SCR   & ~(SCB_SCR_SLEEPDEEPS_Msk    )) |
                   ((SCB_CSR_DEEPSLEEPS_VAL     << SCB_SCR_SLEEPDEEPS_Pos)     & SCB_SCR_SLEEPDEEPS_Msk);

    SCB->AIRCR = (SCB->AIRCR & ~(SCB_AIRCR_VECTKEY_Msk   | SCB_AIRCR_SYSRESETREQS_Msk |
                                 SCB_AIRCR_BFHFNMINS_Msk |  SCB_AIRCR_PRIS_Msk)        )                     |
                   ((0x05FAU                    << SCB_AIRCR_VECTKEY_Pos)      & SCB_AIRCR_VECTKEY_Msk)      |
                   ((SCB_AIRCR_SYSRESETREQS_VAL << SCB_AIRCR_SYSRESETREQS_Pos) & SCB_AIRCR_SYSRESETREQS_Msk) |
                   ((SCB_AIRCR_PRIS_VAL         << SCB_AIRCR_PRIS_Pos)         & SCB_AIRCR_PRIS_Msk)         |
                   ((SCB_AIRCR_BFHFNMINS_VAL    << SCB_AIRCR_BFHFNMINS_Pos)    & SCB_AIRCR_BFHFNMINS_Msk);
  #endif /* defined (SCB_CSR_AIRCR_INIT) && (SCB_CSR_AIRCR_INIT == 1U) */

  #if defined (SCB_ICSR_INIT) && (SCB_ICSR_INIT == 1U)
    SCB->ICSR  = (SCB->ICSR  & ~(SCB_ICSR_STTNS_Msk        )) |
                   ((SCB_ICSR_STTNS_VAL         << SCB_ICSR_STTNS_Pos)         & SCB_ICSR_STTNS_Msk);
  #endif /* defined (SCB_ICSR_INIT) && (SCB_ICSR_INIT == 1U) */

  #if defined (NVIC_INIT_ITNS0) && (NVIC_INIT_ITNS0 == 1U)
    NVIC->ITNS[0] = NVIC_INIT_ITNS0_VAL;
  #endif

  #if defined (NVIC_INIT_ITNS1) && (NVIC_INIT_ITNS1 == 1U)
    NVIC->ITNS[1] = NVIC_INIT_ITNS1_VAL;
  #endif

  #if defined (NVIC_INIT_ITNS2) && (NVIC_INIT_ITNS2 == 1U)
    NVIC->ITNS[2] = NVIC_INIT_ITNS2_VAL;
  #endif

  #if defined (NVIC_INIT_ITNS3) && (NVIC_INIT_ITNS3 == 1U)
    NVIC->ITNS[3] = NVIC_INIT_ITNS3_VAL;
  #endif

  #if defined (NVIC_INIT_ITNS4) && (NVIC_INIT_ITNS4 == 1U)
    NVIC->ITNS[4] = NVIC_INIT_ITNS4_VAL;
  #endif

  #if defined (NVIC_INIT_ITNS5) && (NVIC_INIT_ITNS5 == 1U)
    NVIC->ITNS[5] = NVIC_INIT_ITNS5_VAL;
  #endif

  #if defined (NVIC_INIT_ITNS6) && (NVIC_INIT_ITNS6 == 1U)
    NVIC->ITNS[6] = NVIC_INIT_ITNS6_VAL;
  #endif

  #if defined (NVIC_INIT_ITNS7) && (NVIC_INIT_ITNS7 == 1U)
    NVIC->ITNS[7] = NVIC_INIT_ITNS7_VAL;
  #endif

  /* repeat this for all possible ITNS elements */

}

#endif  /* PARTITION_ARMCM23_H */
