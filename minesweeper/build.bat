@echo off

if not defined DevEnvDir (
	call C:\"Program Files (x86)"\"Microsoft Visual Studio"\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat x64
)

set CommonCompilerFlags=-nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -FC
set InternalCompilerFlags=-DKENGINE_INTERNAL=1 -DKENGINE_SLOW=1
set IncludeDirectories=-I..\kengine
set CommonLinkerFlags=-incremental:no -opt:ref


if "%~1"=="shaders" (

	pushd ..\kengine\kengine
	fxc.exe /nologo /T vs_4_0_level_9_0 /E vs_main /O3 /WX /Zpc /Ges /Fh kengine_vertex_shader_generated.h /Vn VertexShaderData /Qstrip_reflect /Qstrip_debug /Qstrip_priv kengine_shaders.hlsl
	fxc.exe /nologo /T ps_4_0_level_9_0 /E ps_glyph_main /O3 /WX /Zpc /Ges /Fh kengine_glyph_pixel_shader_generated.h /Vn GlyphPixelShaderData /Qstrip_reflect /Qstrip_debug /Qstrip_priv kengine_shaders.hlsl
	fxc.exe /nologo /T ps_4_0_level_9_0 /E ps_sprite_main /O3 /WX /Zpc /Ges /Fh kengine_sprite_pixel_shader_generated.h /Vn SpritePixelShaderData /Qstrip_reflect /Qstrip_debug /Qstrip_priv kengine_shaders.hlsl
	fxc.exe /nologo /T ps_4_0_level_9_0 /E ps_rect_main /O3 /WX /Zpc /Ges /Fh kengine_rect_pixel_shader_generated.h /Vn RectPixelShaderData /Qstrip_reflect /Qstrip_debug /Qstrip_priv kengine_shaders.hlsl
	popd

) else (

	IF NOT EXIST ..\bin mkdir ..\bin
	pushd ..\bin

	cl %CommonCompilerFlags% %InternalCompilerFlags% -MTd -Od -Z7 %IncludeDirectories% ..\minesweeper\minesweeper.c /Fe:minesweeper.exe /link %CommonLinkerFlags%

	del *.obj

	popd

)