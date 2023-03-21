@echo off

if not defined DevEnvDir (
	call C:\"Program Files (x86)"\"Microsoft Visual Studio"\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat x64
)

set CommonCompilerFlags=-nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -FC
set InternalCompilerFlags=-DKENGINE_INTERNAL=1 -DKENGINE_SLOW=1
set IncludeDirectories=-I..\kengine
set CommonLinkerFlags=-incremental:no -opt:ref

IF NOT EXIST ..\bin mkdir ..\bin
pushd ..\bin

cl %CommonCompilerFlags% %InternalCompilerFlags% -MTd -Od -Z7 %IncludeDirectories% ..\preprocessor_hello\preprocessor_hello.c /Fe:preprocessor_hello.exe /link %CommonLinkerFlags%

del *.obj

pushd ..\preprocessor_hello\
..\bin\preprocessor_hello.exe hello_types.h
popd

popd