#!/bin/sh

CurDir=$(pwd)
BuildDir="$CurDir/bin"

IncludeDirectories="-I $CurDir/kengine -I $CurDir/handmade"
CommonCompilerFlags="-Og -g -ggdb -msse4.2 -maes -lm -Wall -Wno-unused-function -Wno-missing-braces $IncludeDirectories"
CommonLinkerFlags="-pthread"
InternalCompilerFlags="-DKENGINE_INTERNAL -DKENGINE_SLOW"

[ -d $BuildDir ] || mkdir -p $BuildDir

pushd $BuildDir > /dev/null

echo WAITING FOR PDB > lock.tmp

# Build library
HandmadeLib="handmade.so"
gcc -shared $CommonCompilerFlags $InternalCompilerFlags "$CurDir/handmade/handmade.c" -o build_$HandmadeLib $CommonLinkerFlags
[ -f $HandmadeLib ] && rm $HandmadeLib
mv build_$HandmadeLib $HandmadeLib

# Build executable
gcc $CommonCompilerFlags $InternalCompilerFlags -DKENGINE_LINUX "$CurDir/handmade/platform_handmade.c" -o platform_handmade $CommonLinkerFlags -lX11 -ldl

rm lock.tmp

popd > /dev/null