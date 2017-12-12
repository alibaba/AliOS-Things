#!/bin/bash

cur_dir=`pwd`
if [ "`which apt-get`" != "" ];then
    sudo apt-get update > /dev/null
    pkg_mgr=apt-get
elif [ "`which yum`" != "" ];then
    pkg_mgr=yum
else
    echo "error: unsupport linux distrubution"
    exit 1
fi

#install testbed files
if [ -d ~/testbed ];then
    echo -n "folder ~/testbed already exist, overwrite?(y/n):"
    read -n 1 overwrite
    if [ "${overwrite}" != "y" ] && [ "${overwrite}" != "Y" ]; then
        exit 0
    fi
    echo ""
    rm -rf ~/testbed
fi
mkdir ~/testbed

#install dependent packages
echo "install dependent packages ..."
result=0
sudo ${pkg_mgr} -yq install vim git python
result=$((result+$?))
sudo ${pkg_mgr} -yq install python-pip screen
result=$((result+$?))
sudo pip install -q pyserial
result=$((result+$?))
if [ ${result} -ne 0 ];then
    echo "\nerror: install dependent package failed\n"
    exit 1
fi
sudo usermod -a -G dialout $(whoami)
echo "install dependent packages ... done"

#install testbed files
echo "install testbed scripts ..."
git clone -q https://github.com/alibaba/AliOS-Things.git aos
if [ $? -ne 0 ];then
    echo "error: git clone files failed"
    exit 1
fi
cd aos/tools/testbed
cp *.py ~/testbed/

cd utilities
support_log_to_file=`screen -V | grep "\-L \[file\]"`
if [ "${support_log_to_file}" != "" ];then
    sed -i 's/screen -dmL/screen -L client_log.txt -dm/g' tbclient_start
    sed -i 's/screen -dmL/screen -L server_log.txt -dm/g' tbserver_start
fi
mv tbclient_start udclient_start
mv tbclient_stop udclient_stop
mv tbserver_start udserver_start
mv tbserver_stop udserver_stop
sudo cp ud*_st* /usr/local/bin/
sudo chmod +x /usr/local/bin/ud*_st*
sudo cp 98-usb-serial.rules /etc/udev/rules.d/
rm -rf ${cur_dir}/aos
echo "install testbed scripts ... done"
echo "installation finished. need reboot to make it effective"
echo -n "reboot now?(y/n):"
read -n 1 reboot
if [ "${reboot}" != "y" ] && [ "${reboot}" != "Y" ]; then
    exit 0
fi
sudo reboot
