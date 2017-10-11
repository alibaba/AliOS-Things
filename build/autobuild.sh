#!/bin/sh

workdir=autobuild
linux_platforms="linuxhost linuxhost@debug linuxhost@release"
mk3060_targets="alinkapp helloworld linuxapp meshapp tls"
linux_targets="alinkapp helloworld linuxapp meshapp tls yts"
linux_posix_targets="alinkapp meshapp"
mk3060_platforms="mk3060 mk3060@release"
linux_platforms="linuxhost linuxhost@debug linuxhost@release"
b_l475e_targets="mqttapp helloworld tls"
b_l475e_platforms="b_l475e"
bins_type="app kernel"

git status > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "error: not in any git repository"
    exit 1
fi

branch=`git status | grep "On branch" | sed -r 's/.*On branch //g'`
cd $(git rev-parse --show-toplevel)

#linuxhost posix
aos make clean > /dev/null 2>&1
for target in ${linux_posix_targets}; do
    for platform in ${linux_platforms}; do
        vcall=posix aos make ${target}@${platform} > ${target}@${platform}@${branch}.log 2>&1
        if [ -f out/${target}@${platform}/binary/${target}@${platform}.elf ]; then
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

#single-bin, mk3060
aos make clean > /dev/null 2>&1
for target in ${mk3060_targets}; do
    for platform in ${mk3060_platforms}; do
        aos make ${target}@${platform} > ${target}@${platform}@${branch}.log 2>&1
        if [ -f out/${target}@${platform}/binary/${target}@${platform}.elf ]; then
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
            if [ ${target} = "tls" -o ${target} = "meshapp" ]; then
                continue
            fi
            aos make ${target}@${platform} BINS=${bins} > ${target}@${platform}@${bins}@${branch}.multi-bins.log 2>&1
            if [ -f out/${target}@${platform}${bins}/binary/${target}@${platform}.${bins}.elf ]; then
                rm -rf ${target}@${platform}@${bins}@${branch}.multi-bins.log
                echo "build ${target}@${platform} BINS=${bins} as multiple BINs at ${branch} branch succeed"
            else
                echo -e "build ${target}@${platform} BINS=${bins} as multiple BINs at ${branch} branch failed, log:\n"
                cat ${target}@${platform}@${bins}@${branch}.multi-bins.log
                rm -rf ${target}@${platform}@${bins}@${branch}.multi-bins.log
                echo -e "\nbuild ${target}@${platform} BINS=${bins} as multiple BINs at ${branch} branch failed"
                aos make clean > /dev/null 2>&1
                exit 1
            fi
        done
    done
done

#linuxhost
aos make clean > /dev/null 2>&1
for target in ${linux_targets}; do
    for platform in ${linux_platforms}; do
        aos make ${target}@${platform} > ${target}@${platform}@${branch}.log 2>&1
        if [ -f out/${target}@${platform}/binary/${target}@${platform}.elf ]; then
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

#single-bin, b_l475e
aos make clean > /dev/null 2>&1
for target in ${b_l475e_targets}; do
    for platform in ${b_l475e_platforms}; do
        aos make ${target}@${platform} > ${target}@${platform}@${branch}.log 2>&1
        if [ -f out/${target}@${platform}/binary/${target}@${platform}.elf ]; then
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

echo "----------check CODE-STYLE now----------"
#./build/astyle.sh
echo "----------------------------------------"
echo "----------check COPYRIGHT now-----------"
python ./build/copyright.py
echo "----------------------------------------"
