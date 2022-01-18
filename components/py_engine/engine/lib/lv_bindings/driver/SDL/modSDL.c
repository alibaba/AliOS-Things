#include "../include/common.h"
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include "SDL_monitor.h"
#include "SDL_mouse.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

/* Defines the LittlevGL tick rate in milliseconds. */
/* Increasing this value might help with CPU usage at the cost of lower
 * responsiveness. */
#define LV_TICK_RATE 20
    
/* Default screen dimensions */
#define LV_HOR_RES_MAX          (480)
#define LV_VER_RES_MAX          (320)

//////////////////////////////////////////////////////////////////////////////

STATIC pthread_t mp_thread;

STATIC mp_obj_t mp_refresh_SDL()
{
    if (monitor_active()) monitor_sdl_refr_core();
    return mp_const_none;
}

STATIC mp_obj_t mp_lv_task_handler(mp_obj_t arg)
{  
    mp_refresh_SDL();
    lv_task_handler();
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_lv_task_handler_obj, mp_lv_task_handler);

#ifndef __EMSCRIPTEN__
STATIC int tick_thread(void * data)
{
    (void)data;

    Uint64 pfreq = SDL_GetPerformanceFrequency();
    Uint64 tick_ms = (pfreq / 1000) * LV_TICK_RATE;
    Uint64 delta, acc = 0;

    while(monitor_active()) {
        delta = SDL_GetPerformanceCounter();
        SDL_Delay(LV_TICK_RATE);   /*Sleep for LV_TICK_RATE millisecond*/
        delta = SDL_GetPerformanceCounter() - delta;
        acc += delta - tick_ms;
        lv_tick_inc(LV_TICK_RATE); /*Tell LittelvGL that LV_TICK_RATE milliseconds were elapsed*/
        if (acc >= tick_ms) {
            lv_tick_inc(LV_TICK_RATE);
            acc -= tick_ms;
        }
        mp_sched_schedule((mp_obj_t)&mp_lv_task_handler_obj, mp_const_none);
        pthread_kill(mp_thread, SIGUSR1); // interrupt REPL blocking input. See handle_sigusr1
    }

    return 0;
}
#else
STATIC void mp_lv_main_loop(void)
{
        mp_sched_schedule((mp_obj_t)&mp_lv_task_handler_obj, mp_const_none);
        lv_tick_inc(LV_TICK_RATE);
}
#endif

static void handle_sigusr1(int signo)
{
    // Let the signal pass. blocking function would return E_INTR.
    // This would cause a call to "mp_handle_pending" even when 
    // waiting for user input.
    // See https://github.com/micropython/micropython/pull/5723
}

STATIC mp_obj_t mp_init_SDL(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    enum { ARG_w, ARG_h, ARG_auto_refresh };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_w, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = LV_HOR_RES_MAX} },
        { MP_QSTR_h, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = LV_VER_RES_MAX} },
        { MP_QSTR_auto_refresh, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    monitor_init(args[ARG_w].u_int, args[ARG_h].u_int);
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mp_lv_main_loop, 1000 / LV_TICK_RATE, 0);
    /* Required for HTML input elements to work */
    SDL_EventState(SDL_TEXTINPUT, SDL_DISABLE);
    SDL_EventState(SDL_KEYDOWN, SDL_DISABLE);
    SDL_EventState(SDL_KEYUP, SDL_DISABLE);
#else
    if (args[ARG_auto_refresh].u_bool) {
        SDL_CreateThread(tick_thread, "tick", NULL);
    }
#endif

    if (args[ARG_auto_refresh].u_bool) {
        mp_thread = pthread_self();
        struct sigaction sa;
        sa.sa_handler = handle_sigusr1;
        sa.sa_flags = 0;
        sigemptyset(&sa.sa_mask);
        if (sigaction(SIGUSR1, &sa, NULL) == -1) {
            perror("sigaction");
            exit(1);
        }
    }

    return mp_const_none;
}

STATIC mp_obj_t mp_deinit_SDL()
{
    monitor_deinit();
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mp_init_SDL_obj, 0, mp_init_SDL);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_deinit_SDL_obj, mp_deinit_SDL);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_refresh_SDL_obj, mp_refresh_SDL);

DEFINE_PTR_OBJ(monitor_flush);
DEFINE_PTR_OBJ(mouse_read);

STATIC const mp_rom_map_elem_t SDL_globals_table[] = {
        { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_SDL) },
        { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&mp_init_SDL_obj) },
        { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&mp_deinit_SDL_obj) },
        { MP_ROM_QSTR(MP_QSTR_refresh), MP_ROM_PTR(&mp_refresh_SDL_obj) },
        { MP_ROM_QSTR(MP_QSTR_monitor_flush), MP_ROM_PTR(&PTR_OBJ(monitor_flush))},
        { MP_ROM_QSTR(MP_QSTR_mouse_read), MP_ROM_PTR(&PTR_OBJ(mouse_read))},
};
         

STATIC MP_DEFINE_CONST_DICT (
    mp_module_SDL_globals,
    SDL_globals_table
);

const mp_obj_module_t mp_module_SDL = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_SDL_globals
};

