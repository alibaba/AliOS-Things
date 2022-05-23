#!/usr/bin/env bash

while read rows
do
    VAR=$rows
    if [[ $VAR =~ "BRANCH" ]]; then
        VAR_BRANCH=$(echo $VAR | grep -Eo "[a-zA-Z0-9./\-\_-]*" | tail -1)
        echo "The target branch:"$VAR_BRANCH
    elif [[ $VAR =~ "PATH" ]]; then
        VAR_PATH=$(echo $VAR | grep -Eo "[a-zA-Z0-9./\-\_-]*" | tail -1)
        cd $VAR_PATH
        echo "This path has been switched to:"$(pwd)
        git fetch
        git checkout $VAR_BRANCH
        git submodule update --init --recursive
        git pull origin $VAR_BRANCH
        cd -
    fi
done < $ADF_PATH/tools/ut/ut_branch.conf