/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     pin_name.h
 * @brief    header file for the pin_name
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _PINNAMES_H
#define _PINNAMES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    GPIO1_1 = 0
} pin_name_t;

typedef enum {
    PORTA = 0,
    PORTB,
} port_name_t;


#define	GPIO_AO_NR	            (SI_GPIO_8)
#define	SI_GPIO_0       		0
#define	SI_GPIO_1       		1
#define	SI_GPIO_2       		2
#define	SI_GPIO_3       		3
#define	SI_GPIO_4       		4
#define	SI_GPIO_5       		5
#define	SI_GPIO_6       		6
#define	SI_GPIO_7       		7
#define	SI_GPIO_8       		8
#define	SI_GPIO_9       		9
#define	SI_GPIO_10       		10
#define	SI_GPIO_11       		11
#define	SI_GPIO_12       		12
#define	SI_GPIO_13       		13
#define	SI_GPIO_14       		14
#define	SI_GPIO_15       		15
#define	SI_GPIO_16       		16
#define	SI_GPIO_17       		17
#define	SI_GPIO_18       		18
#define	SI_GPIO_19       		19
#define	SI_GPIO_20       		20
#define	SI_GPIO_21       		21
#define	SI_GPIO_22       		22
#define	SI_GPIO_23       		23
#define	SI_GPIO_24       		24
#define	SI_GPIO_25       		25
#define	SI_GPIO_26       		26
#define	SI_GPIO_27       		27
#define	SI_GPIO_28       		28
#define	SI_GPIO_29       		29
#define	SI_GPIO_30       		30
#define	SI_GPIO_31       		31
#define	SI_GPIO_32       		32
#define	SI_GPIO_33       		33
#define	SI_GPIO_34       		34
#define	SI_GPIO_35       		35
#define	SI_GPIO_36       		36
#define	SI_GPIO_37       		37
#define	SI_GPIO_38       		38
#define	SI_GPIO_39       		39
#define	SI_GPIO_40       		40
#define	SI_GPIO_41       		41
#define	SI_GPIO_42       		42
#define	SI_GPIO_43       		43
#define	SI_GPIO_44       		44
#define	SI_GPIO_45       		45
#define	SI_GPIO_46       		46
#define	SI_GPIO_47       		47
#define	SI_GPIO_48       		48
#define	SI_GPIO_49       		49
#define	SI_GPIO_50       		50
#define	SI_GPIO_51       		51
#define	SI_GPIO_52       		52
#define	SI_GPIO_53       		53
#define	SI_GPIO_54       		54

#define	SI_GPIO_MAX       	SI_GPIO_54
#define	SI_GPIO_NULL       	255

#ifdef __cplusplus
}
#endif

#endif
