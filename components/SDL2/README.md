@page SDL2 SDL2

# 概述
Simple DirectMedia Layer是一个跨平台开发库，旨在通过OpenGL和Direct3D提供对音频，键盘，鼠标，操纵杆和图形硬件的低级访问。视频播放软件，仿真器和受欢迎的游戏（包括Valve屡获殊荣的目录和许多Humble Bundle游戏）都使用它。

SDL正式支持Windows，Mac OS X，Linux，iOS和Android。在源代码中可以找到对其他平台的支持。

SDL用C编写，可与C ++一起使用，并且有绑定可用于其他几种语言，包括C＃和Python。

# 版权信息
> zlib

# 目录结构
```sh
.
├── acinclude
│   ├── ac_check_define.m4
│   ├── alsa.m4
│   ├── ax_check_compiler_flags.m4
│   ├── ax_gcc_archflag.m4
│   ├── ax_gcc_x86_cpuid.m4
│   ├── esd.m4
│   ├── libtool.m4
│   ├── lt~obsolete.m4
│   ├── ltoptions.m4
│   ├── ltsugar.m4
│   ├── ltversion.m4
│   └── pkg_config.m4
├── autogen.sh
├── BUGS.txt
├── build-scripts
│   ├── androidbuildlibs.sh
│   ├── androidbuild.sh
│   ├── checker-buildbot.sh
│   ├── config.guess
│   ├── config.sub
│   ├── config.sub.patch
│   ├── emscripten-buildbot.sh
│   ├── gcc-fat.sh
│   ├── g++-fat.sh
│   ├── install-sh
│   ├── iosbuild.sh
│   ├── ltmain.sh
│   ├── mkinstalldirs
│   ├── nacl-buildbot.sh
│   ├── naclbuild.sh
│   ├── os2-buildbot.sh
│   ├── raspberrypi-buildbot.sh
│   ├── showrev.sh
│   ├── strip_fPIC.sh
│   ├── update-copyright.sh
│   ├── updaterev.sh
│   ├── windows-buildbot-zipper.bat
│   ├── winrtbuild.bat
│   └── winrtbuild.ps1
├── CMakeLists.txt
├── Config.in
├── config.status
├── configure
├── COPYING.txt
├── CREDITS.txt
├── docs
│   ├── doxyfile
│   ├── README-android.md
│   ├── README-cmake.md
│   ├── README-directfb.md
│   ├── README-dynapi.md
│   ├── README-emscripten.md
│   ├── README-gesture.md
│   ├── README-hg.md
│   ├── README-ios.md
│   ├── README-linux.md
│   ├── README-macosx.md
│   ├── README.md
│   ├── README-nacl.md
│   ├── README-pandora.md
│   ├── README-platforms.md
│   ├── README-porting.md
│   ├── README-psp.md
│   ├── README-raspberrypi.md
│   ├── README-touch.md
│   ├── README-wince.md
│   ├── README-windows.md
│   └── README-winrt.md
├── include
│   ├── begin_code.h
│   ├── close_code.h
│   ├── SDL_assert.h
│   ├── SDL_atomic.h
│   ├── SDL_audio.h
│   ├── SDL_bits.h
│   ├── SDL_blendmode.h
│   ├── SDL_clipboard.h
│   ├── SDL_config_android.h
│   ├── SDL_config.h
│   ├── SDL_config.h.cmake
│   ├── SDL_config.h.in
│   ├── SDL_config_iphoneos.h
│   ├── SDL_config_macosx.h
│   ├── SDL_config_minimal.h
│   ├── SDL_config_os2.h
│   ├── SDL_config_pandora.h
│   ├── SDL_config_psp.h
│   ├── SDL_config_windows.h
│   ├── SDL_config_winrt.h
│   ├── SDL_config_wiz.h
│   ├── SDL_copying.h
│   ├── SDL_cpuinfo.h
│   ├── SDL_egl.h
│   ├── SDL_endian.h
│   ├── SDL_error.h
│   ├── SDL_events.h
│   ├── SDL_filesystem.h
│   ├── SDL_gamecontroller.h
│   ├── SDL_gesture.h
│   ├── SDL.h
│   ├── SDL_haptic.h
│   ├── SDL_hints.h
│   ├── SDL_joystick.h
│   ├── SDL_keyboard.h
│   ├── SDL_keycode.h
│   ├── SDL_loadso.h
│   ├── SDL_locale.h
│   ├── SDL_log.h
│   ├── SDL_main.h
│   ├── SDL_messagebox.h
│   ├── SDL_metal.h
│   ├── SDL_mouse.h
│   ├── SDL_mutex.h
│   ├── SDL_name.h
│   ├── SDL_opengles2_gl2ext.h
│   ├── SDL_opengles2_gl2.h
│   ├── SDL_opengles2_gl2platform.h
│   ├── SDL_opengles2.h
│   ├── SDL_opengles2_khrplatform.h
│   ├── SDL_opengles.h
│   ├── SDL_opengl_glext.h
│   ├── SDL_opengl.h
│   ├── SDL_pixels.h
│   ├── SDL_platform.h
│   ├── SDL_power.h
│   ├── SDL_quit.h
│   ├── SDL_rect.h
│   ├── SDL_render.h
│   ├── SDL_revision.h
│   ├── SDL_rwops.h
│   ├── SDL_scancode.h
│   ├── SDL_sensor.h
│   ├── SDL_shape.h
│   ├── SDL_stdinc.h
│   ├── SDL_surface.h
│   ├── SDL_system.h
│   ├── SDL_syswm.h
│   ├── SDL_test_assert.h
│   ├── SDL_test_common.h
│   ├── SDL_test_compare.h
│   ├── SDL_test_crc32.h
│   ├── SDL_test_font.h
│   ├── SDL_test_fuzzer.h
│   ├── SDL_test.h
│   ├── SDL_test_harness.h
│   ├── SDL_test_images.h
│   ├── SDL_test_log.h
│   ├── SDL_test_md5.h
│   ├── SDL_test_memory.h
│   ├── SDL_test_random.h
│   ├── SDL_thread.h
│   ├── SDL_timer.h
│   ├── SDL_touch.h
│   ├── SDL_ttf.h
│   ├── SDL_types.h
│   ├── SDL_version.h
│   ├── SDL_video.h
│   └── SDL_vulkan.h
├── src
│   ├── atomic
│   ├── audio
│   ├── core
│   ├── cpuinfo
│   ├── dynapi
│   ├── events
│   ├── file
│   ├── filesystem
│   ├── gfx
│   ├── haptic
│   ├── hidapi
│   ├── image
│   ├── joystick
│   ├── libm
│   ├── loadso
│   ├── locale
│   ├── main
│   ├── power
│   ├── render
│   ├── SDL_assert.c
│   ├── SDL_assert_c.h
│   ├── SDL.c
│   ├── SDL_dataqueue.c
│   ├── SDL_dataqueue.h
│   ├── SDL_error.c
│   ├── SDL_error_c.h
│   ├── SDL_hints.c
│   ├── SDL_hints_c.h
│   ├── SDL_internal.h
│   ├── SDL_log.c
│   ├── sensor
│   ├── stdlib
│   ├── test
│   ├── thread
│   ├── timer
│   ├── ttf
│   └── video
├── test
```
>具体可参考http://wiki.libsdl.org/ 了解SDL的具体内容

# 依赖组件
* udisplay

# 常用配置
```sh
def_config:                              # 组件的可配置项
  __ALIOS__: 1
  LOAD_JPG: 1
  LOAD_BMP: 1
  LOAD_PNG: 1
  __native_client__: 1
  AOS_COMP_SDL2: 1
  AOS_COMP_UDISPLAY: 1

> 默认打开支持jpg/bmp/png图像绘制，以及使用udisplay组件进行framebuffer显示

# API说明
SDL2提供了丰富的API接口，支持audio/video/input等设备操作接口，请参考:
http://wiki.libsdl.org/CategoryAPI

# 使用示例

## 添加示例代码
> ugraphics组件的package.yaml中添加example
```sh
source_file:
  - "example/ugraphics_example.c" # add ugraphics_example.c
```

## 添加ugraphics组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - SDL2: dev_aos            # helloworld_demo组件中引入SDL2组件
```

## 编译
```sh
cd solutions/helloworld_demo && aos make
```

## 烧录固件
> 参考具体板子的快速开始文档。

## ugraphics示例测试
在AliOS Things中ugraphics组件对其中部分图像、字体相关接口进行了封装，请参考ugraphics组件的ugraphics_example.c中的示例。

# FAQ
NA


