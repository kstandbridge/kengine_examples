@echo off

SET CurDir=%cd%
SET BuildDir=%CurDir%\bin

set IncludeDirectories=-I%CurDir%\kengine
set CommonCompilerFlags=-Od -FC -Z7 -nologo %IncludeDirectories%
set CommonLinkerFlags=-incremental:no -opt:ref
set InternalCompilerFlags=-DKENGINE_WIN32 -DKENGINE_INTERNAL -DKENGINE_SLOW

IF NOT EXIST %BuildDir% mkdir %BuildDir%

pushd %BuildDir%

@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\haversine_test.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\haversine_generator.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\haversine_processor.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\performance_metrics.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\fault_counter.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\fault_counter_test.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\pointer_anatomy.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\overfaulting_pages.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\circular_buffer.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\change_detection.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\sparse_memory.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\read_overhead_test.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\asm_repetition_tester.c /link %CommonLinkerFlags%

@REM nasm -f win64 -o win32_nop_loop.obj %CurDir%\computer_enhance\haversine\win32_nop_loop.asm
@REM lib -nologo win32_nop_loop.obj
@REM nasm -f win64 -o win32_multinoop_loops.obj %CurDir%\computer_enhance\haversine\win32_multinoop_loops.asm
@REM lib -nologo win32_multinoop_loops.obj
nasm -f win64 -o win32_conditional_nop_loops.obj %CurDir%\computer_enhance\haversine\win32_conditional_nop_loops.asm
lib -nologo win32_conditional_nop_loops.obj

cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\conditional_nop_loops_tester.c /link %CommonLinkerFlags%

popd
