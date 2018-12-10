#! /bin/bash

#echo "COMP_LIB = ${COMP_LIB}"
#echo "STRIP = ${STRIP}"

echo ""
printf "    | %-5s | %-35s | %14s | %26s |\n" "RATE" "OBJ NAME" "BYTES/TOTAL" "MODULE NAME"

for iter in ${ALL_SUB_DIRS}; do
    [ ! -d ${LIBOBJ_TMPDIR}/${iter} ] && continue

    cd ${LIBOBJ_TMPDIR}/${iter}

    ITER_OBJS=$(find . -name "*.o")
    [ "${ITER_OBJS}" != "" ] || continue
    ${STRIP} ${ITER_OBJS} 2>/dev/null

    ITER_SIZE=$(size ${ITER_OBJS} | ${SED} '1d' | awk '{ sum += $1 } END { print sum }')

    [ "${ITER_SIZE}" != "0" ] && \
    printf "    |-%-.5s-|-%-.35s-|-%.14s-|-%.26s-|\n" \
        "-----------------------------------------" \
        "-----------------------------------------" \
        "-----------------------------------------" \
        "-----------------------------------------"


    for j in ${ITER_OBJS}; do
        size $j | ${SED} '1d' \
            | awk -v sum="${ITER_SIZE}" \
                  -v mod="${iter}" \
                  -v obj="$(basename ${j})" \
                  '{ if ($1 != 0) printf("%d %.4s%% %s %-d/%d %s\n", $1, ($1/sum)*100, obj, $1, sum, mod); }'
    done \
         | sort -nr \
         | cut -d' ' -f2- \
         | awk '{ printf("    | %-5s | %-35s | %14s | %26s |\n", $1, $2, $3, $4); }'

    if [ "${BUILD_LITE_SDK_MQTT}" ];then
        DIST_ROOT_DIR=${DIST_DIR}/release/sdk-lite
        DIST_SRC_DIR=${DIST_DIR}/release/sdk-lite/src
        DIST_INC_DIR=${DIST_DIR}/release/sdk-lite/include
        for j in ${ITER_OBJS}; do
            OBJ_SIZE=$(size $j | ${SED} '1d' | awk '{print $1}')
            SRC_PATH=$(echo $j | ${SED} 's?^./??' | ${SED} 's?o$?c?')
            if [ "$OBJ_SIZE" -ne 0 ];then
                mkdir -p ${DIST_SRC_DIR}
                mkdir -p ${DIST_INC_DIR}
                cp -f ${TOP_DIR}/${iter}/${SRC_PATH} ${DIST_SRC_DIR}
            fi
        done
        cp -f ${TOP_DIR}/src/protocol/mqtt/MQTTPacket/*.c ${DIST_SRC_DIR}

        cp -f ${TOP_DIR}/include/iot_export.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/include/iot_import.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/include/exports/iot_export_compat.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/include/exports/iot_export_errno.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/include/exports/iot_export_mqtt.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/include/imports/iot_import_config.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/include/imports/iot_import_product.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/include/imports/iot_import_tcp.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/include/imports/iot_import_tls.h ${DIST_INC_DIR}

        cp -f ${TOP_DIR}/src/infra/system/iotx_system.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/infra/utils/misc/utils_sysinfo.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/infra/system/report.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/infra/utils/iotx_utils.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/infra/utils/iotx_utils_config.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/infra/utils/misc/utils_net.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/infra/log/iotx_log.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/infra/log/iotx_log_config.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/infra/utils/digest/utils_hmac.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/infra/utils/misc/utils_httpc.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/infra/utils/misc/lite-cjson.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/infra/utils/misc/lite-list.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/infra/utils/misc/string_utils.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/infra/utils/misc/json_parser.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/infra/utils/digest/utils_md5.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/infra/utils/digest/utils_sha256.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/infra/utils/misc/utils_timer.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/infra/utils/misc/compat_aos_list.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/infra/utils/iotx_utils_internal.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/sdk-impl/sdk-impl_internal.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/infra/utils/digest/utils_sha1.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/infra/utils/digest/utils_base64.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/infra/system/iotx_system_internal.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/infra/log/iotx_log_internal.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/infra/utils/misc/mem_stats.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/ref-impl/hal/iotx_hal_internal.h ${DIST_INC_DIR}

        cp -f ${TOP_DIR}/src/protocol/mqtt/iotx_mqtt.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/protocol/mqtt/iotx_mqtt_config.h ${DIST_INC_DIR}
        

        cp -f ${TOP_DIR}/src/protocol/mqtt/iotx_mqtt_internal.h ${DIST_INC_DIR}
        cp -f ${TOP_DIR}/src/protocol/mqtt/MQTTPacket/*.h ${DIST_INC_DIR}

        cp -f ${TOP_DIR}/build-rules/misc/makefile.mqtt ${DIST_ROOT_DIR}/Makefile
        cp -f ${TOP_DIR}/examples/litesdk/mqtt_example.c ${DIST_ROOT_DIR}
    fi

    cd ${OLDPWD}
done

echo ""
echo ""
echo ""

printf "    | %-5s | %-35s | %-9s | %-9s | %-10s | %-6s |\n" \
    "RATE" "MODULE NAME" "ROM" "RAM" "BSS" "DATA"
printf "    |-%-.5s-|-%-.35s-|-%-.9s-|-%-.9s-|-%-.10s-|-%-.6s-|\n" \
    "-------------" \
    "--------------------------------------------" \
    "-------------" \
    "-------------" \
    "-------------" \
    "-------------"

cd ${LIBOBJ_TMPDIR}
TOTAL_ROM=$(find $(for i in ${COMP_LIB_COMPONENTS}; do echo "${OUTPUT_DIR}/${i}"; done) -name "*.o" \
    | xargs size \
    | awk '{ sum += $1 } END { print sum }')
cd ${OLDPWD}

for iter in ${COMP_LIB_COMPONENTS}; do
    [ ! -d ${LIBOBJ_TMPDIR}/${iter} ] && continue

    cd ${LIBOBJ_TMPDIR}/${iter}

    ITER_OBJS=$(find . -name "*.o")
    [ "${ITER_OBJS}" != "" ] || continue
    ${STRIP} ${ITER_OBJS} 2>/dev/null

    size ${ITER_OBJS} | ${SED} '1d' \
        | awk -v name=${iter} -v total_rom=${TOTAL_ROM} '
            BEGIN { sum_rom = sum_ram = sum_bss = sum_data = 0 }
            {
                sum_rom += $1 + $2;
                sum_ram += $2 + $3;
                sum_bss += $3;
                sum_data += $2;
            }
            END {
                rate = sum_rom / total_rom * 100;
                printf("%d | %.4s%% | %-35s | %-9s | %-9s | %-10s | %-6s |\n",
                    sum_rom,
                    rate,
                    name,
                    sum_rom,
                    sum_ram,
                    sum_bss,
                    sum_data);
            }
        '

    cd ${OLDPWD}
done \
    | sort -nr \
    | cut -d' ' -f2- \
    | ${SED} 's!.*!    &!g' \
    | awk -v total_rom=${TOTAL_ROM} '
        BEGIN { sum_rom = sum_ram = sum_bss = sum_data = 0 }
        {
            sum_rom += $6;
            sum_ram += $8;
            sum_bss += $10;
            sum_data += $12;
            print;
        }
        END {
            rate = sum_rom / total_rom * 100;
            printf("    |-------|-------------------------------------|-----------|-----------|------------|--------|\n");
            if (rate < 100)
                printf("    | %.4s%% | %-35s | %-9s | %-9s | %-10s | %-6s |\n", rate, "- IN TOTAL -", sum_rom, sum_ram, sum_bss, sum_data);
            else
                printf("    |  %.3s%% | %-35s | %-9s | %-9s | %-10s | %-6s |\n", rate, "- IN TOTAL -", sum_rom, sum_ram, sum_bss, sum_data);
        }
        '

echo ""
