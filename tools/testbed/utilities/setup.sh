#!/usr/bin/env bash

#install testbed related files
sudo apt-get update
sudo apt-get -y install python-pip git screen
sudo pip install pyserial
sudo usermod -a -G dialout $(whoami)

cwd=`pwd`
git clone https://github.com/espressif/esptool.git
cp esptool/esptool.py ../
rm -rf esptool

cd $cwd
sudo cp 98-usb-serial.rules /etc/udev/rules.d/
sudo cp tb*_st* /usr/local/bin/
sudo chmod +x /usr/local/bin/tb*_st*

