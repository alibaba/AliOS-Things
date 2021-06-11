@page SDL2 SDL2

**[更正文档](https://gitee.com/alios-things/SDL2/edit/master/README.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)**

# 概述
Simple DirectMedia Layer是一个跨平台开发库，旨在通过OpenGL和Direct3D提供对音频，键盘，鼠标，操纵杆和图形硬件的低级访问。视频播放软件，仿真器和受欢迎的游戏（包括Valve屡获殊荣的目录和许多Humble Bundle游戏）都使用它。

SDL正式支持Windows，Mac OS X，Linux，iOS和Android。在源代码中可以找到对其他平台的支持。

SDL用C编写，可与C ++一起使用，并且有绑定可用于其他几种语言，包括C＃和Python。

## 版权信息
> zlib license

## 目录结构
```tree
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

## 依赖组件

* freetype
* udisplay

# 常用配置
```yaml
def_config:                              # 组件的可配置项
  __ALIOS__: 1
  LOAD_JPG: 1
  LOAD_BMP: 1
  LOAD_PNG: 1
  __native_client__: 1
  AOS_COMP_SDL2: 1
  AOS_COMP_UDISPLAY: 1
```
> 默认打开支持jpg/bmp/png图像绘制，以及使用udisplay组件进行framebuffer显示

# API说明
SDL2提供了丰富的API接口，支持audio/video/input等设备操作接口，请参考:
http://wiki.libsdl.org/CategoryAPI

# 使用示例

组件使用示例相关的代码下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

## 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。

## 步骤2 添加组件

案例下载完成后，需要在helloworld_demo组件的package.yaml中添加对组件的依赖：

```yaml

depends:
  - SDL2: master            # helloworld_demo组件中引入SDL2组件
  - ili9341: master         # helloworld_demo 中引入ili9341屏幕驱动

def_config:
    CLI_IOBOX_ENABLE: 1        # 可选：使能文件系统测试命令，例如ls, ll, mkdir, touch, echo等

```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install SDL2

```

上述命令执行成功后，组件源码则被下载到了./components/SDL2路径中。

## 步骤4 添加示例
请参考[ugraphics](https://gitee.com/alios-things/ugraphics)组件的测试示例。

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

上述步骤执行后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例
在AliOS Things中ugraphics组件对SDL2中部分图像、字体相关接口进行了封装，请参考请参考[ugraphics](https://gitee.com/alios-things/ugraphics)组件的测试示例。

# FAQ
NA
