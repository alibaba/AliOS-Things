#!/bin/bash

make gcov=1
./ali_crypto_test
sudo lcov -c -b . -d ./mbed -d ./sw -o test.info
genhtml test.info -o yos_test_report

