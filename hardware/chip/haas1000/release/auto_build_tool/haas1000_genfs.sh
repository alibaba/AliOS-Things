#!/usr/bin/env bash

if [ "$(uname)" = "Darwin" ]; then
	./mklittlefs_osx -c ../../prebuild/data/ -d 5 -b 4096 -p 1198 -s 4907008 ../../prebuild/littlefs.bin
elif [ "$(expr substr $(uname -s) 1 5)" = "Linux" ]; then
	./mklittlefs_linux -c ../../prebuild/data/ -d 5 -b 4096 -p 1198 -s 4907008 ../../prebuild/littlefs.bin
elif [ "$(expr substr $(uname -s) 1 10)" = "MINGW32_NT" ]; then
	echo
fi

