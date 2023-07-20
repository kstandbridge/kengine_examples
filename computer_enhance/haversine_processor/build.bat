@echo off

SET CurDir=%cd%
SET BuildDir=%CurDir%\bin

set IncludeDirectories=-I%CurDir%\kengine
set CommonCompilerFlags=-Od -FC -Z7 -nologo %IncludeDirectories%
set CommonLinkerFlags=-incremental:no -opt:ref
set InternalCompilerFlags=-DKENGINE_WIN32 -DKENGINE_INTERNAL -DKENGINE_SLOW

IF NOT EXIST %BuildDir% mkdir %BuildDir%

pushd %BuildDir%

REM cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine_processor\haversine_test.c /link %CommonLinkerFlags%
REM haversine_test.exe

cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine_processor\haversine_generator.c /link %CommonLinkerFlags%
haversine_generator.exe uniform 57389 10000

cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine_processor\haversine_processor.c /link %CommonLinkerFlags%
haversine_processor.exe output.json output.f32

popd

