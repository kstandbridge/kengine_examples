@echo off

SET CurDir=%cd%
SET BuildDir=%CurDir%\bin

set IncludeDirectories=-I%CurDir%\kengine
set CommonCompilerFlags=-O2 -FC -Z7 -nologo %IncludeDirectories%
set CommonLinkerFlags=-incremental:no -opt:ref
set InternalCompilerFlags=-DKENGINE_WIN32 -DKENGINE_INTERNAL -DKENGINE_SLOW

IF NOT EXIST %BuildDir% mkdir %BuildDir%

pushd %BuildDir%

@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\haversine_test.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\haversine_generator.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\haversine_processor.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\performance_metrics.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\read_overhead_test.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\fault_counter.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\fault_counter_test.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\pointer_anatomy.c /link %CommonLinkerFlags%
cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\overfaulting_pages.c /link %CommonLinkerFlags%

popd

