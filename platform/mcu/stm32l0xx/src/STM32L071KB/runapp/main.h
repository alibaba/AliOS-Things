/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __MAIN_H
#define __MAIN_H


#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif


#endif /* __MAIN_H */
