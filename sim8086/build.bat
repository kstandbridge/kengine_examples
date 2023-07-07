@echo off

SET CurDir=%cd%
SET BuildDir=%CurDir%\bin

set IncludeDirectories=-I%CurDir%\kengine
set CommonCompilerFlags=-Od -FC -Z7 -nologo %IncludeDirectories%
set CommonLinkerFlags=-incremental:no -opt:ref
set InternalCompilerFlags=-DKENGINE_WIN32 -DKENGINE_INTERNAL -DKENGINE_SLOW

IF NOT EXIST %BuildDir% mkdir %BuildDir%

pushd %BuildDir%

cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\sim8086\sim8086_console.c /link %CommonLinkerFlags%

call nasm %CurDir%\sim8086\test.asm -o test

sim8086_console.exe -dump -clocks test

cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\sim8086\sim8086_test.c /link %CommonLinkerFlags%
sim8086_test.exe

popd

