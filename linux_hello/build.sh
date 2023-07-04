#!/bin/sh


CurDir=$(pwd)
BuildDir="$CurDir/bin"

IncludeDirectories=-I $CurDir/kengine
CommonCompilerFlags=-O2 -g -ggdb -msse4.2 -maes $IncludeDirectories
CommonLinkerFlags=-pthread
InternalCompilerFlags=-DKENGINE_LINUX -DKENGINE_INTERNAL -DKENGINE_SLOW

[ -d $BuildDir ] || mkdir -p $BuildDir

pushd $BuildDir > /dev/null

gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/linux_hello/linux_hello.c" -o linux_hello $CommonLinkerFlags

popd > /dev/null