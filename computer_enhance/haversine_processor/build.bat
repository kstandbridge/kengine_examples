@echo off

SET CurDir=%cd%
SET BuildDir=%CurDir%\bin

set IncludeDirectories=-I%CurDir%\kengine
set CommonCompilerFlags=-Od -FC -Z7 -nologo %IncludeDirectories%
set CommonLinkerFlags=-incremental:no -opt:ref
set InternalCompilerFlags=-DKENGINE_WIN32 -DKENGINE_INTERNAL -DKENGINE_SLOW

IF NOT EXIST %BuildDir% mkdir %BuildDir%

pushd %BuildDir%

cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine_processor\haversine_test.c /link %CommonLinkerFlags%
haversine_test.exe

cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\computer_enhance\haversine_processor\haversine_generator.c /link %CommonLinkerFlags%
haversine_generator cluster 23453489 10

popd

