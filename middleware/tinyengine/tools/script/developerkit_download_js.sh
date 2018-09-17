#!/usr/bin/env bash
OS=`uname -s`
ARCH=`arch`

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
../../../build/cmd/osx/st-flash --reset write $DIR 0x080E0000
elif [ "$OS" == "Linux" ];then
if ["$ARCH" == "i686"];then
../../../build/cmd/osx/linux32/st-flash --reset write $DIR 0x080E0000
else
../../../build/cmd/osx/linux64/st-flash --reset write $DIR 0x080E0000
fi
else
echo "Not support this os,please refer esp website to download spiffs.bin to 0x080E0000"
fi
