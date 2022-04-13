
#include "WrapperIHaasUI.h"
#include "IHaasUI.h"
#include "HaasLog.h"
#include "HaasErrno.h"
#include "IHaasUI.h"

void* UICreateInstance(UIFrameworkType_t type)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasUI* mIHaasUI =  IHaasUIInstance(type);
    if (mIHaasUI == NULL)
    {
        LOG_D("IHaasInstance failed %s;\n", __func__);
        return NULL;
    }
    return (void*)mIHaasUI;
#else
    return NULL;
#endif
}

void UIDestoryInstance(void* instance)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasUI* mIHaasUI = (IHaasUI*)instance;
    if (mIHaasUI == NULL)
    {
        LOG_D("mIHaasUI is NULL %s;\n", __func__);
        return;
    }
    delete mIHaasUI;
#endif
}

int UIGetDisplayWidth(void* instance)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasUI* mIHaasUI = (IHaasUI*)instance;
    if (mIHaasUI == NULL)
    {
        LOG_D("mIHaasUI is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasUI->GetDisplayWidth();
    return ret;
#else
    return 0;
#endif
}

int UIGetDisplayHeight(void* instance)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasUI* mIHaasUI = (IHaasUI*)instance;
    if (mIHaasUI == NULL)
    {
        LOG_D("mIHaasUI is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasUI->GetDisplayHeight();
    return ret;
#else
    return 0;
#endif
}

int UIGetDisplayBacklight(void* instance)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasUI* mIHaasUI = (IHaasUI*)instance;
    if (mIHaasUI == NULL)
    {
        LOG_D("mIHaasUI is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasUI->GetDisplayBacklight();
    return ret;
#else
    return 0;
#endif
}

int UISetDisplayBacklight(void* instance, int32_t value)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasUI* mIHaasUI = (IHaasUI*)instance;
    if (mIHaasUI == NULL)
    {
        LOG_D("mIHaasUI is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasUI->SetDisplayBacklight(value);
    return ret;
#else
    return 0;
#endif
}

int UIGetDisplayType(void* instance)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasUI* mIHaasUI = (IHaasUI*)instance;
    if (mIHaasUI == NULL)
    {
        LOG_D("mIHaasUI is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasUI->GetDisplayType();
    return ret;
#else
    return 0;
#endif
}

int UIGetDisplayFreq(void* instance)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasUI* mIHaasUI = (IHaasUI*)instance;
    if (mIHaasUI == NULL)
    {
        LOG_D("mIHaasUI is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasUI->GetDisplayFreq();
    return ret;
#else
    return 0;
#endif
}

int UISetDisplayFreq(void* instance, int32_t freq)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasUI* mIHaasUI = (IHaasUI*)instance;
    if (mIHaasUI == NULL)
    {
        LOG_D("mIHaasUI is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasUI->SetDisplayFreq(freq);
    return ret;
#else
    return 0;
#endif
}

int UIDrawPoint(void* instance, Point_t *pt, int32_t color)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasUI* mIHaasUI = (IHaasUI*)instance;
    if (mIHaasUI == NULL)
    {
        LOG_D("mIHaasUI is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasUI->DrawPoint(pt, color);
    return ret;
#else
    return 0;
#endif
}

int UIDrawLine(void* instance, Point_t *start, Point_t *end, int32_t color)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasUI* mIHaasUI = (IHaasUI*)instance;
    if (mIHaasUI == NULL)
    {
        LOG_D("mIHaasUI is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasUI->DrawLine(start, end, color);
    return ret;
#else
    return 0;
#endif
}

int UIDrawRectangle(void* instance, int32_t left, int32_t top, int32_t right, int32_t bottom,
        int32_t color)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasUI* mIHaasUI = (IHaasUI*)instance;
    if (mIHaasUI == NULL)
    {
        LOG_D("mIHaasUI is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasUI->DrawRectangle(left, top, right, bottom, color);
    return ret;
#else
    return 0;
#endif
}

int UIDrawCircle(void* instance, Point_t * center, int32_t radius, int32_t color)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasUI* mIHaasUI = (IHaasUI*)instance;
    if (mIHaasUI == NULL)
    {
        LOG_D("mIHaasUI is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasUI->DrawCircle(center, radius, color);
    return ret;
#else
    return 0;
#endif
}

void UIDrawText(void* instance, char* text, int32_t size, int32_t color, int32_t left,
        int32_t top, int32_t right, int32_t bottom)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasUI* mIHaasUI = (IHaasUI*)instance;
    if (mIHaasUI == NULL)
    {
        LOG_D("mIHaasUI is NULL %s;\n", __func__);
        return;
    }
    mIHaasUI->DrawText(text, size, color, left, top, right, bottom);
#endif
}

bool UIDrawPixels(void* instance, int32_t* pixels, Point_t *pt, int32_t width, int32_t height)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasUI* mIHaasUI = (IHaasUI*)instance;
    if (mIHaasUI == NULL)
    {
        LOG_D("mIHaasUI is NULL %s;\n", __func__);
        return false;
    }
    bool ret = mIHaasUI->DrawPixels(pixels, pt, width, height);
    return ret;
#else
    return 0;
#endif
}

bool UIDrawImage(void* instance, char * path, Point_t *pt, int32_t width, int32_t height)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasUI* mIHaasUI = (IHaasUI*)instance;
    if (mIHaasUI == NULL)
    {
        LOG_D("mIHaasUI is NULL %s;\n", __func__);
        return false;
    }
    bool ret = mIHaasUI->DrawImage(path, pt, width, height);
    return ret;
#else
    return 0;
#endif
}

void UIUpdateDisplay(void* instance)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasUI* mIHaasUI = (IHaasUI*)instance;
    if (mIHaasUI == NULL)
    {
        LOG_D("mIHaasUI is NULL %s;\n", __func__);
        return;
    }
    mIHaasUI->UpdateDisplay();
#endif
}

