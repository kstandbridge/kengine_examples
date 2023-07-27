#!/bin/sh

CurDir=$(pwd)
BuildDir="$CurDir/bin"

IncludeDirectories="-I $CurDir/kengine"
CommonCompilerFlags="-Og -g -ggdb -msse4.2 -maes -Wall -Wno-unused-function -Wno-missing-braces -Wno-switch -Wno-write-strings -Wno-strict-aliasing $IncludeDirectories"
CommonLinkerFlags="-pthread -lm"
InternalCompilerFlags="-DKENGINE_LINUX -DKENGINE_INTERNAL -DKENGINE_SLOW"

[ -d $BuildDir ] || mkdir -p $BuildDir

pushd $BuildDir > /dev/null

# g++ $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine_processor/haversine_test.cpp" -o haversine_test $CommonLinkerFlags
# ./haversine_test

# g++ $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine_processor/haversine_generator.cpp" -o haversine_generator $CommonLinkerFlags
# ./haversine_generator cluster 1127504674 1000000

g++ $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine_processor/haversine_processor.cpp" -o haversine_processor $CommonLinkerFlags
./haversine_processor output.json

# g++ $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine_processor/performance_metrics.cpp" -o performance_metrics $CommonLinkerFlags
# ./performance_metrics 1000


popd > /dev/null