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


function do_build()
{
    build_app=$1
    build_board=$2
    build_option=$3

    build_cmd_log=$app_$build_board@${branch}.log
    build_cmd="aos make $build_app@$build_board"
    if [ "${build_option}" != "" ]; then
        build_cmd="${build_cmd} ${build_option}"
    fi

    #remove @release @debug
    board_without_at=$(echo $build_board | cut -d"@" -f1 )
    if [ -f build/scons_enabled.py ]; then
        scons_support=$(grep $board_without_at build/scons_enabled.py)
    else
        scons_support=""
    fi
    if [ -z "$scons_support" ]; then
        #echo rm -rf out/$build_app@$build_board
        rm -rf out/$build_app@$build_board  > /dev/null 2>&1
    fi

    start_time=$(date +%s.%N)

    #echo $build_cmd
    $build_cmd > $build_cmd_log 2>&1

    ret=$?; end_time=$(date +%s.%N)
    elapsed_time=$(python -c "print '{0:0.1f}'.format(${end_time}-${start_time})")
    if [ ${ret} -eq 0 ]; then
        echo -e "$build_cmd at ${branch} branch succeed in ${elapsed_time}S"
        rm -f $build_cmd_log
    else
        echo -e "$build_cmd at ${branch} branch failed, log:\n"
        cat $build_cmd_log
        echo -e "\n$build_cmd at ${branch} branch failed in ${elapsed_time}S"
        aos make clean > /dev/null 2>&1
        exit 1
    fi
}

function resolveTargets()
{
    targets=$1
    build_type=$(grep "build_type" board/$board/ucube.py | cut -d\" -f2 )
    platform_options=$(grep "platform_options" board/$board/ucube.py | cut -d\" -f2 )

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
    boards_list=$(ls board)
elif [ ! -f $1 ]; then
    echo "boards list file $1 is not existed!"
    exit 1
else
    boards_list=$(cat $1)
fi

for board in $boards_list
do
    #echo $board
    if [ -f board/$board/ucube.py ];then
        targets=$(grep "supported_targets" board/$board/ucube.py | cut -d\" -f2)
        resolveTargets "$targets"

        if [ $CUR_OS = "Windows" ]; then
            targets=$(grep "windows_only_targets" board/$board/ucube.py | cut -d\" -f2 )
            resolveTargets "$targets"
        fi

        if [ $CUR_OS = "Linux" ]; then
            targets=$(grep "linux_only_targets" board/$board/ucube.py | cut -d\" -f2 )
            resolveTargets "$targets"
        fi
    fi
done
