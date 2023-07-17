#!/bin/sh

CurDir=$(pwd)
BuildDir="$CurDir/bin"

IncludeDirectories="-I $CurDir/kengine"
CommonCompilerFlags="-Og -g -ggdb -msse4.2 -maes -Wall -Wno-unused-function -Wno-missing-braces -Wno-switch $IncludeDirectories"
CommonLinkerFlags="-pthread -lm"
InternalCompilerFlags="-DKENGINE_LINUX -DKENGINE_INTERNAL -DKENGINE_SLOW"

[ -d $BuildDir ] || mkdir -p $BuildDir

pushd $BuildDir > /dev/null

gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine_processor/haversine_test.c" -o haversine_test $CommonLinkerFlags
./haversine_test

gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine_processor/haversine_generator.c" -o haversine_generator $CommonLinkerFlags
./haversine_generator cluster 23453489 1000

popd > /dev/null