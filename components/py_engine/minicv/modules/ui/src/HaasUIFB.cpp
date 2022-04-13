
#include "HaasUIFB.h"
#include "HaasLog.h"
#include "HaasErrno.h"

HaasUIFB::HaasUIFB()
{
    LOG_D("entern\n");
}

HaasUIFB::~HaasUIFB()
{
    LOG_D("entern\n");
}

int HaasUIFB::GetDisplayWidth()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUIFB::GetDisplayHeight()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUIFB::GetDisplayBacklight()
{
    LOG_D("entern\n");
    return STATUS_OK;
}
int HaasUIFB::SetDisplayBacklight(int32_t value)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUIFB::GetDisplayType()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUIFB::GetDisplayFreq()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUIFB::SetDisplayFreq(int32_t freq)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUIFB::DrawPoint(Point_t *pt, int32_t color)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUIFB::DrawLine(Point_t *start, Point_t *end, int32_t color)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUIFB::DrawRectangle(int32_t left, int32_t top, int32_t right, int32_t bottom,
        int32_t color)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUIFB::DrawCircle(Point_t * center, int32_t radius, int32_t color)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

void HaasUIFB::DrawText(char* text, int32_t size, int32_t color, int32_t left,
        int32_t top, int32_t right, int32_t bottom)
{
    LOG_D("entern\n");
}

bool HaasUIFB::DrawPixels(int32_t* pixels, Point_t *pt, int32_t width, int32_t height)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

bool HaasUIFB::DrawImage(char * path, Point_t *pt, int32_t width, int32_t height)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

void HaasUIFB::UpdateDisplay()
{
    LOG_D("entern\n");
}

