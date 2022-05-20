# Ubuntu image with tools required to build OpenThread
FROM ubuntu:18.04

ENV DEBIAN_FRONTEND noninteractive
ENV LANG en_US.UTF-8

RUN set -x \
    && apt-get update -y \
    && apt-get install -y locales \
    && localedef -i en_US -c -f UTF-8 -A /usr/share/locale/locale.alias en_US.UTF-8 \
    && apt-get --no-install-recommends install -fy \
        git \
        ninja-build \
        python3 \
        python3-pip \
        python3-setuptools \
        software-properties-common \
        sudo \
    && python3 -m pip install -U cmake \
    && python3 -m pip install wheel

# setup openthread
WORKDIR /
COPY . openthread
RUN set -x \
    && cd openthread \
    && ./script/bootstrap \
    && mkdir build \
    && cd build \
    && cmake -GNinja -DOT_PLATFORM=simulation .. \
    && ninja
