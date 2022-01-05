#! /usr/bin/env bash

pushd $(dirname $0)
. ./imagename

# see https://stackoverflow.com/a/2924755 for switching boldface on/off
bold=$(tput bold)
normal=$(tput sgr0)

echo "${bold}**** Creating build-env image ****${normal}"
docker build -f Dockerfile.build-env -t $USER/$IMAGE:build-env .

echo "${bold}**** Creating develop image ****${normal}"
docker build -f Dockerfile.develop -t $USER/$IMAGE:develop ..

popd
