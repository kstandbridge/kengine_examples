@echo off

SET CurDir=%cd%
SET BuildDir=%CurDir%\bin

set IncludeDirectories=-I%CurDir%\kengine
set CommonCompilerFlags=-Od -FC -Z7 -nologo %IncludeDirectories%
set CommonLinkerFlags=-incremental:no -opt:ref
set InternalCompilerFlags=-DKENGINE_WIN32 -DKENGINE_INTERNAL -DKENGINE_SLOW

IF NOT EXIST %BuildDir% mkdir %BuildDir%

pushd %BuildDir%

cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\kengine_preprocessor\kengine_preprocessor.c /link %CommonLinkerFlags%

pushd %CurDir%\kengine\kengine
..\..\bin\kengine_preprocessor kengine_math.h kengine_types.h > kengine_generated.h
popd

popd

