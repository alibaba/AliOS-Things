#!/usr/bin/env bash

cd ${HOME}/testbed
PIDS=`ps aux | grep "python utest.py" | grep "server" | awk '{ print $2 }'`
if [ "${PIDS}" != "" ]; then
    kill ${PIDS}
fi
if [ -f /tmp/.testbed_server_34567 ]; then
    rm -rf /tmp/.testbed_server_34567
fi
if [ -f ~/.bashrc ]; then
    . ~/.bashrc
fi
screen -dmL python utest.py server
