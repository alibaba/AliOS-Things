#!/usr/bin/env bash

targets="
alinkapp@mk3060,
linkkitapp@mk3060,
mqttapp@b_l475e,
alinkapp@esp32devkitc,
coapapp@esp32devkitc,
linkkitapp@esp8266,
bluetooth.aisilopapp@pca10056
"
recipients="wb-zz173777@alibaba-inc.com wb-zhaorunan@alibaba-inc.com wb-hebobo@alibaba-inc.com wb-ws312389@alibaba-inc.com shaofa.lsf@alibaba-inc.com minghao.zmh@alibaba-inc.com apsaras73@list.alibaba-inc.com"

workdir=aos_scm_build
bkupdir=~/scmbackup
logfile=${PWD}/auto_build.log

if [ $# -gt 0 ]; then
    branch=$1
else
    branch=master
fi

if [ -f ${HOME}/.bashrc ]; then
    . ${HOME}/.bashrc
fi

now=`date`
echo "start aos auto build at ${now}" | tee ${logfile}
echo "start aos auto build at ${now}"
cd ~/
if [ -d ${workdir} ]; then
    echo "removing existing ${workdir} folder ..." | tee -a ${logfile}
    rm -rf ${workdir}
fi

echo "pull code into ${workdir} ..." | tee -a ${logfile}
git clone git@code.aliyun.com:keepwalking.zeng/aos.git ${workdir} > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "error: pull code failed" | tee -a ${logfile}
    echo "error: pull code failed"
    exit 1
fi

cd ${workdir}
datetime=`date +%F@%H-%M`
mkdir ${datetime}
mailcontent="Attached please find the AOS SCM build firmware\n\n"
mailcontent="${mailcontent} Build Infomation\n"
mailcontent="${mailcontent} Time: `date '+%F %T'`\n"

echo "build start ..." | tee -a ${logfile}
echo "build start ..."

#checkout branch
if [ "${branch}" != "master" ]; then
    git branch --track ${branch} origin/${branch}
    if [ $? -ne 0 ]; then
        echo "error: branch ${branch} does not exist" | tee -a ${logfile}
        echo "error: branch ${branch} does not exist"
        exit 1
    fi
    git checkout ${branch}
fi
mailcontent="${mailcontent} Branch: ${branch}\n"
mailcontent="${mailcontent} Commit: `git log -n 1 --pretty=format:"%H"`\n"
fail_num=0

targets=(${targets//,/})
aos make clean > /dev/null
for target in "${targets[@]}"; do
    aos make ${target} > ${target}@${branch}.log 2>&1
    if [ $? -eq 0 ]; then
        echo "build ${target} at branch ${branch} succeed" | tee -a ${logfile}
        cp out/${target}/binary/${target}.elf ${datetime}/${target}@${branch}.elf
        cp out/${target}/binary/${target}.bin ${datetime}/${target}@${branch}.bin
    else
        echo "build ${target} at branch ${branch} failed" | tee -a ${logfile}
        fail_num=$((fail_num+1))
    fi
done

echo "build end ..." | tee -a ${logfile}
echo "build end ..."

#zip files
mv ${logfile} ${datetime}
zip -r ${datetime}.zip ${datetime}/*.bin > /dev/null
mv ${datetime}.zip ${datetime}/

#send email
attachment=${datetime}/${datetime}.zip
echo -e ${mailcontent} | mutt -a ${attachment} -s "AOS SCM Build" -- ${recipients}

#copy to scm backup folder
if [ ! -d ${bkupdir} ]; then
    mkdir ${bkupdir}
fi
rm -f ${datetime}/${datetime}.zip
cp -rf ${datetime} ${bkupdir}/

exit ${fail_num}
