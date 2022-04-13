
#include "HaasUILvgl.h"
#include "HaasLog.h"
#include "HaasErrno.h"

HaasUILvgl::HaasUILvgl()
{
    LOG_D("entern\n");
}

HaasUILvgl::~HaasUILvgl()
{
    LOG_D("entern\n");
}

int HaasUILvgl::GetDisplayWidth()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUILvgl::GetDisplayHeight()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUILvgl::GetDisplayBacklight()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUILvgl::SetDisplayBacklight(int32_t value)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUILvgl::GetDisplayType()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUILvgl::GetDisplayFreq()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUILvgl::SetDisplayFreq(int32_t freq)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUILvgl::DrawPoint(Point_t *pt, int32_t color)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUILvgl::DrawLine(Point_t *start, Point_t *end, int32_t color)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUILvgl::DrawRectangle(int32_t left, int32_t top, int32_t right, int32_t bottom,
        int32_t color)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUILvgl::DrawCircle(Point_t * center, int32_t radius, int32_t color)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

void HaasUILvgl::DrawText(char* text, int32_t size, int32_t color, int32_t left,
        int32_t top, int32_t right, int32_t bottom)
{
    LOG_D("entern\n");
}

bool HaasUILvgl::DrawPixels(int32_t* pixels, Point_t *pt, int32_t width, int32_t height)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

bool HaasUILvgl::DrawImage(char * path, Point_t *pt, int32_t width, int32_t height)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

void HaasUILvgl::UpdateDisplay()
{
    LOG_D("entern\n");
}

