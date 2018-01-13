#!/usr/bin/env bash

workdir=autobuild
linux_posix_targets="alinkapp"
linux_targets="alinkapp helloworld linuxapp yts"
linux_platforms="linuxhost linuxhost@debug linuxhost@release"
mk3060_targets="alinkapp helloworld linuxapp meshapp uDataapp"
mk3060_platforms="mk3060 mk3060@release"
b_l475e_targets="mqttapp helloworld uDataapp"
b_l475e_platforms="b_l475e"
esp32_targets="alinkapp helloworld meshapp"
esp32_platforms="esp32devkitc"
bins_type="app framework kernel"

git status > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "error: not in any git repository"
    exit 1
fi

JNUM=`cat /proc/cpuinfo | grep processor | wc -l`
if [ -f ~/.bashrc ]; then
    . ~/.bashrc
fi

branch=`git status | grep "On branch" | sed -r 's/.*On branch //g'`
cd $(git rev-parse --show-toplevel)

#linuxhost posix
aos make clean > /dev/null 2>&1
for target in ${linux_posix_targets}; do
    for platform in ${linux_platforms}; do
        vcall=posix aos make ${target}@${platform} JOBS=${JNUM} > ${target}@${platform}@${branch}.log 2>&1
        if [ $? -eq 0 ]; then
            echo "build vcall=posix ${target}@${platform} at ${branch} branch succeed"
            rm -rf ${target}@${platform}@${branch}.log
        else
            echo -e "build vcall=posix ${target}@${platform} at ${branch} branch failed, log:\n"
            cat ${target}@${platform}@${branch}.log
            echo -e "\nbuild ${target}@${platform} at ${branch} branch failed"
            aos make clean > /dev/null 2>&1
            exit 1
        fi
    done
done

#linuxhost
aos make clean > /dev/null 2>&1
for target in ${linux_targets}; do
    for platform in ${linux_platforms}; do
        aos make ${target}@${platform} JOBS=${JNUM} > ${target}@${platform}@${branch}.log 2>&1
        if [ $? -eq 0 ]; then
            echo "build ${target}@${platform} at ${branch} branch succeed"
            rm -rf ${target}@${platform}@${branch}.log
        else
            echo -e "build ${target}@${platform} at ${branch} branch failed, log:\n"
            cat ${target}@${platform}@${branch}.log
            echo -e "\nbuild ${target}@${platform} at ${branch} branch failed"
            aos make clean > /dev/null 2>&1
            exit 1
        fi
    done
done

#single-bin, mk3060
aos make clean > /dev/null 2>&1
for target in ${mk3060_targets}; do
    for platform in ${mk3060_platforms}; do
        aos make ${target}@${platform} JOBS=${JNUM} > ${target}@${platform}@${branch}.log 2>&1
        if [ $? -eq 0 ]; then
            rm -rf ${target}@${platform}@${branch}.log
            echo "build ${target}@${platform} at ${branch} branch succeed"
        else
            echo -e "build ${target}@${platform} at ${branch} branch failed, log:\n"
            cat ${target}@${platform}@${branch}.log
            rm -rf ${target}@${platform}@${branch}.log
            echo -e "\nbuild ${target}@${platform} at ${branch} branch failed"
            aos make clean > /dev/null 2>&1
            exit 1
        fi
    done
done

#multi-bins, mk3060
aos make clean > /dev/null 2>&1
for target in ${mk3060_targets}; do
    for platform in ${mk3060_platforms}; do
        for bins in ${bins_type}; do
            if [ "${target}" = "tls" ] || [ "${target}" = "meshapp" ] || [ "${target}" = "uDataapp" ]; then
                continue
            fi
            aos make ${target}@${platform} BINS=${bins} JOBS=${JNUM} > ${target}@${platform}@${bins}@${branch}.log 2>&1
            if [ $? -eq 0 ]; then
                rm -rf ${target}@${platform}@${bins}@${branch}.log
                echo "build ${target}@${platform} BINS=${bins} as multiple BINs at ${branch} branch succeed"
            else
                echo -e "build ${target}@${platform} BINS=${bins} as multiple BINs at ${branch} branch failed, log:\n"
                cat ${target}@${platform}@${bins}@${branch}.log
                rm -rf ${target}@${platform}@${bins}@${branch}.log
                echo -e "\nbuild ${target}@${platform} BINS=${bins} as multiple BINs at ${branch} branch failed"
                aos make clean > /dev/null 2>&1
                exit 1
            fi
        done
    done
done

#single-bin, b_l475e
aos make clean > /dev/null 2>&1
for target in ${b_l475e_targets}; do
    for platform in ${b_l475e_platforms}; do
        aos make ${target}@${platform} JOBS=${JNUM} > ${target}@${platform}@${branch}.log 2>&1
        if [ $? -eq 0 ]; then
            rm -rf ${target}@${platform}@${branch}.log
            echo "build ${target}@${platform} at ${branch} branch succeed"
        else
            echo -e "build ${target}@${platform} at ${branch} branch failed, log:\n"
            cat ${target}@${platform}@${branch}.log
            rm -rf ${target}@${platform}@${branch}.log
            echo -e "\nbuild ${target}@${platform} at ${branch} branch failed"
            aos make clean > /dev/null 2>&1
            exit 1
        fi
    done
done

#single-bin, esp32
aos make clean > /dev/null 2>&1
for target in ${esp32_targets}; do
    for platform in ${esp32_platforms}; do
        aos make ${target}@${platform} wifi=1 JOBS=${JNUM} > ${target}@${platform}@${branch}.log 2>&1
        if [ $? -eq 0 ]; then
            rm -rf ${target}@${platform}@${branch}.log
            echo "build ${target}@${platform} at ${branch} branch succeed"
        else
            echo -e "build ${target}@${platform} at ${branch} branch failed, log:\n"
            cat ${target}@${platform}@${branch}.log
            rm -rf ${target}@${platform}@${branch}.log
            echo -e "\nbuild ${target}@${platform} at ${branch} branch failed"
            aos make clean > /dev/null 2>&1
            exit 1
        fi
    done
done

aos make clean > /dev/null 2>&1
echo "build ${branch} branch succeed"
