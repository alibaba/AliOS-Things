#! /bin/bash

TARGET_FILE=$1
rm -f ${TARGET_FILE}

CONFIG_VERNDOR=$(grep -m 1 "VENDOR *:" .config|awk '{ print $NF }')
IFLAGS=$( \
for iter in \
    $(find -L \
        ${COMP_LIB_COMPONENTS} \
        ${EXPORT_DIR} \
        ${EXTRA_INCLUDE_DIRS} \
        ${IMPORT_DIR}/${CONFIG_VENDOR}/include \
            -type d -not -path "*.git*" -not -path "*.O*" 2>/dev/null); do \
                echo "    -I${iter} \\"; \
    done)
CFLAGS=$( \
echo "${CFLAGS}" \
    | xargs -n 1 \
    | grep -v '\-\-coverage' \
    | awk '{ printf("%s ", $0); }' \
)

ETC_OBJS=$(
for i in ${ALL_LIBS}; do
    j=$(grep "${i}$" ${STAMP_BLD_VAR} | cut -d' ' -f1 | sed 's|LIBA_TARGET_|LIB_OBJS_|g')
    k=$(grep "${j}" ${STAMP_BLD_VAR} | cut -d' ' -f3-)
    for l in ${k}; do
        echo "${j//LIB_OBJS_/}/${l}"
    done
done)
ALL_LIBS=$(for iter in ${ALL_LIBS}; do echo -n "${OUTPUT_DIR}/usr/lib/${iter} "; done)
ALL_BINS=$(for iter in ${ALL_PROG}; do echo -n "${OUTPUT_DIR}/usr/bin/${iter} "; done)
OUTPUT_D=$(basename ${OUTPUT_DIR})

if [ "${CC}" != "gcc" ]; then
    ALL_BINS=""
fi

cat << EOB >> ${TARGET_FILE}
include ${RULE_DIR}/funcs.mk

SHELL := bash
Q     ?= @
VPATH := $(for iter in ${COMP_LIB_COMPONENTS}; do echo -n "${OUTPUT_DIR}/${iter} "; done)

.PHONY: all
all: ${OUTPUT_DIR}/usr/lib/${COMP_LIB} ${ALL_LIBS} ${ALL_BINS}
	\$(Q)cp -rf ${EXTRA_INSTALL_HDRS} ${OUTPUT_DIR}/usr/include 2>/dev/null || true
	@rm -f *.gcda *.gcno \$\$(find ${RULE_DIR} -name "*.o")

${OUTPUT_DIR}/usr/lib/${COMP_LIB}: \\
$(for iter in ${COMP_LIB_OBJS}; do
    echo "    ${iter} \\"
done
)

	\$(Q)mkdir -p \$\$(dirname \$@)
	\$(Q)\$(call Brief_Log,"AR",\$\$(basename \$@),"...")
	\$(Q)${AR} -rcs \$@ \$^

%.o:
	\$(Q)\$(call Brief_Log,"CC",\$\$(basename \$@),"...")
	\$(Q)mkdir -p \$\$(dirname \$@)
	\$(Q)S=\$\$(echo \$@|sed 's,${OUTPUT_DIR},${TOP_DIR},1'); \\
    ${CC} -c \\
        -o \$@ \\
        ${CFLAGS} \\
        ${IFLAGS}
        \$\${S//.o/.c}

EOB

for i in ${ALL_LIBS}; do
    n=$(basename ${i})
    j=$(grep "${n}$" ${STAMP_BLD_VAR}|cut -d' ' -f1|sed 's:LIBA_TARGET_::1')
    k=$(echo 'LIB_OBJS_'"${j}")
    k=$(grep -m 1 "^${k}" ${STAMP_BLD_VAR}|cut -d' ' -f3-)
    k=$(for l in ${k}; do echo -n "${OUTPUT_DIR}/${j}/${l} "; done)

    cat << EOB >> ${TARGET_FILE}
${OUTPUT_DIR}/usr/lib/${n}: \\
$(for m in ${k}; do
    echo "    ${m} \\";
done)

	\$(Q)mkdir -p \$\$(dirname \$@)
	\$(Q)\$(call Brief_Log,"AR",\$\$(basename \$@),"...")
	\$(Q)${AR} -rcs \$@ \$^

EOB
done

for i in ${ALL_PROG}; do
    j=$(grep -w -m 1 "^SRCS_${i}" ${STAMP_BLD_VAR}|cut -d' ' -f3-)
    k=$(grep -m 1 "TARGET_.* = .*${i}" ${STAMP_BLD_VAR}|cut -d' ' -f1|sed 's:TARGET_::1')
    if [ "$(grep -m 1 "^TARGET_${k}" ${STAMP_BLD_VAR}|cut -d' ' -f3-|awk '{ print NF }')" = "1" ]; then
        k=""
    fi
    LFLAGS=$(grep -m 1 "^LDFLAGS_${k}" ${STAMP_BLD_VAR}|cut -d' ' -f3-)
    j=$(for n in ${j}; do echo -n "${TOP_DIR}/${k}/${n} "; done)

    cat << EOB >> ${TARGET_FILE}
${OUTPUT_DIR}/usr/bin/${i}: \\
$(for m in ${j} ${OUTPUT_DIR}/usr/lib/${COMP_LIB} ${ALL_LIBS}; do
    echo "    ${m} \\";
done)

	\$(Q)\$(call Brief_Log,"LD",\$\$(basename \$@),"...")
	\$(Q)${CC} \\
        -o \$@ \\
        ${IFLAGS}
        ${CFLAGS} \\
        -L${OUTPUT_DIR}/usr/lib \\
        \$^ \\
        ${LFLAGS}

EOB
done

# TMP_DEPF=$(mktemp)
# grep -o '/[a-zA-Z][a-zA-Z].*\.o\>' ${TARGET_FILE} \
#     | sed 's!\(.*\)\(.O/\)\(.*\)\.o!\1\2\3.o : \1\3.c!g' > ${TMP_DEPF}
# cat ${TMP_DEPF} >> ${TARGET_FILE}
# rm -f ${TMP_DEPF}
