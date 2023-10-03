#!/bin/sh

CurDir=$(pwd)
BuildDir="$CurDir/bin"

IncludeDirectories="-I $CurDir/kengine"
CommonCompilerFlags="-O2 -g -ggdb -msse4.2 -maes -Wall -Wno-unused-function -Wno-missing-braces -Wno-switch -Wno-strict-aliasing $IncludeDirectories"
CommonLinkerFlags="-pthread -lm"
InternalCompilerFlags="-DKENGINE_LINUX -DKENGINE_INTERNAL -DKENGINE_SLOW"

[ -d $BuildDir ] || mkdir -p $BuildDir

pushd $BuildDir > /dev/null

# gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine_processor/haversine_test.c" -o haversine_test $CommonLinkerFlags
# ./haversine_test

# gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine_processor/haversine_generator.c" -o haversine_generator $CommonLinkerFlags
# ./haversine_generator cluster 1127504674 10000000

# gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine_processor/haversine_processor.c" -o haversine_processor $CommonLinkerFlags
# ./haversine_processor output.json

# gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine_processor/performance_metrics.c" -o performance_metrics $CommonLinkerFlags
# ./performance_metrics 1000

# gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine_processor/read_overhead_test.c" -o read_overhead_test $CommonLinkerFlags

# gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine_processor/fault_counter.c" -o fault_counter $CommonLinkerFlags
# ./fault_counter backward 32

<<<<<<< Updated upstream
gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine_processor/fault_counter_test.c" -o fault_counter_test $CommonLinkerFlags
=======
# gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine_processor/fault_counter_test.c" -o fault_counter_test $CommonLinkerFlags
# ./fault_counter_test

gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine_processor/pointer_anatomy.c" -o pointer_anatomy $CommonLinkerFlags
./pointer_anatomy
>>>>>>> Stashed changes

popd > /dev/null
