#!/usr/bin/env bash

# 1. store workdir path
workdir=$PWD

# 2. get shelldir path
shelldir=$(cd $(dirname ${BASH_SOURCE[0]}); pwd )
echo " =================== target = $1 ======================="
cd ${shelldir}

idf_src_code_folder="esp-idf"

# 3. check export.sh exist and clone source code if not
if [ ! -d "${shelldir}/${idf_src_code_folder}" ]; then
    echo " ************** clone idf source code from gitee ************"
    git_clone=`git clone https://gitee.com/alios-things/esp-idf.git ${idf_src_code_folder}`
fi

# 4. checkout different branchs
cd ${shelldir}/${idf_src_code_folder}
git checkout .
git clean -df
if [ $1 = "GENERIC_C3" ] || [ $1 = "GENERIC_S3" ] ; then
    git_checkout=`git checkout release_v4.4`
else
    git_checkout=`git checkout release_v4.2`
fi

#4. install env
cd ${shelldir}/esp-idf
./install.sh
cd ${workdir}
