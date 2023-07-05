@echo off

SET CurDir=%cd%
SET BuildDir=%CurDir%\bin

set IncludeDirectories=-I%CurDir%\kengine
set CommonCompilerFlags=-Od -FC -Z7 -nologo %IncludeDirectories%
set CommonLinkerFlags=-incremental:no -opt:ref
set InternalCompilerFlags=-DKENGINE_WIN32 -DKENGINE_INTERNAL -DKENGINE_SLOW

IF NOT EXIST %BuildDir% mkdir %BuildDir%

pushd %BuildDir%

cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\kengine_test\kengine_test.c /link %CommonLinkerFlags%

popd

