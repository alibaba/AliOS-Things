# Ubuntu image with tools required to build OpenThread
FROM ubuntu:18.04

ENV DEBIAN_FRONTEND noninteractive
ARG OT_GIT_REF=main

# Install dependencies:
RUN apt-get update -qq

# Install packages needed for build and runtime:
RUN apt-get --no-install-recommends install -y git sudo software-properties-common \
    ca-certificates \
    && update-ca-certificates

# Install OpenThread
RUN mkdir -p ~/src/openthread && \
    cd ~/src/openthread && \
    git init . && \
    git fetch https://github.com/openthread/openthread.git ${OT_GIT_REF} && \
    git checkout FETCH_HEAD && \
    ./script/bootstrap && \
    ./bootstrap && \
    make -f examples/Makefile-simulation

# Install OpenThread Daemon and ot-ctl
RUN cd ~/src/openthread && make -f src/posix/Makefile-posix DAEMON=1
