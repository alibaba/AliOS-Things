/*************************************************
  Copyright (C), 2011-2015 ZheJiang Dahua Technology Co., Ltd.
  文件名:   knlpatch.c
  作  者:   zheng_xingjian(11853)<zheng_xingjian@dahuatech.com>
  版  本:   V1.0.0
  日  期：  2015-10-24
  描  述:   yoc的内核补丁层，用于支持对应的Linux内核接口调用

            1、使用说明
            本文件中，外部函数使用DH_KP_xxx, 内部函数使用KP_eCos_xxxx

            2、局限性
            仅适用于yoc

  修订历史:
  1. 日    期: 2017-11-27
     修订版本: v0.1
     作    者: zhang_jian
     修订备注: 创建

  2. 日    期:
     修订版本:
     作    者:
     修订备注:
*************************************************/
#include <string.h>
#include "knlpatch.h"

/*******************************************************************************
 * 函数名  : copy_from_user
 * 描  述  :
 * 输  入  :  参数        描述
 *         :   to           请输入to的描述
 *         :   from         请输入from的描述
 *         :   n            请输入n的描述
 * 输  出  : 无
 * 返回值  : DH_SUCCESS: 成功
 *           DH_FAILURE: 失败
 *******************************************************************************/
unsigned int copy_from_user(void* to,const void __user* from, unsigned int n)
{
    if( NULL == to || NULL == from )
    {
        return 1;
    }

    //to = from;
    memcpy(to,from,n);

    return 0;
}

/*******************************************************************************
 * 函数名  : copy_to_user
 * 描  述  :
 * 输  入  :  参数        描述
 *         :   to           请输入to的描述
 *         :   from         请输入from的描述
 *         :   n            请输入n的描述
 * 输  出  : 无
 * 返回值  : DH_SUCCESS: 成功
 *           DH_FAILURE: 失败
 *******************************************************************************/
unsigned int copy_to_user(void __user* to,const void* from, unsigned int n)
{
    if( NULL == to || NULL == from )
    {
        return 1;
    }

    //to = from;
    memcpy(to,from,n);

    return 0;
}

/*******************************************************************************
 * 函数名  : ioremap
 * 描  述  :
 * 输  入  :  参数        描述
 *         :   phys_addr         请输入phys_addr的描述
 *         :   size              请输入size的描述
 * 输  出  : 无
 * 返回值  : DH_SUCCESS: 成功
 *           DH_FAILURE: 失败
 *******************************************************************************/
void* ioremap(unsigned int phys_addr, unsigned int size)
{
    return (void *)phys_addr;
}

/*******************************************************************************
 * 函数名  : iorunmap
 * 描  述  :
 * 输  入  :  参数        描述
 *         :   phys_addr         请输入phys_addr的描述
 * 输  出  : 无
 * 返回值  : DH_SUCCESS: 成功
 *           DH_FAILURE: 失败
 *******************************************************************************/
void iounmap(void* phys_addr)
{
    return ;
}


