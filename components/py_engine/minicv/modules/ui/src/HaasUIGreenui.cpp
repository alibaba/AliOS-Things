
#include "HaasUIGreenui.h"
#include "HaasLog.h"
#include "HaasErrno.h"
#include "Window.h"
#include "graphic/Paint.h"
#include "widget/TextView.h"
#include "widget/View.h"
#include "widget/ViewGroup.h"
#include "widget/ImageView.h"
#include <string>

HaasUIGreenui::HaasUIGreenui()
    mWindowManager(NULL),
    mImageView(NULL),
    mTextView(NULL),
    mTickState(false),
    mWin(NULL)
{
    LOG_D("entern\n");
    mWindowManager = WindowManager::instance();
    mWindowManager->getNativeWindow();
    mWindowManager->getNativeWindow()->bootComplete();
    mWin = new Window();
    if (mWin == NULL)
    {
        LOG_D("Create Window failed\n");
    }
    LOG_D("out %s;\n", __func__);
}

HaasUIGreenui::~HaasUIGreenui()
{
    LOG_D("entern\n");
    mWin->dismiss();
    delete mWin;
    mWindowManager->release();
    LOG_D("out %s;\n", __func__);
}

int HaasUIGreenui::GetDisplayWidth()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUIGreenui::GetDisplayHeight()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUIGreenui::GetDisplayBacklight()
{
    LOG_D("entern\n");
    return STATUS_OK;
}
int HaasUIGreenui::SetDisplayBacklight(int32_t value)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUIGreenui::GetDisplayType()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUIGreenui::GetDisplayFreq()
{
    LOG_D("entern\n");
    return STATUS_OK;
}
int HaasUIGreenui::SetDisplayFreq(int32_t freq)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUIGreenui::DrawPoint(Point_t *pt, int32_t color)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUIGreenui::DrawLine(Point_t *start, Point_t *end, int32_t color)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUIGreenui::DrawRectangle(int32_t left, int32_t top, int32_t right, int32_t bottom,
        int32_t color)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasUIGreenui::DrawCircle(Point_t * center, int32_t radius, int32_t color)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

void HaasUIGreenui::DrawText(char* text, int32_t size, int32_t color, int32_t left,
        int32_t top, int32_t right, int32_t bottom)
{
    LOG_D("entern\n");
    int32_t textStride = right - left;
    std::string text_string(text);
    mPaint.setColor(color);
    mPaint.setTextSize(size);
    if (mTextView != NULL)
    {

        mTextView->setTextLayout(new TextLayout(Utf16Text::fromUtf8(text_string), mPaint, textStride));
        mTextView->setFrame(left, top, right, bottom);
        mTextView->invalidate();
    }
    else
    {
        mTextView = new TextView();
        mTextView->setBackgroundColor(0xFFFFFFFF);
        mTextView->setVisible(true);
        mTextView->setTextLayout(new TextLayout(Utf16Text::fromUtf8(text_string), mPaint, textStride));
        mTextView->setFrame(left, top, right, bottom);
        mTextView->REF();
        mTextView->invalidate();
        mWin->getDecorView()->addChild(mTextView);
    }

    TickUIThread();
    LOG_D("out %s;\n", __func__);
}

bool HaasUIGreenui::DrawPixels(int32_t* pixels, Point_t *pt, int32_t width, int32_t height)
{
    LOG_D("entern\n");
    Bitmap *mBitmap = NULL;
    if (mImageView != NULL)
    {
        LOG_D("only set piexles %s;\n", __func__);
        mBitmap = new Bitmap(RGBA8888, width, height);
        mBitmap->setPixels(pixels, 0, width, 0, 0, width, height);
        mImageView->setBitmap(mBitmap);
        mImageView->invalidate();
    }
    else
    {
        LOG_D("new Bitmap %s;\n", __func__);
        mBitmap = new Bitmap(RGBA8888, width, height);
        mBitmap->setPixels(pixels, 0, width, 0, 0, width, height);
        mImageView = new ImageView();
        mImageView->setBackgroundColor(0xFFFFFFFF);
        mImageView->setVisible(true);
        mImageView->setBitmap(mBitmap);
        mImageView->setFrame(0, 0, 800, 600);
        mImageView->REF();
        mImageView->invalidate();
        mWin->getDecorView()->addChild(mImageView);
    }

    TickUIThread();
    LOG_D("out %s;\n", __func__);
    return STATUS_OK;
}

bool HaasUIGreenui::DrawImage(char * path, Point_t *pt, int32_t width, int32_t height)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

void HaasUIGreenui::UpdateDisplay()
{
    LOG_D("entern\n");
    mWin->show();
}

int HaasUIGreenui::startWindowThread()
{
    mWindowManager->getNativeWindow()->loop();
    return STATUS_OK;
}

void HaasUIGreenui::TickUIThread()
{
    LOG_D("entern\n");
    if (mTickState == false)
    {
        mTickState = true;
        UpdateDisplay();
        mUIThread = std::thread([this] {
                LOG_D("stat ui thread\n");
                startWindowThread();
                });
    }
}

