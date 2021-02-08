#!/bin/bash

cd `dirname $0`
echo '-------build unit test----------'

echo 'start a test http server'
NODE=`which nodejs`

if [ "$NODE" ]
then
  echo ''
else
NODE=`which node`
fi

echo 'node binary path: ' $NODE

server=`ps -ef | grep http_test_server | grep -v grep`
echo "check server: " $server
if [ "$server" ]
then
  echo "server is on"
else
  echo "server is off, start it"
  cd test/httpserver
  npm i
  nohup $NODE http_test_server.js &
  cd -
fi

MAKE=make
if command -v python > /dev/null ; then
  MAKE="make -j $(python -c 'import multiprocessing as mp; print(int(mp.cpu_count()))')"
fi

echo $MAKE

UT_BUILD_DIR=ut_build
rm -rf $UT_BUILD_DIR
mkdir $UT_BUILD_DIR
cd $UT_BUILD_DIR
cmake -DBUILD_FUNCTION_TESTS=OFF -DBUILD_UNIT_TESTS=ON -DENABLE_COVERAGE=ON ..
$MAKE core_ut

echo '------- run unit test -----------'

ctest --verbose
