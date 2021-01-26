NAME := SDL2

$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := SDL2

#SDL core
$(NAME)_SOURCES += ./src/SDL.c \
					./src/SDL_dataqueue.c \
					./src/SDL_error.c \
					./src/SDL_hints.c \
					./src/SDL_log.c \
					./src/SDL_assert.c

#SDL atomic
$(NAME)_SOURCES += ./src/atomic/SDL_atomic.c \
					./src/atomic/SDL_spinlock.c

#SDL render
$(NAME)_SOURCES += ./src/render/SDL_render.c \
					./src/render/SDL_yuv_sw.c \
					./src/render/software/SDL_blendfillrect.c \
					./src/render/software/SDL_blendline.c \
					./src/render/software/SDL_blendpoint.c \
					./src/render/software/SDL_drawline.c \
					./src/render/software/SDL_drawpoint.c \
					./src/render/software/SDL_render_sw.c \
					./src/render/software/SDL_rotate.c


#SDL video
$(NAME)_SOURCES += ./src/video/SDL_blit_copy.c \
					./src/video/SDL_clipboard.c \
					./src/video/SDL_rect.c \
					./src/video/SDL_surface.c \
					./src/video/SDL_blit.c \
					./src/video/SDL_blit_0.c \
					./src/video/SDL_blit_1.c \
					./src/video/SDL_blit_A.c \
					./src/video/SDL_blit_N.c \
					./src/video/SDL_blit_auto.c \
					./src/video/SDL_blit_slow.c \
					./src/video/SDL_egl.c \
					./src/video/SDL_RLEaccel.c \
					./src/video/SDL_video.c \
					./src/video/SDL_fillrect.c \
					./src/video/SDL_shape.c \
					./src/video/SDL_vulkan_utils.c \
					./src/video/SDL_pixels.c \
					./src/video/SDL_yuv.c \
					./src/video/yuv2rgb/yuv_rgb.c \
					./src/video/SDL_bmp.c \
					./src/video/SDL_stretch.c

#SDL dummy video
#$(NAME)_SOURCES += ./src/video/dummy/SDL_nullvideo.c \
#					./src/video/dummy/SDL_nullframebuffer.c \
#					./src/video/dummy/SDL_nullevents.c

#SDL video for alios
$(NAME)_SOURCES += ./src/video/alios/SDL_AliOS_framebuffer.c \
					./src/video/alios/SDL_AliOS_video.c \
					./src/video/alios/SDL_AliOS_events.c

#SDL linux evdev
$(NAME)_SOURCES += ./src/core/alios/SDL_evdev.c

#SDL mouse for alios
$(NAME)_SOURCES += ./src/video/alios/SDL_AliOS_mouse.c

#SDL joystick for alios
$(NAME)_SOURCES += ./src/joystick/SDL_joystick.c \
					./src/joystick/SDL_gamecontroller.c

$(NAME)_SOURCES += ./src/joystick/alios/SDL_sysjoystick.c

#SDL audio
$(NAME)_SOURCES +=  ./src/audio/SDL_audio.c \
					./src/audio/SDL_mixer.c \
					./src/audio/SDL_audiocvt.c \
					./src/audio/SDL_audiodev.c \
					./src/audio/SDL_audiotypecvt.c \
					./src/audio/SDL_wave.c \
					./src/audio/alios/SDL_alios_audio.c

#SDL thread
$(NAME)_SOURCES +=  ./src/thread/SDL_thread.c \
					./src/thread/pthread/SDL_sysmutex.c \
					./src/thread/pthread/SDL_syscond.c \
					./src/thread/pthread/SDL_syssem.c \
					./src/thread/pthread/SDL_systls.c \
					./src/thread/pthread/SDL_systhread.c

#SDL timer
$(NAME)_SOURCES +=  ./src/timer/SDL_timer.c \
					./src/timer/unix/SDL_systimer.c

#SDL events
$(NAME)_SOURCES +=  ./src/events/SDL_events.c \
					./src/events/SDL_mouse.c \
					./src/events/SDL_keyboard.c \
					./src/events/SDL_gesture.c \
					./src/events/SDL_touch.c \
					./src/events/SDL_windowevents.c \
					./src/events/SDL_quit.c

#SDL stdlib
$(NAME)_SOURCES += ./src/stdlib/SDL_getenv.c \
					./src/stdlib/SDL_string.c \
					./src/stdlib/SDL_qsort.c \
					./src/stdlib/SDL_stdlib.c

#SDL file
$(NAME)_SOURCES += ./src/file/SDL_rwops.c

EANBLE_SDL_TEST ?= 0
ifeq ($(EANBLE_SDL_TEST), 1)
#SDL stdlib
$(NAME)_SOURCES += ./src/stdlib/SDL_malloc.c

#SDL test
$(NAME)_SOURCES += ./src/test/SDL_test_common.c
$(NAME)_SOURCES += ./src/test/SDL_test_memory.c
$(NAME)_SOURCES += ./src/test/SDL_test_crc32.c
$(NAME)_SOURCES += ./test/testcustomcursor.c
endif

#SDL cpuinfo
$(NAME)_SOURCES += ./src/cpuinfo/SDL_cpuinfo.c

#SDL ttf
$(NAME)_SOURCES += ./src/ttf/SDL_ttf.c

#SDL gfx
#$(NAME)_SOURCES += ./src/gfx/SDL_gfxBlitFunc.c \
					./src/gfx/SDL_rotozoom.c \
					./src/gfx/SDL_gfxPrimitives.c \
					./src/gfx/SDL_framerate.c \
					./src/gfx/SDL_imageFilter.c

$(NAME)_INCLUDES += ./include/ \
					./src/ \
					./src/video/ \
					./src/video/yuv2rgb \
					./src/video/alios \
					./src/thread

$(NAME)_DEFINES += __clang_analyzer__
$(NAME)_DEFINES += __native_client__
$(NAME)_DEFINES += __ALIOS__

ifeq ($(EANBLE_SDL_TEST), 1)
GLOBAL_DEFINES += CONFIG_ENABLE_CURSOR
endif
GLOBAL_INCLUDES += ./include/