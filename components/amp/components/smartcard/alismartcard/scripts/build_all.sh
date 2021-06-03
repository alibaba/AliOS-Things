#!/bin/bash

# this file is used to build all binary and copy to dest
set -e
export LANG=C

DESTDIR=""

usage()
{
   cat << END >&2
Usage:  [options]

Options:
    {--dest}
        specify destination.
END
    exit 1
}

copy_src()
{
    mkdir -p $DESTDIR/doc
    mkdir -p $DESTDIR/arch/Cortex-M/at_lib
    mkdir -p $DESTDIR/arch/Cortex-M/api_lib

    cp README.md $DESTDIR
    cp VERSION $DESTDIR
    cp docs/* $DESTDIR/doc
    cp rtos_hal_impl/exports/aliyun_iot_smartcard_rtos_at_export.h $DESTDIR/arch/Cortex-M/at_lib/aliyun_iot_smartcard_export.h
    cp rtos_hal_impl/exports/aliyun_iot_smartcard_rtos_api_export.h $DESTDIR/arch/Cortex-M/api_lib/aliyun_iot_smartcard_export.h
    cp rtos_hal_impl/imports/aliyun_iot_smartcard_os_hal.h $DESTDIR/arch/Cortex-M/api_lib
    cp rtos_hal_impl/imports/aliyun_iot_smartcard_os_hal.h $DESTDIR/arch/Cortex-M/at_lib
    cp rtos_hal_impl/imports/aliyun_iot_smartcard_api_hal.h $DESTDIR/arch/Cortex-M/api_lib/aliyun_iot_smartcard_hal.h
    cp rtos_hal_impl/imports/aliyun_iot_smartcard_at_hal.h $DESTDIR/arch/Cortex-M/at_lib/aliyun_iot_smartcard_hal.h
}


while [ $# != 0 ]; do
    case $1 in
        -h|--help)
            usage
            break;;
        --dest)
            DESTDIR=$2
            shift;;
        --)
            usage
            exit 1;;
    esac
    shift
done

if [[ -z "$DESTDIR" ]]; then
    usage
    exit 1
fi

copy_src
