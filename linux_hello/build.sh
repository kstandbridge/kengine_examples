#!/bin/sh


curDir=$(pwd)
buildDir="$curDir/../bin"

[ -d $buildDir ] || mkdir -p $buildDir

pushd $buildDir > /dev/null

gcc -O2 -g -ggdb "$curDir/linux_hello.c" -o linux_hello

popd > /dev/null