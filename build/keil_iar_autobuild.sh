#!/usr/bin/env bash

RET_CODE=0
BUILD_FAIL=3
BUILD_TARGETS="nano@b_l475e helloworld@b_l475e mqttapp@b_l475e alinkapp@b_l475e helloworld@starterkit mqttapp@starterkit"
BUILD_SYSTEM="make scons"
BUILD_TOOLS="iar armcc"
BUILD_IDE="iar keil"

exit_handler() {
	echo ret_code: ${RET_CODE}
    exit ${RET_CODE}
}

echo "BUILD START ..."

for i in ${BUILD_TARGETS}; do
    for s in ${BUILD_SYSTEM}; do
        for t in ${BUILD_TOOLS}; do            
            echo -e \\n\\naos make clean
            aos make clean
            
            echo -e \\n\\naos ${s} ${i} COMPILER=${t}
            aos ${s} ${i} COMPILER=${t}
            if [ $? -ne 0 ]; then
                echo aos ${s} ${i} COMPILER=${t} "BUILD FAILED!!"
                RET_CODE=${BUILD_FAIL}
                exit_handler
            fi            
        done
    done
done

for i in ${BUILD_TARGETS}; do
    for d in ${BUILD_IDE}; do            
        echo -e \\n\\naos scons ${i} IDE=${d}
        aos scons ${i} IDE=${d}
        if [ $? -ne 0 ]; then
            echo aos scons ${i} IDE=${d} "BUILD FAILED!!"
            RET_CODE=${BUILD_FAIL}
            exit_handler
        fi            
    done
done

echo "BUILD END ..."
exit_handler
