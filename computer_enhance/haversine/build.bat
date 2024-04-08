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
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\fault_counter.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\fault_counter_test.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\pointer_anatomy.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\overfaulting_pages.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\circular_buffer.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\change_detection.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\sparse_memory.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\read_overhead_test.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\asm_repetition_tester.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\jumping_alignment.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\read_unroll.c /link %CommonLinkerFlags%
@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\memory_bandwidth.c /link %CommonLinkerFlags%

@REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\conditional_nop_loops_tester.c /link %CommonLinkerFlags%
@REM nasm -f win64 -o win32_nop_loop.obj %CurDir%\computer_enhance\haversine\win32_nop_loop.asm
@REM lib -nologo win32_nop_loop.obj
@REM nasm -f win64 -o win32_multinoop_loops.obj %CurDir%\computer_enhance\haversine\win32_multinoop_loops.asm
@REM lib -nologo win32_multinoop_loops.obj
@REM nasm -f win64 -o win32_jumping_alignment.obj %CurDir%\computer_enhance\haversine\win32_jumping_alignment.asm
@REM lib -nologo win32_jumping_alignment.obj
@REM nasm -f win64 -o win32_rat.obj %CurDir%\computer_enhance\haversine\win32_rat.asm
@REM lib -nologo win32_rat.obj
@REM nasm -f win64 -o win32_read_unroll.obj %CurDir%\computer_enhance\haversine\win32_read_unroll.asm
@REM lib -nologo win32_read_unroll.obj
@REM nasm -f win64 -o win32_memory_bandwidth.obj %CurDir%\computer_enhance\haversine\win32_memory_bandwidth.asm
@REM lib -nologo win32_memory_bandwidth.obj
nasm -f win64 -o win32_npt_cache_test.obj %CurDir%\computer_enhance\haversine\win32_npt_cache_test.asm
lib -nologo win32_npt_cache_test.obj

cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine\npt_cache_test.c /link %CommonLinkerFlags%

popd
