/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _FACEBODY_H_
#define _FACEBODY_H_

#ifdef __cplusplus
extern "C" {
#endif
typedef struct _face_rect_t {
    double x;
    double y;
    double w;
    double h;
} face_rect_t;

typedef int (*facebody_comparing_cb_t)(float confidence, face_rect_t *face_rect);
typedef int (*facebody_recognize_expression_cb_t)(char *expression, float face_probability, face_rect_t *face_rect);
typedef int (*facebody_generate_human_anime_stype_cb_t)(char *out_url);

int facebody_comparing_ai(char *url_a, char *url_b, facebody_comparing_cb_t cb);
int facebody_recognize_expression_ai(char *url, facebody_recognize_expression_cb_t cb);
int facebody_generate_human_anime_style_ai(char *url, facebody_generate_human_anime_stype_cb_t cb);
#ifdef __cplusplus
}
#endif
#endif