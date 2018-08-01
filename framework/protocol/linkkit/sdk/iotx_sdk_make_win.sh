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

make -j4 -f aos.makefile
if [ $? -ne 0 ]; then
    echo "make fail!"
    exit 1
fi

echo "copy to library path: $origin_path/$library_path"
cp .O/usr/lib/libiot_sdk.a $origin_path/$library_path
if [ $? -ne 0 ]; then
    echo "cp failed!"
    exit 1
fi


