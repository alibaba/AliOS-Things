/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "menu.h"
#include "aos/kernel.h"

// 添加离自己最近的子级目录
#include "homepage/homepage.h"
#include "humiture/humiture.h"
#include "lightmeter/lightmeter.h"
#include "shakeshake/shakeshake.h"
#include "compass/compass.h"
#include "barometer/barometer.h"
#include "greedySnake/greedySnake.h"
#include "gyroscope/gyroscope.h"
#include "aircraftBattle/aircraftBattle.h"
#include "musicbox/musicbox.h"

#define KEY_CODE_BACK (EDK_KEY_1 | EDK_KEY_2)
#define KEY_CODE_LEFT EDK_KEY_1
#define KEY_CODE_RIGHT EDK_KEY_3
#define KEY_CODE_DOWN EDK_KEY_4

MENU_TYP *Menus[] = {
    &homepage,
    &humiture,
    &gyroscope,
    &shakeshake,
    &compass,
    &barometer,
    &lightmeter,
    &musicbox,
    &greedySnake,
    &aircraftBattle,
};
MENU_LIST_TYP MenuList = {
    Menus,
    10};

MENU_TYP *pCurMenu = NULL;
key_code_cb app_key_code_cb = NULL;

void menu_list_fix(MENU_LIST_TYP *menuChildList, MENU_TYP *menuParent)
{
    uint8_t curLevelMenuSize = menuChildList->MenuListSize;
    printf("curLevelMenuSize %d\n", curLevelMenuSize);
    for (uint8_t i = 0; i < curLevelMenuSize; i++)
    {
        if (menuParent != NULL)
        {
            menuParent->pChild = menuChildList->pMenuList[0];  // 父节点只会指向第一个 懒得做判断了
            menuChildList->pMenuList[i]->pParent = menuParent; // 子节点都连到父节点上
            // menuChildList->pMenuList[i]->MenuID = i | ((menuParent->MenuID) * 10); // 子级目录 id向前推8个bit  elliott
        }
        else
        {
            menuChildList->pMenuList[i]->pParent = NULL;
            // menuChildList->pMenuList[i]->MenuID = i; // id 在第一级上 直接等于当前index
        }
        menuChildList->pMenuList[i]->pRight = ((i == curLevelMenuSize - 1) ? menuChildList->pMenuList[0] : menuChildList->pMenuList[i + 1]);
        menuChildList->pMenuList[i]->pLeft = ((i == 0) ? menuChildList->pMenuList[curLevelMenuSize - 1] : menuChildList->pMenuList[i - 1]);
        printf("menu info %s %d\n", menuChildList->pMenuList[i]->MenuName);
        MENU_LIST_TYP *pchildlist = menuChildList->pMenuList[i]->pChildrenList;
        if (pchildlist != NULL)
            printf("pchildlist->MenuListSize \n", pchildlist->MenuListSize);
        if (pchildlist != NULL && pchildlist->MenuListSize > 0)
        {
            printf("pchildlist->MenuListSize \n", pchildlist->MenuListSize);
            menu_list_fix(menuChildList->pMenuList[i]->pChildrenList, menuChildList->pMenuList[i]); // 如果有孩子 做一个迭代
        }
    }
}

void menu_init()
{
    menu_list_fix(&MenuList, NULL);
    pCurMenu = MenuList.pMenuList[0];
    key_init(public_key_event_handle);
    app_key_code_cb = menu_key_event_handle;
    aos_task_new("menu_show_cover_task", menu_show_cover_task, NULL, 5000);

    if (pCurMenu->MenuCover->MenuCoverMode == MENU_COVER_NONE)
    {
        menu_task_start(pCurMenu);
    }
}

static void menu_show_cover_task(void)
{
    static int loading_draw_index = 0;
    static int error_draw_index = 0;
    while (1)
    {
        if (pCurMenu != NULL)
        {
            // printf("%s state %d\n", pCurMenu->MenuName, pCurMenu->pMenuTask->menu_task_state);
        }
        if (pCurMenu != NULL && pCurMenu->pMenuTask->menu_task_state == MENU_TASK_IDLE) // 没进入task之前，和从task退出之后 才显示封面
        {
            switch (pCurMenu->MenuCover->MenuCoverMode)
            {
            case MENU_COVER_NONE:
                /* code */
                printf("[E] You are not suppose to get here %s:%d\n", __func__, __LINE__);
                aos_msleep(1000);
                break;
            case MENU_COVER_TEXT:
                OLED_Clear();
                if (pCurMenu->MenuCover->text != NULL)
                {
                    OLED_Show_String(22, 4, pCurMenu->MenuCover->text, 12, 1);
                }
                OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
                OLED_Icon_Draw(122, 24, &icon_skip_right, 0);
                OLED_Refresh_GRAM();
                aos_msleep(200);
                break;
            case MENU_COVER_IMG:
                OLED_Clear();
                if (pCurMenu->MenuCover->img != NULL)
                {
                    OLED_Icon_Draw(0, 0, pCurMenu->MenuCover->img, 1);
                }
                OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
                OLED_Icon_Draw(122, 24, &icon_skip_right, 0);
                OLED_Refresh_GRAM();
                aos_msleep(200);
                break;
            case MENU_COVER_FUNC:
                if (pCurMenu->MenuCover->draw_func != NULL)
                    pCurMenu->MenuCover->draw_func(&(pCurMenu->MenuCover->draw_index));
                break;
            default:
                aos_msleep(1000);
                break;
            }
        }
        else if (pCurMenu != NULL && (pCurMenu->pMenuTask->menu_task_state == MENU_TASK_UNINITING || pCurMenu->pMenuTask->menu_task_state == MENU_TASK_INITING))
        {
            menu_loading_draw(&loading_draw_index);
        }
        else if (pCurMenu == NULL)
        {
            menu_error_draw(&error_draw_index);
        }
        else
        {
            aos_msleep(500);
        }
    }
}

void menu_loading_draw(int *draw_index)
{
    (*draw_index)++;
    if ((*draw_index) >= 8)
        (*draw_index) = 0;

    OLED_Clear();
    switch ((*draw_index))
    {
    case 0:
        OLED_Icon_Draw(50, 4, &icon_loadlines0_32_32, 0);
        OLED_Show_String(36, 42, "loading", 12, 1);
        break;
    case 1:
        OLED_Icon_Draw(50, 4, &icon_loadlines1_32_32, 0);
        OLED_Show_String(36, 42, "loading.", 12, 1);
        break;
    case 2:
        OLED_Icon_Draw(50, 4, &icon_loadlines2_32_32, 0);
        OLED_Show_String(36, 42, "loading..", 12, 1);
        break;
    case 3:
        OLED_Icon_Draw(50, 4, &icon_loadlines3_32_32, 0);
        OLED_Show_String(36, 42, "loading...", 12, 1);
        break;
    case 4:
        OLED_Icon_Draw(50, 4, &icon_loadlines4_32_32, 0);
        OLED_Show_String(36, 42, "loading", 12, 1);
        break;
    case 5:
        OLED_Icon_Draw(50, 4, &icon_loadlines5_32_32, 0);
        OLED_Show_String(36, 42, "loading.", 12, 1);
        break;
    case 6:
        OLED_Icon_Draw(50, 4, &icon_loadlines6_32_32, 0);
        OLED_Show_String(36, 42, "loading..", 12, 1);
        break;
    case 7:
        OLED_Icon_Draw(50, 4, &icon_loadlines7_32_32, 0);
        OLED_Show_String(36, 42, "loading...", 12, 1);
        break;
    default:
        break;
    }

    OLED_Refresh_GRAM();
    aos_msleep(100);
}

void menu_error_draw(int *draw_index)
{
    OLED_Clear();
    OLED_Icon_Draw(54, 10, &icon_error_ufo, 1);
    OLED_Show_String(15, 42, "Please reset me !", 12, 1);
    OLED_Refresh_GRAM();
    aos_msleep(500);
}

static void public_key_event_handle(key_code_t key_code)
{
    // printf("public_key_event_handle receive key_code 0x%02X\n", key_code);
    if (key_code == KEY_CODE_BACK) // 按键回退 只有这个不会分发给具体的应用
    {
        app_key_code_cb = menu_key_event_handle; // 还给默认按键处理函数
        if (pCurMenu != NULL)
        {
            // 几种情况 有封面的 如果这个时候没在执行 应该退回上一个封面 也就是 pCurMenu = pCurMenu->pParent;
            // 有封面的 如果这个时候在执行 应该退回到自己的封面 也就是 resume menu_show_cover_task
            // 没封面的 既然在这肯定在执行了 那就直接exit掉 然后该回的回 是父级就不用响应了
            if (pCurMenu->MenuCover->MenuCoverMode != MENU_COVER_NONE)
            {
                if (pCurMenu->pMenuTask->menu_task_state != MENU_TASK_RUNNING)
                {
                    if (pCurMenu->pParent != NULL)
                        pCurMenu = pCurMenu->pParent;
                    // 上级封面肯定不存在无cover的情况 这里不用处理
                }
                else
                {
                    menu_task_exit(pCurMenu);
                }
            }
            else // MENU_COVER_NONE
            {
                if (pCurMenu->pParent != NULL)
                {
                    menu_task_exit(pCurMenu); // 如果没有父节点 就不用响应quit 有的话 就指给父节点 再根据父节点类型判断要不要run task
                    pCurMenu = pCurMenu->pParent;
                    if (pCurMenu->MenuCover->MenuCoverMode == MENU_COVER_NONE)
                    {
                        menu_task_start(pCurMenu);
                    }
                }
            }
        }
    }
    else
    {
        if (app_key_code_cb != NULL)
        {
            app_key_code_cb(key_code);
        }
        else
        {
            printf("app_key_code_cb is null \n");
        }
    }
}

void switch_menu(MENU_TYP *pCurMenu, MENU_TYP *pTargetMenu)
{
    //  如果封面也能够统一成 task 那么所有的操作都是task的切换
}

int menu_task_start(MENU_TYP *pMenu)
{
    if (pMenu == NULL)
    {
        printf("pMenu null in %s:%d\n", __func__, __LINE__);
        return -1;
    }

    // 先卸载按键
    app_key_code_cb = NULL;

    if (pMenu->pMenuTask != NULL)
    {
        if (pMenu->pMenuTask->menu_task_state == MENU_TASK_IDLE)
        {
            if (pMenu->pMenuTask->pMenuTaskInit != NULL)
            {
                pMenu->pMenuTask->menu_task_state = MENU_TASK_INITING;
                if (pMenu->pMenuTask->pMenuTaskInit() == 0)
                {
                    pMenu->pMenuTask->menu_task_state = MENU_TASK_RUNNING;
                    app_key_code_cb = (pMenu->pTaskKeyDeal == NULL) ? menu_key_event_handle : pMenu->pTaskKeyDeal;
                    return 0;
                }
                else
                {
                    printf("[E] %s task init func null\n", pMenu->MenuName);
                    app_key_code_cb = menu_key_event_handle;
                    return 1;
                }
            }
            else
            {
                pMenu->pMenuTask->menu_task_state = MENU_TASK_IDLE;
                app_key_code_cb = menu_key_event_handle;
                return 1;
            }
        }
        else
        {
            printf("[E] %s task state err %d\n", pMenu->MenuName, pMenu->pMenuTask->menu_task_state);
            pMenu->pMenuTask->menu_task_state = MENU_TASK_IDLE;
            app_key_code_cb = menu_key_event_handle;
            return 1;
        }
    }
    else
    {
        printf("[E] %s pMenuTask null in %s:%d\n", pMenu->MenuName, __func__, __LINE__);
        app_key_code_cb = menu_key_event_handle;
        return 1;
    }
}

int menu_task_exit(MENU_TYP *pMenu)
{
    if (pMenu == NULL)
    {
        printf("pMenu null in %s:%d\n", __func__, __LINE__);
        return -1;
    }
    app_key_code_cb = NULL;

    if (pMenu->pMenuTask != NULL)
    {
        if (pMenu->pMenuTask->menu_task_state == MENU_TASK_RUNNING)
        {
            if (pMenu->pMenuTask->pMenuTaskEnd != NULL)
            {
                pMenu->pMenuTask->menu_task_state = MENU_TASK_UNINITING;
                if (pMenu->pMenuTask->pMenuTaskEnd() == 0)
                {
                    pMenu->pMenuTask->menu_task_state = MENU_TASK_IDLE;
                    app_key_code_cb = menu_key_event_handle;
                    return 0;
                }
                else
                {
                    printf("[E] %s task exit fail\n", pMenu->MenuName);
                    return 1;
                }
            }
            else
            {
                pMenu->pMenuTask->menu_task_state = MENU_TASK_IDLE;
                app_key_code_cb = menu_key_event_handle;
                return 0;
            }
        }
        else
        {
            pMenu->pMenuTask->menu_task_state = MENU_TASK_IDLE;
            app_key_code_cb = menu_key_event_handle;
            return 0;
        }
    }
    else
    {
        app_key_code_cb = menu_key_event_handle;
        return 0;
    }
}

// 设计规则 cover app 不响应外部 left right 只响应quit
// no cover app  响应 left right quit（直接回父级）
static void menu_key_event_handle(key_code_t key_code)
{
    if (pCurMenu == NULL)
    {
        printf("pCurMenu null in %s:%d\n", __func__, __LINE__);
        return;
    }

    switch (key_code)
    {
    case KEY_CODE_RIGHT:
        if (pCurMenu->MenuCover->MenuCoverMode == MENU_COVER_NONE) // 对于无cover的app 需要切换指针和task
        {
            menu_task_exit(pCurMenu);
            pCurMenu = pCurMenu->pRight;
            // 下个菜单可能需要自动运行 在下一个switch统一处理
        }
        else
        {
            if (pCurMenu->pMenuTask->menu_task_state == MENU_TASK_IDLE) // 对于有cover的app 如果不在运行 就直接切换 在运行 就不要响应
                pCurMenu = pCurMenu->pRight;
        }

        break;
    case KEY_CODE_LEFT:
        if (pCurMenu->MenuCover->MenuCoverMode == MENU_COVER_NONE) // （对于无cover的app） 如果当前task正在执行 需要还原给menu
        {
            menu_task_exit(pCurMenu);
            pCurMenu = pCurMenu->pLeft;
        }
        else
        {
            if (pCurMenu->pMenuTask->menu_task_state == MENU_TASK_IDLE) // 对于有cover的app 如果不在运行 就直接切换 在运行 就不要响应
                pCurMenu = pCurMenu->pLeft;
        }
        break;
    case KEY_CODE_DOWN:
        if (pCurMenu->pChild != NULL) // 有下级菜单 先进去
            pCurMenu = pCurMenu->pChild;
        else // 没下级菜单了 应该是执行自己了 有child不应该有task
        {
            if (pCurMenu->MenuCover->MenuCoverMode != MENU_COVER_NONE) // 只有有cover的类型才会在这里响应
            {
                menu_task_start(pCurMenu);
            }
        }
        break;
    default:
        break;
    }

    switch (key_code)
    {
    case KEY_CODE_RIGHT:
    case KEY_CODE_LEFT:
    case KEY_CODE_DOWN:
        if (pCurMenu == NULL) // 这里是 已经转换过的 pCurMenu
        {
            printf("pCurMenu null in %s:%d\n", __func__, __LINE__);
            return;
        }
        if (pCurMenu->MenuCover->MenuCoverMode == MENU_COVER_NONE) // （对于无cover的app） 进来直接执行 对于有Cover的 只有Down有反应 在上面处理过了
        {
            menu_task_start(pCurMenu);
        }
        break;
    }
}
