#!/bin/bash

cd `dirname $0`
echo '-------build function test----------'

MAKE=make
if command -v python > /dev/null ; then
  MAKE="make -j $(python -c 'import multiprocessing as mp; print(int(mp.cpu_count()))')"
fi

echo $MAKE

FT_BUILD_DIR=ft_build
rm -rf $FT_BUILD_DIR
mkdir $FT_BUILD_DIR
cd $FT_BUILD_DIR
cmake -DBUILD_FUNCTION_TESTS=ON -DBUILD_UNIT_TESTS=OFF ..
$MAKE cdn core cs ecs rds slb vpc cdn_ft core_ft cs_ft ecs_ft nlp_ft rds_ft slb_ft vpc_ft

echo '------- run function test -----------'

ctest --verbose
