@echo off

SET CurDir=%cd%
SET BuildDir=%CurDir%\bin

set IncludeDirectories=-I%CurDir%\kengine
set CommonCompilerFlags=-Od -FC -Z7 -nologo %IncludeDirectories%
set CommonLinkerFlags=-incremental:no -opt:ref
set InternalCompilerFlags=-DKENGINE_INTERNAL -DKENGINE_SLOW

IF NOT EXIST %BuildDir% mkdir %BuildDir%

pushd %BuildDir%

del *.pdb > NUL 2> NUL
echo WAITING FOR PDB > lock.tmp

REM Build library
cl %CommonCompilerFlags% %InternalCompilerFlags% %CurDir%\handmade\handmade.c /LD /link %CommonLinkerFlags% -PDB:handmade_%random%.pdb /EXPORT:AppUpdateAndRender /EXPORT:AppGetSoundSamples

REM Build executable
cl %CommonCompilerFlags% %InternalCompilerFlags% -DKENGINE_WIN32 %CurDir%\handmade\platform_handmade.c /link %CommonLinkerFlags% user32.lib gdi32.lib winmm.lib

del /q *.obj
del lock.tmp

popd

