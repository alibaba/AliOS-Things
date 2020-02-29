if [ "$(uname)" = "Linux" ]; then
    CUR_OS="Linux"
elif [ "$(uname)" = "Darwin" ]; then
    CUR_OS="OSX"
elif [ "$(uname | grep NT)" != "" ]; then
    CUR_OS="Windows"
else
    echo "error: unkonw OS"
    exit 1
fi
echo "OS: ${CUR_OS}"

if [ -f ${HOME}/.bashrc ]; then
    . ${HOME}/.bashrc
fi
#echo "PATH=${PATH}"

JNUM=`cat /proc/cpuinfo | grep processor | wc -l`
JNUM=$((JNUM + 1))
grep -n "call NO_CONFIG_ERROR" build/Makefile > /dev/null 2>&1
if [ $? -eq 0 ];then
    TWO_STEP_BUILD=yes
else
    TWO_STEP_BUILD=no
fi
echo "TWO_STEP_BUILD=${TWO_STEP_BUILD}"
grep "^update:" build/build_rules/aos_kconfig.mk > /dev/null 2>&1
if [ $? -eq 0 ];then
    CREATE_THEN_MAKE=yes
else
    CREATE_THEN_MAKE=no
fi
echo "CREATE_THEN_MAKE=${CREATE_THEN_MAKE}"
RET=0

function do_build()
{
    build_app=$1
    build_board=$2
    build_option=$3

    app_mkfile_legacy="application/example/example_legacy/$(sed 's/\./\//g' <<< ${build_app})/aos.mk"
    if [ ! -f ${app_mkfile_legacy} ]; then
		app_mkfile="application/example/$(sed 's/\./\//g' <<< ${build_app})/aos.mk"
		if [ ! -f ${app_mkfile} ]; then
			echo "warning: ${app_mkfile} or ${app_mkfile_legacy} none exist, build ${build_app}@${build_board} ${build_option} skipped"
			return 0
		fi	
    fi

    build_log=${build_app}_${build_board}.log
    start_time=$(date +%s.%N)
    rm -rf out .config* .defconfig
    if [ "${TWO_STEP_BUILD}" = "yes" ]; then
        ret=0
        if [ "${CREATE_THEN_MAKE}" = "yes" ]; then
            if [ -z "$(echo \"$build_app\" | grep \.)" ]; then
                app=$build_app
            else
                app=$(echo $build_app | cut -d"." -f2)
            fi
            aos create project -b $build_board -d /tmp -t $app $app > ${build_log} 2>&1
        else
            aos make $build_app@$build_board -c config > ${build_log} 2>&1
        fi
        ret=$(($ret+$?))
        if [ "${CREATE_THEN_MAKE}" = "yes" ]; then
            cd /tmp/$app
        fi
        if [ "${build_option}" != "" ]; then
            aos make JOBS=${JNUM} ${build_option} >> ${build_log} 2>&1
        else
            aos make JOBS=${JNUM} >> ${build_log} 2>&1
        fi
        ret=$(($ret+$?))
        if [ "${CREATE_THEN_MAKE}" = "yes" ]; then
            cd -
            rm -rf /tmp/$app
        fi
    else
        if [ "${build_option}" != "" ]; then
            aos make $build_app@$build_board JOBS=${JNUM} ${build_option} > $build_log 2>&1
        else
            aos make $build_app@$build_board JOBS=${JNUM} > $build_log 2>&1
        fi
        ret=$?
    fi
    end_time=$(date +%s.%N)
    elapsed_time=$(python -c "print '{0:0.1f}'.format(${end_time}-${start_time})")

    build_cmd="aos make ${build_app}@${build_board}"
    if [ "${build_option}" != "" ]; then
        build_cmd="${build_cmd} ${build_option}"
    fi
    if [ ${ret} -eq 0 ]; then
        echo -e "$build_cmd succeed in ${elapsed_time}S"
        rm -f $build_log
    else
        echo -e "$build_cmd failed, log:\n"
        cat $build_log
        echo -e "\n$build_cmd failed in ${elapsed_time}S"
        RET=$((RET+1))
        exit 1
    fi
}

function resolveTargets()
{
    targets=$1
    build_type=$(grep "build_type" platform/board/*/$board/ucube.py | cut -d\" -f2 )
    platform_options=$(grep "platform_options" platform/board/*/$board/ucube.py | cut -d\" -f2 )

    for target in ${targets[@]};do
        if [ -z "$(echo \"$target\" | grep \|)" ]; then
            app=$target
            option=$platform_options
        else
            app=$(echo $target | cut -d"|" -f1)
            option="$platform_options "$(echo $target | cut -d"|" -f2- | sed -r 's/\|/ /g')
        fi

        do_build "$app" "$board" "$option"

        for platform_type in ${build_type[@]};do
            do_build "$app" "$board@$platform_type" "$option"
        done
    done
}


#--- main_proces ---
if [ xx$1 = xx ]; then
    boards_list=$(ls platform/board platform/board/board_legacy)
elif [ ! -f $1 ]; then
    echo "boards list file $1 is not existed!"
    exit 1
else
    boards_list=$(cat $1)
fi

for board in $boards_list
do
    #echo $board
    if [ -f platform/board/$board/ucube.py ];then
        targets=$(grep "supported_targets" platform/board/$board/ucube.py | cut -d\" -f2)
        resolveTargets "$targets"

        if [ $CUR_OS = "Windows" ]; then
            targets=$(grep "windows_only_targets" platform/board/$board/ucube.py | cut -d\" -f2 )
            resolveTargets "$targets"
        fi

        if [ $CUR_OS = "Linux" ]; then
            targets=$(grep "linux_only_targets" platform/board/$board/ucube.py | cut -d\" -f2 )
            resolveTargets "$targets"
        fi
    fi
	if [ -f platform/board/board_legacy/$board/ucube.py ];then
        targets=$(grep "supported_targets" platform/board/board_legacy/$board/ucube.py | cut -d\" -f2)
        resolveTargets "$targets"

        if [ $CUR_OS = "Windows" ]; then
            targets=$(grep "windows_only_targets" platform/board/board_legacy/$board/ucube.py | cut -d\" -f2 )
            resolveTargets "$targets"
        fi

        if [ $CUR_OS = "Linux" ]; then
            targets=$(grep "linux_only_targets" platform/board/board_legacy/$board/ucube.py | cut -d\" -f2 )
            resolveTargets "$targets"
        fi
    fi	
done
exit ${RET}
