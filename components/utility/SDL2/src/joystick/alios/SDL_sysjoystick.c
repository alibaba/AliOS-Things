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

#if defined(SDL_JOYSTICK_ALIOS) || defined(SDL_JOYSTICK_DISABLED)

/* This is the dummy implementation of the SDL joystick API */

#include "SDL_joystick.h"
#include "../SDL_sysjoystick.h"
#include "../SDL_joystick_c.h"
#include <aos/hal/gpio.h>
#include <aos/hal/adc.h>
#include <SDL_events.h>

void game_key_irq_fun(int key_num)
{
    int i = 0;
    Uint8 button;
    SDL_Joystick *joystick = SDL_JoystickFromInstanceID(0);
    // do your things here
    printf("key %d down\n", key_num);

    if (!joystick)
        return;

    if (key_num == 0) {
        button = SDL_SCANCODE_LSHIFT;
    } else if (key_num == 1) {
        button = SDL_SCANCODE_SPACE;
    } else if (key_num == 2) {
        button = SDL_SCANCODE_RETURN;
    } else if (key_num == 3) {
        button = SDL_SCANCODE_ESCAPE;
    }

    SDL_PrivateJoystickButton(joystick, button, SDL_PRESSED);
}

static gpio_dev_t key_array[4] = {
    /*key_num, Port*/
    {33, IRQ_MODE, NULL}, /*Y*/
    {32, IRQ_MODE, NULL}, /*R*/
    {22, IRQ_MODE, NULL}, /*B*/
    {23, IRQ_MODE, NULL}, /*W*/
};

static int
AliOS_JoystickInit(void)
{
    int ret = 0, i = 0;
    int key_num = 0; // 在这里填键值
    gpio_dev_t key;

    printf("AliOS_JoystickInit\n");
    SDL_PrivateJoystickAdded(0);

    for (i = 0; i < 4; i++) {
        key_num = i;
        key.port = key_array[i].port;
        key.config = key_array[i].config;
        key.priv = key_array[i].priv;
        ret |= hal_gpio_init(&key);
        ret |= hal_gpio_enable_irq(&key, IRQ_TRIGGER_RISING_EDGE, game_key_irq_fun, key_num);
    }

    return 0;
}

static int
AliOS_JoystickGetCount(void)
{
    return 1;
}

static void
AliOS_JoystickDetect(void)
{
}

static const char *
AliOS_JoystickGetDeviceName(int device_index)
{
    return NULL;
}

static int
AliOS_JoystickGetDevicePlayerIndex(int device_index)
{
    return 0;
}

static void
AliOS_JoystickSetDevicePlayerIndex(int device_index, int player_index)
{
}

static SDL_JoystickGUID
AliOS_JoystickGetDeviceGUID(int device_index)
{
    SDL_JoystickGUID guid;
    SDL_zero(guid);
    return guid;
}

static SDL_JoystickID
AliOS_JoystickGetDeviceInstanceID(int device_index)
{
    return 0;
}

static int
AliOS_JoystickOpen(SDL_Joystick * joystick, int device_index)
{
    joystick->nballs = 1;
    joystick->nbuttons = 4;
    printf("AliOS_JoystickOpen ok\n");
    return 0;
}

static int
AliOS_JoystickRumble(SDL_Joystick * joystick, Uint16 low_frequency_rumble, Uint16 high_frequency_rumble)
{
    return SDL_Unsupported();
}

static void
AliOS_JoystickUpdate(SDL_Joystick * joystick)
{
	int adc_ret = 0, i;
    static Sint16 last_xrel, last_yrel;
    adc_dev_t roll_x;
    adc_dev_t roll_y;

    if (!joystick || !joystick->balls)
        return;

    roll_x.port = 0;
    roll_x.config.sampling_cycle = 100;
	hal_adc_init(&roll_x);
    roll_y.port = 2;
    roll_y.config.sampling_cycle = 100;
	hal_adc_init(&roll_y);

    /* Deliver ball motion updates */
    for (i = 0; i < joystick->nballs; ++i) {
        int xrel, yrel;
        adc_ret |= hal_adc_value_get(&roll_x, &xrel, 5);
        adc_ret |= hal_adc_value_get(&roll_y, &yrel, 5);
        //printf("xrel: %d, yrel: %d\n", xrel, yrel);
        if (xrel || yrel) {
            if ((last_xrel == xrel) && (last_yrel == yrel))
                return;
            SDL_PrivateJoystickBall(joystick, (Uint8) i, xrel, yrel);
            last_xrel = xrel;
            last_yrel = yrel;
        }
    }
}

static void
AliOS_JoystickClose(SDL_Joystick * joystick)
{
}

static void
AliOS_JoystickQuit(void)
{
}

static SDL_bool
AliOS_JoystickGetGamepadMapping(int device_index, SDL_GamepadMapping *out)
{
    return SDL_FALSE;
}

SDL_JoystickDriver SDL_AliOS_JoystickDriver =
{
    AliOS_JoystickInit,
    AliOS_JoystickGetCount,
    AliOS_JoystickDetect,
    AliOS_JoystickGetDeviceName,
    AliOS_JoystickGetDevicePlayerIndex,
    AliOS_JoystickSetDevicePlayerIndex,
    AliOS_JoystickGetDeviceGUID,
    AliOS_JoystickGetDeviceInstanceID,
    AliOS_JoystickOpen,
    AliOS_JoystickRumble,
    AliOS_JoystickUpdate,
    AliOS_JoystickClose,
    AliOS_JoystickQuit,
    AliOS_JoystickGetGamepadMapping
};

#endif /* SDL_JOYSTICK_AliOS || SDL_JOYSTICK_DISABLED */

/* vi: set ts=4 sw=4 expandtab: */
