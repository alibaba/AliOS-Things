#!/usr/bin/env bash

if [ "$(uname)" == "Darwin" ];then
    MYOS="Darwin"
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ];then
    MYOS="Linux"
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ];then
    MYOS="Win32"
fi

if [ -z $(which curl) ];then
    echo "can not find curl. please install it"
    exit 1
fi

if [ $MYOS == "Darwin" ]; then
    curl https://repo.anaconda.com/miniconda/Miniconda3-latest-MacOSX-x86_64.sh -o ~/miniconda.sh
elif [ $MYOS == "Linux" ]; then
    curl https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh -o ~/miniconda.sh
else
    echo "not support $MYOS"
    exit 1
fi

bash ~/miniconda.sh -b -f -p $HOME/miniconda3
if [[ $SHELL = *zsh ]]
then
    $HOME/miniconda3/bin/conda init zsh
else
    $HOME/miniconda3/bin/conda init
fi
$HOME/miniconda3/bin/conda config --set auto_activate_base false

echo "start a new terminal  to install aos-tools."

exit 0

