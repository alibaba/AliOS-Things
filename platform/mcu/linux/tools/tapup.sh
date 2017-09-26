#!/bin/sh
sudo openvpn --mktun --dev $1 --user `id -un`
sudo ifconfig $1 192.168.0.1 netmask 255.255.255.0
