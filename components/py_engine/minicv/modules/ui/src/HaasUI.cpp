
#include "HaasUI.h"
#include "HaasLog.h"
#include "HaasErrno.h"
#include "HaasUIFB.h"
#ifdef SYSTEM_OS_LINUX
#include "HaasUIGreenui.h"
#endif
#include "HaasUILvgl.h"
#include "HaasUISdl.h"

extern "C" {
    IHaasUI* IHaasUIInstance(UIFrameworkType_t type)
    {
        LOG_D("entern type = %d;\n", type);
        IHaasUI* mIHaasUI = NULL;
        switch (type)
        {
            case UI_FRAMEWORK_GREENUI:
#ifdef SYSTEM_OS_LINUX
                mIHaasUI = new HaasUIGreenui();
#endif
                break;
            case UI_FRAMEWORK_FB:
                mIHaasUI = new HaasUIFB();
                break;
            case UI_FRAMEWORK_LVGL:
                mIHaasUI = new HaasUILvgl();
                break;
            case UI_FRAMEWORK_SDL:
                mIHaasUI = new HaasUISdl();
                break;
            default:
                LOG_D("illegal type = %d;\n", type);
                mIHaasUI = NULL;
        }
        LOG_D("out type = %d;\n", type);
        return mIHaasUI;
    }
}
