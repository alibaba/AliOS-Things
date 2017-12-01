#!/usr/bin/env bash

PIDS=`ps aux | grep "python utest.py" | grep "client" | awk '{ print $2 }'`
if [ "${PIDS}" != "" ]; then
    kill ${PIDS}
fi
if [ -f /tmp/.testbed_client ]; then
    rm -rf /tmp/.testbed_client
fi
