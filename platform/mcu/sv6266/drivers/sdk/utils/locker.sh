#!/bin/sh

while ! mkdir $1.lock 2>/dev/null; 
do
    #echo "Myscript is already running." >&2
    sleep 0.001s
done
