#!/bin/sh

#TARGET=uapp_test@imx6sl_svc
TARGET1=uapp1@imx6sl_svc
TARGET2=uapp2@imx6sl_svc

build=0
run=0
debug=0

while getopts "brd" opt; do
    case $opt in
        b)
            build=1;;
        r)
            run=1;;
        d)
            debug=1;;
    esac
done

if [ $build == 1 ]; then
#aos make ${TARGET} MBINS=kernel
#aos make ${TARGET} MBINS=app MBINS_APP=app1
aos make ${TARGET1} MBINS=kernel
aos make ${TARGET1} MBINS=app MBINS_APP=app1
aos make ${TARGET2} MBINS=app MBINS_APP=app2


rm .gdbinit
touch .gdbinit
cat > .gdbinit << EOF
tar ext:2331

#file out/${TARGET}kernel/binary/${TARGET}.kernel.elf
#load out/${TARGET}app/binary/${TARGET}.app.elf
#load out/${TARGET}kernel/binary/${TARGET}.kernel.elf

file out/${TARGET1}kernel/binary/${TARGET1}.kernel.elf
load out/${TARGET1}app/binary/${TARGET1}.app.elf
load out/${TARGET2}app/binary/${TARGET2}.app.elf
load out/${TARGET1}kernel/binary/${TARGET1}.kernel.elf
b vector_reset

EOF

fi

if [ $debug == 1 ]; then
#    echo "start gdb server"
#    JLINKGDBServer -select USB -device MCIMX6Q5 -endian little -if JTAG -speed auto -noir -LocalhostOnly > /dev/null 2>&1&

    echo "run gdb"
    arm-none-eabi-gdb

#    echo "kill gdb server"
#    ps -ef | grep JLINKGDBServer | grep -v grep | awk '{print $2}' | xargs kill -9

fi

