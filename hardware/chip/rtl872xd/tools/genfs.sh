#!/usr/bin/env bash

TOOLS_FOLDER=$(dirname "$0")

if [ "$(uname)" = "Darwin" ]; then
	${TOOLS_FOLDER}/mklittlefs_osx -c ${TOOLS_FOLDER}/../prebuild/data/ -d 5 -b 4096 -p 256 -s 684032 ${TOOLS_FOLDER}/../prebuild/littlefs.bin
elif [ "$(expr substr $(uname -s) 1 5)" = "Linux" ]; then
	${TOOLS_FOLDER}/mklittlefs_linux -c ${TOOLS_FOLDER}/../prebuild/data/ -d 5 -b 4096 -p 256 -s 684032 ${TOOLS_FOLDER}/../prebuild/littlefs.bin
elif [ "$(expr substr $(uname -s) 1 10)" = "MINGW32_NT" ]; then
	echo
fi

