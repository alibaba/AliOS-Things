echo "Making iotx-sdk-c"
origin_path="$(pwd)"
comp_path="$( cd "$(dirname "$0")" ; pwd -P )"
echo origin_path: $origin_path
echo comp_path: $comp_path
cd $comp_path
if [ $? -ne 0 ]; then
    echo "cd fail!"
    exit 1
fi

cd iotx-sdk-c_clone

library_path=$1
app_path=$2
host_os=$3

MAKE="make"
echo host_os $host_os
if [ "$host_os" = "Win32" ];then
MAKE="$origin_path/build/cmd/win32/make"
fi

echo MAKE $MAKE


cp $origin_path/$app_path/make.settings .
if [ $? -ne 0 ]; then
    echo "cp make.settings fail!"
    exit 1
fi

$MAKE distclean
if [ $? -ne 0 ]; then
    echo "make distclean fail!"
    exit 1
fi

$MAKE prune
if [ $? -ne 0 ]; then
    echo "make prune fail!"
    exit 1
fi

DEFAULT_BLD=$PWD/src/board/config.rhino.make $MAKE config
if [ $? -ne 0 ]; then
    echo "make config fail!"
    exit 1
fi


$MAKE -j4 -f .O/.one_makefile
if [ $? -ne 0 ]; then
    echo "make fail!"
    exit 1
fi

echo "copy to library path: $origin_path/$library_path"
cp ./.O/usr/lib/libiot_sdk.a $origin_path/$library_path
if [ $? -ne 0 ]; then
    echo "cp failed!"
    exit 1
fi


