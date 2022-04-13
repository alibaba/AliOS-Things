/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2020 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "../../SDL_internal.h"

#ifdef SDL_INPUT_LINUXEV
#include "uinput.h"
#include "input/input.h"

/* This is based on the linux joystick driver */
/* References: https://www.kernel.org/doc/Documentation/input/input.txt
 *             https://www.kernel.org/doc/Documentation/input/event-codes.txt
 *             /usr/include/linux/input.h
 *             The evtest application is also useful to debug the protocol
 */

#include "SDL_evdev.h"

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "SDL.h"
#include "SDL_assert.h"
#include "SDL_endian.h"
#include "SDL_scancode.h"
#include "../../events/SDL_events_c.h"
#include "../../events/scancodes_linux.h" /* adds linux_scancode_table */
#include "../../core/linux/SDL_udev.h"

/* These are not defined in older Linux kernel headers */
#ifndef SYN_DROPPED
#define SYN_DROPPED 3
#endif
#ifndef ABS_MT_SLOT
#define ABS_MT_SLOT         0x2f
#define ABS_MT_POSITION_X   0x35
#define ABS_MT_POSITION_Y   0x36
#define ABS_MT_TRACKING_ID  0x39
#define ABS_MT_PRESSURE     0x3a
#endif
//#define DEBUG_UINPUT

typedef struct SDL_EVDEV_PrivateData
{
    int ref_count;
    int event_count;
    int num_devices;
    SDL_mutex *mutex;
    SDL_cond *cond;
} SDL_EVDEV_PrivateData;

#undef _THIS
#define _THIS SDL_EVDEV_PrivateData *_this
static _THIS = NULL;
//#define USE_TOUCH

static int SDL_EVDEV_device_removed(const char *dev_path);

#if SDL_USE_LIBUDEV
static int SDL_EVDEV_device_added(const char *dev_path, int udev_class);
static void SDL_EVDEV_udev_callback(SDL_UDEV_deviceevent udev_type, int udev_class,
    const char *dev_path);
#endif /* SDL_USE_LIBUDEV */

static Uint8 EVDEV_MouseButtons[] = {
    SDL_BUTTON_LEFT,            /*  BTN_LEFT        0x110 */
    SDL_BUTTON_RIGHT,           /*  BTN_RIGHT       0x111 */
    SDL_BUTTON_MIDDLE,          /*  BTN_MIDDLE      0x112 */
    SDL_BUTTON_X1,              /*  BTN_SIDE        0x113 */
    SDL_BUTTON_X2,              /*  BTN_EXTRA       0x114 */
    SDL_BUTTON_X2 + 1,          /*  BTN_FORWARD     0x115 */
    SDL_BUTTON_X2 + 2,          /*  BTN_BACK        0x116 */
    SDL_BUTTON_X2 + 3           /*  BTN_TASK        0x117 */
};

static int
SDL_EVDEV_SetRelativeMouseMode(SDL_bool enabled)
{
    /* Mice already send relative events through this interface */
    return 0;
}


static void
uinput_callback(uinput_event_type_t event, void* pdata, uint32_t len)
{
    int mouse_button;
    SDL_Mouse *mouse;
    uinput_event_t* events;
    float fx, fy;
    float norm_pressure;

    if (!_this) {
        return;
    }

    if (!pdata)
        return;

    events = (uinput_event_t*)pdata;
    mouse = SDL_GetMouse();
    mouse_button = 1; //use SDL_BUTTON_LEFT
#ifdef USE_TOUCH
    fx = (float)events->abs.x / (float)800.0f;
    fy = (float)events->abs.y / (float)480.0f;
    norm_pressure = 1.0f;
#endif
    switch (events->type) {
        case UINPUT_EVENT_TOUCH_DOWN: {
#ifdef USE_TOUCH
            SDL_SendTouch(1, 1, NULL, SDL_TRUE, fx, fy, norm_pressure);
#else
            SDL_SendMouseButton(mouse->focus, mouse->mouseID, SDL_PRESSED, EVDEV_MouseButtons[mouse_button]);
#endif
            break;
        }
        case UINPUT_EVENT_TOUCH_UP: {
#ifdef USE_TOUCH
            SDL_SendTouch(1, 1, NULL, SDL_FALSE, fx, fy, norm_pressure);
#else
            SDL_SendMouseButton(mouse->focus, mouse->mouseID, SDL_RELEASED, EVDEV_MouseButtons[mouse_button]);
#endif
            break;
        }
        case UINPUT_EVENT_TOUCH_MOTION: {
#ifdef USE_TOUCH
            printf("fx: %.1f, fy: %.1f\n", fx, fy);
            SDL_SendTouchMotion(1, 1, NULL, fx, fy, norm_pressure);
#else
            SDL_SendMouseMotion(mouse->focus, mouse->mouseID, SDL_FALSE, events->abs.x, events->abs.y);
#endif
            break;
        }
    }
#if defined(DEBUG_UINPUT)
    printf("uintput callback type=%d", events->type);
    if (events->type >= UINPUT_EVENT_TOUCH_DOWN) {
        printf("uintput callback touch point(%d,%d)", events->abs.x, events->abs.y);
    } else {
        printf("uintput callback key code=%d", events->key_code);
    }
#endif
    /*send event to uplayer*/
    SDL_LockMutex(_this->mutex);
    SDL_CondSignal(_this->cond);
    SDL_UnlockMutex(_this->mutex);
}

int
SDL_EVDEV_Init(void)
{
    if (_this == NULL) {
        _this = (SDL_EVDEV_PrivateData*)SDL_calloc(1, sizeof(*_this));
        if (_this == NULL) {
            return SDL_OutOfMemory();
        }
    }

    _this->mutex = SDL_CreateMutex();
    _this->cond = SDL_CreateCond();

    SDL_GetMouse()->SetRelativeMouseMode = SDL_EVDEV_SetRelativeMouseMode;

    _this->ref_count += 1;
    _this->event_count = 0;

    uinput_service_init(uinput_callback);
    return 0;
}

void
SDL_EVDEV_Quit(void)
{
    if (_this == NULL) {
        return;
    }

    _this->ref_count -= 1;

    if (_this->mutex != NULL)
        SDL_DestroyMutex(_this->mutex);
    if (_this->cond != NULL)
        SDL_DestroyCond(_this->cond);
    if (_this->ref_count < 1) {
#if SDL_USE_LIBUDEV
        SDL_UDEV_DelCallback(SDL_EVDEV_udev_callback);
        SDL_UDEV_Quit();
#endif /* SDL_USE_LIBUDEV */

        SDL_assert(_this->num_devices == 0);
        SDL_free(_this);
        _this = NULL;
    }
}

#if SDL_USE_LIBUDEV
static void SDL_EVDEV_udev_callback(SDL_UDEV_deviceevent udev_event, int udev_class,
    const char* dev_path)
{
    if (dev_path == NULL) {
        return;
    }

    switch(udev_event) {
    case SDL_UDEV_DEVICEADDED:
        if (!(udev_class & (SDL_UDEV_DEVICE_MOUSE | SDL_UDEV_DEVICE_KEYBOARD |
            SDL_UDEV_DEVICE_TOUCHSCREEN)))
            return;

        SDL_EVDEV_device_added(dev_path, udev_class);
        break;
    case SDL_UDEV_DEVICEREMOVED:
        SDL_EVDEV_device_removed(dev_path);
        break;
    default:
        break;
    }
}
#endif /* SDL_USE_LIBUDEV */

void
SDL_EVDEV_Poll(void)
{
    SDL_LockMutex(_this->mutex);
    if (!SDL_HasEvents(SDL_FIRSTEVENT, SDL_LASTEVENT))
        SDL_CondWait(_this->cond, _this->mutex);
    SDL_UnlockMutex(_this->mutex);
}


#ifdef SDL_USE_LIBUDEV
static int
SDL_EVDEV_init_touchscreen(SDL_evdevlist_item* item)
{
    int ret, i;
    unsigned long xreq, yreq;
    char name[64];
    struct input_absinfo abs_info;

    if (!item->is_touchscreen)
        return 0;

    item->touchscreen_data = SDL_calloc(1, sizeof(*item->touchscreen_data));
    if (item->touchscreen_data == NULL)
        return SDL_OutOfMemory();

    ret = ioctl(item->fd, EVIOCGNAME(sizeof(name)), name);
    if (ret < 0) {
        SDL_free(item->touchscreen_data);
        return SDL_SetError("Failed to get evdev touchscreen name");
    }

    item->touchscreen_data->name = SDL_strdup(name);
    if (item->touchscreen_data->name == NULL) {
        SDL_free(item->touchscreen_data);
        return SDL_OutOfMemory();
    }

    ret = ioctl(item->fd, EVIOCGABS(ABS_MT_SLOT), &abs_info);
    if (ret < 0) {
        SDL_free(item->touchscreen_data->name);
        SDL_free(item->touchscreen_data);
        return SDL_SetError("Failed to get evdev touchscreen limits");
    }

    if (abs_info.maximum == 0) {
        item->touchscreen_data->max_slots = 1;
        xreq = EVIOCGABS(ABS_X);
        yreq = EVIOCGABS(ABS_Y);
    } else {
        item->touchscreen_data->max_slots = abs_info.maximum + 1;
        xreq = EVIOCGABS(ABS_MT_POSITION_X);
        yreq = EVIOCGABS(ABS_MT_POSITION_Y);
    }

    ret = ioctl(item->fd, xreq, &abs_info);
    if (ret < 0) {
        SDL_free(item->touchscreen_data->name);
        SDL_free(item->touchscreen_data);
        return SDL_SetError("Failed to get evdev touchscreen limits");
    }
    item->touchscreen_data->min_x = abs_info.minimum;
    item->touchscreen_data->max_x = abs_info.maximum;
    item->touchscreen_data->range_x = abs_info.maximum - abs_info.minimum;

    ret = ioctl(item->fd, yreq, &abs_info);
    if (ret < 0) {
        SDL_free(item->touchscreen_data->name);
        SDL_free(item->touchscreen_data);
        return SDL_SetError("Failed to get evdev touchscreen limits");
    }
    item->touchscreen_data->min_y = abs_info.minimum;
    item->touchscreen_data->max_y = abs_info.maximum;
    item->touchscreen_data->range_y = abs_info.maximum - abs_info.minimum;

    ret = ioctl(item->fd, EVIOCGABS(ABS_MT_PRESSURE), &abs_info);
    if (ret < 0) {
        SDL_free(item->touchscreen_data->name);
        SDL_free(item->touchscreen_data);
        return SDL_SetError("Failed to get evdev touchscreen limits");
    }
    item->touchscreen_data->min_pressure = abs_info.minimum;
    item->touchscreen_data->max_pressure = abs_info.maximum;
    item->touchscreen_data->range_pressure = abs_info.maximum - abs_info.minimum;

    item->touchscreen_data->slots = SDL_calloc(
        item->touchscreen_data->max_slots,
        sizeof(*item->touchscreen_data->slots));
    if (item->touchscreen_data->slots == NULL) {
        SDL_free(item->touchscreen_data->name);
        SDL_free(item->touchscreen_data);
        return SDL_OutOfMemory();
    }

    for(i = 0; i < item->touchscreen_data->max_slots; i++) {
        item->touchscreen_data->slots[i].tracking_id = -1;
    }

    ret = SDL_AddTouch(item->fd, /* I guess our fd is unique enough */
        SDL_TOUCH_DEVICE_DIRECT,
        item->touchscreen_data->name);
    if (ret < 0) {
        SDL_free(item->touchscreen_data->slots);
        SDL_free(item->touchscreen_data->name);
        SDL_free(item->touchscreen_data);
        return ret;
    }

    return 0;
}
#endif /* SDL_USE_LIBUDEV */

#if SDL_USE_LIBUDEV
static int
SDL_EVDEV_device_added(const char *dev_path, int udev_class)
{
    int ret;
    SDL_evdevlist_item *item;

    /* Check to make sure it's not already in list. */
    for (item = _this->first; item != NULL; item = item->next) {
        if (SDL_strcmp(dev_path, item->path) == 0) {
            return -1;  /* already have this one */
        }
    }

    item = (SDL_evdevlist_item *) SDL_calloc(1, sizeof (SDL_evdevlist_item));
    if (item == NULL) {
        return SDL_OutOfMemory();
    }

    item->fd = open(dev_path, O_RDONLY | O_NONBLOCK);
    if (item->fd < 0) {
        SDL_free(item);
        return SDL_SetError("Unable to open %s", dev_path);
    }

    item->path = SDL_strdup(dev_path);
    if (item->path == NULL) {
        close(item->fd);
        SDL_free(item);
        return SDL_OutOfMemory();
    }

    if (udev_class & SDL_UDEV_DEVICE_TOUCHSCREEN) {
        item->is_touchscreen = 1;

        if ((ret = SDL_EVDEV_init_touchscreen(item)) < 0) {
            close(item->fd);
            SDL_free(item);
            return ret;
        }
    }

    if (_this->last == NULL) {
        _this->first = _this->last = item;
    } else {
        _this->last->next = item;
        _this->last = item;
    }

    SDL_EVDEV_sync_device(item);

    return _this->num_devices++;
}
#endif /* SDL_USE_LIBUDEV */

#endif /* SDL_INPUT_LINUXEV */

/* vi: set ts=4 sw=4 expandtab: */
