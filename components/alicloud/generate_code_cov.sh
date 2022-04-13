#!/bin/bash

cd `dirname $0`

UT_BUILD_DIR=ut_build

echo '--------- generate initial info ---------------- '
lcov -d $UT_BUILD_DIR -z
lcov -d $UT_BUILD_DIR -b . --no-external --initial -c -o initCoverage.info

echo '--------- run test ---------------- '
cd $UT_BUILD_DIR
ctest --verbose
cd ..

echo '--------- generate post info ---------------- '
lcov -d $UT_BUILD_DIR -b . --no-external -c -o testCoverage.info

echo '--------- generate html report ---------------- '
genhtml -o coverageReport --prefix=`pwd` initCoverage.info testCoverage.info

echo 'check report: ' `pwd`/coverageReport/index.html

echo ' ------remove tmp file ------'

rm initCoverage.info
rm testCoverage.info

