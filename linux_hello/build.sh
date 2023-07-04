#!/bin/sh


curDir=$(pwd)
buildDir="$curDir/bin"

[ -d $buildDir ] || mkdir -p $buildDir

pushd $buildDir > /dev/null

gcc -O2 -g -ggdb -msse4.2 -maes -I $curDir/kengine "$curDir/linux_hello/linux_hello.c" -o linux_hello

popd > /dev/null