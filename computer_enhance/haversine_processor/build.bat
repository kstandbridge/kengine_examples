@echo off

SET CurDir=%cd%
SET BuildDir=%CurDir%\bin

set IncludeDirectories=-I%CurDir%\kengine
set CommonCompilerFlags=-O2 -FC -Z7 -nologo %IncludeDirectories%
set CommonLinkerFlags=-incremental:no -opt:ref
set InternalCompilerFlags=-DKENGINE_WIN32 -DKENGINE_INTERNAL -DKENGINE_SLOW

IF NOT EXIST %BuildDir% mkdir %BuildDir%

pushd %BuildDir%

REM cl %CommonCompilerFlags% %InternalCompilerFlags%\n %CurDir%\computer_enhance\haversine_processor\haversine_test.c /link %CommonLinkerFlags%
REM haversine_test.exe

REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine_processor\haversine_generator.c /link %CommonLinkerFlags%
REM haversine_generator.exe uniform 1127504674 10000000

REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine_processor\haversine_processor.c /link %CommonLinkerFlags%
REM haversine_processor.exe output.json

REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine_processor\performance_metrics.c /link %CommonLinkerFlags%
REM performance_metrics.exe 1000

REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine_processor\read_overhead_test.c /link %CommonLinkerFlags%

REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine_processor\fault_counter.c /link %CommonLinkerFlags%
REM fault_counter.exe backward 32

REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine_processor\fault_counter_test.c /link %CommonLinkerFlags%

cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine_processor\pointer_anatomy.c /link %CommonLinkerFlags%

popd

