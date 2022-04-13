/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef __ICON_H__
#define __ICON_H__

#include <stdio.h>

typedef struct {
    uint8_t *p_icon_data;
    uint16_t width;
    uint16_t height;
    uint8_t *p_icon_mask;
} icon_t;
// 由于不同扫描方式得到的结果不一样 有的可能是一维数组有的可能是二维
// 很麻烦做抽象 先统一使用列行式吧 可以搞一个枚举 根据不同扫描方式
// 通过像素长宽计算数组长宽和访问方式 增加了p_icon_mask
// 用于绘制透明图象时提供遮罩

#endif
