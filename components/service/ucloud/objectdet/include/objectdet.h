/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _OBJECTDET_H_
#define _OBJECTDET_H_
#ifdef __cplusplus
extern "C" {
#endif
typedef struct _object_rect_t {
    double x;
    double y;
    double w;
    double h;
} object_rect_t;

typedef int (*objectdet_detect_object_cb_t)(char *type, float score, object_rect_t *rect);
int objectdet_detect_object_ai(char *in_url, objectdet_detect_object_cb_t cb);
#ifdef __cplusplus
}
#endif
#endif //_OBJECTDET_H_