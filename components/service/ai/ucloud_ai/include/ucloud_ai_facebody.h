/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */

#ifndef _UCLOUD_AI_FACEBODY_H_
#define _UCLOUD_AI_FACEBODY_H_
#include "facebody.h"

void ucloud_ai_comparing_facebody(char *path, facebody_comparing_cb_t cb);
void ucloud_ai_recognize_expression(char *path, facebody_recognize_expression_cb_t cb);
void ucloud_ai_facebody_generate_human_anime_style(char *path, facebody_generate_human_anime_stype_cb_t cb);

#endif //_UCLOUD_AI_FACEBODY_H_