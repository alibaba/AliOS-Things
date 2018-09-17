#!/usr/bin/env bash

# be-cli install only support Darwin & Ubuntu
# usb2serial driver install only support Darwin
# 
# we will install node & npm firstly
# if it is Darwin and use esp32 platform,we will 
# help to install USB2Serial driver.the other os
# and chip will ignore

OS=`uname -s`

# set default platform
PLATFORM='notsupport'

if [ "$OS" != "Darwin" ] && [ "$OS" != "Linux" ]; then
    echo "error: unsupported OS $OS"
    exit 1
fi

# Under linux node and npm installed by default,so the following
# brew command will only be executed under MacOS which dose not
# install node and npm

echo "-----install node be-cli on $OS------"
if [ "`which node`" = "" ];then
    brew install node > /dev/null
fi
if [ "`which npm`" = "" ];then
    brew install npm > /dev/null
fi

# Under linux node and npm install by default,but the directory
# is protected by root&sudo for some reason,so here to change it

if [ "$OS" = "Linux" ]; then
    echo "Linux platform, change rw access to node directory"
    sudo chown -R $(whoami) ~/.npm
    if [ "`which be`" = "" ];then
        sudo chown -R $(whoami) /usr/lib/node_modules
    fi
    sudo chown -R $(whoami) /usr/local/bin
fi
curVer=`node -v`
baseVer="6.4.0"
cutVer=`echo ${curVer#*v}`
echo $cutVer

# syntax compatibility under Mac & ubuntu

if [ "$OS" = "Darwin" ];then
    if [[ $cutVer < $baseVer ]];then
        echo "Node version lower under Darwin, please update to v6.4.0 and newer manually"
    fi
elif [ "$OS" = "Linux" ]; then
    if [ $cutVer \< $baseVer ];then
        echo "Node version lower under Linux, please update to v6.4.0 and newer manually"
    fi
fi

if [ "`which tnpm`" != "" ];then
    echo "Use inner tnpm install be-cli"
    tnpm install be-cli@0.1.32 -g -P
else
    echo "Use cnpm install be-cli"
    npm install be-cli@0.1.32 -g -P --registry=https://registry.npm.taobao.org
fi

# find whether .config file exsits,will according the parameter
# to install USB2Serial Driver. Now only support MacOS

if [ ! -f ".config.h" ];then
    echo ".config dose not exsist,ignore usb2serial driver install"
    exit 0
else
    for line in `cat .config.h`
	do
            PLATFORM=`echo ${line#*=}`
            if [ "$PLATFORM" = "esp32devkitc" ] || [ "$PLATFORM" = "developerkit" ];then
	        break
            fi
        done
fi

if [ "$OS" = "Darwin" ] && [ "$PLATFORM" = "esp32devkitc" ]; then 
    # If driver installed,do nothing
    echo "It's esp32"
    if [ ! -d "/Library/Extensions/SiLabsUSBDriver.kext" ];then
        echo "-----download & install usb2serial driver -----"
        curl -O https://www.silabs.com/documents/public/software/Mac_OSX_VCP_Driver.zip -m 10
        sleep 2
        unzip ./Mac_OSX_VCP_Driver.zip -d ./tools/
        hdiutil convert ./tools/SiLabsUSBDriverDisk.dmg -format UDTO -o bar.cdr
        sleep 1
        hdiutil attach ./bar.cdr
        sudo installer -store -pkg /Volumes/Silicon\ Labs\ VCP\ Driver\ Install\ Disk/Silicon\ Labs\ VCP\ Driver.pkg -target /
        hdiutil detach /Volumes/Silicon\ Labs\ VCP\ Driver\ Install\ Disk/
    fi

    if [ ! -f "./Mac_OSX_VCP_Driver.zip" ];then
        rm -rf ./Mac_OSX_VCP_Driver.zip
    fi
    if [ ! -f "./tools/SiLabsUSBDriverDisk.dmg" ];then
        rm -rf ./tools/SiLabsUSBDriverDisk.dmg
    fi
    rm -rf ./bar.cdr ./Mac_OSX_VCP_Driver.zip ./tools/SiLabsUSBDriverDisk.dmg 
elif [ "$OS" = "Darwin" ] && [ "$PLATFORM" = "developerkit" ]; then
    echo "MacOS & stm32 ignore usb2serial driver install"
else
  echo "Not suport OS or platform, ignore usb2serial driver install"
fi

echo "done"
