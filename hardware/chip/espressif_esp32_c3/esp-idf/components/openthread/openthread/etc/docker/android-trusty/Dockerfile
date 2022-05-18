#
#  Copyright (c) 2020, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#

#
# Ubuntu 14.04 with tools required to run OpenThread Android check
#

FROM ubuntu:14.04

ENV DEBIAN_FRONTEND noninteractive
ENV ANDROID_JAVA_HOME=/usr/lib/jvm/java-6-openjdk-amd64
ENV OPT_BIN=/opt/bin
ENV PATH=$OPT_BIN:$PATH
ENV ANDROID_NDK_PATH=/opt/ndk-bundle

WORKDIR /build

RUN apt-get -y update && apt-get --no-install-recommends install -y \
        gcc-multilib \
        g++-multilib \
        git \
        make \
        python \
        unzip \
        wget

RUN wget https://dl.google.com/android/repository/android-ndk-r17c-linux-x86_64.zip \
        && unzip android-ndk-r17c-linux-x86_64.zip > /dev/null \
        && mv android-ndk-r17c $ANDROID_NDK_PATH \
        && rm android-ndk-r17c-linux-x86_64.zip

# Android build system
RUN mkdir build && cd build && git init && git pull --depth 1 https://android.googlesource.com/platform/build 2db32730e79cafcf13e1f898a7bee7f82b0449d6
RUN ln -s build/core/main.mk Makefile

RUN mkdir /opt/bin

# Workarounds for java checking
RUN printf '#!/bin/sh\n\
echo java version \\"1.6\\"'\
> $OPT_BIN/java \
    && printf '#!/bin/sh\n\
echo javac \\"1.6\\"'\
> $OPT_BIN/javac \
    && chmod a+x $OPT_BIN/java $OPT_BIN/javac \
    && mkdir -p /usr/lib/jvm/java-6-openjdk-amd64/lib/ \
    && touch /usr/lib/jvm/java-6-openjdk-amd64/lib/tools.jar

# Files for building ndk
# The default libstdc++.so does not contain full stl implementation, see https://developer.android.com/ndk/guides/cpp-support
RUN mkdir -p system/core/include/arch/linux-arm \
    && touch system/core/include/arch/linux-arm/AndroidConfig.h \
    && mkdir -p system/core/include/arch/linux-x86 \
    && touch system/core/include/arch/linux-x86/AndroidConfig.h \
    && mkdir -p bionic/libc/ \
    && cp -r "$ANDROID_NDK_PATH"/sysroot/usr/include bionic/libc/include \
    && mv bionic/libc/include/arm-linux-androideabi/asm bionic/libc/include/asm \
    && mkdir -p out/target/product/generic/obj/ \
    && cp -r "$ANDROID_NDK_PATH"/platforms/android-27/arch-arm/usr/lib out/target/product/generic/obj/ \
    && mkdir -p bionic/libstdc++ \
    && cp -r "$ANDROID_NDK_PATH"/sources/cxx-stl/gnu-libstdc++/4.9/include bionic/libstdc++ \
    && cp -r "$ANDROID_NDK_PATH"/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a/include/* bionic/libstdc++/include \
    && cp "$ANDROID_NDK_PATH"/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a/libgnustl_shared.so out/target/product/generic/obj/lib/libstdc++.so \
    && printf "TARGET_PRODUCT := generic\n\
TARGET_BUILD_VARIANT := eng\n\
TARGET_BUILD_TYPE := release\n\
TARGET_TOOLS_PREFIX := $ANDROID_NDK_PATH/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin/arm-linux-androideabi-"\
> buildspec.mk

RUN rm -rf "$ANDROID_NDK_PATH"/platforms
RUN rm -rf "$ANDROID_NDK_PATH"/prebuilt
RUN rm -rf "$ANDROID_NDK_PATH"/shader-tools
RUN rm -rf "$ANDROID_NDK_PATH"/sources
RUN rm -rf "$ANDROID_NDK_PATH"/sysroot
RUN rm -rf "$ANDROID_NDK_PATH"/simpleperf
RUN cd "$ANDROID_NDK_PATH"/toolchains && rm -rf aarch64-linux-android-4.9 llvm mips64el-linux-android-4.9 mipsel-linux-android-4.9 x86-4.9 x86_64-4.9
RUN apt-get purge -y unzip wget git && apt-get -y autoremove && apt-get -y clean && rm -rf /var/lib/apt/lists/*

CMD ["bash"]

