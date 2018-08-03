#!/usr/bin/env bash
OS=`uname -s`
if [ "$OS" == "Linux" ];then
echo "please input your usb port (default is ttyUSB0):" 
read USBTTY
fi

if [ "$USBTTY" == "" ];then
	USBTTY=ttyUSB0
fi

DIR=$1
if [ "$DIR" == "" ];then
DIR=./spiffs.bin
fi
echo "image dirctory is $DIR"
if [ "$OS" == "Darwin" ];then
esptool.py --chip esp32 --port /dev/tty.SLAB_USBtoUART write_flash -z -p 0x315000 $DIR

elif [ "$OS" == "Linux" ];then
esptool.py --chip esp32 --port /dev/$USBTTY write_flash -z -p 0x315000 $DIR 

else
echo "Not support this os,please refer esp website to download spiffs.bin to 0x315000"
fi
