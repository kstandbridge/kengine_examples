#!/bin/sh

CurDir=$(pwd)
BuildDir="$CurDir/bin"

IncludeDirectories="-I $CurDir/kengine"
CommonCompilerFlags="-Og -g -ggdb -msse4.2 -maes -Wall -Wno-unused-function -Wno-missing-braces -Wno-switch $IncludeDirectories"
CommonLinkerFlags="-pthread"
InternalCompilerFlags="-DKENGINE_LINUX -DKENGINE_INTERNAL -DKENGINE_SLOW"

[ -d $BuildDir ] || mkdir -p $BuildDir

pushd $BuildDir > /dev/null

gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/sim8086/sim8086_console.c" -o sim8086_console $CommonLinkerFlags

nasm $CurDir/sim8086/test.asm -o test
./sim8086_console -dump -clocks test

gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/sim8086/sim8086_test.c" -o sim8086_test $CommonLinkerFlags
./sim8086_test

popd > /dev/null