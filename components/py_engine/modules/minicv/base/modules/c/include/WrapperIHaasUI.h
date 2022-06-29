
/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#ifndef WRAPPER_IHAAS_UI_H
#define WRAPPER_IHAAS_UI_H

#include "base/include/base/HaasCommonImage.h"
#include "base/include/base/HaasUIDef.h"
#ifdef __cplusplus
extern "C" {
#endif

    void* UICreateInstance(UIFrameworkType_t type);
    void UIDestoryInstance(void* instance);
	int UIGetDisplayWidth(void* instance);
	int UIGetDisplayHeight(void* instance);
	int UIGetDisplayBacklight(void* instance);
	int UISetDisplayBacklight(void* instance, int32_t value);
	int UIGetDisplayType(void* instance);
	int UIGetDisplayFreq(void* instance);
	int UISetDisplayFreq(void* instance, int32_t freq);
    int UIDrawPoint(void* instance, Point_t *pt, int32_t color);
    int UIDrawLine(void* instance, Point_t *start, Point_t *end, int32_t color);
    int UIDrawRectangle(void* instance, int32_t left, int32_t top, int32_t right, int32_t bottom,
            int32_t color);
    int UIDrawCircle(void* instance, Point_t * center, int32_t radius, int32_t color);
    void UIDrawText(void* instance, char* text, int32_t size, int32_t color, int32_t left,
            int32_t top, int32_t right, int32_t bottom);
    bool UIDrawPixels(void* instance, int32_t* pixels, Point_t *pt, int32_t width, int32_t height);
    bool UIDrawImage(void* instance, char * path, Point_t *pt, int32_t width, int32_t height);
	void UIUpdateDisplay(void* instance);

#ifdef __cplusplus
};
#endif

#endif
