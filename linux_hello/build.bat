@echo off

SET CurDir=%cd%
SET BuildDir=%CurDir%\bin

set IncludeDirectories=-I%CurDir%\kengine
set CommonCompilerFlags=-Od -FC -Z7 -nologo %IncludeDirectories%
set CommonLinkerFlags=-incremental:no -opt:ref

pushd %BuildDir%

cl %CommonCompilerFlags% %CurDir%\linux_hello\linux_hello.c /link %CommonLinkerFlags%

popd

