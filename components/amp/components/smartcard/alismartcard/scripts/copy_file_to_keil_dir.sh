#!/bin/bash

# this file is used to build all binary and copy to dest
set -e
export LANG=C

DESTDIR=""
IS_API=0
IS_AT=0
IS_API_EXPORT=0
IS_AT_EXPORT=0

declare -a api_file_list
declare -a at_file_list

api_file_list=(
 "./modules/mn_common.c"
 "./modules/mn_common.h"
 "./modules/mn_export.c"
 "./modules/version.h"
 "./modules/mn_dummy_config.c"
 "./exports/aliyun_iot_smartcard_internal.h"
 "./exports/mn_obfus.h"
 "./libalilog/log_simple.c"
 "./rtos_hal_impl/rtos_log.h"
 "./rtos_hal_impl/module_common.h"
 "./rtos_hal_impl/exports/aliyun_iot_smartcard_rtos_api_export.h"
 "./imports/aliyun_iot_smartcard_hal.h"
 "./imports/aliyun_iot_smartcard_os_hal.h"
 "./sim_ref_impl/common_sim.c"
 "./sim_ref_impl/sim.h"
 "./sim_ref_impl/std_ali_card.c"
 "./sim_ref_impl/sim.c"
 "./sim_ref_impl/sim_internal.h"
)

at_file_list=(
 "./modules/mn_common.c"
 "./modules/mn_common.h"
 "./modules/mn_export.c"
 "./modules/version.h"
 "./modules/mn_dummy_config.c"
 "./exports/aliyun_iot_smartcard_internal.h"
 "./exports/mn_obfus.h"
 "./libalilog/log_simple.c"
 "./rtos_hal_impl/rtos_log.h"
 "./rtos_hal_impl/module_common.h"
 "./rtos_hal_impl/at_hal_impl.c"
 "./rtos_hal_impl/module_at.c"
 "./rtos_hal_impl/module_at.h"
 "./rtos_hal_impl/exports/aliyun_iot_smartcard_rtos_at_export.h"
 "./imports/aliyun_iot_smartcard_hal.h"
 "./imports/aliyun_iot_smartcard_os_hal.h"
 "./sim_ref_impl/common_sim.c"
 "./sim_ref_impl/sim.h"
 "./sim_ref_impl/std_ali_card.c"
 "./sim_ref_impl/sim.c"
 "./sim_ref_impl/sim_internal.h"
)

api_export_file_list=(
 "./rtos_hal_impl/exports/aliyun_iot_smartcard_rtos_api_export.h"
 "./rtos_hal_impl/imports/aliyun_iot_smartcard_api_hal.h"
 "./rtos_hal_impl/imports/aliyun_iot_smartcard_os_hal.h"
 "./rtos_hal_impl/HAL_HelloBike_FreeRTOS.c"
 "./rtos_hal_impl/api_hal_dummy_impl.c"
)

at_export_file_list=(
 "./rtos_hal_impl/exports/aliyun_iot_smartcard_rtos_at_export.h"
 "./rtos_hal_impl/imports/aliyun_iot_smartcard_at_hal.h"
 "./rtos_hal_impl/imports/aliyun_iot_smartcard_os_hal.h"
 "./rtos_hal_impl/HAL_HelloBike_FreeRTOS.c"
 "./rtos_hal_impl/at_hal_dummy_impl.c"
)

usage()
{
   cat << END >&2
Usage:  [options]

Options:
    {--dest}
        specify destination.
    {--at}
        specify at file list.
    {--api}
        specify api file list.
    {--at-export}
        specify at export file list.
    {--api-export}
        specify api export file list.
END
    exit 1
}


copy_at_src()
{
    for i in $(find $DESTDIR -name "*.[ch]"); do
        rm $i
    done

	for var in ${at_file_list[@]};do
		echo "copy $var to $DESTDIR"
        cp $var $DESTDIR
	done

    mv $DESTDIR/rtos_log.h $DESTDIR/log.h
    mv $DESTDIR/aliyun_iot_smartcard_rtos_at_export.h $DESTDIR/aliyun_iot_smartcard_export.h
}

copy_api_src()
{
    for i in $(find $DESTDIR -name "*.[ch]"); do
        rm $i
    done

	for var in ${api_file_list[@]};do
		echo "copy $var to $DESTDIR"
        cp $var $DESTDIR
	done

    mv $DESTDIR/rtos_log.h $DESTDIR/log.h
    mv $DESTDIR/aliyun_iot_smartcard_rtos_api_export.h $DESTDIR/aliyun_iot_smartcard_export.h
}

copy_api_export_src()
{
    for i in $(find $DESTDIR -name "*.[ch]"); do
        rm $i
    done

	for var in ${api_export_file_list[@]};do
		echo "copy $var to $DESTDIR"
        cp $var $DESTDIR
	done

    mv $DESTDIR/aliyun_iot_smartcard_api_hal.h $DESTDIR/aliyun_iot_smartcard_hal.h
    mv $DESTDIR/aliyun_iot_smartcard_rtos_api_export.h $DESTDIR/aliyun_iot_smartcard_export.h
}

copy_at_export_src()
{
    for i in $(find $DESTDIR -name "*.[ch]"); do
        rm $i
    done

	for var in ${at_export_file_list[@]};do
		echo "copy $var to $DESTDIR"
        cp $var $DESTDIR
	done

    mv $DESTDIR/aliyun_iot_smartcard_at_hal.h $DESTDIR/aliyun_iot_smartcard_hal.h
    mv $DESTDIR/aliyun_iot_smartcard_rtos_at_export.h $DESTDIR/aliyun_iot_smartcard_export.h
}

while [ $# != 0 ]; do
    case $1 in
        -h|--help)
            usage
            break;;
        --dest)
            DESTDIR=$2
            shift;;
        --api)
            IS_API=1;;
        --at)
            IS_AT=1;;
        --api-export)
            IS_API_EXPORT=1;;
        --at-export)
            IS_AT_EXPORT=1;;
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

mkdir -p $DESTDIR

if [[ "$IS_API" -eq 1 ]]; then
    copy_api_src
elif [[ "$IS_API_EXPORT" -eq 1 ]]; then
    copy_api_export_src
elif [[ "$IS_AT_EXPORT" -eq 1 ]]; then
    copy_at_export_src
else
    copy_at_src
fi
