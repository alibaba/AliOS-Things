#!/usr/bin/env bash
if [ $# -lt 3 ] ; then
    echo "USAGE: $0 appname boardname[option1=xxx]"
    echo " e.g.: $0 uapp1 vexpressa9-mk ."
    exit 1;
fi
app=$1
board=$2
cur_dir=$3
BUILD_MINIAPP=$4

SCRIPT_DIR=`dirname $0`
SRC_ROOT_DIR=${SCRIPT_DIR}/..


# step1: copy user-mode app
mkdir -p $cur_dir/temp/lfs_sys_dir
mkdir -p $cur_dir/temp/lfs_data_dir

if [ "$BUILD_MINIAPP" -eq 1 ] ; then
# copy miniapp resources to system partition dir
MINIAPP_RESOURCES=${SRC_ROOT_DIR}/miniapp/resources
if [ -d $MINIAPP_RESOURCES ] ; then
    MINIAPP_SYS_RESOURCES=$cur_dir/temp/lfs_sys_dir/miniapp/resources
#   if [ -d $MINIAPP_SYS_RESOURCES ] ; then
#       rm -rf $MINIAPP_SYS_RESOURCES
#   fi
    mkdir -p $MINIAPP_SYS_RESOURCES
    mkdir -p $MINIAPP_SYS_RESOURCES/manifest
    mkdir -p $MINIAPP_SYS_RESOURCES/pkg
    mkdir -p $MINIAPP_SYS_RESOURCES/fonts
    mkdir -p $MINIAPP_SYS_RESOURCES/systemui

    #cp -rf $MINIAPP_RESOURCES/ $MINIAPP_SYS_RESOURCES/
    cp -rf $MINIAPP_RESOURCES/framework $MINIAPP_SYS_RESOURCES/
    cp -rf $MINIAPP_RESOURCES/systemui/ $MINIAPP_SYS_RESOURCES/
    cp -rf $MINIAPP_RESOURCES/fonts $MINIAPP_SYS_RESOURCES/
    cp -rf $MINIAPP_RESOURCES/pkg/2021001183612524.amr $MINIAPP_SYS_RESOURCES/pkg/

    cp -rf $MINIAPP_RESOURCES/local_file.json $MINIAPP_SYS_RESOURCES/
    cp -rf $MINIAPP_RESOURCES/preload_package.json $MINIAPP_SYS_RESOURCES/
    cp -rf $MINIAPP_RESOURCES/manifest/2021001183612524.json $MINIAPP_SYS_RESOURCES/manifest/
fi
fi

#copy movie file to /system folder
MOVIE_RESOURCES=${SRC_ROOT_DIR}/components/utilities/ffmpeg/share/ffmpeg/test.mp4
FFMPEG_SYS_RESOURCES=$cur_dir/temp/lfs_sys_dir/ffmpeg
mkdir -p $FFMPEG_SYS_RESOURCES
cp -rf $MOVIE_RESOURCES $FFMPEG_SYS_RESOURCES/
cp -rf ${SRC_ROOT_DIR}/components/utilities/tinyalsa/6.wav $cur_dir/temp/lfs_sys_dir

find $cur_dir/out -name *.stripped.elf -exec cp {} $cur_dir/temp/lfs_sys_dir \;
rm -f $cur_dir/temp/lfs_sys_dir/alios_kernel*.stripped.elf

find $cur_dir/out -name *.stripped.elf -exec cp {} $cur_dir/temp/lfs_sys_dir \;
rm -f $cur_dir/temp/lfs_sys_dir/alios_kernel*.stripped.elf
if [ -f $cur_dir/temp/lfs_sys_dir/uapp1@vexpressa9-mk.app.stripped.elf ];then
    mv $cur_dir/temp/lfs_sys_dir/uapp1@vexpressa9-mk.app.stripped.elf $cur_dir/temp/lfs_sys_dir/uapp1.elf
fi
# step2: create system file system with app
sys_dir=$cur_dir/temp/lfs_sys_dir
sys_img_file=$cur_dir/temp/lfs_sys.img
data_dir=$cur_dir/temp/lfs_data_dir
data_img_file=$cur_dir/temp/lfs_data.img
qemu_out=$cur_dir/out/vexpressa9
# blk_size 64*4096=256K
# partition size = blk_size * blk_cnt = 64*4096 * 256 = 64MB
page_size=$((4096))
blk_size=$((4096*64))
data_blk_cnt=$((192))
sys_blk_cnt=$((256))
data_partition_size=$(($blk_size*$data_blk_cnt))
sys_partition_size=$(($blk_size*$sys_blk_cnt))
echo "System partition info:"
echo "page_size      : "  $page_size "B"
echo "blk_size       : "  $blk_size  "B"
echo "blk_cnt        : "  $sys_blk_cnt
echo "partition_size : "  $(($sys_partition_size/1024)) "KB"
$cur_dir//vendor/vexpress/board/vexpressa9-mk/script/mklfs -c $sys_dir -b $blk_size -r $page_size -p $page_size -s $sys_partition_size -i $sys_img_file
echo "Data partition info:"
echo "page_size      : "  $page_size "B"
echo "blk_size       : "  $blk_size  "B"
echo "blk_cnt        : "  $data_blk_cnt
echo "partition_size : "  $(($data_partition_size/1024)) "KB"
$cur_dir//vendor/vexpress/board/vexpressa9-mk/script/mklfs -c $data_dir -b $blk_size -r $page_size -p $page_size -s $data_partition_size -i $data_img_file

rm -rf $qemu_out
mkdir -p $qemu_out
#step3: create whole file system
dd if=/dev/zero of=$qemu_out/flash_kv.img count=32768
cat $cur_dir/temp/lfs_data.img $qemu_out/flash_kv.img > $qemu_out/flash1.img && rm -rf $qemu_out/flash_kv.img
dd if=$cur_dir/temp/lfs_sys.img of=$qemu_out/flash2.img count=131072
rm -rf $cur_dir/temp

