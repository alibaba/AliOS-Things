/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_bsp_interrupt_config.h
* Description  : This module maps Interrupt A & B interrupts. More information on how this is done is given below.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version   Description
*         : 01.10.2016 1.00      First Release
*         : 15.05.2017 2.00      Changed the name of the following macro definition, because there was a mistake
*                                in the name of macro definition.
*                                - From XXX_TPU0_TGI0V to XXX_TPU0_TCI0V.
*                                - From XXX_TPU1_TGI1V to XXX_TPU1_TCI1V.
*                                - From XXX_TPU1_TGI1U to XXX_TPU1_TCI1U.
*                                - From XXX_TPU2_TGI2V to XXX_TPU2_TCI2V.
*                                - From XXX_TPU2_TGI2U to XXX_TPU2_TCI2U.
*                                - From XXX_TPU3_TGI3V to XXX_TPU3_TCI3V.
*                                - From XXX_TPU4_TGI4V to XXX_TPU4_TCI4V.
*                                - From XXX_TPU4_TGI4U to XXX_TPU4_TCI4U.
*                                - From XXX_TPU5_TGI5V to XXX_TPU5_TCI5V.
*                                - From XXX_TPU5_TGI5U to XXX_TPU5_TCI5U.
*                                - From XXX_MTU0_TGIV0 to XXX_MTU0_TCIV0.
*                                - From XXX_MTU1_TGIV1 to XXX_MTU1_TCIV1.
*                                - From XXX_MTU1_TGIU1 to XXX_MTU1_TCIU1.
*                                - From XXX_MTU2_TGIV2 to XXX_MTU2_TCIV2.
*                                - From XXX_MTU2_TGIU2 to XXX_MTU2_TCIU2.
*                                - From XXX_MTU3_TGIV3 to XXX_MTU3_TCIV3.
*                                - From XXX_MTU4_TGIV4 to XXX_MTU4_TCIV4.
*                                - From XXX_MTU6_TGIV6 to XXX_MTU6_TCIV6.
*                                - From XXX_MTU7_TGIV7 to XXX_MTU7_TCIV7.
*                                - From XXX_MTU8_TGIV8 to XXX_MTU8_TCIV8.
*                                Added select processing of the following software configurable interrupt source.
*                                - TSIP_PROC_BUSY
*                                - TSIP_ROMOK
*                                - TSIP_LONG_PLG
*                                - TSIP_TEST_BUSY
*                                - TSIP_WRRDY0
*                                - TSIP_WRRDY1
*                                - TSIP_WRRDY4
*                                - TSIP_RDRDY0
*                                - TSIP_RDRDY1
*                                - TSIP_INTEGRATE_WRRDY
*                                - TSIP_INTEGRATE_RDRDY
***********************************************************************************************************************/
#ifndef R_BSP_INTERRUPT_CONFIG_REF_HEADER_FILE
#define R_BSP_INTERRUPT_CONFIG_REF_HEADER_FILE

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/* If you wish to use one of the interrupt below then set the appropriate configuration macro to the vector number you
 * wish to use for that interrupt. For example, if you want to use the RTC carry interrupt (CUP) at vector 176 then you
 * would do the following:
 *
 * #define BSP_MAPPED_INT_CFG_B_VECT_RTC_CUP                                 176
 */

/* Interrupt B Sources.
 * -Valid vector numbers are 128-207.
 * -There are more vector slots for B sources than actual B sources. By default all B sources are mapped.
 * -If using the 'TPU1, TGI1A' interrupt it must be vector 144 or 145. It is set to 144 by default.
 * -If a peripheral interrupt is going to be used to wake up the MCU from All-Module Clock Stop Mode then it must be
 *  in a vector between 146 to 157. Peripheral interrupts that can do this are TMR interrupts and the 'USB0, USBI0'
 *  interrupt. By default the TMR interrupts are chosen since there are 12 of them and there are 12 slots.
 */
#define BSP_MAPPED_INT_CFG_B_VECT_CMT2_CMI2                               128
#define BSP_MAPPED_INT_CFG_B_VECT_CMT3_CMI3                               129
#define BSP_MAPPED_INT_CFG_B_VECT_TMR0_CMIA0                              146
#define BSP_MAPPED_INT_CFG_B_VECT_TMR0_CMIB0                              147
#define BSP_MAPPED_INT_CFG_B_VECT_TMR0_OVI0                               148
#define BSP_MAPPED_INT_CFG_B_VECT_TMR1_CMIA1                              149
#define BSP_MAPPED_INT_CFG_B_VECT_TMR1_CMIB1                              150
#define BSP_MAPPED_INT_CFG_B_VECT_TMR1_OVI1                               151
#define BSP_MAPPED_INT_CFG_B_VECT_TMR2_CMIA2                              152
#define BSP_MAPPED_INT_CFG_B_VECT_TMR2_CMIB2                              153
#define BSP_MAPPED_INT_CFG_B_VECT_TMR2_OVI2                               154
#define BSP_MAPPED_INT_CFG_B_VECT_TMR3_CMIA3                              155
#define BSP_MAPPED_INT_CFG_B_VECT_TMR3_CMIB3                              156
#define BSP_MAPPED_INT_CFG_B_VECT_TMR3_OVI3                               157
#define BSP_MAPPED_INT_CFG_B_VECT_TPU0_TGI0A                              130
#define BSP_MAPPED_INT_CFG_B_VECT_TPU0_TGI0B                              131
#define BSP_MAPPED_INT_CFG_B_VECT_TPU0_TGI0C                              132
#define BSP_MAPPED_INT_CFG_B_VECT_TPU0_TGI0D                              133
#define BSP_MAPPED_INT_CFG_B_VECT_TPU0_TCI0V                              134
#define BSP_MAPPED_INT_CFG_B_VECT_TPU1_TGI1A                              144
#define BSP_MAPPED_INT_CFG_B_VECT_TPU1_TGI1B                              135
#define BSP_MAPPED_INT_CFG_B_VECT_TPU1_TCI1V                              136
#define BSP_MAPPED_INT_CFG_B_VECT_TPU1_TCI1U                              137
#define BSP_MAPPED_INT_CFG_B_VECT_TPU2_TGI2A                              138
#define BSP_MAPPED_INT_CFG_B_VECT_TPU2_TGI2B                              139
#define BSP_MAPPED_INT_CFG_B_VECT_TPU2_TCI2V                              140
#define BSP_MAPPED_INT_CFG_B_VECT_TPU2_TCI2U                              141
#define BSP_MAPPED_INT_CFG_B_VECT_TPU3_TGI3A                              142
#define BSP_MAPPED_INT_CFG_B_VECT_TPU3_TGI3B                              143
#define BSP_MAPPED_INT_CFG_B_VECT_TPU3_TGI3C                              145
#define BSP_MAPPED_INT_CFG_B_VECT_TPU3_TGI3D                              158
#define BSP_MAPPED_INT_CFG_B_VECT_TPU3_TCI3V                              159
#define BSP_MAPPED_INT_CFG_B_VECT_TPU4_TGI4A                              160
#define BSP_MAPPED_INT_CFG_B_VECT_TPU4_TGI4B                              161
#define BSP_MAPPED_INT_CFG_B_VECT_TPU4_TCI4V                              162
#define BSP_MAPPED_INT_CFG_B_VECT_TPU4_TCI4U                              163
#define BSP_MAPPED_INT_CFG_B_VECT_TPU5_TGI5A                              164
#define BSP_MAPPED_INT_CFG_B_VECT_TPU5_TGI5B                              165
#define BSP_MAPPED_INT_CFG_B_VECT_TPU5_TCI5V                              166
#define BSP_MAPPED_INT_CFG_B_VECT_TPU5_TCI5U                              167
#define BSP_MAPPED_INT_CFG_B_VECT_CMTW0_IC0I0                             168
#define BSP_MAPPED_INT_CFG_B_VECT_CMTW0_IC1I0                             169
#define BSP_MAPPED_INT_CFG_B_VECT_CMTW0_OC0I0                             170
#define BSP_MAPPED_INT_CFG_B_VECT_CMTW0_OC1I0                             171
#define BSP_MAPPED_INT_CFG_B_VECT_CMTW1_IC0I1                             172
#define BSP_MAPPED_INT_CFG_B_VECT_CMTW1_IC1I1                             173
#define BSP_MAPPED_INT_CFG_B_VECT_CMTW1_OC0I1                             174
#define BSP_MAPPED_INT_CFG_B_VECT_CMTW1_OC1I1                             175
#define BSP_MAPPED_INT_CFG_B_VECT_RTC_CUP                                 176
#define BSP_MAPPED_INT_CFG_B_VECT_CAN0_RXF0                               177
#define BSP_MAPPED_INT_CFG_B_VECT_CAN0_TXF0                               178
#define BSP_MAPPED_INT_CFG_B_VECT_CAN0_RXM0                               179
#define BSP_MAPPED_INT_CFG_B_VECT_CAN0_TXM0                               180
#define BSP_MAPPED_INT_CFG_B_VECT_CAN1_RXF1                               181
#define BSP_MAPPED_INT_CFG_B_VECT_CAN1_TXF1                               182
#define BSP_MAPPED_INT_CFG_B_VECT_CAN1_RXM1                               183
#define BSP_MAPPED_INT_CFG_B_VECT_CAN1_TXM1                               184
#define BSP_MAPPED_INT_CFG_B_VECT_USB0_USBI0                              185
#define BSP_MAPPED_INT_CFG_B_VECT_S12ADC0_S12ADI0                         186
#define BSP_MAPPED_INT_CFG_B_VECT_S12ADC0_S12GBADI0                       187
#define BSP_MAPPED_INT_CFG_B_VECT_S12ADC0_S12GCADI0                       188
#define BSP_MAPPED_INT_CFG_B_VECT_S12ADC1_S12ADI1                         189
#define BSP_MAPPED_INT_CFG_B_VECT_S12ADC1_S12GBADI1                       190
#define BSP_MAPPED_INT_CFG_B_VECT_S12ADC1_S12GCADI1                       191
#define BSP_MAPPED_INT_CFG_B_VECT_RNG_RNGEND                              192
#define BSP_MAPPED_INT_CFG_B_VECT_ELC_ELSR18I                             193
#define BSP_MAPPED_INT_CFG_B_VECT_ELC_ELSR19I                             194
#define BSP_MAPPED_INT_CFG_B_VECT_TSIP_PROC_BUSY                          195
#define BSP_MAPPED_INT_CFG_B_VECT_TSIP_ROMOK                              196
#define BSP_MAPPED_INT_CFG_B_VECT_TSIP_LONG_PLG                           197
#define BSP_MAPPED_INT_CFG_B_VECT_TSIP_TEST_BUSY                          198
#define BSP_MAPPED_INT_CFG_B_VECT_TSIP_WRRDY0                             199
#define BSP_MAPPED_INT_CFG_B_VECT_TSIP_WRRDY1                             200
#define BSP_MAPPED_INT_CFG_B_VECT_TSIP_WRRDY4                             201
#define BSP_MAPPED_INT_CFG_B_VECT_TSIP_RDRDY0                             202
#define BSP_MAPPED_INT_CFG_B_VECT_TSIP_RDRDY1                             203
#define BSP_MAPPED_INT_CFG_B_VECT_TSIP_INTEGRATE_WRRDY                    204
#define BSP_MAPPED_INT_CFG_B_VECT_TSIP_INTEGRATE_RDRDY                    205

/* Interrupt A Sources.
 * -Valid vector numbers are 208-255.
 * -There are more A sources than A vector slots. By default none of the GPT interrupts are mapped.
 * -If using the 'MTU1, TGI1A' interrupt it must be vector 208 or 209. It is set to 208 by default.
 */
#define BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGIA0                              209
#define BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGIB0                              210
#define BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGIC0                              211
#define BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGID0                              212
#define BSP_MAPPED_INT_CFG_A_VECT_MTU0_TCIV0                              213
#define BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGIE0                              214
#define BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGIF0                              215
#define BSP_MAPPED_INT_CFG_A_VECT_MTU1_TGIA1                              208
#define BSP_MAPPED_INT_CFG_A_VECT_MTU1_TGIB1                              216
#define BSP_MAPPED_INT_CFG_A_VECT_MTU1_TCIV1                              217
#define BSP_MAPPED_INT_CFG_A_VECT_MTU1_TCIU1                              218
#define BSP_MAPPED_INT_CFG_A_VECT_MTU2_TGIA2                              219
#define BSP_MAPPED_INT_CFG_A_VECT_MTU2_TGIB2                              220
#define BSP_MAPPED_INT_CFG_A_VECT_MTU2_TCIV2                              221
#define BSP_MAPPED_INT_CFG_A_VECT_MTU2_TCIU2                              222
#define BSP_MAPPED_INT_CFG_A_VECT_MTU3_TGIA3                              223
#define BSP_MAPPED_INT_CFG_A_VECT_MTU3_TGIB3                              224
#define BSP_MAPPED_INT_CFG_A_VECT_MTU3_TGIC3                              225
#define BSP_MAPPED_INT_CFG_A_VECT_MTU3_TGID3                              226
#define BSP_MAPPED_INT_CFG_A_VECT_MTU3_TCIV3                              227
#define BSP_MAPPED_INT_CFG_A_VECT_MTU4_TGIA4                              228
#define BSP_MAPPED_INT_CFG_A_VECT_MTU4_TGIB4                              229
#define BSP_MAPPED_INT_CFG_A_VECT_MTU4_TGIC4                              230
#define BSP_MAPPED_INT_CFG_A_VECT_MTU4_TGID4                              231
#define BSP_MAPPED_INT_CFG_A_VECT_MTU4_TCIV4                              232
#define BSP_MAPPED_INT_CFG_A_VECT_MTU5_TGIU5                              233
#define BSP_MAPPED_INT_CFG_A_VECT_MTU5_TGIV5                              234
#define BSP_MAPPED_INT_CFG_A_VECT_MTU5_TGIW5                              235
#define BSP_MAPPED_INT_CFG_A_VECT_MTU6_TGIA6                              236
#define BSP_MAPPED_INT_CFG_A_VECT_MTU6_TGIB6                              237
#define BSP_MAPPED_INT_CFG_A_VECT_MTU6_TGIC6                              238
#define BSP_MAPPED_INT_CFG_A_VECT_MTU6_TGID6                              239
#define BSP_MAPPED_INT_CFG_A_VECT_MTU6_TCIV6                              240
#define BSP_MAPPED_INT_CFG_A_VECT_MTU7_TGIA7                              241
#define BSP_MAPPED_INT_CFG_A_VECT_MTU7_TGIB7                              242
#define BSP_MAPPED_INT_CFG_A_VECT_MTU7_TGIC7                              243
#define BSP_MAPPED_INT_CFG_A_VECT_MTU7_TGID7                              244
#define BSP_MAPPED_INT_CFG_A_VECT_MTU7_TCIV7                              245
#define BSP_MAPPED_INT_CFG_A_VECT_MTU8_TGIA8                              246
#define BSP_MAPPED_INT_CFG_A_VECT_MTU8_TGIB8                              247
#define BSP_MAPPED_INT_CFG_A_VECT_MTU8_TGIC8                              248
#define BSP_MAPPED_INT_CFG_A_VECT_MTU8_TGID8                              249
#define BSP_MAPPED_INT_CFG_A_VECT_MTU8_TCIV8                              250
#define BSP_MAPPED_INT_CFG_A_VECT_AES_AESRDY                              251
#define BSP_MAPPED_INT_CFG_A_VECT_AES_AESEND                              252

#endif /* R_BSP_INTERRUPT_CONFIG_REF_HEADER_FILE */

