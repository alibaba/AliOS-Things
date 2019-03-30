#!/usr/bin/env bash

ORIG_PATH=${PWD}
RUN_TIME=$(( 60*60 ))
MAIN_LOG=main.txt
TARGET_ARGS=
RET_CODE=0

#return code definations
AOS_RUN_SUCCESS=0
TARGET_PROGRAM_NONEXIST=4
RUN_PROGRAM_CRASHED=5
RUN_PROGRAM_TIMEOUT=6
PARSE_RESULT_FAIL=8

exit_handler() {
    cd ${ORIG_PATH}

    PID_OF_TARGET=`pidof ${TARGET_BASENAME}`
    if [ "${PID_OF_TARGET}" != "" ]; then
        kill -9 ${PID_OF_TARGET}
    fi

    if [ -f ${MAIN_LOG} ]; then
        rm -f ${MAIN_LOG}
    fi

    exit ${RET_CODE}
}

#USAGE: bash aos_ut.sh target [args]
c=0
for i in $@; do
    c=$((c+1));
    if [ $c -eq 1 ]; then
        TARGET_PROGRAM=$i
    elif [ $c -eq 2 ]; then
        TARGET_ARGS=$i
    else
        TARGET_ARGS="${TARGET_ARGS} ${i}"
    fi
done

TARGET_BASENAME=$(basename ${TARGET_PROGRAM})
if [ "${TARGET_PROGRAM}" = "${TARGET_BASENAME}" ]; then
    TARGET_PROGRAM="./${TARGET_PROGRAM}"
fi
PID_OF_TARGET=`pidof ${TARGET_BASENAME}`
if [ "${PID_OF_TARGET}" != "" ]; then
    echo "KILLING EXISTING TARGET PROGRAM ..."
    kill -9 ${PID_OF_TARGET}
fi

ulimit -c unlimited

echo "Start at $(date)" | tee -a ${MAIN_LOG}

if [ ! -f dev_key ]; then
    echo "Warning: security test dev_key none exist"
fi

if [ ! -f mesh_topology.txt ]; then
    echo "Warning: mesh topology config file none exist"
fi

if [ ! -f ${TARGET_PROGRAM} ]; then
    RET_CODE=${TARGET_PROGRAM_NONEXIST}
    exit_handler
fi

chmod +x ${TARGET_PROGRAM}
${TARGET_PROGRAM} ${TARGET_ARGS} | tee -a ${MAIN_LOG} &
TARGET_PID=$!

i=1
TARGET_RUNNING=`ps | grep ${TARGET_PID}`
while expr $i \<= ${RUN_TIME} > /dev/null && [ "${TARGET_RUNNING}" != "" ]; do
    sleep 1;
    i=$(( i+1 ));
    TARGET_RUNNING=`ps | grep ${TARGET_PID}`;
done

if expr $i \> ${RUN_TIME}; then
    echo "Program did NOT exit in ${RUN_TIME} seconds"
    RET_CODE=${RUN_PROGRAM_TIMEOUT}
    exit_handler
fi

echo "End at $(date)" | tee -a ${MAIN_LOG}

wait ${TARGET_PID}
TARGET_RET=$?

if [ ${TARGET_RET} -ne 0 ]; then
    echo "Program crashed in ${i} seconds, ret=${TARGET_RET}"
    RET_CODE=${RUN_PROGRAM_CRASHED}
    exit_handler
else
    echo "Program exited in ${i} seconds"
fi

TOTAL_NUM=`tail -300 ${MAIN_LOG} | grep -a 'completed with' -A 3 | sed '$!d' | awk '{ print $1 }'`
FAIL_NUM=`tail -300 ${MAIN_LOG} | grep -a 'completed with' -A 3 | sed '$!d' | awk '{ print $4 }'`
if [ ${TOTAL_NUM} = "" ] || [ ${FAIL_NUM} = "" ]; then
    echo "Parse result data failed"
    RET_CODE=${PARSE_RESULT_FAIL}
    exit_handler
fi

PASS_COUNT=`echo "$TOTAL_NUM - $FAIL_NUM" | bc`
FAIL_COUNT="${FAIL_NUM}"
echo "PASS_COUNT = ${PASS_COUNT}"
echo "FAIL_COUNT = ${FAIL_COUNT}"

RET_CODE=${AOS_RUN_SUCCESS}
exit_handler
