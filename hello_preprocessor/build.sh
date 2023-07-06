#!/bin/sh

CurDir=$(pwd)
BuildDir="$CurDir/bin"

IncludeDirectories="-I $CurDir/kengine"
CommonCompilerFlags="-Og -g -ggdb -msse4.2 -maes -Wall -Wno-unused-function -Wno-missing-braces $IncludeDirectories"
CommonLinkerFlags="-pthread"
InternalCompilerFlags="-DKENGINE_LINUX -DKENGINE_INTERNAL -DKENGINE_SLOW"

[ -d $BuildDir ] || mkdir -p $BuildDir

pushd $BuildDir > /dev/null

gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/hello_preprocessor/hello_preprocessor.c" -o hello_preprocessor $CommonLinkerFlags

pushd $CurDir/hello_preprocessor > /dev/null
../bin/hello_preprocessor hello_types.h
popd > /dev/null

popd > /dev/null