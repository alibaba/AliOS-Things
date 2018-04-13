#! /bin/bash

if [ $# != 2 ]; then echo "$# != 2"; exit 1; fi
if [ ! -f $1 ]; then echo "$1 not exist"; exit 2; fi

BLDV=$1
CMDV=$2

if [ "${CMDV}" = "list" ]; then
    grep "^PKG_UPSTREAM_[-/_a-zA-Z0-9]*" ${BLDV} \
        | sed 's/^PKG_UPSTREAM_\([^ ]*\) = \(.*\)$/[\1] \2/g'
fi

if [ "${CMDV}" = "update" ]; then
    REPOS=$(grep -o "^PKG_UPSTREAM_[-/_a-zA-Z0-9]*" ${BLDV}|sort -u|sed 's:PKG_UPSTREAM_::')
    for R in ${REPOS}; do
        UPSTREAM=$(grep -m 1 "^PKG_UPSTREAM_${R}" ${BLDV}|awk '{ print $NF }')
        SOURCE=$(grep -m 1 "^PKG_SOURCE_${R}" ${BLDV}|awk '{ print $NF }')
        BRANCH=$(grep -m 1 "^PKG_BRANCH_${R}" ${BLDV}|awk '{ print $NF }')
        [ "${BRANCH}" = "" ] && BRANCH="master"

        echo "[ ${SOURCE} ] <= : ${UPSTREAM} :: ${BRANCH}"
        set -x
        cd ${PACKAGE_DIR}
        rm -rf ${SOURCE}

        git clone -q --bare -b ${BRANCH} --single-branch ${UPSTREAM} ${SOURCE}
        rm -rf ${SOURCE}/hooks/
        mkdir -p ${SOURCE}/hooks/
        touch ${SOURCE}/hooks/.gitkeep
        touch ${SOURCE}/refs/{heads,tags}/.gitkeep

        rm -rf ${TOP_DIR}.pkgs/${SOURCE/.git}
        cd ${OLDPWD}
        set +x
    done
fi
