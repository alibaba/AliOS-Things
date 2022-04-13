#!/usr/bin/env bash
# step1: copy user-mode app
mkdir -p temp/lfs_sys_dir
mkdir -p temp/lfs_data_dir
find ./out -name *.stripped.elf -exec cp {} ./temp/lfs_sys_dir \;
rm -f ./temp/lfs_sys_dir/alios_kernel*.stripped.elf

# step2: create system file system with app
cur_dir="$(pwd)"
echo "cur_dir:" $cur_dir
sys_dir=$cur_dir/temp/lfs_sys_dir
sys_img_file=$cur_dir/temp/lfs_sys.img
data_dir=$cur_dir/temp/lfs_data_dir
data_img_file=$cur_dir/temp/lfs_data.img
qemu_out=$cur_dir/out/vexpressa9

# blk_size 1*4096=4096
# partition size = blk_size * blk_cnt = 4096 * 16384 = 64MB
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
./vendor/vexpress/board/vexpressa9-mk/script/mklfs -c $sys_dir -b $blk_size -r $page_size -p $page_size -s $sys_partition_size -i $sys_img_file
echo "Data partition info:"
echo "page_size      : "  $page_size "B"
echo "blk_size       : "  $blk_size  "B"
echo "blk_cnt        : "  $data_blk_cnt
echo "partition_size : "  $(($data_partition_size/1024)) "KB"
./vendor/vexpress/board/vexpressa9-mk/script/mklfs -c $data_dir -b $blk_size -r $page_size -p $page_size -s $data_partition_size -i $data_img_file

rm -rf $qemu_out
mkdir -p $qemu_out
#step3: create whole file system
dd if=/dev/zero of=$qemu_out/flash_kv.img count=32768
cat $cur_dir/temp/lfs_data.img $qemu_out/flash_kv.img > $qemu_out/flash1.img
dd if=$cur_dir/temp/lfs_sys.img of=$qemu_out/flash2.img count=131072
rm -rf $cur_dir/temp
