/*************************************************
  Copyright (C), 2011-2015 ZheJiang Dahua Technology Co., Ltd.
  文件名:   init.h
  作  者:   zheng_xingjian(11853)<zheng_xingjian@dahuatech.com>
  版  本:   V1.0.0
  日  期：  2015-10-24
  描  述:   系统初始化相关头文件，属于eCos3.0对Linux内核的移植补丁层

            1、使用说明
            无

            2、局限性
            仅适用于eCos3.0

  修订历史:
  1. 日    期:
     修订版本:
     作    者:
     修订备注:

  2. 日    期:
     修订版本:
     作    者:
     修订备注:
*************************************************/
#ifndef _KNLPATCH_H_
#define _KNLPATCH_H_


#define I2C_M_RD    (0x0001)
#define __init
#define __exit
#define __user
#define __iomem


/*内存拷贝*/
unsigned int copy_from_user(void* to,const void __user* from, unsigned int n);
unsigned int copy_to_user(void __user* to,const void* from, unsigned int n);

void* ioremap(unsigned int phys_addr, unsigned int size);
void iounmap(void* phys_addr);

#endif
