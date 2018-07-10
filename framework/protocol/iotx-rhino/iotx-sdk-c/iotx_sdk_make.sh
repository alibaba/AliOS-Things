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

make distclean
if [ $? -ne 0 ]; then
    echo "make distclean fail!"
    exit 1
fi

make prune
if [ $? -ne 0 ]; then
    echo "make prune fail!"
    exit 1
fi

make
if [ $? -ne 0 ]; then
    echo "make fail!"
    exit 1
fi

echo "copy to library path： $origin_path/$1"
cp output/release/lib/libiot_sdk.a $origin_path/$1
if [ $? -ne 0 ]; then
    echo "cp failed!"
    exit 1
fi
