#! /bin/bash
set -e
if [ $# != 1 ]; then exit 1; fi

LCOV_DIR=$1
SRC_NAME_LENGTH=28
DIR_NAME_LENGTH=16
COVERAGE_HIGH=90
COVERAGE_MID=75

echo ""
echo "Processing [${LCOV_DIR}] for Coverage Brief"
echo ""
#
#      sdk-example / example.c               : [100.00%]  (7/7)            [100.00%]  (1/1)
#
echo -ne "\e[1;36m"
printf "%${DIR_NAME_LENGTH}s   %-${SRC_NAME_LENGTH}s: %-24s %-20s\n\n" \
    "Directory" \
    "Source File" \
    "Line Coverage" \
    "Function Coverage"
echo -ne "\e[0m"

REPORT_LIST=$(find ${LCOV_DIR} -mindepth 2 -name "index.html")
for R in ${REPORT_LIST}; do
    SOURCE_LIST=$(grep "coverFile" ${R} | awk -F '<' '{ print $3 }' | cut -d'>' -f2)
    for S in ${SOURCE_LIST}; do
        STATS=$(grep -A 7 "${S}</a>" ${R} | sed -n '6p;8p'|grep -o [0-9]*)
        DIR_NAME=$(expr substr $(dirname ${R}|xargs basename) 1 15)
        SRC_NAME=$(expr substr ${S} 1 $(( SRC_NAME_LENGTH -1 )))
        COVER_LINE=$(echo ${STATS}|cut -d' ' -f1)
        TOTAL_LINE=$(echo ${STATS}|cut -d' ' -f2)
        COVER_FUNC=$(echo ${STATS}|cut -d' ' -f3)
        TOTAL_FUNC=$(echo ${STATS}|cut -d' ' -f4)

        LINE_PERCENT="$(echo "scale=2; ${COVER_LINE} * 100 / ${TOTAL_LINE}"|bc -l)%"
        FUNC_PERCENT="$(echo "scale=2; ${COVER_FUNC} * 100 / ${TOTAL_FUNC}"|bc -l)%"

        printf \
            "%-8s %${DIR_NAME_LENGTH}s / %-${SRC_NAME_LENGTH}s: [ %s ] (%s/%s) \t [ %s ] (%s/%s)\n" \
            ${LINE_PERCENT} \
            ${DIR_NAME} \
            ${SRC_NAME} \
            ${LINE_PERCENT} \
            ${COVER_LINE} \
            ${TOTAL_LINE} \
            ${FUNC_PERCENT} \
            ${COVER_FUNC} \
            ${TOTAL_FUNC}
    done
done \
    | sort -nr \
    | cut -d' ' -f2- \
    | awk -v SRC_LEN=${SRC_NAME_LENGTH} -v DIR_LEN=${DIR_NAME_LENGTH} \
          -v HIGH=${COVERAGE_HIGH} -v MID=${COVERAGE_MID} \
            '{
                split($6, arr, ".");

                if (arr[1] > HIGH)
                    printf("\033[1;32m");
                else if (arr[1] > MID)
                    printf("\033[1;35m");
                else
                    printf("\033[1;31m");

                printf("%" DIR_LEN "s / %-" SRC_LEN "s: [ %-8s] %-12s [ %-8s] %-12s\n",
                       $1, $3, $6, $8, $10, $12);

                printf("\033[0m");
            }'

#
# 1           2 3         4 5 6       7 8     9 10      11 12
# sdk-example / example.c : [ 100.00% ] (7/7) [ 100.00% ]  (1/1)
#
echo ""
exit 0
