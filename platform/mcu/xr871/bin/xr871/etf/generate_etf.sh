#!/bin/bash

# copy common bin
cp ./../boot.bin ./boot.bin
cp ./../wlan_bl.bin ./wlan_bl.bin
cp ./../wlan_sdd.bin ./wlan_sdd.bin

# creating image
if [ x'uname'x = xCygwinx ]; then
  ./../../../tools/mkimage.exe -c image_etf.cfg -o image_etf.img
else
  ./../../../tools/mkimage -c image_etf.cfg -o image_etf.img
fi

