#!/bin/sh

CurDir=$(pwd)
BuildDir="$CurDir/bin"

IncludeDirectories="-I $CurDir/kengine"
CommonCompilerFlags="-O2 -g -ggdb -msse4.2 -maes -Wall -Wno-unused-function -Wno-missing-braces -Wno-switch -Wno-strict-aliasing $IncludeDirectories"
CommonLinkerFlags="-pthread -lm"
InternalCompilerFlags="-DKENGINE_LINUX -DKENGINE_INTERNAL -DKENGINE_SLOW"

[ -d $BuildDir ] || mkdir -p $BuildDir

pushd $BuildDir > /dev/null

# gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine/haversine_test.c" -o haversine_test $CommonLinkerFlags
# gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine/haversine_generator.c" -o haversine_generator $CommonLinkerFlags
# gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine/haversine_processor.c" -o haversine_processor $CommonLinkerFlags
# gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine/performance_metrics.c" -o performance_metrics $CommonLinkerFlags
# gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine/read_overhead_test.c" -o read_overhead_test $CommonLinkerFlags
# gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine/fault_counter.c" -o fault_counter $CommonLinkerFlags
# gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine/fault_counter_test.c" -o fault_counter_test $CommonLinkerFlags
# gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine/pointer_anatomy.c" -o pointer_anatomy $CommonLinkerFlags
# gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine/overfaulting_pages.c" -o overfaulting_pages $CommonLinkerFlags
# gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine/circular_buffer.c" -o circular_buffer $CommonLinkerFlags
# gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine/osread_revisited.c" -o osread_revisited $CommonLinkerFlags

gcc $CommonCompilerFlags $InternalCompilerFlags "$CurDir/computer_enhance/haversine/osread_sum.c" -o osread_sum $CommonLinkerFlags

popd > /dev/null
