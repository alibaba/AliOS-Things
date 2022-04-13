
#include "HaasUISdl.h"
#include "HaasLog.h"
#include "HaasErrno.h"

HaasUISdl::HaasUISdl()
{
    LOG_D("entern\n");
}

HaasUISdl::~HaasUISdl()
{
    LOG_D("entern\n");
}

int HaasUISdl::GetDisplayWidth()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUISdl::GetDisplayHeight()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUISdl::GetDisplayBacklight()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUISdl::SetDisplayBacklight(int32_t value)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUISdl::GetDisplayType()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUISdl::GetDisplayFreq()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUISdl::SetDisplayFreq(int32_t freq)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUISdl::DrawPoint(Point_t *pt, int32_t color)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUISdl::DrawLine(Point_t *start, Point_t *end, int32_t color)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUISdl::DrawRectangle(int32_t left, int32_t top, int32_t right, int32_t bottom,
        int32_t color)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUISdl::DrawCircle(Point_t * center, int32_t radius, int32_t color)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

void HaasUISdl::DrawText(char* text, int32_t size, int32_t color, int32_t left,
        int32_t top, int32_t right, int32_t bottom)
{
    LOG_D("entern\n");
}

bool HaasUISdl::DrawPixels(int32_t* pixels, Point_t *pt, int32_t width, int32_t height)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

bool HaasUISdl::DrawImage(char * path, Point_t *pt, int32_t width, int32_t height)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

void HaasUISdl::UpdateDisplay()
{
    LOG_D("entern\n");
}

