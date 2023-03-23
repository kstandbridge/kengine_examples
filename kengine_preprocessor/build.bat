@echo off

if not defined DevEnvDir (
	call C:\"Program Files (x86)"\"Microsoft Visual Studio"\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat x64
)

set CommonCompilerFlags=-nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -FC
set IncludeDirectories=-I..\kengine
set CommonLinkerFlags=-incremental:no -opt:ref

IF NOT EXIST ..\bin mkdir ..\bin
pushd ..\bin

cl %CommonCompilerFlags% -MTd -Od -Z7 %IncludeDirectories% ..\kengine_preprocessor\kengine_preprocessor.c /Fe:kengine_preprocessor.exe /link %CommonLinkerFlags%

del *.obj

pushd ..\kengine\kengine
REM ..\..\bin\kengine_preprocessor.exe kengine_math.h
..\..\bin\kengine_preprocessor.exe kengine_math.h kengine_types.h > kengine_generated.h
popd

popd