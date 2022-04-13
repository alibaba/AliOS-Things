#!/usr/bin/env bash
if [ $# -lt 2 ] ; then
    echo "USAGE: $0 appname boardname[option1=xxx]"
    echo " e.g.: $0 uapp1 vexpressa9-mk"
    exit 1;
fi
app=$1
board=$2
shift 2
params=$@

SCRIPT_DIR=$(cd $(dirname $0); pwd )
SRC_ROOT_DIR=$(cd $(dirname $0)/../; pwd )
SCRIPTS_PATH=${SCRIPT_DIR}/scripts
OUTPUT_PATH=${SRC_ROOT_DIR}/out

# step 1: analysis aos.mk of app, and get dependent chain 
python ${SCRIPTS_PATH}/aos_parse_components.py ${SRC_ROOT_DIR} ${OUTPUT_PATH}/all_components.mk
python ${SCRIPTS_PATH}/aos_parse_apps.py ${app} ${OUTPUT_PATH}/all_components.mk ${OUTPUT_PATH}/dep_apps.mk

# step 2: build apps one by one
IFS=$'\n'
for app in $(<${OUTPUT_PATH}/dep_apps.mk)
do
    aos make $app@$board -c config && aos make MBINS=app $params
done

