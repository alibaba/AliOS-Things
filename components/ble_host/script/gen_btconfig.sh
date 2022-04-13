#!/bin/sh

CONFIG_FILE=$1
C_HEAD_FILE=$2

if [ ! -f "$C_HEAD_FILE" ]; then
    echo "[INFO] Configuration created"
	python3 script/Kconfiglib-10.21.0/genconfig.py --header-path $C_HEAD_FILE --config-out $CONFIG_FILE --defconfig-file ../../../defconfig bt_host/Kconfig
else
    python3 script/Kconfiglib-10.21.0/genconfig.py --header-path $C_HEAD_FILE.tmp --config-out $CONFIG_FILE --defconfig-file ../../../defconfig bt_host/Kconfig
    grep -Fvf $C_HEAD_FILE.tmp $C_HEAD_FILE
    ret1=$?
    grep -Fvf $C_HEAD_FILE $C_HEAD_FILE.tmp
    ret2=$?

    if [ "$ret1$ret2" = "11" ]; then
        echo "[INFO] Configuration unchanged"
        rm $C_HEAD_FILE.tmp
    else
        echo "[INFO] Configuration changed"
        mv $C_HEAD_FILE.tmp $C_HEAD_FILE
    fi
fi
