//------------------------------------------------------------------------
// Copyright (C) 2016 RDA microelectronic Inc.                                           
// All Rights Reserved                                                    
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of RDA microelectronic Inc.                
// The copyright notice above does not evidence any actual or intended    
// publication of such source code.                                       
// No part of this code may be reproduced, stored in a retrieval system,  
// or transmitted, in any form or by any means, electronic, mechanical,   
// photocopying, recording, or otherwise, without the prior written       
// permission of RDA microelectronic Inc.                                                 
//------------------------------------------------------------------------
// File name      : cparm_f8                                            
// Author         : ganghualuo                                                 
// Date of created: 2016-08-22 11:12:11                                                  
// Last modified  : 2016-08-22 11:12:11                                                  
//------------------------------------------------------------------------
// Functional Description:                                                
// N/A                                                                    
//------------------------------------------------------------------------
// Reversion History:                                                     
// 2016-08-22 11:12:11 --  Create by ganghualuo                                               
//------------------------------------------------------------------------
#ifndef __CPARM_F8_H__
#define __CPARM_F8_H__
//Auto-gen by reg_gen





//F8_CONF
typedef union
{
    U32 v;
    struct
    {
        U32 F8_START                       :1 ; /*0 :0 , F8算法启动控制位0：不启动F8 算法，或完成后自动清零；1：启动F8算法 */
        U32 F8_IRQ_EN                      :1 ; /*1 :1 , F8算法中断使能位0：F8 算法/搬数在整个多块group结束后，不产生中断；1：F8 算法/搬数在整个多块group结束后，产生中断*/
        U32 F8_AR_SEL                      :2 ; /*3 :2 , F8算法类型和只做搬数选择：00：只搬数，不做加解密01：AES加解密，并搬数10：snow3G加解密，并搬数11：zuc加解密，并搬数*/
        U32 F8_CONF_reserved_0             :28; /*31:4 , No description.               */
    } b;
} T_CPARM_F8_F8_CONF;

//F8_GROUP_ADDR
typedef union
{
    U32 v;
    struct
    {
        U32 GROUP_ADDR                     :32; /*31:0 , group首地址                   */
    } b;
} T_CPARM_F8_F8_GROUP_ADDR;

//F8_GROUP_CNT
typedef union
{
    U32 v;
    struct
    {
        U32 GROUP_ADDR                     :32; /*31:0 , group个数                     */
    } b;
} T_CPARM_F8_F8_GROUP_CNT;

//F8_STATUS
typedef union
{
    U32 v;
    struct
    {
        U32 F8_STAT                        :1 ; /*0 :0 , 0：F8算法/搬数，未完成或未开始1：F8算法/搬数，已完成*/
        U32 F9_STAT                        :1 ; /*1 :1 , 0：F9算法/搬数，未完成或未开始1：F9算法/搬数，已完成*/
        U32 F8_STATUS_reserved_0           :30; /*31:2 , No description.               */
    } b;
} T_CPARM_F8_F8_STATUS;

//F9_CONF
typedef union
{
    U32 v;
    struct
    {
        U32 F9_START                       :1 ; /*0 :0 , F9算法启动控制位0：不启动F9 算法，或完成后自动清零；1：启动F9算法 */
        U32 F9_IRQ_EN                      :1 ; /*1 :1 , F9算法中断使能位0：F9 算法/搬数在整个多块group结束后，不产生中断；1：F9 算法/搬数在整个多块group结束后，产生中断*/
        U32 F9_AR_SEL                      :2 ; /*3 :2 , F9算法类型选择：00：AES完整性算法01：AES完整性算法10：snow3G完整性算法11：zuc完整性算法*/
        U32 F9_CONF_reserved_0             :28; /*31:4 , No description.               */
    } b;
} T_CPARM_F8_F9_CONF;

//F9_GROUP_ADDR
typedef union
{
    U32 v;
    struct
    {
        U32 F9_ADDR                        :32; /*31:0 , F9 group首地址                */
    } b;
} T_CPARM_F8_F9_GROUP_ADDR;

//F9_RESULT
typedef union
{
    U32 v;
    struct
    {
        U32 F9_MAC                         :32; /*31:0 , F9结果寄存器                  */
    } b;
} T_CPARM_F8_F9_RESULT;

typedef struct
{
    volatile T_CPARM_F8_F8_CONF              F8_CONF                       ; /*0x0  , RW   , 0x00000000, F8配置寄存器                  */
    volatile T_CPARM_F8_F8_GROUP_ADDR        F8_GROUP_ADDR                 ; /*0x4  , RW   , 0x00000000, F8 group首地址寄存器          */
    volatile T_CPARM_F8_F8_GROUP_CNT         F8_GROUP_CNT                  ; /*0x8  , RW   , 0x00000000, F8 group个数寄存器            */
    volatile T_CPARM_F8_F8_STATUS            F8_STATUS                     ; /*0xc  , RW   , 0x00000000, F8状态寄存器                  */
    volatile T_CPARM_F8_F9_CONF              F9_CONF                       ; /*0x10 , RW   , 0x00000000, F9配置寄存器                  */
    volatile T_CPARM_F8_F9_GROUP_ADDR        F9_GROUP_ADDR                 ; /*0x14 , RW   , 0x00000000, F9 group首地址寄存器          */
    volatile T_CPARM_F8_F9_RESULT            F9_RESULT                     ; /*0x18 , RO   , 0x00000000, F9 结果寄存器                 */
} T_HWP_CPARM_F8_T;


//F8_CONF
#define F8_CONF_F8_START               (1<<0)              

#define F8_CONF_F8_IRQ_EN              (1<<1)              

#define F8_CONF_F8_AR_SEL(n)           (((n)&0x3)<<2)      


//F8_GROUP_ADDR
#define F8_GROUP_ADDR_GROUP_ADDR(n)    (((n)&0xFFFFFFFF)<<0)


//F8_GROUP_CNT
#define F8_GROUP_CNT_GROUP_ADDR(n)     (((n)&0xFFFFFFFF)<<0)


//F8_STATUS
#define F8_STATUS_F8_STAT              (1<<0)              

#define F8_STATUS_F9_STAT              (1<<1)              


//F9_CONF
#define F9_CONF_F9_START               (1<<0)              

#define F9_CONF_F9_IRQ_EN              (1<<1)              

#define F9_CONF_F9_AR_SEL(n)           (((n)&0x3)<<2)      


//F9_GROUP_ADDR
#define F9_GROUP_ADDR_F9_ADDR(n)       (((n)&0xFFFFFFFF)<<0)


//F9_RESULT
#define F9_RESULT_F9_MAC(n)            (((n)&0xFFFFFFFF)<<0)

#define  F8_LTE_F8      0x01
#define  F8_LTE_F9      0x02



typedef struct                       
{
    u32 handle;                                           /*模块标志*/     
    u32 open_flg;                                        /*模块开启标志*/             
    void (*txcallback)(void *pbuff, u32 length);          /*回调函数指针*/
    void (*rxcallback)(void *pbuff, u32 length);          /*回调函数指针*/
    u32 reserve_len;  
    u32 rx_max_len;                                       /*某类业务接收可能的最大数据个数，用于动态申请内存用*/
}HAL_MODULES_INFO_T, *pHAL_MODULES_INFO_T;


#endif


