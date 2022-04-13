#ifndef HAAS_UI_H
#define HAAS_UI_H

#include <thread>
#include "IHaasUI.h"
#ifdef SYSTEM_OS_LINUX
#include "HaasUIStarry.h"
#endif

class HaasUI : public IHaasUI
{
public:
	HaasUI();
	virtual ~HaasUI();

	virtual int GetDisplayWidth();
	virtual int GetDisplayHeight();

	virtual int GetDisplayBacklight();
	virtual int SetDisplayBacklight(int32_t value);

	virtual int GetDisplayType();

	virtual int GetDisplayFreq();
	virtual int SetDisplayFreq(int32_t freq);

    virtual int DrawPoint(Point_t *pt, int32_t color);
    virtual int DrawLine(Point_t *start, Point_t *end, int32_t color);
    virtual int DrawRectangle(int32_t left, int32_t top, int32_t right, int32_t bottom,
            int32_t color);
    virtual int DrawCircle(Point_t * center, int32_t radius, int32_t color);
    virtual void DrawText(char* text, int32_t size, int32_t color, int32_t left,
            int32_t top, int32_t right, int32_t bottom);
    virtual bool DrawPixels(int32_t* pixels, Point_t *pt, int32_t width, int32_t height);
    virtual bool DrawImage(char * path, Point_t *pt, int32_t width, int32_t height);

	virtual void UpdateDisplay();
private:
    void TickUIThread();
#ifdef SYSTEM_OS_LINUX
    void * mWindow;
    StarryUI *mStarryUI;
    bool   mTickState;
    std::thread mUIThread;
#endif
};
#endif
