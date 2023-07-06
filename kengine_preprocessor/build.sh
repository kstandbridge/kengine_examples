#!/bin/sh

CurDir=$(pwd)
BuildDir="$CurDir/bin"

IncludeDirectories="-I $CurDir/kengine"
CommonCompilerFlags="-Og -g -ggdb -msse4.2 -maes -Wall -Wno-unused-function -Wno-missing-braces $IncludeDirectories"
CommonLinkerFlags="-pthread"
InternalCompilerFlags="-DKENGINE_LINUX -DKENGINE_INTERNAL -DKENGINE_SLOW"

[ -d $BuildDir ] || mkdir -p $BuildDir

pushd $BuildDir > /dev/null

gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/kengine_preprocessor/kengine_preprocessor.c" -o kengine_preprocessor $CommonLinkerFlags

pushd $CurDir/kengine/kengine > /dev/null
../../bin/kengine_preprocessor kengine_math.h kengine_types.h > kengine_generated.h
popd > /dev/null

popd > /dev/null