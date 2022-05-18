#!/usr/bin/env bash

# 1. store workdir path
workdir=$PWD

# 2. get shelldir path
shelldir=$(cd $(dirname ${BASH_SOURCE[0]}); pwd )
echo " =================== target = $1 ======================="
cd ${shelldir}

# 3. checkout branch and copy yamls
git checkout .
git clean -df
if [ $1 = "GENERIC_C3" ] ; then
    git_checkout=`git checkout release_v4.4`
    cp -rf ${shelldir}/package-esp32-c3.yaml ${shelldir}/package.yaml
elif [ $1 = "GENERIC_S3" ] ; then
    git_checkout=`git checkout release_v4.4`
    cp -rf ${shelldir}/package-esp32-s3.yaml ${shelldir}/package.yaml
else
    git_checkout=`git checkout dev_aos`
    cp -rf ${shelldir}/package-esp32.yaml ${shelldir}/package.yaml
fi

#4. install env
cd ${shelldir}/esp-idf
./install.sh
cd ${workdir}
