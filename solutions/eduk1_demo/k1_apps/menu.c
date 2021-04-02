#include <stdio.h>
#include <stdlib.h>

#include "aos/kernel.h"
#include "menu.h"

// 添加离自己最近的子级目录
#include "aircraftBattle/aircraftBattle.h"
#include "barometer/barometer.h"
#include "compass/compass.h"
#include "greedySnake/greedySnake.h"
#include "gyroscope/gyroscope.h"
#include "homepage/homepage.h"
#include "humiture/humiture.h"
#include "lightmeter/lightmeter.h"
#include "musicbox/musicbox.h"
#include "shakeshake/shakeshake.h"

#define KEY_CODE_BACK  (EDK_KEY_1 | EDK_KEY_2)
#define KEY_CODE_LEFT  EDK_KEY_1
#define KEY_CODE_RIGHT EDK_KEY_3
#define KEY_CODE_DOWN  EDK_KEY_4

MENU_TYP *Menus[] = {
    &homepage,
    &humiture, &gyroscope, &shakeshake,  &compass,        &barometer,
    &lightmeter, &musicbox,  &greedySnake, &aircraftBattle,
};
MENU_LIST_TYP MenuList = {Menus, sizeof(Menus)/sizeof(Menus[0])};

MENU_TYP *pCurMenu = NULL;
key_code_cb app_key_code_cb = NULL;

void menu_list_fix(MENU_LIST_TYP *menuChildList, MENU_TYP *menuParent)
{
    uint8_t curLevelMenuSize = menuChildList->MenuListSize;
    printf("curLevelMenuSize %d\n", curLevelMenuSize);
    for (uint8_t i = 0; i < curLevelMenuSize; i++) {
        if (menuParent != NULL) {
            menuParent->pChild = menuChildList->pMenuList[0];
            menuChildList->pMenuList[i]->pParent = menuParent;
        } else {
            menuChildList->pMenuList[i]->pParent = NULL;
        }
        menuChildList->pMenuList[i]->pRight =
            ((i == curLevelMenuSize - 1) ? menuChildList->pMenuList[0] :
                                           menuChildList->pMenuList[i + 1]);
        menuChildList->pMenuList[i]->pLeft =
            ((i == 0) ? menuChildList->pMenuList[curLevelMenuSize - 1] :
                        menuChildList->pMenuList[i - 1]);
        MENU_LIST_TYP *pchildlist = menuChildList->pMenuList[i]->pChildrenList;
        if (pchildlist != NULL)
            printf("pchildlist->MenuListSize\n", pchildlist->MenuListSize);
        if (pchildlist != NULL && pchildlist->MenuListSize > 0) {
            printf("pchildlist->MenuListSize\n", pchildlist->MenuListSize);
            menu_list_fix(menuChildList->pMenuList[i]->pChildrenList,
                          menuChildList->pMenuList[i]);
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

    if (pCurMenu->MenuCover->MenuCoverMode == MENU_COVER_NONE) {
        menu_task_start(pCurMenu);
    }
}

static void menu_show_cover_task(void)
{
    static int loading_draw_index = 0;
    static int error_draw_index   = 0;
    while (1) {
        if (pCurMenu != NULL) {
            // printf("%s state %d\n", pCurMenu->MenuName,
            // pCurMenu->pMenuTask->menu_task_state);
        }
        if (pCurMenu != NULL &&
            pCurMenu->pMenuTask->menu_task_state == MENU_TASK_IDLE) {
            switch (pCurMenu->MenuCover->MenuCoverMode) {
                case MENU_COVER_NONE:
                    printf("[E] You are not suppose to get here %s:%d\n",
                           __func__, __LINE__);
                    aos_msleep(1000);
                    break;
                case MENU_COVER_TEXT:
                    OLED_Clear();
                    if (pCurMenu->MenuCover->text != NULL) {
                        OLED_Show_String(22, 4, pCurMenu->MenuCover->text, 12,
                                         1);
                    }
                    OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
                    OLED_Icon_Draw(122, 24, &icon_skip_right, 0);
                    OLED_Refresh_GRAM();
                    aos_msleep(200);
                    break;
                case MENU_COVER_IMG:
                    OLED_Clear();
                    if (pCurMenu->MenuCover->img != NULL) {
                        OLED_Icon_Draw(0, 0, pCurMenu->MenuCover->img, 1);
                    }
                    OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
                    OLED_Icon_Draw(122, 24, &icon_skip_right, 0);
                    OLED_Refresh_GRAM();
                    aos_msleep(200);
                    break;
                case MENU_COVER_FUNC:
                    if (pCurMenu->MenuCover->draw_func != NULL)
                        pCurMenu->MenuCover->draw_func(
                            &(pCurMenu->MenuCover->draw_index));
                    break;
                default:
                    aos_msleep(1000);
                    break;
            }
        } else if (pCurMenu != NULL && (pCurMenu->pMenuTask->menu_task_state ==
                                            MENU_TASK_UNINITING ||
                                        pCurMenu->pMenuTask->menu_task_state ==
                                            MENU_TASK_INITING)) {
            menu_loading_draw(&loading_draw_index);
        } else if (pCurMenu == NULL) {
            menu_error_draw(&error_draw_index);
        } else {
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
    switch ((*draw_index)) {
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
    if (key_code == KEY_CODE_BACK) {
        app_key_code_cb = menu_key_event_handle;
        if (pCurMenu != NULL) {
            if (pCurMenu->MenuCover->MenuCoverMode != MENU_COVER_NONE) {
                if (pCurMenu->pMenuTask->menu_task_state != MENU_TASK_RUNNING) {
                    if (pCurMenu->pParent != NULL)
                        pCurMenu = pCurMenu->pParent;
                } else {
                    menu_task_exit(pCurMenu);
                }
            } else {
                if (pCurMenu->pParent != NULL) {
                    menu_task_exit(pCurMenu);
                    pCurMenu = pCurMenu->pParent;
                    if (pCurMenu->MenuCover->MenuCoverMode == MENU_COVER_NONE) {
                        menu_task_start(pCurMenu);
                    }
                }
            }
        }
    } else {
        if (app_key_code_cb != NULL) {
            app_key_code_cb(key_code);
        } else {
            printf("app_key_code_cb is null\n");
        }
    }
}

static int menu_task_start(MENU_TYP *pMenu)
{
    if (pMenu == NULL) {
        printf("pMenu null in %s:%d\n", __func__, __LINE__);
        return -1;
    }

    // 先卸载按键
    app_key_code_cb = NULL;

    if (pMenu->pMenuTask != NULL) {
        if (pMenu->pMenuTask->menu_task_state == MENU_TASK_IDLE) {
            if (pMenu->pMenuTask->pMenuTaskInit != NULL) {
                pMenu->pMenuTask->menu_task_state = MENU_TASK_INITING;
                if (pMenu->pMenuTask->pMenuTaskInit() == 0) {
                    pMenu->pMenuTask->menu_task_state = MENU_TASK_RUNNING;
                    app_key_code_cb = (pMenu->pTaskKeyDeal == NULL) ?
                                          menu_key_event_handle :
                                          pMenu->pTaskKeyDeal;
                    return 0;
                } else {
                    printf("[E] %s task init func null\n", pMenu->MenuName);
                    app_key_code_cb = menu_key_event_handle;
                    return 1;
                }
            } else {
                pMenu->pMenuTask->menu_task_state = MENU_TASK_IDLE;
                app_key_code_cb                   = menu_key_event_handle;
                return 1;
            }
        } else {
            printf("[E] %s task state err %d\n", pMenu->MenuName,
                   pMenu->pMenuTask->menu_task_state);
            pMenu->pMenuTask->menu_task_state = MENU_TASK_IDLE;
            app_key_code_cb                   = menu_key_event_handle;
            return 1;
        }
    } else {
        printf("[E] %s pMenuTask null in %s:%d\n", pMenu->MenuName, __func__,
               __LINE__);
        app_key_code_cb = menu_key_event_handle;
        return 1;
    }
}

static int menu_task_exit(MENU_TYP *pMenu)
{
    if (pMenu == NULL) {
        printf("pMenu null in %s:%d\n", __func__, __LINE__);
        return -1;
    }
    app_key_code_cb = NULL;

    if (pMenu->pMenuTask != NULL) {
        if (pMenu->pMenuTask->menu_task_state == MENU_TASK_RUNNING) {
            if (pMenu->pMenuTask->pMenuTaskEnd != NULL) {
                pMenu->pMenuTask->menu_task_state = MENU_TASK_UNINITING;
                if (pMenu->pMenuTask->pMenuTaskEnd() == 0) {
                    pMenu->pMenuTask->menu_task_state = MENU_TASK_IDLE;
                    app_key_code_cb                   = menu_key_event_handle;
                    return 0;
                } else {
                    printf("[E] %s task exit fail\n", pMenu->MenuName);
                    return 1;
                }
            } else {
                pMenu->pMenuTask->menu_task_state = MENU_TASK_IDLE;
                app_key_code_cb                   = menu_key_event_handle;
                return 0;
            }
        } else {
            pMenu->pMenuTask->menu_task_state = MENU_TASK_IDLE;
            app_key_code_cb                   = menu_key_event_handle;
            return 0;
        }
    } else {
        app_key_code_cb = menu_key_event_handle;
        return 0;
    }
}

static void menu_key_event_handle(key_code_t key_code)
{
    if (pCurMenu == NULL) {
        printf("pCurMenu null in %s:%d\n", __func__, __LINE__);
        return;
    }

    switch (key_code) {
        case 4:
            if (pCurMenu->MenuCover->MenuCoverMode == MENU_COVER_NONE) {
                menu_task_exit(pCurMenu);
                pCurMenu = pCurMenu->pRight;
            } else {
                if (pCurMenu->pMenuTask->menu_task_state == MENU_TASK_IDLE)
                    pCurMenu = pCurMenu->pRight;
            }

            break;
        case KEY_CODE_LEFT:
            if (pCurMenu->MenuCover->MenuCoverMode == MENU_COVER_NONE) {
                menu_task_exit(pCurMenu);
                pCurMenu = pCurMenu->pLeft;
            } else {
                if (pCurMenu->pMenuTask->menu_task_state == MENU_TASK_IDLE)
                    pCurMenu = pCurMenu->pLeft;
            }
            break;
        case KEY_CODE_DOWN:
            if (pCurMenu->pChild != NULL) {
                pCurMenu = pCurMenu->pChild;
            }

            else {
                if (pCurMenu->MenuCover->MenuCoverMode != MENU_COVER_NONE) {
                    menu_task_start(pCurMenu);
                }
            }
            break;
        default:
            break;
    }

    switch (key_code) {
        case KEY_CODE_RIGHT:
        case KEY_CODE_LEFT:
            if (pCurMenu == NULL) {
                printf("pCurMenu null in %s:%d\n", __func__, __LINE__);
                return;
            }
            if (pCurMenu->MenuCover->MenuCoverMode == MENU_COVER_NONE) {
                menu_task_start(pCurMenu);
            }
            break;
    }
}
